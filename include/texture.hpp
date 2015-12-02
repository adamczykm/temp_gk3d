#include <cstdlib>
#include <iostream>
#include <map>
#include <SDL/SDL_image.h>
#include <GL/glew.h>

#define RGB = 3
#define R   = 0
#define G   = 1
#define B   = 2

class Texture{
protected:
  GLuint _tex_id;
  SDL_Surface* _tex;
  GLint _format;
  GLint _level;

  Texture(SDL_Surface* tex, GLint format, GLint level=0) :
    _tex_id(0), _tex(tex),_format(format), _level(level) {}

public:
  ~Texture();

  static GLint FromAsset(Texture *out_tex, std::string);
  static GLint FromAsset(Texture *out_tex, const char* );

  void ActivateAndBindTexture(GLuint);

  GLuint GenBindAndSpecifyTexture(const std::map<GLenum, GLint> &parameters_int,
                                  const std::map<GLenum, GLfloat> &parameters_float,
                                  char);
};
