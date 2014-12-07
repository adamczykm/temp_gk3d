#ifndef OBJ_MODEL_HPP
#define OBJ_MODEL_HPP

#include <vector>
#include <glm/glm.hpp>


struct ObjModel{
  std::vector<glm::vec3> vertices;

  std::vector<glm::vec3> normals;

  bool hasTextures;
  std::vector<glm::vec2> uvs;

};

bool LoadOBJModel(const char * path,
                  const ObjModel& model);


#endif
