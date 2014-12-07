#include "assets.hpp"

std::string GetPathForAsset(std::string str){
  return std::string(ASSETS) + "/" + str;
}

std::string GetPathForAsset(const char* str){
  return std::string(ASSETS) + "/" + std::string(str);
}
