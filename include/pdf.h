#ifndef PDF_H
#define PDF_H

#include "hittable_list.h"
#include "onb.h"
#include "rtweekend.h"

/**
 * @brief 光线采样类
 * 必须实现两个函数:
 * value(direction): 计算采样光线为direction时的采样概率
 * generate(): 按照特定的pdf采样得到一条光线
 */
class pdf {
 public:
  virtual ~pdf() {}
  virtual double value(const vec3& direction) const = 0;
  virtual vec3 generate() const = 0;
};

// 球面上均匀采样 pdf
class sphere_pdf : public pdf {
 public:
  sphere_pdf() {}

  double value(const vec3& direction) const override { return 1 / (4 * pi); }

  vec3 generate() const override { return random_unit_vector(); }
};

// 球面上与法向cos(theta) pdf
class cosine_pdf : public pdf {
 public:
  cosine_pdf(const vec3& w) { uvw.build_from_w(w); }

  double value(const vec3& direction) const override {
    auto cosine_theta = dot(unit_vector(direction), uvw.w());
    return fmax(0, cosine_theta / pi);
  }

  vec3 generate() const override {
    return uvw.local(random_cosine_direction());
  }

 private:
  onb uvw;
};

// 在物体表面上按平面采样
class hittable_pdf : public pdf {
 public:
  hittable_pdf(const hittable& _objects, const point3& _origin)
      : objects(_objects), origin(_origin) {}

  // 计算生成 direction 方向光线的概率pdf
  double value(const vec3& direction) const override {
    return objects.pdf_value(origin, direction);
  }
  // 按照规定的材料, 采样得到一个 随机的光线方向
  vec3 generate() const override { return objects.random(origin); }

 private:
  const hittable& objects;
  point3 origin;
};

// 混合两种 pdf
class mixture_pdf : public pdf {
 public:
  mixture_pdf(shared_ptr<pdf> p0, shared_ptr<pdf> p1) {
    p[0] = p0;
    p[1] = p1;
  }

  double value(const vec3& direction) const override {
    return 0.5 * p[0]->value(direction) + 0.5 * p[1]->value(direction);
  }

  vec3 generate() const override {
    if (random_double() < 0.5)
      return p[0]->generate();
    else
      return p[1]->generate();
  }

 private:
  shared_ptr<pdf> p[2];
};
#endif