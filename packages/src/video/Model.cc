#include "video/Model.hpp"
#include "core/AutoPtr.hpp"
#include "core/Buffer.hpp"
#include "core/Singleton.hpp"
#include "exception/Exception.hpp"
#include "runtime/Media.hpp"
#include "video/Attribute.hpp"
#include "video/Geometry.hpp"
#include "video/Material.hpp"
#include "video/Shader.hpp"
#include <assimp/Importer.hpp>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <queue>
#include <vector>

using namespace firefly;
using namespace firefly::video;
Model::Model(core::AutoPtr<Geometry> geometry, core::AutoPtr<Material> material)
    : _geometry(geometry), _material(material) {}

std::vector<core::AutoPtr<Model>> Model::load(const std::string &name) {
  std::vector<core::AutoPtr<Model>> models;

  auto media = core::Singleton<runtime::Media>::instance();
  auto buf = media->load(name)->read();
  Assimp::Importer importer;
  const aiScene *scene = importer.ReadFileFromMemory(
      buf->getData(), buf->getSize(), aiProcess_Triangulate);
  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode) {
    throw exception::RuntimeException<"LoadModel">(importer.GetErrorString());
  }
  std::queue<aiNode *> workflow;
  std::vector<aiMesh *> meshs;
  workflow.push(scene->mRootNode);
  while (!workflow.empty()) {
    auto node = workflow.front();
    workflow.pop();
    for (auto i = 0; i < node->mNumMeshes; i++) {
      auto it = std::find(meshs.begin(), meshs.end(),
                          scene->mMeshes[node->mMeshes[i]]);
      if (it == meshs.end()) {
        meshs.push_back(scene->mMeshes[node->mMeshes[i]]);
      }
    }
    for (auto i = 0; i < node->mNumChildren; i++) {
      workflow.push(node->mChildren[i]);
    }
  }
  for (auto &mesh : meshs) {
    core::AutoPtr _geometry = new Geometry();
    core::AutoPtr _material = new Material("base");
    _material->setShader(new Shader("shader::sprite_2d"));
    _material->setTexture("texture0", new video::Image("texture::diffuse.jpg"));
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> coord[8];
    std::vector<glm::vec3> normal;
    std::vector<uint32_t> indices;
    for (auto i = 0; i < mesh->mNumVertices; i++) {
      auto v = mesh->mVertices[i];
      vertices.push_back({v.x, v.y, v.z});
      for (auto t = 0; mesh->HasTextureCoords(t); t++) {
        auto v = mesh->mTextureCoords[t][i];
        coord[t].push_back({v.x, v.y});
      }
      if (mesh->HasNormals()) {
        auto v = mesh->mNormals[i];
        normal.push_back({v.x, v.y, v.z});
      }
    }
    for (auto i = 0; i < mesh->mNumFaces; i++) {
      auto face = mesh->mFaces[i];
      for (auto j = 0; j < face.mNumIndices; j++) {
        indices.push_back(face.mIndices[j]);
      }
    }
    _geometry->setAttribute(
        Geometry::ATTR_POSITION,
        new Attribute(new core::Buffer(vertices.size() * sizeof(float) * 3,
                                       vertices.data()),
                      typeid(float), 3));
    _geometry->setAttribute(
        Geometry::ATTR_POSITION,
        new Attribute(new core::Buffer(vertices.size() * sizeof(float) * 3,
                                       vertices.data()),
                      typeid(float), 3));
    _geometry->setAttribute(
        Geometry::ATTR_NORMAL,
        new Attribute(
            new core::Buffer(normal.size() * sizeof(float) * 3, normal.data()),
            typeid(float), 3));
    for (auto i = 0; i < mesh->HasTextureCoords(i); i++) {
      _geometry->setAttribute(
          video::Geometry::ATTR_TEXCOORD_0 + i,
          new video::Attribute(
              new core::Buffer(coord[i].size() * 2 * sizeof(float),
                               coord[i].data()),
              typeid(float), 2));
    }
    _geometry->getIndices()->write(0, indices.size(), indices.data());
    models.push_back(new Model(_geometry, _material));
  }
  return models;
}
core::AutoPtr<Geometry> Model::getGeometry() { return _geometry; }
const core::AutoPtr<Geometry> &Model::getGeometry() const { return _geometry; }
core::AutoPtr<Material> Model::getMaterial() { return _material; }
const core::AutoPtr<Material> &Model::getMaterial() const { return _material; }