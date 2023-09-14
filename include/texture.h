/**
 * @file texture.h
 * @author Liuzengqiang (12021032@zju.edu.cn)
 * @brief
 * @version 0.1
 * @date 2023-09-09
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef TEXTURE_H
#define TEXTURE_H
#include "perlin.h"
#include "rtw_stb_image.h"
#include "rtweekend.h"

// 纹理类
class texture {
 public:
  virtual ~texture() = default;
  // 得到空间坐标位置为p, 纹理坐标为(u,v)处的颜色值
  virtual color value(double u, double v, const point3& p) const = 0;
};

// 一个简单的纹理类
// 该类只是单纯的颜色纹理
class solid_color : public texture {
 public:
  solid_color(color c) : color_value(c) {}

  solid_color(double red, double green, double blue)
      : solid_color(color(red, green, blue)) {}

  color value(double u, double v, const point3& p) const override {
    return color_value;
  }

 private:
  color color_value;
};

// 这是一个网格纹理(checkered pattern)
class checker_texture : public texture {
 public:
  checker_texture(double _scale, shared_ptr<texture> _even,
                  shared_ptr<texture> _odd)
      : inv_scale(1.0 / _scale), even(_even), odd(_odd) {}

  checker_texture(double _scale, color c1, color c2)
      : inv_scale(1.0 / _scale),
        even(make_shared<solid_color>(c1)),
        odd(make_shared<solid_color>(c2)) {}

  color value(double u, double v, const point3& p) const override {
    auto xInteger = static_cast<int>(std::floor(inv_scale * p.x()));
    auto yInteger = static_cast<int>(std::floor(inv_scale * p.y()));
    auto zInteger = static_cast<int>(std::floor(inv_scale * p.z()));
    // 得到一个网格类型的纹理
    bool isEven = (xInteger + yInteger + zInteger) % 2 == 0;
    // 尝试使用参数 u,v
    // isEven = static_cast<int>(u * 100 + v * 100) % 2;

    return isEven ? even->value(u, v, p) : odd->value(u, v, p);
  }

 private:
  double inv_scale;
  shared_ptr<texture> even;
  shared_ptr<texture> odd;
};

// 图片纹理类
class image_texture : public texture {
 public:
  image_texture(const char* filename) : image(filename) {}

  // 获取图片 image 在二维局部坐标(u,v)下的颜色值
  color value(double u, double v, const point3& p) const override {
    // If we have no texture data, then return solid cyan as a debugging aid.
    if (image.height() <= 0) return color(0, 1, 1);

    // Clamp input texture coordinates to [0,1] x [1,0]

    // 计算(u,v)坐标
    u = interval(0, 1).clamp(u);
    v = 1.0 - interval(0, 1).clamp(v);  // Flip V to image coordinates

    auto i = static_cast<int>(u * image.width());
    auto j = static_cast<int>(v * image.height());
    auto pixel = image.pixel_data(i, j);

    auto color_scale = 1.0 / 255.0;
    return color(color_scale * pixel[0], color_scale * pixel[1],
                 color_scale * pixel[2]);
  }

 private:
  rtw_image image;
};

// perlin noise 纹理类
class noise_texture : public texture {
 public:
  noise_texture() {}
  noise_texture(double sc) : scale(sc) {}

  color value(double u, double v, const point3& p) const override {
    return color(1, 1, 1) * 0.5 *
           (1 + sin(scale * p.z() + 10 * noise.turb(scale * p)));
  }

 private:
  perlin noise;  // perlin noise 对象
  double scale;  // 用于调整 noise 块大小的参数
};
#endif