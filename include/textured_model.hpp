#include "model2.hpp"

class TexturedModel : public Model{
protected:
  GLuint _vbo_texcoords;
  GLfloat * _texcoords;
  GLuint _texcoords_count;
  GLuint _texcoords_attr;

  void BindAndEnableTexCoordsAttribute(GLuint attr_id);
  GLuint GenAndBindTexCoordsToBuffer();
  void DisableVertexArrays();
};
