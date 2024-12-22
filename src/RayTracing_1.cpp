#include "../include/Ray.hpp"
#include <cstdint>
#include <fstream>
#include <iostream>

#define COLOR_RGBA(r, g, b, a)                                                 \
  ((((uint32_t)((r) * 255.0f)) & 0xFF) << 24 |                                 \
   (((uint32_t)((g) * 255.0f)) & 0xFF) << 16 |                                 \
   (((uint32_t)((b) * 255.0f)) & 0xFF) << 8 |                                  \
   (((uint32_t)((a) * 255.0f)) & 0xFF))
/* #define FLOAT_TO_U32(R, G, B) \ */
/*   ((uint32_t)R * 255.9) << 24) | ((uint32_t)G * 255.9) \ */
/*                                        << 16((uint32_t)(B * 255.9)) << 8) */

glm::vec3 color(const Ray &r);
bool hit_sphere(const Ray &ray, glm::vec3 center, float r);

const char *file_name = "output/test.ppm";
const int WIDTH = 640;
const int HEIGHT = 320;

typedef uint32_t Pixel32;

static Pixel32 IMAGE[WIDTH][HEIGHT];
void save_to_ppm(const char *file_name);
int main(void) {

  glm::vec3 origin(0.0f, 0.0f, 0.0f);
  glm::vec3 H(4.0f, 0.0f, 0.0f); // horizontal direction vector
  glm::vec3 V(0.0f, 2.0f, 0.0f);
  glm::vec3 LeftCorner(-2.0f, -1.0f, -1.0f);
  for (int y = HEIGHT - 1; y >= 0; y--) {
    for (int x = 0; x < WIDTH; x++) {

      float u = (float)(x) / (float)(WIDTH);
      float v = (float)(y) / (float)(HEIGHT);
      Ray ray(origin, LeftCorner + u * H + v * V);
      glm::vec3 col = color(ray);
      IMAGE[x][HEIGHT - 1 - y] = COLOR_RGBA(col.r, col.g, col.b, 1.0);
    }
  }
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
glm::vec3 color(const Ray &r) {

  if(hit_sphere(r, glm::vec3(0.0f, 0.0f, -1.0), 0.5))
  {
    return glm::vec3(1.0f, 0.0f, 0.0f);
  }
  float y = glm::normalize(r.direction()).y;
  float t = 0.5f * (y + 1.0f);
  return glm::vec3(1.0f, 1.0f, 1.0f) * (1.0f - t) +
         (t) * (glm::vec3(0.5f, 0.7f, 1.0f));
}
bool hit_sphere(const Ray &ray, glm::vec3 center, float r) {
  glm::vec3 oc = ray.origin() - center;
  float a = dot(ray.direction(), ray.direction());

  float b = 2 * dot(ray.direction(), oc);
  float c = dot(oc, oc) - r * r;

  return (b * b - 4.0 * a * c) > 0;
}
