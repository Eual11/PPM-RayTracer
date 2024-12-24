#ifndef CAMERA_HPP
#define CAMERA_HPP
#include "Ray.hpp"
#include <glm/glm.hpp>
#include <glm/vec3.hpp>

class Camera {

public:
  Camera() {

    origin = glm::vec3(0.0f, 0.0f, 0.0f);
    H = glm::vec3(4.0f, 0.0f, 0.0f); // horizontal direction vector
    V = glm::vec3(0.0f, 2.0f, 0.0f);
    LeftCorner = glm::vec3(-2.0f, -1.0f, -1.0f);
  }
  glm::vec3 origin;
  glm::vec3 H; // horizontal direction vector
  glm::vec3 V;
  glm::vec3 LeftCorner;

  Ray gen_rays(float u, float v) const {
    return Ray(origin, LeftCorner + u * H + v * V - origin);
  }
};
#endif
