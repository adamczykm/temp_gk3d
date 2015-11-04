#ifndef OBJ_MODEL_HPP
#define OBJ_MODEL_HPP

#include <vector>
#include <glm/glm.hpp>
#include <string>

#include "assets.hpp"

typedef unsigned short ushort;

struct Model{

  // ctors

  struct ObjAsset{
    std::string Path;
    inline ObjAsset(std::string asset_path) : Path(GetPathForAsset(asset_path)) {}
    inline ObjAsset(const char * asset_path) : Path(GetPathForAsset(asset_path)) {}
  };

  struct Rectangle{
    float Width;
    float Height;
    ushort Triang_depth;

    inline Rectangle(float width, float height, ushort triangulation_depth=1)
      : Width(width), Height(height), Triang_depth(triangulation_depth) {}
  };


  struct Cuboid{
    float Width;
    float Height;
    float Depth;
    ushort Triang_depth;

    inline Cuboid(float width, float height, float depth, ushort triangulation_depth=1)
      : Width(width), Height(height), Depth(depth), Triang_depth(triangulation_depth) {}
  };

  Model(const Model::ObjAsset & obj_asset);
  Model(const Model::Cuboid & cuboid);
  Model(const Model::Rectangle & rect);

  // -------------------------------

  std::vector<glm::vec3> Vertices;
  std::vector<glm::vec3> Normals;
  std::vector<ushort> Indices;

  // textures
  std::vector<glm::vec2> Uvs;
  bool HasTextures(){
    return !Uvs.empty();
  }

};


#endif
