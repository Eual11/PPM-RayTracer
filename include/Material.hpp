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
    (void)(r_in);
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

class Dielectric : public Material {

public:
  Dielectric(float f) : ref_index(f) {}
  bool scatter(const Ray &r_in, const hit_record &rec, glm::vec3 &attenuation,
               Ray &scattered) const override {

    glm::vec3 r_dir = glm::normalize(r_in.direction());

    glm::vec3 out_normal;
    glm::vec3 refracted = glm::vec3(0.0f);
    float theta;
    float eta;
    float reflect_prop;

    // refleced ray
    glm::vec3 reflected = reflect(r_dir, rec.normal);

    attenuation = glm::vec3(1.0f);

    // checking for incident ray's direction

    if (glm::dot(r_dir, rec.normal) > 0.0f) {
      // ray is from the inside the surface of object object

      out_normal = -rec.normal;
      eta = ref_index;
      theta = (glm::dot(r_in.direction(), rec.normal) * ref_index) /
              glm::length(r_in.direction());
    } else {
      out_normal = rec.normal;
      eta = 1.0f / ref_index;
      theta = -(glm::dot(r_in.direction(), rec.normal)) /
              glm::length(glm::length(r_in.direction()));
    }

    if (refract(r_dir, out_normal, eta, refracted)) {

      reflect_prop = schlick(theta, ref_index);
    } else {
      reflect_prop = 1.0f;
    }

    if (rng.random_float() < reflect_prop) {
      scattered = Ray(rec.p, reflected);
    } else {
      scattered = Ray(rec.p, refracted);
    }

    return true;
  }

  float ref_index;
};
float random_double() { return rng.random_float(); }

#endif
