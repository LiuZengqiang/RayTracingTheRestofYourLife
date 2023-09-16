/**
 * @file ray.h
 * @author Liuzengqiang (12021032@zju.edu.cn)
 * @brief
 * @version 0.1
 * @date 2023-09-06
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef RAY_H
#define RAY_H

#include "vec3.h"
/**
 * @brief 光线类
 *
 */
class ray {
 public:
  ray() {}

  ray(const point3& origin, const vec3& direction, double time = 0.0)
      : orig(origin), dir(direction), tm(time) {}

  // 取 ray 的原点
  point3 origin() const { return orig; }
  // 取 ray 的方向
  vec3 direction() const { return dir; }
  // 取 ray 在t位置处的坐标
  point3 at(double t) const { return orig + t * dir; }
  // 取 ray 发出的时刻
  double time() const { return tm; }

 private:
  point3 orig;  // 光线起点
  vec3 dir;     // 光线方向
  double tm;    // 光线的时刻
};

#endif