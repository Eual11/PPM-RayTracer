#ifndef SPHERE_H
#define SPHERE_H
#include "Hitable.hpp"
#include <glm/geometric.hpp>
class Sphere : public Hitable {
public:
  glm::vec3 center;
  float radius;
  Material *material = nullptr;

  Sphere(const glm::vec3 &cen, float r, Material *mat)
      : center(cen), radius(r), material(mat) {}
  Sphere()
      : center(0.0f), radius(1.0f) {
  } // Default sphere at the origin with radius 1.0

  bool hit(const Ray &r, float t_min, float t_max,
           hit_record &record) const override {

    glm::vec3 oc = r.origin() - center;
    float a = dot(r.direction(), r.direction());
    float b = 2.0f * dot(r.direction(), oc);
    float c = dot(oc, oc) - radius * radius;

    float discriminant = b * b - 4.0 * a * c;

    if (discriminant < 0.0)
      return false;
    float temp = (-b - sqrt(discriminant)) / (2.0f * a);

    if (temp < t_max && temp > t_min) {
      record.t = temp;
      record.p = r.point_at_parmt(record.t);
      record.normal = (record.p - center) / radius;
      record.mat = material;

      return true;
    }

    temp = (-b + sqrt(discriminant)) / (2.0f * a);

    if (temp < t_max && temp > t_min) {
      record.t = temp;
      record.p = r.point_at_parmt(record.t);
      record.normal = (record.p - center) / radius;
      record.mat = material;
      return true;
    }
    return false;
  }
};
#endif
