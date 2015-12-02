#include "texture.hpp"
#include "assets.hpp"
#include <SDL/SDL_image.h>


using namespace std;

Texture::~Texture(){
  SDL_FreeSurface(this->_tex);
  glDeleteTextures(1, &this->_tex_id);
}

GLint Texture::FromAsset(Texture *out_tex, std::string asset_name){
  return Texture::FromAsset(out_tex, asset_name.c_str());
}

GLint Texture::FromAsset(Texture *out_tex, const char* asset_name){
  // loading texture file
  string asset_path = GetPathForAsset(asset_name);
  SDL_Surface* tex = IMG_Load(GetPathForAsset(asset_path).c_str());

  if(tex == NULL){
    cerr << "IMG_Load: " << SDL_GetError() << endl;
    return -1;
  }

  *out_tex = Texture(tex, GL_RGBA);

  return 0;
}

GLuint Texture::GenBindAndSpecifyTexture(const map<GLenum, GLint> &parameters_int,
                                         const map<GLenum, GLfloat> &parameters_float,
                                         char replace_if_exists=0){
  if(this->_tex_id && replace_if_exists){
    glDeleteTextures(1, &this->_tex_id);
  }

  GLuint tid = this->_tex_id;

  glGenTextures(1, &tid);
  glBindTexture(GL_TEXTURE_2D, tid);

  for(const auto& k : parameters_int){
    glTexParameteri(GL_TEXTURE_2D, k.first, k.second);
  }

  for(const auto& k : parameters_float){
    glTexParameterf(GL_TEXTURE_2D, k.first, k.second);
  }

  glTexImage2D(GL_TEXTURE_2D, // target
               0,  // level, 0 = base, no minimap,
               GL_RGBA, // internalformat
               this->_tex->w,  // width
               this->_tex->h,  // height
               0,  // border, always 0 in OpenGL ES
               GL_RGBA,  // format
               GL_UNSIGNED_BYTE, // type
               this->_tex->pixels);
  this->_tex_id = tid;
  return tid;
}

void Texture::ActivateAndBindTexture(GLuint index=0){
	glActiveTexture(index);
	glBindTexture(GL_TEXTURE_2D, this->_tex_id);
}



// GLuint TextureFromAsset(const char* asset_name){
// // loading texture file
//   string asset_path = GetPathForAsset(asset_name);
//   SDL_Surface* text = IMG_Load(GetPathForAsset(asset_path).c_str());

//   if(text == NULL){
//     cerr << "IMG_Load: " << SDL_GetError() << endl;
//     return -1;
//   }

//   GLuint texture_id;

// 	glGenTextures(1, &texture_id);
// 	glBindTexture(GL_TEXTURE_2D, texture_id);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
// 	glTexImage2D(GL_TEXTURE_2D, // target
//                0,  // level, 0 = base, no minimap,
//                GL_RGBA, // internalformat
//                text->w,  // width
//                text->h,  // height
//                0,  // border, always 0 in OpenGL ES
//                GL_RGBA,  // format
//                GL_UNSIGNED_BYTE, // type
//                text->pixels);

// 	SDL_FreeSurface(text);

//   return texture_id;
// }


 
// void Texture::_GenAndBind(const char* name){
//   glEnable(GL_TEXTURE_2D);
//   glGenTextures(1, &this->_texBufferId);
//   glBindTexture(GL_TEXTURE_2D, this->_texBufferId);
//   glTexImage2D(GL_TEXTURE_2D, this->_mipmappingLevel, GL_RGB, this->Width(), this->Height(),
//                0, GL_RGB, GL_UNSIGNED_BYTE, this->RawRGB());

//   // --- wrapping
//   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

//   // --- mag
//   // GL_TEXTURE_MAG_FILTER(GL_LINEAR)
//   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

//   // --- texture unit / selection
//   glActiveTexture (GL_TEXTURE0);

// }

// void Texture::_InitMembers(){
//   this->_mipmappingLevel = 0;
//   this->_idForProgId = new map<GLuint, GLuint>();
// }

// void Texture::SetupForShader(GLuint progId, const char* name){
//   GLuint ret = glGetUniformLocation(progId, name);
//   (*(this->_idForProgId))[progId] = ret;
//   return ret;
// }
