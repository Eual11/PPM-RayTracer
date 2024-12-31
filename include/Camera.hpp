#ifndef CAMERA_HPP
#define CAMERA_HPP
#include "Ray.hpp"
#include "Utils.hpp"
#include <cmath>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
class Camera {

public:
  Camera(glm::vec3 pos, glm::vec3 look_at, glm::vec3 up, float fov,
         float aspect) {

    // camera position

    glm::vec3 u, v, w;
    origin = pos;

    w = glm::normalize(pos - look_at);
    u = glm::normalize(glm::cross(up, w));
    v = (glm::cross(w, u));

    float theta = (fov * PI) / 180.0f;
    float half_h = tan(theta / 2.0f);

    float half_w = aspect * half_h;

    H = 2.0f * half_w * u; // horizontal direction vector
    V = 2.0f * half_h * v;

    LeftCorner = origin - u * half_w - v * half_h - w;
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
