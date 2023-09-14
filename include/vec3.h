/**
 * @file vec3.h
 * @author Liuzengqiang (12021032@zju.edu.cn)
 * @brief
 * @version 0.1
 * @date 2023-09-06
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef VEC3_H
#define VEC3_H
#include <cmath>
#include <iostream>

using std::sqrt;
/**
 * @brief 三维向量类
 *
 */
class vec3 {
 public:
  double e[3];

  vec3() : e{0, 0, 0} {}
  vec3(double e0, double e1, double e2) : e{e0, e1, e2} {}

  double x() const { return e[0]; }
  double y() const { return e[1]; }
  double z() const { return e[2]; }

  vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
  // 常量成员重载函数，假如对象为 vec3 const temp_vec3，则temp_vec3[0]调用该函数
  double operator[](int i) const { return e[i]; }
  // 假如对象为 vec3 temp_vec3，则temp_vec3[0]调用该函数
  double &operator[](int i) { return e[i]; }

  vec3 &operator+=(const vec3 &v) {
    e[0] += v.e[0];
    e[1] += v.e[1];
    e[2] += v.e[2];
    return *this;
  }

  vec3 &operator*=(double t) {
    e[0] *= t;
    e[1] *= t;
    e[2] *= t;
    return *this;
  }

  vec3 &operator/=(double t) { return *this *= 1 / t; }

  double length() const { return sqrt(length_squared()); }

  double length_squared() const {
    return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
  }
  // 随机采样 vec3
  static vec3 random() {
    return vec3(random_double(), random_double(), random_double());
  }
  // 随机采样 vec3(min, max)
  static vec3 random(double min, double max) {
    return vec3(random_double(min, max), random_double(min, max),
                random_double(min, max));
  }
  // 判断向量是否接近0, 用于误差判断
  bool near_zero() const {
    auto s = 1e-8;
    return fabs(e[0] < s) && fabs(e[1] < s) && fabs(e[2] < s);
  }
};
// point3 is just an alias for vec3, but useful for geometric clarity in the
// code.
using point3 = vec3;

// Vector Utility Functions

inline std::ostream &operator<<(std::ostream &out, const vec3 &v) {
  return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline vec3 operator+(const vec3 &u, const vec3 &v) {
  return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline vec3 operator-(const vec3 &u, const vec3 &v) {
  return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline vec3 operator*(const vec3 &u, const vec3 &v) {
  return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline vec3 operator*(double t, const vec3 &v) {
  return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

inline vec3 operator*(const vec3 &v, double t) { return t * v; }

inline vec3 operator/(vec3 v, double t) { return (1 / t) * v; }

inline double dot(const vec3 &u, const vec3 &v) {
  return u.e[0] * v.e[0] + u.e[1] * v.e[1] + u.e[2] * v.e[2];
}

inline vec3 cross(const vec3 &u, const vec3 &v) {
  return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
              u.e[2] * v.e[0] - u.e[0] * v.e[2],
              u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

// 正则化一个向量
inline vec3 unit_vector(vec3 v) { return v / v.length(); }

// 在一个单位球内采样一个点
inline vec3 random_in_unit_sphere() {
  while (true) {
    auto p = vec3::random(-1, 1);
    if (p.length_squared() < 1) return p;
  }
}
// 在一个单位球面上采样一个点
inline vec3 random_unit_vector() {
  return unit_vector(random_in_unit_sphere());
}
// 在 normal 表示的半球内部采样得到一个单位向量
inline vec3 random_on_hemisphere(const vec3 &normal) {
  vec3 on_unit_sphere = random_unit_vector();
  if (dot(normal, on_unit_sphere) >= 0.0) {
    return on_unit_sphere;
  } else {
    return -on_unit_sphere;
  }
}

// 在圆盘内均匀采样
inline vec3 random_in_unit_disk() {
  while (true) {
    auto p = vec3(random_double(-1, 1), random_double(-1, 1), 0);
    if (p.length_squared() < 1) {
      return p;
    }
  }
}
/**
 * @brief 计算理想的反射光线
 *
 * @param v 入射光
 * @param n 法向
 * @return vec3
 */
inline vec3 reflect(const vec3 &v, const vec3 &n) {
  return v - 2 * dot(v, n) * n;
}
/**
 * @brief 计算折射的折射光线
 *
 * @param v 入射光线
 * @param n 法向
 * @param etai_over_etat 入射介质折射率/出射介质折射率
 * @return vec3
 */
inline vec3 refract(const vec3 &v, const vec3 &n, double etai_over_etat) {
  double cos_theta = fmin(1.0, dot(-v, n));
  vec3 r_out_perp = etai_over_etat * (v + cos_theta * n);
  vec3 r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.length_squared())) * n;
  return r_out_perp + r_out_parallel;
}
#endif