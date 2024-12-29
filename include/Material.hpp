#ifndef __MATERIAL_HPP
#define __MATERIAL_HPP

#include "Hitable.hpp"
#include "Ray.hpp"
#include "Utils.hpp"
#include <glm/geometric.hpp>
class Material {
public:
  virtual bool scatter(const Ray &r_in, const hit_record &rec,
                       glm::vec3 &attenuation, Ray &scattered) const = 0;

  virtual ~Material() {};
};

class Lambertian : public Material {

public:
  Lambertian(const glm::vec3 &a) : albedo(a) {};

  bool scatter(const Ray &r_in, const hit_record &rec, glm::vec3 &attenuation,
               Ray &scattered) const override {

    glm::vec3 target = rec.normal + rec.p + random_point_in_unit_sphere();

    scattered = Ray(rec.p, target - rec.p);
    attenuation = albedo;
    return true;
  }

public:
  glm::vec3 albedo;
};

class Metal : public Material {

public:
  Metal(const glm::vec3 &a, float f = 0.0) : albedo(a) {

    if (f < 1.0f) {
      fuzz = f;
    } else
      fuzz = 1.0f;
  };
  glm::vec3 albedo;
  float fuzz; // to randoize reflection
  bool scatter(const Ray &r_in, const hit_record &rec, glm::vec3 &attenuation,
               Ray &scattered) const override {

    glm::vec3 reflected = reflect(glm::normalize(r_in.direction()), rec.normal);
    scattered = Ray(rec.p, reflected + fuzz * random_point_in_unit_sphere());
    attenuation = albedo;

    return dot(scattered.direction(), rec.normal) > 0.0;
  }
};
#endif
