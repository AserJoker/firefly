#include "video/Model.hpp"
#include "core/AutoPtr.hpp"
#include "core/Singleton.hpp"
#include "runtime/Media.hpp"
#include "video/Geometry.hpp"
#include "video/Material.hpp"
#include <assimp/Importer.hpp>
#include <assimp/material.h>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/types.h>
#include <unordered_map>

using namespace firefly;
using namespace firefly::video;
Model::Model(core::AutoPtr<Geometry> geometry, core::AutoPtr<Material> material)
    : _geometry(geometry), _material(material) {}
core::AutoPtr<Geometry> Model::getGeometry() { return _geometry; }
const core::AutoPtr<Geometry> &Model::getGeometry() const { return _geometry; }
core::AutoPtr<Material> Model::getMaterial() { return _material; }
const core::AutoPtr<Material> &Model::getMaterial() const { return _material; }
const std::unordered_map<std::string, core::AutoPtr<Model>> &
Model::getChildren() const {
  return _children;
}
std::unordered_map<std::string, core::AutoPtr<Model>> &Model::getChildren() {
  return _children;
}

static std::vector<std::string> loadMaterialTextures(const aiMaterial *mat,
                                                     aiTextureType type) {
  std::vector<std::string> result;
  for (auto i = 0; i < mat->GetTextureCount(type); i++) {
    aiString name;
    mat->GetTexture(type, i, &name);
    result.push_back(name.C_Str());
  }
  return result;
}

static void
parseMaterials(std::unordered_map<std::string, core::AutoPtr<Material>> &mat,
               const aiScene *scene) {
  for (auto i = 0; i < scene->mNumMaterials; i++) {
    auto &mat = scene->mMaterials[i];
    aiColor3D color;
    if (mat->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS) {
    }
    if (mat->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
    }
    if (mat->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS) {
    }
    if (mat->Get(AI_MATKEY_COLOR_EMISSIVE, color) == AI_SUCCESS) {
    }
    if (mat->Get(AI_MATKEY_COLOR_REFLECTIVE, color) == AI_SUCCESS) {
    }
    if (mat->Get(AI_MATKEY_COLOR_TRANSPARENT, color) == AI_SUCCESS) {
    }
    float opacity;
    if (mat->Get(AI_MATKEY_OPACITY, opacity) == AI_SUCCESS) {
    }
  }
}

static std::unordered_map<std::string, core::AutoPtr<Model>> parseModels(
    const aiNode *node, const aiScene *scene,
    const std::unordered_map<std::string, core::AutoPtr<Material>> &material) {
  return {};
}

std::unordered_map<std::string, core::AutoPtr<Model>>
Model::load(const std::string &name) {
  std::unordered_map<std::string, core::AutoPtr<Material>> materials;
  auto media = core::Singleton<runtime::Media>::instance();
  std::string filepath;
  auto fp = media->resolve(name);
  Assimp::Importer importer;
  auto scene = importer.ReadFile(fp[fp.size() - 1], aiProcess_Triangulate);
  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode) {
    throw exception::RuntimeException<"LoadModel">(importer.GetErrorString());
  }
  parseMaterials(materials, scene);
  return parseModels(scene->mRootNode, scene, materials);
}