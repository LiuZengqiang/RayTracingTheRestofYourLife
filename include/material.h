/**
 * @file material.h
 * @author Liuzengqiang (12021032@zju.edu.cn)
 * @brief
 * @version 0.1
 * @date 2023-09-06
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef MATERIAL_H
#define MATERIAL_H
#include "color.h"
#include "hittable.h"
#include "onb.h"
#include "pdf.h"
#include "rtweekend.h"
#include "texture.h"

class hit_record;

// 记录 散射光属性
class scatter_record {
 public:
  color attenuation;
  shared_ptr<pdf> pdf_ptr;
  bool skip_pdf;
  ray skip_pdf_ray;
};
/**
 * @brief 材料类, 所有特定的材料都必须继承该类并实现其中的
 * scatter() 函数, 如果材料有自发光可以重载 emitted() 函数
 *
 */
class material {
 public:
  virtual ~material() = default;

  /**
   * @brief 计算入射光线照射到该材料上的行为, 若存在反射光线返回true,
   * 没有反射光线返回false
   * @param r_in
   * @param rec
   * @param srec
   * @return
   */
  virtual bool scatter(const ray& r_in, const hit_record& rec,
                       scatter_record& srec) const {
    return false;
  }
  virtual double scattering_pdf(const ray& r_in, const hit_record& rec,
                                const ray& scattered) const {
    return 0;
  }

  /**
   * @brief
   *
   * @param r_in 入射光
   * @param rec 交点
   * @param u 纹理坐标 u
   * @param v 纹理坐标 v
   * @param p 交点空间位置坐标 p
   * @return color
   */
  virtual color emitted(const ray& r_in, const hit_record& rec, double u,
                        double v, const point3& p) const {
    return color(0, 0, 0);
  }
};

/**
 * @brief 漫反射类
 * 1. 在交点表面法向方向存在散射
 * 2. 衰减
 *
 */
class lambertian : public material {
 public:
  lambertian(const color& a) : albedo(make_shared<solid_color>(a)) {}
  lambertian(shared_ptr<texture> _texture) : albedo(_texture){};

  bool scatter(const ray& r_in, const hit_record& rec,
               scatter_record& srec) const override {
    srec.attenuation = albedo->value(rec.u, rec.v, rec.p);
    srec.pdf_ptr = make_shared<cosine_pdf>(rec.normal);
    srec.skip_pdf = false;
    return true;
  }

  // 此处的 scattering_pdf 表示 从 scattered 入射的光射向 r_in 方向的占比
  double scattering_pdf(const ray& r_in, const hit_record& rec,
                        const ray& scattered) const override {
    auto cos_theta = dot(rec.normal, unit_vector(scattered.direction()));
    return cos_theta < 0 ? 0 : cos_theta / pi;
  }

 private:
  shared_ptr<texture> albedo;  // 颜色
};

/**
 * @brief 金属漫反射类
 * 1. 在理想反射光方向存在散射
 * 2. 衰减
 *
 */
class metal : public material {
 public:
  metal(const color& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}
  // bool scatter(const ray& r_in, const hit_record& rec, color& attenuation,
  //              ray& scattered, double& pdf) const override {
  //   // 理想反射光线方向
  //   vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
  //   scattered =
  //       ray(rec.p, reflected + fuzz * random_unit_vector(), r_in.time());
  //   attenuation = albedo;
  //   return (dot(scattered.direction(), rec.normal) > 0);
  // }
  bool scatter(const ray& r_in, const hit_record& rec,
               scatter_record& srec) const override {
    srec.attenuation = albedo;
    srec.pdf_ptr = nullptr;
    srec.skip_pdf = true;
    vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
    srec.skip_pdf_ray =
        ray(rec.p, reflected + fuzz * random_in_unit_sphere(), r_in.time());
    return true;
  }

 private:
  color albedo;  // 衰减系数
  double fuzz;   // 散射系数
};

/**
 * @brief 玻璃类
 * 1. 折射
 * 2. 反射
 */
class dielectric : public material {
 public:
  dielectric(double _etai_over_etat) : etai_over_etat(_etai_over_etat) {}

  bool scatter(const ray& r_in, const hit_record& rec,
               scatter_record& srec) const override {
    srec.attenuation = color(1.0, 1.0, 1.0);
    srec.pdf_ptr = nullptr;
    srec.skip_pdf = true;
    // 根据入射光的方向进行判断 eta/eta' 的值
    double refraction_ratio =
        rec.front_face ? (1.0 / etai_over_etat) : (etai_over_etat);
    vec3 unit_direction = unit_vector(r_in.direction());

    // 入射角若很大则只发生反射, 没有折射
    double cos_theta = fmin(dot(rec.normal, -unit_direction), 1.0);

    double sin_theta = sqrt(1.0 - cos_theta * cos_theta);
    bool cannot_refraction = refraction_ratio * sin_theta > 1.0;
    vec3 direction;
    if (cannot_refraction ||
        reflectance(cos_theta, refraction_ratio) > random_double()) {
      // 若为反射
      direction = reflect(unit_direction, rec.normal);
    } else {
      // 若为折射
      direction = refract(unit_direction, rec.normal, refraction_ratio);
    }
    srec.skip_pdf_ray = ray(rec.p, direction, r_in.time());
    return true;
  }

 private:
  double etai_over_etat;  // 入射介质折射率/出射介质折射率

  // 计算 反射折射比率
  static double reflectance(double cosine, double ref_idx) {
    // 使用 Schlick's 逼近公式计算
    auto r0 = (1 - ref_idx) / (1 + ref_idx);
    r0 = r0 * r0;
    return r0 + (1 - r0) * pow((1 - cosine), 5);
  }
};

/**
 * @brief 发射光源的材料类
 *
 */
class diffuse_light : public material {
 public:
  diffuse_light(shared_ptr<texture> a) : emit(a) {}
  diffuse_light(color c) : emit(make_shared<solid_color>(c)) {}

  color emitted(const ray& r_in, const hit_record& rec, double u, double v,
                const point3& p) const override {
    // 添加约束, 只有跟光源的正面相交, 才反射光线
    if (!rec.front_face) return color(0, 0, 0);
    return emit->value(u, v, p);
  }

 private:
  shared_ptr<texture> emit;  // 发出的光
};

/**
 * @brief 各向同性的类似于烟雾的材料类
 *
 */
class isotropic : public material {
 public:
  isotropic(color c) : albedo(make_shared<solid_color>(c)) {}
  isotropic(shared_ptr<texture> a) : albedo(a) {}

  bool scatter(const ray& r_in, const hit_record& rec,
               scatter_record& srec) const override {
    srec.attenuation = albedo->value(rec.u, rec.v, rec.p);
    // 使用球面上采样
    srec.pdf_ptr = make_shared<sphere_pdf>();
    srec.skip_pdf = false;
    return true;
  }
  // 此处的 scattering_pdf 表示 从 scattered 入射的光射向 r_in 方向的占比
  double scattering_pdf(const ray& r_in, const hit_record& rec,
                        const ray& scattered) const override {
    return 1 / (4 * pi);
  }

 private:
  shared_ptr<texture> albedo;
};

#endif