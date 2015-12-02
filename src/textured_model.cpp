#include "textured_model.hpp"

void TexturedModel::BindAndEnableTexCoordsAttribute(GLuint attr_id){
	glEnableVertexAttribArray(attr_id);
	glBindBuffer(GL_ARRAY_BUFFER, this->_vbo_texcoords);
	glVertexAttribPointer(
                        attr_id, // attribute
                        2,                  // number of elements per vertex, here (x,y)
                        GL_FLOAT,           // the type of each element
                        GL_FALSE,           // take our values as-is
                        0,                  // no extra data between each position
                        0                   // offset of first element
                        );
  this->_texcoords_attr = attr_id;
}

GLuint TexturedModel::GenAndBindTexCoordsToBuffer(){
  GLuint buff_id;
  glGenBuffers(1, &buff_id);
  glBindBuffer(GL_ARRAY_BUFFER, buff_id);
  glBufferData(GL_ARRAY_BUFFER, this->_texcoords_count, this->_texcoords, GL_STATIC_DRAW);
  this->_vbo_texcoords = buff_id;
  return buff_id;
}

void TexturedModel::DisableVertexArrays(){
  Model::DisableVertexArrays();
  glDisableVertexAttribArray(this->_texcoords_attr);
  this->_texcoords_attr = 0;
}

