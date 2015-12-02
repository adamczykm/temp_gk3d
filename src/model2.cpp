#include "model2.hpp"

GLuint Model::GenAndBindVerticesToBuffer(){
  GLuint buffId;
	glGenBuffers(1, &buffId);
	glBindBuffer(GL_ARRAY_BUFFER, buffId);
	glBufferData(GL_ARRAY_BUFFER, this->_vertices_count, this->_vertices, GL_STATIC_DRAW);
  this->_vbo_vertices = buffId;
  return buffId;
}

GLuint Model::GenAndBindElementsToBuffer(){
  GLuint buffId;
	glGenBuffers(1, &buffId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffId);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->_elements_count, this->_elements, GL_STATIC_DRAW);
  this->_vbo_elements = buffId;
  return buffId;
}


void Model::BindAndEnableVerticesAttribute(GLuint attr_id){
  glEnableVertexAttribArray(attr_id);
	// Describe our vertices array to OpenGL (it can't guess its format automatically)
	glBindBuffer(GL_ARRAY_BUFFER, this->_vbo_vertices);
	glVertexAttribPointer(
                        attr_id, // attribute
                        3,                 // number of elements per vertex, here (x,y,z)
                        GL_FLOAT,          // the type of each element
                        GL_FALSE,          // take our values as-is
                        0,                 // no extra data between each position
                        0                  // offset of first element
                        );
  this->_vertices_attr = attr_id;
}

GLint Model::BindAndGetSizeElements(){
  GLint size;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_vbo_elements);
  glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
  return size;
}

void Model::DisableVertexArrays(){
  glDisableVertexAttribArray(this->_vertices_attr);
  this->_vertices_attr = 0;
}


Model::~Model(){
  delete [] this->_elements;
  delete [] this->_vertices;
  glDeleteBuffers(1, &(this->_vbo_vertices));
}
