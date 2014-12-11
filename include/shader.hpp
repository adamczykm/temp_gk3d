#ifndef SHADER_HPP
#define SHADER_HPP

#include <GL/glew.h>
#include <map>
#include <glm/glm.hpp>

class Shader {

public:

  GLuint Index;

  Shader(const char * vertex_file_path, const char * fragment_file_path);

  Shader(std::string vertex_file_path, std::string fragment_file_path) :
    Shader(vertex_file_path.c_str(), fragment_file_path.c_str()) {};

  void BindParameter(const char* key, float value);

  void BindParameter(const char* key, glm::vec3 value);

  void BindParameter(const char* key, glm::mat4 value);

};

#endif
