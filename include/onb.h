/**
 * @file onb.h
 * @author Liuzengqiang (12021032@zju.edu.cn)
 * @brief 基础的坐标类
 * @version 0.1
 * @date 2023-09-16
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef ONB_H
#define ONB_H

#include "rtweekend.h"
/**
 * @brief Orthonormal basis class, 标准正交类
 *
 */
class onb {
 public:
  onb() {}

  vec3 operator[](int i) const { return axis[i]; }
  vec3& operator[](int i) { return axis[i]; }

  // 三个基坐标向量
  vec3 u() const { return axis[0]; }
  vec3 v() const { return axis[1]; }
  vec3 w() const { return axis[2]; }

  // 局部坐标下的位置
  vec3 local(double a, double b, double c) const {
    return a * u() + b * v() + c * w();
  }

  // 将局部坐标转为世界坐标
  vec3 local(const vec3& a) const {
    return a.x() * u() + a.y() * v() + a.z() * w();
  }
  // 根据世界坐标下的 w(normal), 建立局部坐标 (u,v,w)
  // 在局部坐标下默认 w 为 z
  void build_from_w(const vec3& w) {
    vec3 unit_w = unit_vector(w);
    vec3 a = (fabs(unit_w.x()) > 0.9) ? vec3(0, 1, 0) : vec3(1, 0, 0);
    vec3 v = unit_vector(cross(unit_w, a));
    vec3 u = cross(unit_w, v);
    axis[0] = u;
    axis[1] = v;
    axis[2] = unit_w;
  }

 public:
  vec3 axis[3];  // 坐标的三个基坐标, 基坐标需要相互正交且长度为1
};

#endif