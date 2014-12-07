#ifndef OBJ_MODEL_HPP
#define OBJ_MODEL_HPP

#include <vector>
#include <glm/glm.hpp>


struct ObjModel{
  std::vector<glm::vec3> vertices;

  std::vector<glm::vec3> normals;

  std::vector<glm::vec2> uvs;

  bool hasTextures(){
    return !uvs.empty();
  }

};

bool LoadOBJModel(const char * path,
                  ObjModel& model);


#endif
