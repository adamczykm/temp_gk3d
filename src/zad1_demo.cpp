#include "assets.hpp"
#include "zad1_demo.hpp"
#include "model.hpp"
#include "object_instance.hpp"
#include "shader.hpp"
#include "camera.hpp"
#include "controls.hpp"
#include <algorithm>

#include "dbg.h"

// GL includes
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

const int W_WIDTH = 1024*2;
const int W_HEIGHT = 576*2;

struct VBO{
  GLuint VertexBuffer;
  GLuint NormalBuffer;
  GLuint ElementBuffer;
};


void InitVbo(VBO & vbo);

void DrawTriangles(VBO & vbo, const Model & geometry,
                   std::vector<ObjectInstance> & instances,
                   Shader & shader, glm::mat4 & P, glm::mat4 & V);

void DrawTriangles(VBO & vbo, const Model & geometry,
                   ObjectInstance & instance,
                   Shader & shader, glm::mat4 & P, glm::mat4 & V);


void CleanVbo(VBO & vbo);

GLFWwindow* InitGLWindow();

glm::mat4 GetViewMatrixFromInputs(Camera & camera, Controls & controls);

int Run(){

  auto window = InitGLWindow();

  // ------------------ Camera & Controls

  auto camera = Camera();
  auto controls = Controls(window, W_WIDTH, W_HEIGHT);

  // ------------------ Init Vertex Array

  GLuint VertexArrayID;
  glGenVertexArrays(1, &VertexArrayID);
  glBindVertexArray(VertexArrayID);


  // ----------------------- SHADERS & LIGHTING

  std::string vs_path = GetPathForAsset("test.vertexshader");
  std::string fs_path = GetPathForAsset("test.fragmentshader");

  Shader shader(vs_path, fs_path);
  glm::vec3 light_color(1.0,0.9,0.25);
  glm::vec3 lamp1_pos(12.6,2.9,0);
  glm::vec3 lamp2_pos(-12.6,2.9,0);
  float lightpow = 10.0f;

  shader.BindParameter("lightpow", lightpow);
  shader.BindParameter("lightcol", light_color);
  shader.BindParameter("lamp1_pos_world", lamp1_pos);
  shader.BindParameter("lamp2_pos_world", lamp2_pos);


  size_t screen_light_color_change_freq = 60;
  std::vector<glm::vec3> screen_colors = {
    glm::vec3(1,0,0),
    glm::vec3(0,1,0),
    glm::vec3(0,0,1),
    glm::vec3(1,1,0),
    glm::vec3(0,1,1),
    glm::vec3(1,0,1)};

  glm::vec3 screen_pos(55,8,0);
  glm::vec3 screen_dir(-1,0,0);
  float screen_pow = 200.0f;




  // ----------------------- MODELS

  // -------- bench
  Model bench_model(Model::ObjAsset("bank.obj"));
  auto base_m = glm::scale(glm::mat4(1), glm::vec3(0.2));
  base_m = glm::translate(base_m, glm::vec3(40,-63,45));

  ObjectInstance bench(bench_model, glm::translate(base_m,  glm::vec3(20,0,0)),
                    PhongColor(glm::vec3(0.8,0.2,0.0), light_color, 0.55));


  ObjectInstance bench2(bench_model, glm::translate(base_m,  glm::vec3(20,0,-90)),
                    PhongColor(glm::vec3(0.8,0.2,0.0), light_color, 0.55));
  

  // -------- lamp
  Model lamp_model(Model::ObjAsset("lamppost.obj"));
  base_m = glm::scale(glm::mat4(1), glm::vec3(0.005));

  ObjectInstance lamp(lamp_model, glm::translate(base_m, glm::vec3(2500, -2600 ,0)),
                      PhongColor(glm::vec3(0.9,0.9,0.9), light_color, 0.65));

  ObjectInstance lamp2(lamp_model, glm::translate(base_m, glm::vec3(-2500, -2600 ,0)),
                      PhongColor(glm::vec3(0.9,0.9,0.9), light_color, 0.65));



  // -------- grass
  Model grass_model(Model::Rectangle(90,50,2));
  ObjectInstance grass(grass_model, glm::translate(glm::mat4(1), glm::vec3(0,-12.5,0)),
                       PhongColor(glm::vec3(0.,0.8,0.0), light_color));

  // -------- path
  Model path_model(Model::Rectangle(20,50,2));
  ObjectInstance path(path_model, glm::translate(glm::mat4(1), glm::vec3(0,-12.45,0)),
                       PhongColor(glm::vec3(0.7,0.7,0.7), light_color));


  // ----------- screen
  Model screen_model(Model::Cuboid(.1,12,28,4));
  ObjectInstance screen(screen_model, glm::translate(glm::mat4(1), screen_pos),
                        PhongColor(glm::vec3(0.,1.,0.0), light_color, 0., 4.));

  // ----------- pointlight
  Model pointlight_model(Model::Cuboid(1.2,1.3,1.1,2));
  ObjectInstance pointlight(pointlight_model, glm::translate(glm::mat4(1), lamp1_pos),
                            PhongColor(glm::vec3(1.,0.9,0.2), light_color, 0., 6.));

  ObjectInstance pointlight2(pointlight_model, glm::translate(glm::mat4(1), lamp2_pos),
                            PhongColor(glm::vec3(1.,0.9,0.2), light_color, 0., 6.));



  // ---------------------- VBO
  VBO vbo;
  glGenBuffers(1, &vbo.VertexBuffer);
  glGenBuffers(1, &vbo.NormalBuffer);
  glGenBuffers(1, &vbo.ElementBuffer);


  // ---------------------- MVP
  glm::mat4 V;
  glm::mat4 P = camera.GetProjectionMatrix();
  glm::mat4 MVP;

  shader.BindParameter("MVP",MVP);

  // ---------------------- GL DRAW LOOP ----------------------
  int ind = 0;
  int color_index =0;
  glm::vec3 screen_color;
  do{
    ind = (ind + 1) % screen_light_color_change_freq;
    if(!ind){
      color_index = (color_index + 1) % screen_colors.size();
      screen_color = screen_colors[color_index];
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    InitVbo(vbo); // <------------ attribarrays


    // ---------------  controls & camera  --------------------------

    V = GetViewMatrixFromInputs(camera, controls);


    // -------------- SHADERS UPDATES ------------

    glUseProgram(shader.Index);

    shader.BindParameter("V", V);
    shader.BindParameter("lightpow", lightpow);
    shader.BindParameter("lightcol", light_color);
    shader.BindParameter("lamp1_pos_world", lamp1_pos);
    shader.BindParameter("lamp2_pos_world", lamp2_pos);
    shader.BindParameter("screenpow", screen_pow);
    shader.BindParameter("screencol", screen_colors[color_index]);
    shader.BindParameter("screenpos_world", screen_pos);
    shader.BindParameter("screendir_world", screen_dir);

    // ---------------- SCENE -------------------

    // draw benchs
    DrawTriangles(vbo, bench_model, bench, shader, V, P);
    DrawTriangles(vbo, bench_model, bench2, shader, V, P);

    // lamp
    DrawTriangles(vbo, lamp_model, lamp, shader, V, P);
    DrawTriangles(vbo, lamp_model, lamp2, shader, V, P);


    // grass
    DrawTriangles(vbo, grass_model, grass, shader, V, P);

    // path
    DrawTriangles(vbo, path_model, path, shader, V, P);


    // screen
    screen.Color = PhongColor(screen_color, light_color, 0., 4.);
    DrawTriangles(vbo, screen_model, screen, shader, V, P);

    // point light
    DrawTriangles(vbo, pointlight_model, pointlight, shader, V, P);
    DrawTriangles(vbo, pointlight_model, pointlight2, shader, V, P);



    // ------------------------------------------


    // --------- cleaning buffers

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    // Swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();

  } // Check if the ESC key was pressed or the window was closed
  while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0 );

  // --------------------- CLEANUP
  CleanVbo(vbo);

  glDeleteProgram(shader.Index);
  glDeleteVertexArrays(1, &VertexArrayID);

  // Close OpenGL window and terminate GLFW
  glfwTerminate();

  return 0;
}


glm::mat4 GetViewMatrixFromInputs(Camera & camera, Controls & controls){

  float yaw=0., pitch=0., roll=0.;
  vec3 translation(0);
  controls.GetMovementFromInputs(translation, yaw, pitch, roll);
  camera.TranslateCamera(translation);

  if(yaw || pitch || roll){
    camera.RotateCamera(yaw, pitch,roll);
  }
  // if(roll){
  //     camera.RollCamera(roll);
  // }

  return camera.GetViewMatrix();
}


void InitVbo(VBO & vbo){

  // ------------ vertex buffers
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vbo.VertexBuffer);
  glVertexAttribPointer(
                        0,                  // attribute
                        3,                  // size
                        GL_FLOAT,           // type
                        GL_FALSE,           // normalized?
                        0,                  // stride
                        (void*)0            // array buffer offset
                        );

  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, vbo.NormalBuffer);
  glVertexAttribPointer(
                        1,                                // attribute
                        3,                                // size
                        GL_FLOAT,                         // type
                        GL_FALSE,                         // normalized?
                        0,                                // stride
                        (void*)0                          // array buffer offset
                        );

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo.ElementBuffer);

  // ---- options
  glPixelTransferf(GL_RED_SCALE, 0.299);
  glPixelTransferf(GL_GREEN_SCALE, 0.587);
  glPixelTransferf(GL_BLUE_SCALE, 0.114);
}


void CleanVbo(VBO & vbo){
  glDeleteBuffers(1, &vbo.VertexBuffer);
  glDeleteBuffers(1, &vbo.NormalBuffer);
  glDeleteBuffers(1, &vbo.ElementBuffer);
}


void DrawTriangles(VBO & vbo, const Model & geometry,
                   ObjectInstance & instance,
                   Shader & shader, glm::mat4 & V, glm::mat4 & P){

  std::vector<ObjectInstance> tmp;
  tmp.push_back(instance);

  DrawTriangles(vbo, geometry, tmp, shader, V, P);
}


void DrawTriangles(VBO & vbo, const Model & geometry,
                   std::vector<ObjectInstance> & instances,
                   Shader & shader, glm::mat4 & V, glm::mat4 & P){

  glBindBuffer(GL_ARRAY_BUFFER, vbo.VertexBuffer);
  glBufferData(GL_ARRAY_BUFFER, geometry.Vertices.size() * sizeof(glm::vec3), &geometry.Vertices[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, vbo.NormalBuffer);
  glBufferData(GL_ARRAY_BUFFER, geometry.Normals.size() * sizeof(glm::vec3), &geometry.Normals[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo.ElementBuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, geometry.Indices.size() * sizeof(unsigned short), &geometry.Indices[0] , GL_STATIC_DRAW);

  for(auto & instance : instances){

    const auto & M = instance.PosM;

    auto VM = V * M;
    shader.BindParameter("MV", VM);
    shader.BindParameter("MVP", P * VM);
    shader.BindParameter("M", M);
    shader.BindParameter("ambient", instance.Color.Ambient);
    shader.BindParameter("diffuse", instance.Color.Diffuse);
    shader.BindParameter("specular", instance.Color.Specular);

    // Draw the triangles !
    glDrawElements(
                   GL_TRIANGLES,      // mode
                   geometry.Indices.size(),    // count
                   GL_UNSIGNED_SHORT,   // type
                   (void*)0           // element array buffer offset
                   );

  }
}

GLFWwindow* InitGLWindow(){
  // Initialise GLFW
  GLFWwindow* window;

  if( !glfwInit() ){
    log_err("Failed to initialize GLFW\n" );
    return NULL;
  }

  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Open a window and create its OpenGL context
  window = glfwCreateWindow( W_WIDTH, W_HEIGHT, "GK3D - Zadanie 1", NULL, NULL);

  if( window == NULL ){
    log_err("Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible.\n" );
    glfwTerminate();
    return NULL;
  }

  glfwMakeContextCurrent(window);

  glewExperimental = true;
  if (glewInit() != GLEW_OK) {
    log_err("Failed to initialize GLEW\n");
    return NULL;
  }

  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
  glfwSetCursorPos(window, W_WIDTH/2, W_HEIGHT/2);

  glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  //    glEnable(GL_CULL_FACE);
  return window;
}
