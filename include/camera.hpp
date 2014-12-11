#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <stdio.h>

class Camera{
  glm::vec3 _pos;
  glm::quat _rot;

  glm::vec3 _up;
  glm::vec3 _right;
  glm::vec3 _eye;

  glm::mat4 _projectionMatrix;
public:
  Camera();
  ~Camera(){}

  void ResetCamera();
  glm::mat4 GetViewMatrix();
  glm::mat4 GetProjectionMatrix() const{
    return _projectionMatrix;
  }

  void PrintUp(){
    printf("%f %f %f",_up.x, _up.y, _up.z);
  }
  void TranslateCamera(glm::vec3 translation);
  void RotateCamera(float yaw, float pitch);
  void RollCamera(float roll);

};
