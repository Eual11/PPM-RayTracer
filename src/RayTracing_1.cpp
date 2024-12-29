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
#include <random>

#define PI acosf(-1)

#define WORLD_SIZE 4
#define COLOR_RGBA(r, g, b, a)                                                 \
  ((((uint32_t)((r) * 255.0f)) & 0xFF) << 24 |                                 \
   (((uint32_t)((g) * 255.0f)) & 0xFF) << 16 |                                 \
   (((uint32_t)((b) * 255.0f)) & 0xFF) << 8 |                                  \
   (((uint32_t)((a) * 255.0f)) & 0xFF))

glm::vec3 color(const Ray &r, HitableList *world, int depth = 0);
float hit_sphere(const Ray &ray, glm::vec3 center, float r);

const char *file_name = "output/test.ppm";
const int WIDTH = 640;
const int HEIGHT = 320;
const int SAMPLES = 32;

typedef uint32_t Pixel32;

static Pixel32 IMAGE[WIDTH][HEIGHT];
void save_to_ppm(const char *file_name);
int main(void) {
  std::random_device rd; // random number generator
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dist(0.0f, 1.0f);

  auto start = std::chrono::high_resolution_clock::now();

  Hitable *list[WORLD_SIZE] = {nullptr};

  list[0] = new Sphere(glm::vec3(0, 0, -1), 0.5,
                       new Lambertian(glm::vec3(0.2, 0.2, 0.9)));
  list[1] = new Sphere(glm::vec3(0, -100.5, -1), 100,
                       new Lambertian(glm::vec3(0.8, 0.8, 0.0)));
  list[2] =
      new Sphere(glm::vec3(1, 0, -1), 0.5, new Metal(glm::vec3(0.8, 0.6, 0.2)));
  list[3] = new Sphere(glm::vec3(-1, 0, -1), 0.5,
                       new Metal(glm::vec3(0.8, 0.8, 0.8)));
  HitableList *world = new HitableList(list, WORLD_SIZE);

  srand(0);
  Camera cam;
  for (int y = HEIGHT - 1; y >= 0; y--) {
    for (int x = 0; x < WIDTH; x++) {
      glm::vec3 col = glm::vec3(0.0f);

      for (int i = 0; i < SAMPLES; i++) {

        float s = ((float)(x) + dist(gen)) / ((float)(WIDTH));
        float t = ((float)(y) + dist(gen)) / ((float)(HEIGHT));
        col += color(cam.gen_rays(s, t), world, 50);
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
  ;
  save_to_ppm(file_name);

  for (unsigned int i = 0; i < WORLD_SIZE; i++) {
    if (list[i]) {
      free(list[i]);
    }
  }
  std::cout << "memory cleared\n";
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
glm::vec3 color(const Ray &r, HitableList *world, int depth) {

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
