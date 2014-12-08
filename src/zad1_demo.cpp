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
  auto shader_mvp = glGetUniformLocation(shaders, "mvp");

  // ----------------------- MODEL
  ObjModel model;
  // if(!LoadOBJModel(GetPathForAsset("chair_simple.obj"), model)){
  //   return 1;
  // }
  if(!LoadCuboidModel(40,20,20,model)){
    return 1;
  }

  // model vertices vbo
  GLuint vertex_buffer;
  const auto vertex_buffer_index=1;
  glGenBuffers(vertex_buffer_index, &vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, model.vertices.size() * sizeof(glm::vec3), &model.vertices[0], GL_STATIC_DRAW);


  // mvp
  glm::mat4 ViewMatrix;
  glm::mat4 ProjectionMatrix = camera.GetProjectionMatrix();
  glm::mat4 ModelMatrix = glm::mat4(1.0);
  glm::mat4 MVP;

  // ---------------------- GL DRAW LOOP

  do{

    yaw = pitch = roll = 0;
    // controls && mvp
    controls.GetMovementFromInputs(translation, yaw, pitch, roll);
    camera.TranslateCamera(translation);
    camera.RotateCamera(yaw, pitch, roll);

    ViewMatrix = camera.GetViewMatrix();
    MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaders);
    glUniformMatrix4fv(shader_mvp, 1, GL_FALSE, &MVP[0][0]);

    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glVertexAttribPointer(
        0,                  // attribute
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );

    // Draw the triangle !
    glDrawArrays(GL_TRIANGLES, 0, model.vertices.size() );

    glDisableVertexAttribArray(0);

    // Swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();

  } // Check if the ESC key was pressed or the window was closed
  while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
    glfwWindowShouldClose(window) == 0 );


  // --------------------- CLEANUP

  glDeleteBuffers(vertex_buffer_index, &vertex_buffer);
  glDeleteProgram(shaders);
  glDeleteVertexArrays(1, &VertexArrayID);

  // Close OpenGL window and terminate GLFW
  glfwTerminate();

  return 0;
}
