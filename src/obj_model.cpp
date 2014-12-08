#include "obj_model.hpp"
#include "dbg.h"

#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <numeric>

using namespace std;

void PushTriangle(vector<int> & triangle,
                  int a, int b, int c){
  triangle.push_back(a);
  triangle.push_back(b);
  triangle.push_back(c);
}

std::vector<int> MeanPointMethod(vector<int> poly,
                                 vector<glm::vec3> & vertices){

  auto sum = accumulate(poly.begin(), poly.end(), glm::vec3(0,0,0),
                        [&vertices](glm::vec3 acc, int i){
                          return acc + vertices[i];});

  sum /= poly.size();

  vertices.push_back(sum);

  int ind = vertices.size()-1;

  auto ret = vector<int>();
  for(size_t i=0; i < poly.size()-1; i++){
    PushTriangle(ret,poly[i],poly[i+1], ind);
  }
  PushTriangle(ret,poly[poly.size()-1], poly[0], ind);

  return ret;
}

vector<int> ParsePolygonFace(const char* line,
                             vector<glm::vec3> & vertices,
                             int& inc_index){
  vector<int> ret;
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
    ret = MeanPointMethod(ret, vertices);
    inc_index++;
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


bool LoadOBJModel(string path,
                  ObjModel& model,
                  bool computeNormals){
  return LoadOBJModel(path.c_str(), model, computeNormals);
}

bool LoadOBJModel(const char * path,
                  ObjModel & model,
                  bool computeNormals){

  vector<glm::vec3> tempVertices;
  vector<int> tempVerticesIndices;

  int inc_index=0;

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
      auto tmp = inc_index;
      auto ret = ParsePolygonFace(lineHeader, tempVertices, tmp);
      for(auto vi : ret){
        tempVerticesIndices.push_back(vi + inc_index);
      }
      inc_index = tmp;
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

  return true;
}

bool LoadCuboidModel(float width, float height, float depth,
                     ObjModel& model, bool computeNormals){

  glm::vec3 vertices[] = {
    glm::vec3(-width/2, -height/2, -depth/2),
    glm::vec3(-width/2, -height/2, depth/2),
    glm::vec3(-width/2, height/2, -depth/2),
    glm::vec3(-width/2, height/2, depth/2),
    glm::vec3(width/2, -height/2, -depth/2),
    glm::vec3(width/2, -height/2, depth/2),
    glm::vec3(width/2, height/2, -depth/2),
    glm::vec3(width/2, height/2, depth/2),
  };

  vector<int> triangles;
  PushTriangle(triangles, 0, 1, 3);
  PushTriangle(triangles, 1, 2, 3);
  PushTriangle(triangles, 4, 5, 7);
  PushTriangle(triangles, 5, 6, 7);

  PushTriangle(triangles, 0, 1, 4);
  PushTriangle(triangles, 1, 5, 4);
  PushTriangle(triangles, 2, 3, 6);
  PushTriangle(triangles, 3, 7, 6);

  PushTriangle(triangles, 3, 0, 7);
  PushTriangle(triangles, 0, 4, 7);
  PushTriangle(triangles, 1, 2, 5);
  PushTriangle(triangles, 2, 6, 5);

  for(auto i : triangles){
    model.vertices.push_back(vertices[i]);
  }

  if(computeNormals){
    ComputeNormals(model);
  }
  return true;
}
