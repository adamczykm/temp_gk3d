#include "assets.hpp"
#include "zad1_demo.hpp"
#include "model.hpp"
#include "object_instance.hpp"
#include "shader.hpp"
#include "camera.hpp"
#include "controls.hpp"

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
                   Shader shader, glm::mat4 & P, glm::mat4 & V);

void DrawTriangles(VBO & vbo, const Model & geometry,
                   ObjectInstance & instance,
                   Shader shader, glm::mat4 & P, glm::mat4 & V);


void CleanVbo(VBO & vbo);

GLFWwindow* InitGLWindow();

glm::mat4 GetViewMatrixFromInputs(Camera camera, Controls controls);

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
  glm::vec3 light_color(1.0,1.0,0.85);
  glm::vec3 lightpos(0,0,0);

  shader.BindParameter("lightpos_world", lightpos);

  // ----------------------- MODELS

  // -------- chair
  Model chair_model(Model::ObjAsset("Vitra03.obj"));
  size_t chairs_count = 5;
  std::vector<ObjectInstance> chairs;
  glm::mat4 base_m = glm::lookAt(glm::vec3(0,0,0), glm::vec3(0,1,0), glm::vec3(0,0,1));
  base_m = glm::scale(base_m, glm::vec3(0.2));
  base_m = glm::translate(base_m, glm::vec3(0,130,-63));

  for(size_t i=0; i<chairs_count; i++){
    auto m = base_m;
    ObjectInstance oi(chair_model, glm::translate(m, float(i) * glm::vec3(20,0,0)),
                      PhongColor(glm::vec3(0.8,0.0,0.0), light_color, 0.35));
    chairs.push_back(oi);
  }

  // -------- hall
  Model hall_model(Model::Cuboid(100,25,60,5));
  ObjectInstance hall(hall_model, glm::mat4(1),
                      PhongColor(glm::vec3(0.8,0.2,0.5), light_color, 0.05));

  // -------- floor
  Model floor_model(Model::Cuboid(90,1,50,5));
  ObjectInstance floor(floor_model, glm::translate(glm::mat4(1), glm::vec3(0,-12.4,0)),
                       PhongColor(glm::vec3(0.9,0.2,0.0), light_color));


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
  do{

    InitVbo(vbo); // <------------ attribarrays


    // ---------------  controls & camera  --------------------------

    V = GetViewMatrixFromInputs(camera, controls);


    // -------------- SHADERS UPDATES ------------

    glUseProgram(shader.Index);

    shader.BindParameter("V", V);


    // ---------------- SCENE -------------------

    // draw chairs
    DrawTriangles(vbo, chair_model, chairs, shader, V, P);

    // hall
    DrawTriangles(vbo, hall_model, hall, shader, V, P);

    // floor
    DrawTriangles(vbo, floor_model, floor, shader, V, P);

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


glm::mat4 GetViewMatrixFromInputs(Camera camera, Controls controls){

    float yaw, pitch, roll;
    glm::vec3 translation;

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


    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


void CleanVbo(VBO & vbo){
    glDeleteBuffers(1, &vbo.VertexBuffer);
    glDeleteBuffers(1, &vbo.NormalBuffer);
    glDeleteBuffers(1, &vbo.ElementBuffer);
}


void DrawTriangles(VBO & vbo, const Model & geometry,
                   ObjectInstance & instance,
                   Shader shader, glm::mat4 & V, glm::mat4 & P){

    std::vector<ObjectInstance> tmp;
    tmp.push_back(instance);

    DrawTriangles(vbo, geometry, tmp, shader, V, P);
}


void DrawTriangles(VBO & vbo, const Model & geometry,
                   std::vector<ObjectInstance> & instances,
                   Shader shader, glm::mat4 & V, glm::mat4 & P){

    glBindBuffer(GL_ARRAY_BUFFER, vbo.VertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, geometry.Vertices.size() * sizeof(glm::vec3), &geometry.Vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo.NormalBuffer);
    glBufferData(GL_ARRAY_BUFFER, geometry.Normals.size() * sizeof(glm::vec3), &geometry.Normals[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo.ElementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, geometry.Indices.size() * sizeof(unsigned short), &geometry.Indices[0] , GL_STATIC_DRAW);

    for(auto & instance : instances){

       const auto & M = instance.PosM;

       shader.BindParameter("MVP", P * V * M);
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

    // glUniformMatrix4fv(shader_mvp, 1, GL_FALSE, &MVP[0][0]);
    // glUniformMatrix4fv(shader_m, 1, GL_FALSE, &M[0][0]);
    // glUniformMatrix4fv(shader_v, 1, GL_FALSE, &V[0][0]);
    // glUniform3f(shader_light, lightPos.x, lightPos.y, lightPos.z);


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
