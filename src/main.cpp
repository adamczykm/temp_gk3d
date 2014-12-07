#include "obj_model.hpp"


int main(int argc, char** argv){
  ObjModel model;
  if(!LoadOBJModel("dupa",model)){
    return 1;
  }
  return 0;
}
