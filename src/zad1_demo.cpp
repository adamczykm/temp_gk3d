#include "assets.hpp"
#include "zad1_demo.hpp"
#include "obj_model.hpp"

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
  glEnable(GL_CULL_FACE);
  return window;
}





int Run(){
  ObjModel model;
  if(!LoadOBJModel(GetPathForAsset("chair_simple.obj"), model)){
    return 1;
  }
  return 0;
}
