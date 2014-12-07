#include "zad1_demo.hpp"
#include "obj_model.hpp"

const char * CHAIR_PATH = "/home/anks/Studia/g3d/zad1/assets/chair_simple.obj";

int Run(){
  ObjModel model;
  if(!LoadOBJModel(CHAIR_PATH, model)){
    return 1;
  }
  return 0;
}
