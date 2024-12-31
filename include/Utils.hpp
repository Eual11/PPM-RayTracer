#ifndef __UTILS_HPP
#define __UTILS_HPP

#include <cmath>
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>
#include <random>
#define PI acosf(-1)

struct RandomGenerator {
  std::mt19937 gen;
  std::uniform_real_distribution<float> dist;

  RandomGenerator() : gen(std::random_device{}()), dist(0.0f, 1.0f) {}

  float random_float() { return dist(gen); }

  glm::vec3 random_color() {
    return glm::vec3(random_float(), random_float(), random_float());
  }
};

// Singleton instance of RNG
inline RandomGenerator rng;

glm::vec3 random_point_in_unit_sphere() {
  float u = rng.random_float();
  float v = rng.random_float();
  float r = std::cbrt(rng.random_float());

  float theta = acos(1.0f - 2.0f * u); // 0 to Pi
  float phi = 2.0f * PI * v;

  float rtheta = r * sin(theta);

  return glm::vec3(rtheta * cos(phi), rtheta * sin(phi), r * cos(theta));
}

glm::vec3 reflect(const glm::vec3 &v, const glm::vec3 &n) {
  return v - 2.0f * glm::dot(n, v) * n;
}

bool refract(const glm::vec3 &v_in, const glm::vec3 &n, float eta,
             glm::vec3 &refracted) {
  float dt = glm::dot(v_in, n);
  float k = 1.0f - eta * eta * (1.0f - dt * dt);

  if (k < 0.0f) {
    return false;
  }

  refracted = eta * v_in - (eta * dt + sqrtf(k)) * n;
  return true;
}

float schlick(float theta, float eta) {
  float r0 = (1.0f - eta) / (1.0f + eta);
  r0 *= r0;
  return r0 + (1.0f - r0) * powf(1.0f - theta, 5);
}

#endif // __UTILS_HPP
