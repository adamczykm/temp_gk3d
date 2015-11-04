#include <glm/glm.hpp>
#include <GLFW/glfw3.h>


class Controls{
  GLFWwindow* _window;
  double _lastTime = 0.0f;
  int _screenWidth;
  int _screenHeight;
public:
  Controls(GLFWwindow* window, int screenWidth, int screenHeight);
  ~Controls(){}
  float translationSpeed = 15.0f;
  float mouseSpeed = 0.03f;
  float rollSpeed = 5.0f;

  void GetMovementFromInputs(glm::vec3 & translation,
                             float & yaw,
                             float & pitch,
                             float & roll);
};
