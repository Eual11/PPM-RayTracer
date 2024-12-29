#ifndef __UTILS_HPP
#define __UTILS_HPP

#include <cmath>
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>
#include <random>
std::random_device rd; // random number generator
std::mt19937 gen(rd());
std::uniform_int_distribution<> dist(0.0f, 1.0f);

glm::vec3 random_point_in_unit_sphere() {

  float u = dist(gen);
  float v = dist(gen);

  // max(rand(), rand(), rand()) = cbrt(rand())
  float r = std::cbrt(dist(gen));

  float theta = acos(1 - 2 * u); // 0-Pi
  float phi = 2 * M_PI * v;

  float rtheta = r * sin(theta);

  return glm::vec3(rtheta * cos(phi), rtheta * sin(phi), r * cos(theta));
}

glm::vec3 reflect(const glm::vec3 &v, const glm::vec3 &n) {
  return v - 2 * glm::dot(n, v) * n;
}

#endif
