#ifndef HITABLE_H
#define HITABLE_H
#include "Ray.hpp"

class Material;
struct hit_record {
  float t;
  glm::vec3 p;
  glm::vec3 normal;
  Material *mat;
};

class Hitable {
public:
  virtual bool hit(const Ray &r, float t_min, float t_max,
                   hit_record &record) const = 0;
  virtual ~Hitable() {}
};
#endif
