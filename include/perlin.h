/**
 * @file perlin.h
 * @author Liuzengqiang (12021032@zju.edu.cn)
 * @brief 柏林噪声类
 * @version 0.1
 * @date 2023-09-09
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef PERLIN_H
#define PERLIN_H

#include "rtweekend.h"
/**
 * @brief perlin noise 类
 *
 */
class perlin {
 public:
  perlin() {
    ranvec = new vec3[point_count];
    for (int i = 0; i < point_count; ++i) {
      ranvec[i] = unit_vector(vec3::random(-1, 1));
    }

    perm_x = perlin_generate_perm();
    perm_y = perlin_generate_perm();
    perm_z = perlin_generate_perm();
  }

  ~perlin() {
    delete[] ranvec;
    delete[] perm_x;
    delete[] perm_y;
    delete[] perm_z;
  }

  // 改函数给出一个随机的 perlin noise 随机数, 在(0,1)范围内
  double noise(const point3& p) const {
    auto u = p.x() - floor(p.x());
    auto v = p.y() - floor(p.y());
    auto w = p.z() - floor(p.z());
    // 使用 Hermitian 参数插值, 而不是简单的线程参数插值
    u = u * u * (3 - 2 * u);
    v = v * v * (3 - 2 * v);
    w = w * w * (3 - 2 * w);

    // p 的整数部分
    auto i = static_cast<int>(floor(p.x()));
    auto j = static_cast<int>(floor(p.y()));
    auto k = static_cast<int>(floor(p.z()));
    vec3 c[2][2][2];

    // 点p跟前后左右上下共6个点进行插值
    for (int di = 0; di < 2; di++)
      for (int dj = 0; dj < 2; dj++)
        for (int dk = 0; dk < 2; dk++)
          c[di][dj][dk] =
              ranvec[perm_x[(i + di) & 255] ^ perm_y[(j + dj) & 255] ^
                     perm_z[(k + dk) & 255]];

    return perlin_interp(c, u, v, w);
  }
  /**
   * @brief turbulence, 使用多个 perlin noise 产生紊流的效果
   *
   * @param p
   * @param depth
   * @return double
   */
  double turb(const point3& p, int depth = 7) const {
    auto accum = 0.0;
    auto temp_p = p;
    auto weight = 1.0;

    for (int i = 0; i < depth; i++) {
      accum += weight * noise(temp_p);
      weight *= 0.5;
      temp_p *= 2;
    }

    return fabs(accum);
  }

 private:
  static const int point_count = 256;
  vec3* ranvec;
  int* perm_x;
  int* perm_y;
  int* perm_z;

  static int* perlin_generate_perm() {
    auto p = new int[point_count];

    for (int i = 0; i < perlin::point_count; i++) p[i] = i;

    permute(p, point_count);

    return p;
  }

  static void permute(int* p, int n) {
    for (int i = n - 1; i > 0; i--) {
      int target = random_int(0, i);
      int tmp = p[i];
      p[i] = p[target];
      p[target] = tmp;
    }
  }
  
  /**
   * @brief 插值函数, 用于平滑
   * 
   * @param c 
   * @param u 
   * @param v 
   * @param w 
   * @return double 
   */
  static double perlin_interp(vec3 c[2][2][2], double u, double v, double w) {
    auto uu = u * u * (3 - 2 * u);
    auto vv = v * v * (3 - 2 * v);
    auto ww = w * w * (3 - 2 * w);
    auto accum = 0.0;

    for (int i = 0; i < 2; i++)
      for (int j = 0; j < 2; j++)
        for (int k = 0; k < 2; k++) {
          vec3 weight_v(u - i, v - j, w - k);
          accum += (i * uu + (1 - i) * (1 - uu)) *
                   (j * vv + (1 - j) * (1 - vv)) *
                   (k * ww + (1 - k) * (1 - ww)) * dot(c[i][j][k], weight_v);
        }

    return accum;
  }
};

#endif