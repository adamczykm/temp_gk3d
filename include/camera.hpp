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

  void TranslateCamera(const glm::vec3 & translation);
  void RotateCamera(float yaw, float pitch);
  void RotateCamera(float yaw, float pitch, float roll);
  void RollCamera(float roll);

};
