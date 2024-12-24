#ifndef HITABLE_LIST_H
#define HITABLE_LIST_H

#include "Hitable.hpp"
#include <iostream>
class HitableList : public Hitable {

public:
  HitableList(Hitable **l, unsigned int n) : list(l), len(n) {}
  Hitable **list = nullptr;
  unsigned int len = 0;
  bool hit(const Ray &r, float t_min, float t_max,
           hit_record &record) const override {

    if (!list)
      return false;

    bool hit_anything = false;

    hit_record tmp_record;

    double closet_hit = t_max;

    for (unsigned int i = 0; i < len; i++) {
      Hitable *hitable = list[i];

      if (hitable) {
        if (hitable->hit(r, t_min, closet_hit, tmp_record)) {

          closet_hit = tmp_record.t;
          hit_anything = true;
          record = tmp_record;
        }
      }
    }

    return hit_anything;
  }
};
#endif
