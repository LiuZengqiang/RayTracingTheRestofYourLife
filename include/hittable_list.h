/**
 * @file hittable_list.h
 * @author Liuzengqiang (12021032@zju.edu.cn)
 * @brief
 * @version 0.1
 * @date 2023-09-06
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include <memory>
#include <vector>

#include "hittable.h"

using std::make_shared;
using std::shared_ptr;

class hittable_list : public hittable {
 public:
  // 物体列表
  std::vector<shared_ptr<hittable>> objects;

  hittable_list() {}
  hittable_list(shared_ptr<hittable> object) { add(object); }

  void clear() { objects.clear(); }

  void add(shared_ptr<hittable> object) {
    objects.push_back(object);
    // 更新 bbox
    bbox = aabb(bbox, object->bounding_box());
  }
  /**
   * @brief 计算光线与物体的交点(第一个交点), 有交点则返回true,
   * 没有交点返回false
   *
   * @param r 入射光线
   * @param ray_t 入射光线的合法传播距离, 通常为 (0.001, infinity)
   * @param rec 如果有交点则将交点信息保存在 rec 中
   * @return true
   * @return false
   */
  bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
    hit_record temp_rec;
    bool hit_anything = false;
    auto closest_so_far = ray_t.max;

    for (const auto& object : objects) {
      if (object->hit(r, interval(ray_t.min, closest_so_far), temp_rec)) {
        hit_anything = true;
        closest_so_far = temp_rec.t;
        rec = temp_rec;
      }
    }
    return hit_anything;
  }
  aabb bounding_box() const override { return bbox; }

 private:
  aabb bbox;
};

#endif