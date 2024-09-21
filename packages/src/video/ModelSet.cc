#include "video/ModelSet.hpp"
#include "core/AutoPtr.hpp"
#include "core/Buffer.hpp"
#include "gl/TextureWrapMode.hpp"
#include "runtime/Media.hpp"
#include "video/Attribute.hpp"
#include "video/AttributeIndex.hpp"
#include "video/Geometry.hpp"
#include "video/Material.hpp"
#include "video/Model.hpp"
#include <assimp/Importer.hpp>
#include <assimp/material.h>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/texture.h>
#include <assimp/types.h>
#include <cstdint>
#include <unordered_map>
#include <vector>
using namespace firefly;
using namespace firefly::video;

static core::AutoPtr<Material> parseMaterial(aiMaterial *mat) {
  core::AutoPtr<Material> material = new Material();
  aiColor3D color;
  float fvalue;
  int ivalue;
  material->setName(mat->GetName().C_Str());
  if (mat->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS) {
    glm::vec3 v = {color.r, color.g, color.b};
    material->setAmbient(v);
  }
  if (mat->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
    glm::vec3 v = {color.r, color.g, color.b};
    material->setDiffuse(v);
  }
  if (mat->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS) {
    glm::vec3 v = {color.r, color.g, color.b};
    material->setSpecular(v);
  }
  if (mat->Get(AI_MATKEY_COLOR_EMISSIVE, color) == AI_SUCCESS) {
    glm::vec3 v = {color.r, color.g, color.b};
    material->setEmissive(v);
  }
  if (mat->Get(AI_MATKEY_COLOR_REFLECTIVE, color) == AI_SUCCESS) {
    glm::vec3 v = {color.r, color.g, color.b};
    material->setReflective(v);
  }
  if (mat->Get(AI_MATKEY_COLOR_TRANSPARENT, color) == AI_SUCCESS) {
    glm::vec3 v = {color.r, color.g, color.b};
    material->setTransparent(v);
  }
  if (mat->Get(AI_MATKEY_REFLECTIVITY, fvalue) == AI_SUCCESS) {
    material->setReflectivity(fvalue);
  }
  if (mat->Get(AI_MATKEY_ENABLE_WIREFRAME, ivalue) == AI_SUCCESS) {
    material->setIsWireframe(ivalue);
  }
  if (mat->Get(AI_MATKEY_TWOSIDED, ivalue) == AI_SUCCESS) {
    material->setIsCullBackface(ivalue);
  }
  if (mat->Get(AI_MATKEY_BLEND_FUNC, ivalue) == AI_SUCCESS) {
    material->setIsBlendAdd(ivalue == aiBlendMode_Additive);
  }
  if (mat->Get(AI_MATKEY_OPACITY, fvalue) == AI_SUCCESS) {
    material->setOpacity(fvalue);
  }
  if (mat->Get(AI_MATKEY_SHININESS, fvalue) == AI_SUCCESS) {
    material->setShininess(fvalue);
  }
  if (mat->Get(AI_MATKEY_SHININESS_STRENGTH, fvalue) == AI_SUCCESS) {
    material->setShininessStrength(fvalue);
  }
  aiString svalue;
  for (auto type = 1; type <= 21; type++) {
    auto count = mat->GetTextureCount((aiTextureType)type);
    if (count) {
      std::string name;
      switch ((aiTextureType)type) {
      case aiTextureType_DIFFUSE:
        name = Material::DIFFUSE_TEX;
        break;
      case aiTextureType_SPECULAR:
        name = Material::SPECULAR_TEX;
        break;
      case aiTextureType_AMBIENT:
        name = Material::AMBIENT_COLOR;
        break;
      case aiTextureType_EMISSIVE:
        name = Material::EMISSIVE_TEX;
        break;
      case aiTextureType_HEIGHT:
        name = Material::HEIGHT_TEX;
        break;
      case aiTextureType_NORMALS:
        name = Material::NORMALS_TEX;
        break;
      case aiTextureType_SHININESS:
        name = Material::SHININESS_TEX;
        break;
      case aiTextureType_OPACITY:
        name = Material::OPACITY_TEX;
        break;
      case aiTextureType_DISPLACEMENT:
        name = Material::DISPLACEMENT_TEX;
        break;
      case aiTextureType_LIGHTMAP:
        name = Material::LIGHTMAP_TEX;
        break;
      case aiTextureType_REFLECTION:
        name = Material::REFLECTION_TEX;
        break;
      case aiTextureType_BASE_COLOR:
        name = Material::BASE_COLOR_TEX;
        break;
      case aiTextureType_NORMAL_CAMERA:
        name = Material::NORMAL_CAMERA_TEX;
        break;
      case aiTextureType_EMISSION_COLOR:
        name = Material::EMISSION_COLOR_TEX;
        break;
      case aiTextureType_METALNESS:
        name = Material::METALNESS_TEX;
        break;
      case aiTextureType_DIFFUSE_ROUGHNESS:
        name = Material::DIFFUSE_ROUGHNESS_TEX;
        break;
      case aiTextureType_AMBIENT_OCCLUSION:
        name = Material::AMBIENT_OCCLUSION_TEX;
        break;
      case aiTextureType_SHEEN:
        name = Material::SHEEN_TEX;
        break;
      case aiTextureType_CLEARCOAT:
        name = Material::CLEARCOAT_TEX;
        break;
      case aiTextureType_TRANSMISSION:
        name = Material::TRANSMISSION_TEX;
        break;
      default:
        break;
      };
      Material::TextureInfo info{};
      for (uint32_t i = 0; i < count; i++) {
        if (mat->GetTexture((aiTextureType)type, i, &svalue) == AI_SUCCESS) {
          info.path = svalue.C_Str();
          if (mat->Get(AI_MATKEY_TEXBLEND((aiTextureType)type, i), fvalue) ==
              AI_SUCCESS) {
            info.blend = fvalue;
          } else {
            info.blend = 1.0f;
          }
          if (mat->Get(AI_MATKEY_TEXOP((aiTextureType)type, i), ivalue) ==
              AI_SUCCESS) {
            info.op = (Material::TEXTURE_OPERATOR)ivalue;
          } else {
            info.op = Material::TEXTURE_OPERATOR::ADD;
          }
          if (mat->Get(AI_MATKEY_MAPPING((aiTextureType)type, i), ivalue) ==
              AI_SUCCESS) {
            info.mapping = ivalue;
          } else {
            info.mapping = 0;
          }
          if (mat->Get(AI_MATKEY_UVWSRC((aiTextureType)type, i), ivalue) ==
              AI_SUCCESS) {
            info.uvwsrc = ivalue;
          } else {
            info.uvwsrc = 0;
          }
          if (mat->Get(AI_MATKEY_MAPPINGMODE_U((aiTextureType)type, i),
                       ivalue) == AI_SUCCESS) {
            switch (ivalue) {
            case aiTextureMapMode_Wrap:
              info.mappingmodeU = gl::TEXTURE_WRAP_MODE::REPEAT;
              break;
            case aiTextureMapMode_Clamp:
              info.mappingmodeU = gl::TEXTURE_WRAP_MODE::CLAMP_TO_EDGE;
              break;
            case aiTextureMapMode_Mirror:
              info.mappingmodeU = gl::TEXTURE_WRAP_MODE::MIRRORED_REPEAT;
              break;
            case aiTextureMapMode_Decal:
              info.mappingmodeU = gl::TEXTURE_WRAP_MODE::CLAMP_TO_BORDER;
              break;
            }
          } else {
            info.mappingmodeU = gl::TEXTURE_WRAP_MODE::REPEAT;
          }
          if (mat->Get(AI_MATKEY_MAPPINGMODE_V((aiTextureType)type, i),
                       ivalue) == AI_SUCCESS) {
            switch (ivalue) {
            case aiTextureMapMode_Wrap:
              info.mappingmodeV = gl::TEXTURE_WRAP_MODE::REPEAT;
              break;
            case aiTextureMapMode_Clamp:
              info.mappingmodeV = gl::TEXTURE_WRAP_MODE::CLAMP_TO_EDGE;
              break;
            case aiTextureMapMode_Mirror:
              info.mappingmodeV = gl::TEXTURE_WRAP_MODE::MIRRORED_REPEAT;
              break;
            case aiTextureMapMode_Decal:
              info.mappingmodeV = gl::TEXTURE_WRAP_MODE::CLAMP_TO_BORDER;
              break;
            }
          } else {
            info.mappingmodeV = gl::TEXTURE_WRAP_MODE::REPEAT;
          }
          glm::vec3 position;
          if (mat->Get(AI_MATKEY_TEXMAP_AXIS((aiTextureType)type, i),
                       position) == AI_SUCCESS) {
            info.texmapAxis = {position.x, position.y, position.z};
          }
          if (mat->Get(AI_MATKEY_TEXFLAGS((aiTextureType)type, i), ivalue) ==
              AI_SUCCESS) {
            if (ivalue & aiTextureFlags_Invert) {
              info.invert = true;
            } else {
              info.invert = false;
            }
            if (ivalue & aiTextureFlags_UseAlpha) {
              info.useAlpha = true;
            } else {
              info.useAlpha = false;
            }
          }
          material->setTexture(fmt::format("{}_{}", name, i), info);
          if (i == 0) {
            material->setTexture(name, info);
          }
        }
      }
    }
  }
  return material;
}
static core::AutoPtr<Geometry> parseGeometry(const aiMesh *mesh) {
  core::AutoPtr<Geometry> geometry = new Geometry();
  std::vector<glm::vec3> vertices;
  std::vector<glm::vec3> normals;
  std::vector<glm::vec3> tangents;
  std::vector<glm::vec3> bitangents;
  std::vector<glm::vec2> texcoords[AI_MAX_NUMBER_OF_TEXTURECOORDS];
  std::vector<glm::vec4> colors;
  for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
    auto &v = mesh->mVertices[i];
    vertices.push_back({v.x, v.y, v.z});
    for (auto j = 0; j < AI_MAX_NUMBER_OF_TEXTURECOORDS; j++) {
      if (mesh->HasTextureCoords(j)) {
        texcoords[j].push_back(
            {mesh->mTextureCoords[j][i].x, mesh->mTextureCoords[j][i].y});
      }
    }
    if (mesh->HasVertexColors(0)) {
      colors.push_back({mesh->mColors[0][i].r, mesh->mColors[0][i].g,
                        mesh->mColors[0][i].b, mesh->mColors[0][i].a});
    }
    if (mesh->HasNormals()) {
      auto &v = mesh->mNormals[i];
      normals.push_back({v.x, v.y, v.z});
    }
    if (mesh->HasTangentsAndBitangents()) {
      auto &v = mesh->mTangents[i];
      tangents.push_back({v.x, v.y, v.z});
      v = mesh->mBitangents[i];
      bitangents.push_back({v.x, v.y, v.z});
    }
  }
  core::AutoPtr<Attribute> attrVertices = new Attribute(
      new core::Buffer((uint32_t)vertices.size() * sizeof(glm::vec3),
                       vertices.data()),
      typeid(float), 3);
  geometry->setAttribute(Geometry::ATTR_POSITION, attrVertices);
  core::AutoPtr<Attribute> attrNormals = new Attribute(
      new core::Buffer((uint32_t)normals.size() * sizeof(glm::vec3),
                       normals.data()),
      typeid(float), 3);
  geometry->setAttribute(Geometry::ATTR_NORMAL, attrVertices);
  if (colors.empty()) {
    core::AutoPtr attrColor =
        new Attribute((uint32_t)colors.size() * sizeof(glm::vec4),
                      colors.data(), typeid(float), 4);
    geometry->setAttribute(Geometry::ATTR_COLOR, attrColor);
  }
  for (auto i = 0; i < AI_MAX_NUMBER_OF_TEXTURECOORDS; i++) {
    if (!texcoords[i].empty()) {
      core::AutoPtr attrTex =
          new Attribute((uint32_t)texcoords[i].size() * sizeof(glm::vec2),
                        texcoords[i].data(), typeid(float), 2);
      geometry->setAttribute(Geometry::ATTR_TEXCOORD + i, attrTex);
    }
  }
  if (!tangents.empty()) {
    core::AutoPtr attrTangent = new Attribute(
        new core::Buffer((uint32_t)tangents.size() * sizeof(glm::vec3),
                         tangents.data()),
        typeid(float), 3);
    geometry->setAttribute(Geometry::ATTR_TANGENT, attrTangent);
    core::AutoPtr attrBitangent =
        new Attribute((uint32_t)bitangents.size() * sizeof(glm::vec3),
                      bitangents.data(), typeid(float), 3);
    geometry->setAttribute(Geometry::ATTR_BITANGENT, attrBitangent);
  }
  std::vector<uint32_t> indices;
  for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
    for (uint32_t j = 0; j < mesh->mFaces[i].mNumIndices; j++) {
      indices.push_back(mesh->mFaces[i].mIndices[j]);
    }
  }
  geometry->setAttributeIndex(new AttributeIndex(
      (uint32_t)indices.size() * sizeof(uint32_t), indices.data()));
  return geometry;
}
static std::unordered_map<std::string, core::AutoPtr<Model>> parseModels(
    const aiScene *scene, const aiNode *node,
    std::unordered_map<std::string, core::AutoPtr<Material>> &materials) {
  std::unordered_map<std::string, core::AutoPtr<Model>> models;
  for (uint32_t i = 0; i < node->mNumMeshes; i++) {
    auto mesh = scene->mMeshes[node->mMeshes[i]];
    auto mat = scene->mMaterials[mesh->mMaterialIndex];
    if (!materials.contains(mat->GetName().C_Str())) {
      materials[mat->GetName().C_Str()] = parseMaterial(mat);
    }
    auto &material = materials.at(mat->GetName().C_Str());
    auto geometry = parseGeometry(mesh);
    models[mesh->mName.C_Str()] = new Model(geometry, material);
  }
  return models;
}

static void loadModelSet(
    core::AutoPtr<ModelSet> &modelset, const aiScene *scene, aiNode *node,
    std::unordered_map<std::string, core::AutoPtr<Material>> &materials) {
  auto models = parseModels(scene, node, materials);
  for (auto &[name, model] : models) {
    modelset->setModel(name, model);
  }
  for (uint32_t i = 0; i < node->mNumChildren; i++) {
    core::AutoPtr<ModelSet> mset = new ModelSet();
    loadModelSet(mset, scene, node->mChildren[i], materials);
    modelset->setChild(node->mChildren[i]->mName.C_Str(), mset);
  }
}

ModelSet::ModelSet() {}
ModelSet::ModelSet(const std::string &name) {
  std::unordered_map<std::string, core::AutoPtr<Material>> materials;
  auto media = core::Singleton<runtime::Media>::instance();
  std::string filepath;
  auto fp = media->resolve(name);
  Assimp::Importer importer;
  auto scene = importer.ReadFile(fp[fp.size() - 1],
                                 aiProcess_Triangulate | aiProcess_GenNormals |
                                     aiProcess_CalcTangentSpace);
  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode) {
    throw exception::RuntimeException<"LoadModel">(importer.GetErrorString());
  }
  _models = parseModels(scene, scene->mRootNode, materials);
  for (uint32_t i = 0; i < scene->mRootNode->mNumChildren; i++) {
    auto node = scene->mRootNode->mChildren[i];
    core::AutoPtr<ModelSet> mset = new ModelSet();
    loadModelSet(mset, scene, node, materials);
    _children[node->mName.C_Str()] = mset;
  }
}
const std::unordered_map<std::string, core::AutoPtr<Model>> &
ModelSet::getModels() const {
  return _models;
}

std::unordered_map<std::string, core::AutoPtr<Model>> ModelSet::getAllModels() {
  std::unordered_map<std::string, core::AutoPtr<Model>> models;
  for (auto &[name, model] : getModels()) {
    models[name] = model;
  }
  for (auto &[_, child] : _children) {
    for (auto &[name, model] : child->getAllModels()) {
      models[name] = model;
    }
  }
  return models;
}
std::unordered_map<std::string, core::AutoPtr<Material>>
ModelSet::getAllMaterials() {
  std::unordered_map<std::string, core::AutoPtr<Material>> materials;
  for (auto &[name, model] : getModels()) {
    auto material = model->getMaterial();
    materials[material->getName()] = material;
  }
  for (auto &[_, child] : _children) {
    for (auto &[name, material] : child->getAllMaterials()) {
      materials[name] = material;
    }
  }
  return materials;
};

const std::unordered_map<std::string, core::AutoPtr<Model>>
ModelSet::getAllModels() const {
  std::unordered_map<std::string, core::AutoPtr<Model>> models;
  for (auto &[name, model] : getModels()) {
    models[name] = model;
  }
  for (auto &[_, child] : _children) {
    for (auto &[name, model] : child->getAllModels()) {
      models[name] = model;
    }
  }
  return models;
}
const std::unordered_map<std::string, core::AutoPtr<Material>>
ModelSet::getAllMaterials() const {
  std::unordered_map<std::string, core::AutoPtr<Material>> materials;
  for (auto &[name, model] : getModels()) {
    auto material = model->getMaterial();
    materials[material->getName()] = material;
  }
  for (auto &[_, child] : _children) {
    for (auto &[name, material] : child->getAllMaterials()) {
      materials[name] = material;
    }
  }
  return materials;
};

void ModelSet::setModel(const std::string &name,
                        const core::AutoPtr<Model> &model) {
  _models[name] = model;
}
const core::AutoPtr<Model> ModelSet::getModel(const std::string &name) const {
  if (!_models.contains(name)) {
    return nullptr;
  }
  return _models.at(name);
}
const std::unordered_map<std::string, core::AutoPtr<ModelSet>> &
ModelSet::getChildren() const {
  return _children;
}
const core::AutoPtr<ModelSet>
ModelSet::getChild(const std::string &name) const {
  if (_children.contains(name)) {
    return _children.at(name);
  }
  return nullptr;
}
void ModelSet::setChild(const std::string &name,
                        const core::AutoPtr<ModelSet> &modelset) {
  _children[name] = modelset;
}