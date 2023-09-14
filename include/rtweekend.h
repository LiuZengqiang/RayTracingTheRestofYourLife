/**
 * @file rtweekend.h
 * @author Liuzengqiang (12021032@zju.edu.cn)
 * @brief 包含常用的常量, 命名空间, 函数, 头文件
 * @version 0.1
 * @date 2023-09-06
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include <cmath>
#include <limits>
#include <memory>
#include <random>

// Usings
// 命名空间
using std::make_shared;
using std::shared_ptr;
using std::sqrt;

// Constants
// 常数
const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Functions
// 函数
inline double degrees_to_radians(double degrees) {
  return degrees * pi / 180.0;
}

inline double random_double() {
  static std::uniform_real_distribution<double> distribution(0.0, 1.0);
  static std::mt19937 generator;
  return distribution(generator);
}

inline double random_double(double min, double max) {
  return min + (max - min) * random_double();
}
inline int random_int(int min, int max) {
  // Returns a random integer in [min,max].
  return static_cast<int>(random_double(min, max + 1));
}

// Common Headers
// #include "interval.h" 必须放在这个位置，因为 interval.h 需要 infinity 常量
#include "interval.h"
#include "ray.h"
#include "vec3.h"

#endif