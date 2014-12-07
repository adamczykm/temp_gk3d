#include "obj_model.hpp"

const char * CHAIR_PATH = "/home/anks/Studia/g3d/zad1/assets/chair_simple.obj";

int main(int argc, char** argv){
  ObjModel model;
  if(!LoadOBJModel(CHAIR_PATH, model)){
    return 1;
  }
  return 0;
}
