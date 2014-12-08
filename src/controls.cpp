#include "controls.hpp"
#include <GLFW/glfw3.h>


Controls::Controls(GLFWwindow* window, int screenWidth, int screenHeight){
  _screenWidth = screenWidth;
  _screenHeight = screenHeight;
  _window = window;
}

void Controls::GetMovementFromInputs(glm::vec3 & translation,
                                     float & yaw,
                                     float & pitch,
                                     float & roll){
  auto currentTime = glfwGetTime();
  if(_lastTime==0){
    _lastTime = glfwGetTime();
  }
  auto dt = float(currentTime - _lastTime);

  double xpos, ypos;
  glfwGetCursorPos(_window, &xpos, &ypos);
  glfwSetCursorPos(_window, _screenWidth/2, _screenHeight/2);
  pitch = mouseSpeed * float(_screenWidth/2 - xpos );
  yaw = mouseSpeed * float(_screenHeight/2 - ypos );
  roll = 0;

  if (glfwGetKey(_window, GLFW_KEY_W ) == GLFW_PRESS){
    translation.z = dt * translationSpeed;
  }
  else if (glfwGetKey(_window, GLFW_KEY_S ) == GLFW_PRESS){
    translation.z = - dt * translationSpeed;
  }
  else{
    translation.z = 0;
  }
  if (glfwGetKey(_window, GLFW_KEY_D ) == GLFW_PRESS){
    translation.x = dt * translationSpeed;
  }
  else if (glfwGetKey(_window, GLFW_KEY_A ) == GLFW_PRESS){
    translation.x = - dt * translationSpeed;
  }
  else{
    translation.x = 0;
  }
  if (glfwGetKey(_window, GLFW_KEY_R ) == GLFW_PRESS){
    translation.y = dt * translationSpeed;
  }
  else if (glfwGetKey(_window, GLFW_KEY_F ) == GLFW_PRESS){
    translation.y = - dt * translationSpeed;
  }
  else{
    translation.y = 0;
  }
  if (glfwGetKey(_window, GLFW_KEY_E ) == GLFW_PRESS){
    roll = dt * rollSpeed;
  }
  else if (glfwGetKey(_window, GLFW_KEY_Q ) == GLFW_PRESS){
    roll = - dt * rollSpeed;
  }
  else{
    roll = 0;
  }

  _lastTime = currentTime;
}
