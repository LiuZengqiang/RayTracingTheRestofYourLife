/**
 * @file color.h
 * @author Liuzengqiang (12021032@zju.edu.cn)
 * @brief
 * @version 0.1
 * @date 2023-09-06
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef COLOR_H
#define COLOR_H

#include <iostream>

#include "rtweekend.h"
#include "vec3.h"

using color = vec3;
/**
 * @brief gamma 矫正
 *
 * @param linear_component
 * @return double
 */
inline double linear_to_gamma(double linear_component) {
  return sqrt(linear_component);
}

/**
 * @brief 写入像素值
 *
 * @param out 输出流
 * @param pixel_color 像素颜色
 * @param samples_per_pixel 该像素内的采样光线数
 */
void write_color(std::ostream &out, color pixel_color, int samples_per_pixel) {
  // Write the translated [0,255] value of each color component.

  auto r = pixel_color.x();
  auto g = pixel_color.y();
  auto b = pixel_color.z();

  auto scale = 1.0 / samples_per_pixel;
  r *= scale;
  g *= scale;
  b *= scale;
  // gamma 矫正
  r = linear_to_gamma(r);
  g = linear_to_gamma(g);
  b = linear_to_gamma(b);

  static const interval intensity(0.0, 0.999);

  out << static_cast<int>(255.999 * intensity.clamp(r)) << ' '
      << static_cast<int>(255.999 * intensity.clamp(g)) << ' '
      << static_cast<int>(255.999 * intensity.clamp(b)) << '\n';
}

#endif
