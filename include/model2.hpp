#include <GL/glew.h>

class Model{
protected:
  GLfloat * _vertices;
  GLuint _vertices_count;
  GLuint _vbo_vertices;
  GLuint _vertices_attr;

  GLuint * _elements;
  GLuint _elements_count;
  GLuint _vbo_elements;

public:
  Model(){}
  ~Model();

  virtual void BindAndEnableVerticesAttribute(GLuint attr_id);
  virtual void BindAndEnableElementerticesAttribute(GLuint attr_id);
  virtual GLuint GenAndBindVerticesToBuffer();
  virtual GLuint GenAndBindElementsToBuffer();
  virtual GLint BindAndGetSizeElements();

  virtual void DisableVertexArrays();

};
