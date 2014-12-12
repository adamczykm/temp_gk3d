#include "camera.hpp"
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

Camera::Camera(){
  this->ResetCamera();
}

void Camera::ResetCamera(){
  _rot = quat(1,0,0,0);
  _pos = vec3( 0, 0, 20. );

  _eye = vec3( 0, 0, -1);
  _up = vec3(0,1,0);
  _right = vec3(1,0,0);

  _projectionMatrix = perspective(45.0f, 16.0f / 9.0f, 0.1f, 5000.0f);
}

void Camera::TranslateCamera(const vec3 & translation){
  _pos += translation.z * _eye
    + translation.x * _right
    + translation.y * cross(_right, _eye);
}

void Camera::RollCamera(float roll){
  _rot = cross(angleAxis(roll, _eye),quat(1,0,0,0));
  _rot = normalize(_rot);
  _up = _rot * _up; //rotate(_rot, _up);
  _right = normalize(cross(_eye,_up));
}

void Camera::RotateCamera(float yaw, float pitch){
  _rot = normalize(cross(quat(1,0,0,0), angleAxis(pitch, _up)));
  _rot = normalize(cross(_rot, angleAxis(yaw, _right)));

  _right = _rot * _right; //rotate(_rot, _right);
  _eye = _rot * _eye; // rotate(_rot, _eye);
}

mat4 Camera::GetViewMatrix(){

  return lookAt(_pos, _pos + _eye, _up);
}
