#include "model.hpp"

#include <glm/glm.hpp>

using namespace glm;


struct PhongColor{
  vec3 Ambient;
  vec3 Diffuse;
  vec3 Specular;

  inline PhongColor(vec3 diffuse, vec3 ambient, vec3 specular) :
    Ambient(ambient), Diffuse(diffuse), Specular(specular) {}

  inline PhongColor(vec3 color, vec3 lightColor, float specular=.2f, float ambient=.15f) :
    Ambient(ambient*color), Diffuse(color), Specular(specular*lightColor) {}
};


struct ObjectInstance{
  const Model & Geometry;
  mat4 PosM;
  PhongColor Color;

  inline ObjectInstance(const Model & geometry, mat4 posM, PhongColor color) :
    Geometry(geometry), PosM(posM), Color(color) {}

};
