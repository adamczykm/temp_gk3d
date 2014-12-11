#include "assets.hpp"
#include "zad1_demo.hpp"
#include "obj_model.hpp"
#include "shader_loader.hpp"
#include "camera.hpp"
#include "controls.hpp"

#include "dbg.h"

// GL includes
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

const char * CHAIR_PATH = "../assets/chair_simple.obj";


// --------------- GL init
const int W_WIDTH = 1024;
const int W_HEIGHT = 576;


GLFWwindow* InitGLWindow(){
  // Initialise GLFW
  GLFWwindow* window;

  if( !glfwInit() )
    {
      log_err("Failed to initialize GLFW\n" );
      return NULL;
    }

  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Open a window and create its OpenGL context
  window = glfwCreateWindow( W_WIDTH, W_HEIGHT, "Tutorial 07 - Model Loading", NULL, NULL);
  if( window == NULL ){
    log_err("Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible.\n" );
    glfwTerminate();
    return NULL;
  }
  glfwMakeContextCurrent(window);

  // Initialize GLEW
  glewExperimental = true; // Needed for core profile
  if (glewInit() != GLEW_OK) {
    log_err("Failed to initialize GLEW\n");
    return NULL;
  }

  // Ensure we can capture the escape key being pressed below
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
  glfwSetCursorPos(window, W_WIDTH/2, W_HEIGHT/2);

  // Dark blue background
  glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

  // Enable depth test
  glEnable(GL_DEPTH_TEST);
  // Accept fragment if it closer to the camera than the former one
  glDepthFunc(GL_LESS);

  // Cull triangles which normal is not towards the camera
  // glEnable(GL_CULL_FACE);
  return window;
}





int Run(){
  auto window = InitGLWindow();

  // ------------------ Camera & Controls

  auto camera = Camera();
  auto controls = Controls(window, W_WIDTH, W_HEIGHT);
  glm::vec3 translation;
  float yaw, pitch, roll;

  // ------------------ Init Vertex Array
  GLuint VertexArrayID;
  glGenVertexArrays(1, &VertexArrayID);
  glBindVertexArray(VertexArrayID);


  // ----------------------- SHADERS
  std::string vs_path = GetPathForAsset("test.vertexshader");
  std::string fs_path = GetPathForAsset("test.fragmentshader");

  auto shaders = LoadShaders(vs_path.c_str(), fs_path.c_str());
  auto shader_mvp = glGetUniformLocation(shaders, "MVP");
  auto shader_v = glGetUniformLocation(shaders, "V");
  auto shader_m = glGetUniformLocation(shaders, "M");
  auto shader_light = glGetUniformLocation(shaders, "lightpos_world");
  // ----------------------- MODEL
  ObjModel model;
  // if(!LoadOBJModel(GetPathForAsset("chair_simple.obj"), model)){
  //   return 1;
  // }
  if(!LoadOBJModel(GetPathForAsset("Vitra03.obj"), model)){
    return 1;
  }
  // if(!LoadCuboidModel(40,20,20,model)){
  //   return 1;
  // }

  // VBO
  GLuint vertexbuffer;
  glGenBuffers(1, &vertexbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  glBufferData(GL_ARRAY_BUFFER, model.vertices.size() * sizeof(glm::vec3), &model.vertices[0], GL_STATIC_DRAW);

  GLuint normalbuffer;
  glGenBuffers(1, &normalbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
  glBufferData(GL_ARRAY_BUFFER, model.normals.size() * sizeof(glm::vec3), &model.normals[0], GL_STATIC_DRAW);

  GLuint elementbuffer;
  glGenBuffers(1, &elementbuffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, model.indices.size() * sizeof(unsigned short), &model.indices[0] , GL_STATIC_DRAW);
  //

  // mvp
  glm::mat4 V;
  glm::mat4 P = camera.GetProjectionMatrix();
  glm::mat4 M = glm::mat4(1.0);
  glm::mat4 MVP;
  glm::vec3 lightPos = glm::vec3(0,70,-50);

  // ---------------------- GL DRAW LOOP
  auto nbFrames = 0;
  auto lastTime = glfwGetTime();
  do{

    yaw = pitch = roll = 0;
    // controls && mvp
    controls.GetMovementFromInputs(translation, yaw, pitch, roll);
    camera.TranslateCamera(translation);
    camera.RotateCamera(yaw, pitch, roll);

    V = camera.GetViewMatrix();
    MVP = P * V * M;

    // Measure speed
    double currentTime = glfwGetTime();
    nbFrames++;
    if ( currentTime - lastTime >= 1.0 ){ // If last prinf() was more than 1sec ago
      printf("%d fps\n", nbFrames);
      nbFrames = 0;
      lastTime += 1.0;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaders);
    glUniformMatrix4fv(shader_mvp, 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(shader_m, 1, GL_FALSE, &M[0][0]);
    glUniformMatrix4fv(shader_v, 1, GL_FALSE, &V[0][0]);
    glUniform3f(shader_light, lightPos.x, lightPos.y, lightPos.z);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
                          0,                  // attribute
                          3,                  // size
                          GL_FLOAT,           // type
                          GL_FALSE,           // normalized?
                          0,                  // stride
                          (void*)0            // array buffer offset
                          );

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glVertexAttribPointer(
                          1,                                // attribute
                          3,                                // size
                          GL_FLOAT,                         // type
                          GL_FALSE,                         // normalized?
                          0,                                // stride
                          (void*)0                          // array buffer offset
                          );

    // Index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

    // Draw the triangles !
    glDrawElements(
                   GL_TRIANGLES,      // mode
                   model.indices.size(),    // count
                   GL_UNSIGNED_SHORT,   // type
                   (void*)0           // element array buffer offset
                   );

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    // Swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();

  } // Check if the ESC key was pressed or the window was closed
  while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
    glfwWindowShouldClose(window) == 0 );


  // --------------------- CLEANUP

  glDeleteBuffers(1, &vertexbuffer);
  glDeleteBuffers(1, &normalbuffer);
  glDeleteBuffers(1, &elementbuffer);

  glDeleteProgram(shaders);
  glDeleteVertexArrays(1, &VertexArrayID);

  // Close OpenGL window and terminate GLFW
  glfwTerminate();

  return 0;
}
