#ifndef OBJ_MODEL_HPP
#define OBJ_MODEL_HPP

#include <vector>
#include <glm/glm.hpp>
#include <string>

struct ObjModel{
  std::vector<glm::vec3> vertices;
  std::vector<glm::vec3> normals;
  std::vector<unsigned short> indices;

  std::vector<glm::vec2> uvs;

  bool hasTextures(){
    return !uvs.empty();
  }

};

bool LoadOBJModel(std::string path,
                  ObjModel& model,
                  bool computeNormals=true);


bool LoadOBJModel(const char * path,
                  ObjModel& model,
                  bool computeNormals=true);

bool LoadCuboidModel(float width, float height, float depth,
                     ObjModel& model, bool computeNormals=true);


#endif
