#include "../include/Camera.hpp"
#include "../include/Hitable.hpp"
#include "../include/HitableList.hpp"
#include "../include/Material.hpp"
#include "../include/Ray.hpp"
#include "../include/Sphere.hpp"
#include "../include/Utils.hpp"
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <glm/geometric.hpp>
#include <iostream>

#define COLOR_RGBA(r, g, b, a)                                                 \
  ((((uint32_t)((r) * 255.0f)) & 0xFF) << 24 |                                 \
   (((uint32_t)((g) * 255.0f)) & 0xFF) << 16 |                                 \
   (((uint32_t)((b) * 255.0f)) & 0xFF) << 8 |                                  \
   (((uint32_t)((a) * 255.0f)) & 0xFF))

glm::vec3 color(const Ray &r, Hitable *world, int depth = 0);
float hit_sphere(const Ray &ray, glm::vec3 center, float r);

const char *file_name = "output/test.ppm";
const int WIDTH = 1280;
const int HEIGHT = 720;

typedef uint32_t Pixel32;

static Pixel32 IMAGE[WIDTH][HEIGHT];
void save_to_ppm(const char *file_name);
Hitable *random_scene();
int main(void) {

  int depth;
  int SAMPLES;
  std::cout << "Depth: ";
  std::cin >> depth;
  std::cout << "Samples: ";
  std::cin >> SAMPLES;

  auto start = std::chrono::high_resolution_clock::now();

  Hitable *world = random_scene();

  Camera cam(glm::vec3(13.0f, 2.0f, 3), glm::vec3(0.0f, 0.0f, 0.0f),
             glm::vec3(0.0f, 1.0f, 0.0f), 20.0f, float(WIDTH) / float(HEIGHT));
  for (int y = HEIGHT - 1; y >= 0; y--) {
    for (int x = 0; x < WIDTH; x++) {
      glm::vec3 col = glm::vec3(0.0f);

      for (int i = 0; i < SAMPLES; i++) {

        float s = ((float)(x) + rng.random_float()) / ((float)(WIDTH));
        float t = ((float)(y) + rng.random_float()) / ((float)(HEIGHT));
        col += color(cam.gen_rays(s, t), world, depth);
      }
      col /= SAMPLES;

      // gamma encoding
      col = glm::vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
      IMAGE[x][HEIGHT - 1 - y] = COLOR_RGBA(col.r, col.g, col.b, 1.0);
    }
  }
  auto elapsed = std::chrono::high_resolution_clock::now() - start;

  std::cout << "Time to Render: "
            << std::chrono::duration<float>(elapsed).count() << std::endl;
  save_to_ppm(file_name);

  return 0;
}
void save_to_ppm(const char *file_name) {
  std::fstream out_image =
      std::fstream(file_name, std::ios::binary | std::ios::out);

  if (!out_image.is_open()) {
    std::cerr << "Failed to open file\n";

    exit(1);
  } else {
    out_image << "P6\n" << WIDTH << " " << HEIGHT << "\n" << 0xFF << "\n";

    for (int y = 0; y < HEIGHT; y++) {
      for (int x = 0; x < WIDTH; x++) {

        Pixel32 pixel = IMAGE[x][y];
        // AABBGGRR
        uint8_t byte[3] = {static_cast<uint8_t>((pixel & 0xFF000000) >> 24),
                           static_cast<uint8_t>((pixel & 0x00FF0000) >> 16),
                           static_cast<uint8_t>((pixel & 0x0000FF00) >> 8)};
        out_image.write(reinterpret_cast<const char *>(byte), sizeof(byte));
      }
    }
    std::cout << "File " << file_name << " Saved\n";
  }
}
glm::vec3 color(const Ray &r, Hitable *world, int depth) {

  hit_record record;
  float t;
  if (world->hit(r, 0.001f, FLT_MAX, record)) {
    glm::vec3 attenuation;
    Ray scattered;

    if (depth > 0 && record.mat &&
        record.mat->scatter(r, record, attenuation, scattered)) {
      return attenuation * color(scattered, world, depth - 1);
    } else
      return glm::vec3(0.0f);
  }

  float y = glm::normalize(r.direction()).y;
  t = 0.5f * (y + 1.0f);
  return glm::vec3(1.0f, 1.0f, 1.0f) * (1.0f - t) +
         (t) * (glm::vec3(0.5f, 0.7f, 1.0f));
}
float hit_sphere(const Ray &ray, glm::vec3 center, float r) {
  glm::vec3 oc = ray.origin() - center;
  float a = dot(ray.direction(), ray.direction());

  float b = 2 * dot(ray.direction(), oc);
  float c = dot(oc, oc) - r * r;

  float discriminant = b * b - 4.0f * a * c;

  if (discriminant < 0)
    return -1.0f;
  return (-b - sqrt(discriminant)) / (2.0f * a);
}
Hitable *random_scene() {
  using namespace glm;
  int n = 500;
  Hitable **list = new Hitable *[n + 1];
  list[0] =
      new Sphere(vec3(0, -1000, 0), 1000, new Lambertian(vec3(0.5, 0.5, 0.5)));
  int i = 1;
  for (int a = -11; a < 11; a++) {
    for (int b = -11; b < 11; b++) {
      float choose_mat = rng.random_float();
      vec3 center(a + 0.9 * rng.random_float(), 0.2, b + 0.9 * random_double());
      if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
        if (choose_mat < 0.8) { // diffuse
          list[i++] = new Sphere(
              center, 0.2,
              new Lambertian(vec3(random_double() * random_double(),
                                  random_double() * random_double(),
                                  random_double() * random_double())));
        } else if (choose_mat < 0.95) { // metal
          list[i++] = new Sphere(center, 0.2, new Metal(vec3(0.5, 0.5, 0.5)));
        } else { // glass
          list[i++] = new Sphere(center, 0.2, new Dielectric(1.5));
        }
      }
    }
  }

  list[i++] = new Sphere(vec3(0, 1, 0), 1.0, new Dielectric(1.5));
  list[i++] =
      new Sphere(vec3(-4, 1, 0), 1.0, new Lambertian(vec3(0.4, 0.2, 0.1)));
  list[i++] =
      new Sphere(vec3(4, 1, 0), 1.0, new Metal(vec3(0.7, 0.6, 0.5), 0.0));

  return new HitableList(list, i);
};
