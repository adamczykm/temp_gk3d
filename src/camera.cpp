#include "camera.hpp"
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

Camera::Camera(){
  this->ResetCamera();
}

void Camera::ResetCamera(){
  _rot = quat(1,0,0,0);
  _pos = vec3( 0, 0, 45. );

  _eye = vec3( 0, 0, -1);
  _up = vec3(0,1,0);
  _right = vec3(1,0,0);

  _projectionMatrix = perspective(50.0f, 16.0f / 9.0f, 0.1f, 500.0f);
}

void Camera::TranslateCamera(vec3 translation){
  _pos += translation.z * _eye
          + translation.x * _right
          + translation.y * _up;
}

void Camera::RotateCamera(float yaw, float pitch, float roll){
  _rot = cross(quat(1,0,0,0), angleAxis(yaw, _right));
  _rot = cross(_rot, angleAxis(pitch, _up));
  _rot = cross(_rot, angleAxis(roll, _eye));

  _rot = normalize(_rot);

  _right = _rot * _right; //rotate(_rot, _right);
  _eye = _rot * _eye; // rotate(_rot, _eye);
  _up = _rot * _up; //rotate(_rot, _up);

}

mat4 Camera::GetViewMatrix(){

  return lookAt(_pos, _pos + _eye, _up);
}
