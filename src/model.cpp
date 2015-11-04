#include "model.hpp"
#include "dbg.h"

#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <numeric>
#include <map>

using namespace std;


void PushTriangle(vector<ushort> & triangle,
                  ushort a, ushort b, ushort c){
  triangle.push_back(a);
  triangle.push_back(b);
  triangle.push_back(c);
}

std::vector<ushort> MeanPointMethod(vector<ushort> poly,
                                 vector<glm::vec3> & vertices){

  auto sum = accumulate(poly.begin(), poly.end(), glm::vec3(0,0,0),
                        [&vertices](glm::vec3 acc, int i){
                          return acc + vertices[i];});

  sum /= poly.size();

  vertices.push_back(sum);

  int ind = vertices.size()-1;

  auto ret = vector<ushort>();
  for(size_t i=0; i < poly.size()-1; i++){
    PushTriangle(ret,poly[i],poly[i+1], ind);
  }
  PushTriangle(ret,poly[poly.size()-1], poly[0], ind);

  return ret;
}

vector<ushort> ParsePolygonFace(const char* line,
                             vector<glm::vec3> & vertices,
                             int& inc_index){
  vector<ushort> ret;
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
    ret = vector<ushort>();

  }
  // quads
  else if(ret.size()==4){
    auto tmp = vector<ushort>();
    for(auto i : {0,1,3,1,2,3}){
      tmp.push_back(ret[i]);
    }
    ret = tmp;
  }
  return ret;
}

void ComputeNormals(Model * model){
  auto const & vi = model->Indices;
  auto const & vs = model->Vertices;
  map<int, vector<glm::vec3> > vertexNormals;
  for(size_t i = 0; i< vi.size(); i+=3){
    auto const & a = vs[vi[i]];
    auto const & b = vs[vi[i+1]];
    auto const & c = vs[vi[i+2]];
    auto normal = glm::cross(c - a, b - a);
    for(size_t j=i; j< i+3; j++){
      if(!vertexNormals.count(vi[j])){
        vertexNormals[vi[j]] = vector<glm::vec3>();
      }
      vertexNormals[vi[j]].push_back(normal);
    }
  }
  model->Normals.clear();
  for(size_t v=0; v<vs.size(); v++){
      auto sum = accumulate(vertexNormals[v].begin(), vertexNormals[v].end(), glm::vec3(0,0,0),
                            [](glm::vec3 acc, glm::vec3 vv){
                                return acc + vv;});
      model->Normals.push_back(-sum);
  }
}

Model::Model(const Model::ObjAsset & obj_asset){

  int inc_index=0;
  auto path = obj_asset.Path.c_str();

  FILE * file = fopen(path, "r");
  if( file == NULL ){
    log_err(" %s: Can't find %s.",
            __FUNCTION__,
            path);
    getchar();
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
      Vertices.push_back(vertex);
    }
    // parsing polygons faces
    else if ( strcmp( lineHeader, "f" ) == 0 ){

      fgets(lineHeader, sizeof(lineHeader), file);
      auto tmp = inc_index;
      auto ret = ParsePolygonFace(lineHeader, Vertices, tmp);
      for(auto vi : ret){
        Indices.push_back(vi + inc_index);
      }
      inc_index = tmp;
    }else{
      // Probably a comment, eat up the rest of the line
      char stupidBuffer[1000];
      fgets(stupidBuffer, 1000, file);
    }

  }

  ComputeNormals(this);

}

vector<ushort> Triangulate(Model * model, ushort a, ushort b, ushort c, int depth=1){

  auto & mv = model->Vertices;

  auto va=mv[a];
  auto vb=mv[b];
  auto vc=mv[c];

  auto ab = mv.size();
  auto vab = (va+vb)*0.5f;
  mv.push_back(vab);

  auto bc = ab+1;
  auto vbc = (vb+vc)*0.5f;
  mv.push_back(vbc);

  auto ca = bc+1;
  auto vca = (vc+va)*0.5f;
  mv.push_back(vca);


  if(depth>1){
    vector<ushort> ret;
    auto r1 = Triangulate(model, a,  ab, ca, depth-1);
    auto r2 = Triangulate(model, ab, b,  bc, depth-1);
    auto r3 = Triangulate(model, ab, bc, ca, depth-1);
    auto r4 = Triangulate(model, ca, bc, c,  depth-1);
    ret.insert(ret.end(),r1.begin(), r1.end());
    ret.insert(ret.end(),r2.begin(), r2.end());
    ret.insert(ret.end(),r3.begin(), r3.end());
    ret.insert(ret.end(),r4.begin(), r4.end());
    return ret;
  }
  else{
    return vector<ushort>{
      a, ab,ca,
      ab,b, bc,
      ab,bc,ca,
      ca,bc,c  };
  }
}

Model::Model(const Model::Rectangle & rect){

  auto const & w = rect.Width;
  auto const & h = rect.Height;

  Vertices = {
    glm::vec3( w/2, 0, -h/2),
    glm::vec3(-w/2, 0, -h/2),
    glm::vec3( w/2, 0,  h/2),
    glm::vec3(-w/2, 0,  h/2),
  };

  auto& mi = Indices;

  ushort base[] = { 0, 1, 2, 2, 1, 3 };
             

  // ushort base[] = { 0, 1, 2, 2, 1, 3, 1, 5, 3, 3, 5, 7, 0, 4, 1, 1, 4, 5, 3, 7, 2, 2, 7, 6, 5, 4, 7, 7, 4, 6, 4, 0, 6, 6, 0, 2};

  for(size_t i=0; i<6; i+=3){
    auto t = Triangulate(this, base[i], base[i+1], base[i+2], rect.Triang_depth);
    mi.insert(mi.end(),t.begin(), t.end());
  }

  ComputeNormals(this);

}

Model::Model(const Model::Cuboid & cuboid){

  auto const & w = cuboid.Width;
  auto const & h = cuboid.Height;
  auto const & d = cuboid.Depth;

  Vertices = {
    glm::vec3(-w/2, -h/2, -d/2),
    glm::vec3(-w/2, -h/2, d/2),
    glm::vec3(-w/2, h/2, -d/2),
    glm::vec3(-w/2, h/2, d/2),
    glm::vec3(w/2, -h/2, -d/2),
    glm::vec3(w/2, -h/2, d/2),
    glm::vec3(w/2, h/2, -d/2),
    glm::vec3(w/2, h/2, d/2),
  };

  auto& mi = Indices;

  ushort base[] = { 0, 1, 2, 2, 1, 3,
                    1, 5, 3, 3, 5, 7,
                    5, 4, 7, 7, 4, 6,
                    4, 0, 6, 6, 0, 2,
                    3, 7, 2, 2, 7, 6,
                    0, 4, 1, 1, 4, 5};

  // ushort base[] = { 0, 1, 2, 2, 1, 3, 1, 5, 3, 3, 5, 7, 0, 4, 1, 1, 4, 5, 3, 7, 2, 2, 7, 6, 5, 4, 7, 7, 4, 6, 4, 0, 6, 6, 0, 2};

  for(size_t i=0; i<36; i+=3){
    auto t = Triangulate(this, base[i], base[i+1], base[i+2], cuboid.Triang_depth);
    mi.insert(mi.end(),t.begin(), t.end());
  }

  ComputeNormals(this);

}
