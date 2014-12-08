#include "obj_model.hpp"
#include "dbg.h"

#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <numeric>
#include <map>

using namespace std;

void PushTriangle(vector<unsigned short> & triangle,
                  unsigned short a, unsigned short b, unsigned short c){
  triangle.push_back(a);
  triangle.push_back(b);
  triangle.push_back(c);
}

std::vector<unsigned short> MeanPointMethod(vector<unsigned short> poly,
                                 vector<glm::vec3> & vertices){

  auto sum = accumulate(poly.begin(), poly.end(), glm::vec3(0,0,0),
                        [&vertices](glm::vec3 acc, int i){
                          return acc + vertices[i];});

  sum /= poly.size();

  vertices.push_back(sum);

  int ind = vertices.size()-1;

  auto ret = vector<unsigned short>();
  for(size_t i=0; i < poly.size()-1; i++){
    PushTriangle(ret,poly[i],poly[i+1], ind);
  }
  PushTriangle(ret,poly[poly.size()-1], poly[0], ind);

  return ret;
}

vector<unsigned short> ParsePolygonFace(const char* line,
                             vector<glm::vec3> & vertices,
                             int& inc_index){
  vector<unsigned short> ret;
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
      ret.push_back(atoi(index.c_str())-1);
    }
    else{
      i++;
    }
  }
  // tylko 3 i 4 katy obsslugiwane
  if(ret.size() > 4){
    // ret = MeanPointMethod(ret, vertices);
    // inc_index++;
    ret = vector<unsigned short>();

  }
  // quads
  else if(ret.size()==4){
    auto tmp = vector<unsigned short>();
    for(auto i : {0,1,3,1,2,3}){
      tmp.push_back(ret[i]);
    }
    ret = tmp;
  }
  return ret;
}

void ComputeNormals(ObjModel& model){
  auto const & vi = model.indices;
  auto const & vs = model.vertices;
  map<int, vector<glm::vec3> > vertexNormals;
  for(size_t i = 0; i< vi.size(); i+=3){
    auto const & a = vs[vi[i]];
    auto const & b = vs[vi[i+1]];
    auto const & c = vs[vi[i+2]];
    auto normal = glm::normalize(glm::cross(c - a, b - a));
    for(size_t j=i; j< i+3; j++){
      if(!vertexNormals.count(vi[j])){
        vertexNormals[vi[j]] = vector<glm::vec3>();
      }
      vertexNormals[vi[j]].push_back(normal);
    }
  }
  model.normals.clear();
  for(auto v : vi){
      auto sum = accumulate(vertexNormals[v].begin(), vertexNormals[v].end(), glm::vec3(0,0,0),
                            [](glm::vec3 acc, glm::vec3 vv){
                          return acc + vv;});
      model.normals.push_back(glm::normalize(sum));
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
      model.vertices.push_back(vertex);
    }
    // parsing polygons faces
    else if ( strcmp( lineHeader, "f" ) == 0 ){

      fgets(lineHeader, sizeof(lineHeader), file);
      auto tmp = inc_index;
      auto ret = ParsePolygonFace(lineHeader, model.vertices, tmp);
      for(auto vi : ret){
        model.indices.push_back(vi + inc_index);
      }
      inc_index = tmp;
    }else{
      // Probably a comment, eat up the rest of the line
      char stupidBuffer[1000];
      fgets(stupidBuffer, 1000, file);
    }

  }

  if(computeNormals){
    ComputeNormals(model);
  }

  return true;
}

bool LoadCuboidModel(float width, float height, float depth,
                     ObjModel& model, bool computeNormals){

  model.vertices = {
    glm::vec3(-width/2, -height/2, -depth/2),
    glm::vec3(-width/2, -height/2, depth/2),
    glm::vec3(-width/2, height/2, -depth/2),
    glm::vec3(-width/2, height/2, depth/2),
    glm::vec3(width/2, -height/2, -depth/2),
    glm::vec3(width/2, -height/2, depth/2),
    glm::vec3(width/2, height/2, -depth/2),
    glm::vec3(width/2, height/2, depth/2),
  };

  PushTriangle(model.indices, 0, 1, 2);
  PushTriangle(model.indices, 2, 1, 3);
  PushTriangle(model.indices, 1, 5, 3);
  PushTriangle(model.indices, 3, 5, 7);

  PushTriangle(model.indices, 0, 4, 1);
  PushTriangle(model.indices, 1, 4, 5);
  PushTriangle(model.indices, 3, 7, 2);
  PushTriangle(model.indices, 2, 7, 6);

  PushTriangle(model.indices, 5, 4, 7);
  PushTriangle(model.indices, 7, 4, 6);
  PushTriangle(model.indices, 4, 0, 6);
  PushTriangle(model.indices, 6, 0, 2);

  if(computeNormals){
    ComputeNormals(model);
  }
  return true;
}
