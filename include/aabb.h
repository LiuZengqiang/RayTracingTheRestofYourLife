/**
 * @file aabb.h
 * @author Liuzengqiang (12021032@zju.edu.cn)
 * @brief aabb 包围盒 类
 * @version 0.1
 * @date 2023-09-09
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef AABB_H
#define AABB_H

#include "rtweekend.h"

/**
 * @brief AABB 包围盒类
 *
 */
class aabb {
 public:
  // AABBA包围盒在x,y,z三个维度上的间距
  interval x, y, z;

  aabb() {}  // The default AABB is empty, since intervals are empty by default.

  aabb(const interval& ix, const interval& iy, const interval& iz)
      : x(ix), y(iy), z(iz) {}
  // 使用两个点构造三维包围盒
  aabb(const point3& a, const point3& b) {
    // Treat the two points a and b as extrema for the bounding box, so we don't
    // require a particular minimum/maximum coordinate order.
    x = interval(fmin(a[0], b[0]), fmax(a[0], b[0]));
    y = interval(fmin(a[1], b[1]), fmax(a[1], b[1]));
    z = interval(fmin(a[2], b[2]), fmax(a[2], b[2]));
  }
  // 使用两个 AABB包围盒 构造
  aabb(const aabb& box0, const aabb& box1) {
    x = interval(box0.x, box1.x);
    y = interval(box0.y, box1.y);
    z = interval(box0.z, box1.z);
  }
  aabb pad() {
    // Return an AABB that has no side narrower than some delta, padding if
    // necessary.
    double delta = 0.0001;
    interval new_x = (x.size() >= delta) ? x : x.expand(delta);
    interval new_y = (y.size() >= delta) ? y : y.expand(delta);
    interval new_z = (z.size() >= delta) ? z : z.expand(delta);
    return aabb(new_x, new_y, new_z);
  }
  // 取出包围盒的某一维的 interval
  const interval& axis(int n) const {
    if (n == 1) return y;
    if (n == 2) return z;
    return x;
  }
  // 计算光线是否与包围盒相交，如果相交返回true并把相交范围存储在 ray_t 中
  bool hit(const ray& r, interval ray_t) const {
    for (int a = 0; a < 3; a++) {
      // 计算第 a 维度的交点 t0,t1
      auto invD = 1.0 / r.direction()[a];
      auto orig = r.origin()[a];

      auto t0 = (axis(a).min - orig) * invD;
      auto t1 = (axis(a).max - orig) * invD;
      if (invD < 0) {
        std::swap(t0, t1);
      }

      // 更新相交区间
      if (t0 > ray_t.min) {
        ray_t.min = t0;
      }
      if (t1 < ray_t.max) {
        ray_t.max = t1;
      }
      // 如果相交区间不合法，说明光线 r 与 interval 不相交
      if (ray_t.max <= ray_t.min) return false;
    }
    return true;
  }
};

// aabb包围盒平移运算
aabb operator+(const aabb& bbox, const vec3& offset) {
  return aabb(bbox.x + offset.x(), bbox.y + offset.y(), bbox.z + offset.z());
}
aabb operator+(const vec3& offset, const aabb& bbox) { return bbox + offset; }
#endif