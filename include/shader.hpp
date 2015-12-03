#ifndef SHADER_HPP
#define SHADER_HPP

#include <cstdlib>
#include <iostream>
#include <GL/glew.h>
#include <map>
#include <glm/glm.hpp>

class Shader {

  GLuint _program_id;

  Shader(GLuint pid) : _program_id(pid) {}

public:

  static Shader FromAssets(const char* vertex_shader_asset,
                           const char* fragment_shader_asset);

  static Shader FromAssets(std::string vertex_shader_asset,
                           std::string fragment_shader_asset);


  void BindUniform(const char* key, float value);
  void BindUniform(const char* key, glm::vec3 value);
  void BindUniform(const char* key, glm::mat4 value);

};

#endif
