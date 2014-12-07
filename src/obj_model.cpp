#include "obj_model.hpp"
#include "dbg.h"

#include <stdio.h>
#include <cstdlib>
#include <iostream>

using namespace std;

bool TEST = false;

std::vector<int> ParsePolygonFace(const char* line){
  std::vector<int> ret;
  char c;
  int i=0;
  while((c=line[i])!='\0' || c!='\n'){
    if(c==' '){
      string index = "";
      while((c=line[++i])!=' ' && c!='/' && c!='\0' && c!='\n'){
        index += c;
        if(c=='\0' || c=='\n') break;
      }
      if(c=='\0' || c=='\n') break;
      ret.push_back(atoi(index.c_str()));
    }
    else{
      i++;
    }
  }
  // tylko 3 i 4 katy obsslugiwane
  if(ret.size() > 4){
    TEST=true;
    ret.clear();
  }
  // quads
  else if(ret.size()==4){
    auto tmp = vector<int>();
    for(auto i : {0,1,3,1,2,3}){
      tmp.push_back(ret[i]);
    }
    ret = tmp;
  }
  return ret;
}

void ComputeNormals(ObjModel& model){
  auto const & vs = model.vertices;
  for(size_t i = 0; i< vs.size(); i+=3){
    auto const & a = vs[i];
    auto const & b = vs[i+1];
    auto const & c = vs[i+2];
    model.normals.push_back(glm::normalize(glm::cross(c - a, b - a)));
  }
}


bool LoadOBJModel(std::string path,
                  ObjModel& model,
                  bool computeNormals){
  return LoadOBJModel(path.c_str(), model, computeNormals);
}

bool LoadOBJModel(const char * path,
                  ObjModel & model,
                  bool computeNormals){

  std::vector<glm::vec3> tempVertices;
  std::vector<int> tempVerticesIndices;

  TEST=false;
  FILE * file = fopen(path, "r");
  if( file == NULL ){
    log_err(" %s: Can't find %s.",
            __FUNCTION__,
            path);
    getchar();
    return false;
  }

  while( 1 ){

    char lineHeader[128];
    // read the first word of the line
    int res = fscanf(file, "%s", lineHeader);
    if (res == EOF)
      break;


    // parsing vertices
    if ( strcmp( lineHeader, "v" ) == 0 ){
      glm::vec3 vertex;
      fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
      tempVertices.push_back(vertex);
    }
    // parsing polygons faces
    else if ( strcmp( lineHeader, "f" ) == 0 ){

      fgets(lineHeader, sizeof(lineHeader), file);
      auto ret = ParsePolygonFace(lineHeader);
      for(auto vi : ret){
        tempVerticesIndices.push_back(vi);
      }
    }else{
      // Probably a comment, eat up the rest of the line
      char stupidBuffer[1000];
      fgets(stupidBuffer, 1000, file);
    }

  }

  for(auto vi : tempVerticesIndices){
    model.vertices.push_back(tempVertices[vi-1]);
  }

  if(computeNormals){
    ComputeNormals(model);
  }
  if(TEST){
    log_warn("ObjLoader don't support 5+-gons");
  }
  return true;
}
