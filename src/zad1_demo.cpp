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

const int W_WIDTH = 1024;
const int W_HEIGHT = 576;

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
  glm::vec3 light_color(1.0,1.0,0.95);
  glm::vec3 lightpos(-15,15,0);
  float lightpow = 8.0f;

  shader.BindParameter("lightpow", lightpow);
  shader.BindParameter("lightcol", light_color);
  shader.BindParameter("lightpos_world", lightpos);


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
  float screen_pow = 300.0f;




  // ----------------------- MODELS

  // -------- chair
  Model chair_model(Model::ObjAsset("Vitra03.obj"));
  size_t chairs_count = 5;
  size_t chair_sets = 4;
  std::vector<ObjectInstance> chairs[chair_sets];

  for(size_t i=0; i < chair_sets; i++){

    int xside = i/2 ? 1 : -1;
    int yside = i%2 ? 1 : -1;


    glm::mat4 base_m = glm::lookAt(glm::vec3(0,0,0), glm::vec3(0,xside,0), glm::vec3(0,0,1));
    base_m = glm::scale(base_m, glm::vec3(0.2));
    base_m = glm::translate(base_m, glm::vec3(-150 * yside , 144 ,-63));


    for(size_t j=0; j<chairs_count; j++){
      auto m = base_m;
      ObjectInstance oi(chair_model, glm::translate(m, float(j) * yside * glm::vec3(20,0,0)),
                        PhongColor(glm::vec3(0.9,0.0,0.0), light_color, 0.65));
      chairs[i].push_back(oi);
    }

  }


  // -------- hall
  Model hall_model(Model::Cuboid(110,35,70,5));
  auto & tmp = hall_model.Normals;

  std::transform(tmp.begin(), tmp.end(), tmp.begin(),
                 [](glm::vec3 v){return -v;});

  ObjectInstance hall(hall_model, glm::translate(glm::mat4(1), glm::vec3(0,5,0)),
                      PhongColor(glm::vec3(0.9,0.9,0.9), light_color, 0.05));

  // -------- floor
  Model floor_model(Model::Cuboid(90,.2,50,5));
  ObjectInstance floor(floor_model, glm::translate(glm::mat4(1), glm::vec3(0,-12.5,0)),
                       PhongColor(glm::vec3(1.,0.8,0.0), light_color));

  // --------- net
  Model net_model(Model::Cuboid(.1,6,44,4));
  ObjectInstance net(net_model, glm::mat4(1),
                     PhongColor(glm::vec3(1.,1.,1.0), light_color, 0.2, .35));
  auto & tmp2 = net_model.Normals;
  std::transform(tmp2.begin(), tmp2.end(), tmp2.begin(),
                 [](glm::vec3 v){return -v;});

  // --------- posts
  Model post_model(Model::Cuboid(.2,16,.2,4));
  std::vector<ObjectInstance> posts;
  posts.push_back(ObjectInstance(post_model, glm::translate(glm::mat4(1), glm::vec3(0,-5,22)),
                                 PhongColor(glm::vec3(0.2,0.8,0.0), light_color, 0.8)));
  posts.push_back(ObjectInstance(post_model, glm::translate(glm::mat4(1), glm::vec3(0,-5,-22)),
                                 PhongColor(glm::vec3(0.2,0.8,0.0), light_color, 0.8)));



  // ----------- screen
  Model screen_model(Model::Cuboid(.1,12,28,4));
  ObjectInstance screen(screen_model, glm::translate(glm::mat4(1), screen_pos),
                     PhongColor(glm::vec3(0.,1.,0.0), light_color, 0., 4.));

    // ----------- pointlight
  Model pointlight_model(Model::Cuboid(1.2,1.2,1.2,2));
  ObjectInstance pointlight(pointlight_model, glm::translate(glm::mat4(1), lightpos),
                     PhongColor(light_color, light_color, 0., 6.));



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
    shader.BindParameter("lightpos_world", lightpos);
    shader.BindParameter("screenpow", screen_pow);
    shader.BindParameter("screencol", screen_colors[color_index]);
    shader.BindParameter("screenpos_world", screen_pos);
    shader.BindParameter("screendir_world", screen_dir);

    // ---------------- SCENE -------------------

    // draw chairs
    for(size_t i=0; i < chair_sets; i++){
      DrawTriangles(vbo, chair_model, chairs[i], shader, V, P);
    }

    // hall
    DrawTriangles(vbo, hall_model, hall, shader, V, P);

    // floor
    DrawTriangles(vbo, floor_model, floor, shader, V, P);

    // net
    DrawTriangles(vbo, net_model, net, shader, V, P);

    // posts
    DrawTriangles(vbo, post_model, posts, shader, V, P);

    // screen
    screen.Color = PhongColor(screen_color, light_color, 0., 4.);
    DrawTriangles(vbo, screen_model, screen, shader, V, P);

    // point light
    DrawTriangles(vbo, pointlight_model, pointlight, shader, V, P);



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

    if(yaw || pitch){
        camera.RotateCamera(yaw, pitch);
    }
    if(roll){
        camera.RollCamera(roll);
    }

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

    // glEnable(GL_CULL_FACE);
    return window;
}
