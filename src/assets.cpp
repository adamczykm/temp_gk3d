// #ifndef ASSETS
// #define ASSETS

#include "../include/assets.hpp"
#include <string>
#include <SDL/SDL_image.h>
#include <iostream>

using namespace std;

string GetPathForAsset(string str){
  return string(ASSETS) + "/" + str;
}

string GetPathForAsset(const char* str){
  return string(ASSETS) + "/" + string(str);
}
