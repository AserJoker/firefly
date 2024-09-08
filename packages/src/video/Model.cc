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
    float fvalue;
    if (mat->Get(AI_MATKEY_REFLECTIVITY, fvalue) == AI_SUCCESS) {
    }
    int ivalue;
    if (mat->Get(AI_MATKEY_ENABLE_WIREFRAME, ivalue) == AI_SUCCESS) {
    }
    if (mat->Get(AI_MATKEY_TWOSIDED, ivalue) == AI_SUCCESS) {
    }
    if (mat->Get(AI_MATKEY_BLEND_FUNC, ivalue) == AI_SUCCESS) {
    }
    if (mat->Get(AI_MATKEY_OPACITY, fvalue) == AI_SUCCESS) {
    }
    if (mat->Get(AI_MATKEY_SHININESS, fvalue) == AI_SUCCESS) {
    }
    if (mat->Get(AI_MATKEY_SHININESS_STRENGTH, fvalue) == AI_SUCCESS) {
    }
    if (mat->Get(AI_MATKEY_REFRACTI, fvalue) == AI_SUCCESS) {
    }
    aiString svalue;
    for (auto i = 0; i < mat->GetTextureCount(aiTextureType_DIFFUSE); i++) {
      mat->GetTexture(aiTextureType_DIFFUSE, i, &svalue);
    }
    for (auto i = 0; i < mat->GetTextureCount(aiTextureType_SPECULAR); i++) {
      mat->GetTexture(aiTextureType_SPECULAR, i, &svalue);
    }
    for (auto i = 0; i < mat->GetTextureCount(aiTextureType_AMBIENT); i++) {
      mat->GetTexture(aiTextureType_AMBIENT, i, &svalue);
    }
    for (auto i = 0; i < mat->GetTextureCount(aiTextureType_EMISSIVE); i++) {
      mat->GetTexture(aiTextureType_EMISSIVE, i, &svalue);
    }
    for (auto i = 0; i < mat->GetTextureCount(aiTextureType_HEIGHT); i++) {
      mat->GetTexture(aiTextureType_HEIGHT, i, &svalue);
    }
    for (auto i = 0; i < mat->GetTextureCount(aiTextureType_NORMALS); i++) {
      mat->GetTexture(aiTextureType_NORMALS, i, &svalue);
    }
    for (auto i = 0; i < mat->GetTextureCount(aiTextureType_SHININESS); i++) {
      mat->GetTexture(aiTextureType_SHININESS, i, &svalue);
    }
    for (auto i = 0; i < mat->GetTextureCount(aiTextureType_OPACITY); i++) {
      mat->GetTexture(aiTextureType_OPACITY, i, &svalue);
    }
    for (auto i = 0; i < mat->GetTextureCount(aiTextureType_DISPLACEMENT);
         i++) {
      mat->GetTexture(aiTextureType_DISPLACEMENT, i, &svalue);
    }
    for (auto i = 0; i < mat->GetTextureCount(aiTextureType_LIGHTMAP); i++) {
      mat->GetTexture(aiTextureType_LIGHTMAP, i, &svalue);
    }
    for (auto i = 0; i < mat->GetTextureCount(aiTextureType_REFLECTION); i++) {

      mat->GetTexture(aiTextureType_REFLECTION, i, &svalue);
    }
    for (auto i = 0; i < mat->GetTextureCount(aiTextureType_BASE_COLOR); i++) {
      mat->GetTexture(aiTextureType_BASE_COLOR, i, &svalue);
    }
    for (auto i = 0; i < mat->GetTextureCount(aiTextureType_NORMAL_CAMERA);
         i++) {
      mat->GetTexture(aiTextureType_NORMAL_CAMERA, i, &svalue);
    }
    for (auto i = 0; i < mat->GetTextureCount(aiTextureType_EMISSION_COLOR);
         i++) {
      mat->GetTexture(aiTextureType_EMISSION_COLOR, i, &svalue);
    }
    for (auto i = 0; i < mat->GetTextureCount(aiTextureType_METALNESS); i++) {
      mat->GetTexture(aiTextureType_METALNESS, i, &svalue);
    }
    for (auto i = 0; i < mat->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS);
         i++) {
      mat->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, i, &svalue);
    }
    for (auto i = 0; i < mat->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION);
         i++) {
      mat->GetTexture(aiTextureType_AMBIENT_OCCLUSION, i, &svalue);
    }
    for (auto i = 0; i < mat->GetTextureCount(aiTextureType_SHEEN); i++) {
      mat->GetTexture(aiTextureType_SHEEN, i, &svalue);
    }
    for (auto i = 0; i < mat->GetTextureCount(aiTextureType_CLEARCOAT); i++) {
      mat->GetTexture(aiTextureType_CLEARCOAT, i, &svalue);
    }
    for (auto i = 0; i < mat->GetTextureCount(aiTextureType_TRANSMISSION);
         i++) {
      mat->GetTexture(aiTextureType_TRANSMISSION, i, &svalue);
    }
    for (auto i = 0; i < mat->GetTextureCount(aiTextureType_UNKNOWN); i++) {
      mat->GetTexture(aiTextureType_UNKNOWN, i, &svalue);
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