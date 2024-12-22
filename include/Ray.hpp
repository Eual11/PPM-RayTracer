#ifndef RAY_H
#define RAY_H
#include <glm/common.hpp>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
class Ray {
public:
  Ray() = default;

  Ray(const glm::vec3 &origin, const glm::vec3 &direction) {

    A = origin;
    B = direction;
  }

  glm::vec3 origin() const { return A; }
  glm::vec3 direction() const { return B; }

  glm::vec3 point_at_parmt(float t) { return A + t * B; }

  glm::vec3 A; // vector origin
  glm::vec3 B; // vector origin
};
#endif
