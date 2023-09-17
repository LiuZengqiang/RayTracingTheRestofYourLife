/**
 * @file sphere.h
 * @author Liuzengqiang (12021032@zju.edu.cn)
 * @brief
 * @version 0.1
 * @date 2023-09-06
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "material.h"
#include "vec3.h"
/**
 * @brief 球类
 *
 *
 */
class sphere : public hittable {
 public:
  // 如果构造参数只有一个 center, 说明该 sphere 是静止的
  sphere(point3 _center, double _radius, shared_ptr<material> _material)
      : center1(_center), radius(_radius), mat(_material), is_moving(false) {
    // 静态 sphere 的 bounding_box 比较容易计算
    auto rvec = vec3(radius, radius, radius);
    bbox = aabb(center1 - rvec, center1 + rvec);
  }
  // 如果构造参数有两个 center, 说明该 sphere 有可能会运动
  sphere(point3 _center1, point3 _center2, double _radius,
         shared_ptr<material> _material)
      : center1(_center1), radius(_radius), mat(_material), is_moving(true) {
    // 对于动态的 sphere, 构造一个包围盒可以容纳t=[0,1]时刻的sphere
    auto rvec = vec3(radius, radius, radius);
    aabb box1 = aabb(_center1 - rvec, _center1 + rvec);
    aabb box2 = aabb(_center1 - rvec, _center1 + rvec);
    bbox = aabb(box1, box2);
    center_vec = _center2 - _center1;
  }

  /**
   * @brief 根据 球与射线的相交公式 计算光线是否与球相交, 详细公式推导过程见
   * [Ray Tracing in One Weekend - 5.Adding a Sphere]
   * (https://raytracing.github.io/books/RayTracingInOneWeekend.html#addingasphere/creatingourfirstraytracedimage)
   * 如有合法交点则返回true, 否则返回false
   *
   * @param r 入射光线
   * @param ray_t 入射光(射线)的合法距离
   * @param rec 如果有合法的交点, 则在rec内记录交点信息
   * @return true
   * @return false
   */
  bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
    point3 center = is_moving ? (sphere_center(r.time())) : (center1);
    vec3 oc = r.origin() - center;
    auto a = r.direction().length_squared();
    auto half_b = dot(oc, r.direction());
    auto c = oc.length_squared() - radius * radius;

    auto discriminant = half_b * half_b - a * c;
    // 如果discriminant<0 说明没有交点
    if (discriminant < 0) return false;

    auto sqrtd = sqrt(discriminant);

    // Find the nearest root that lies in the acceptable range.
    auto root = (-half_b - sqrtd) / a;
    if (!ray_t.surrounds(root)) {
      // 如果较小的解不在合法范围内 那就尝试使用较大的那个解 root =
      // (-half_b+sqrtd)/a;
      root = (-half_b + sqrtd) / a;
      if (!ray_t.surrounds(root)) {
        // 如果较大的那个节也不在合法范围内，说明没有合法交点
        return false;
      }
    }
    // 更新交点 rec
    rec.t = root;
    rec.p = r.at(rec.t);
    vec3 outward_normal = (rec.p - center) / radius;
    rec.set_face_normal(r, outward_normal);

    get_sphere_uv(outward_normal, rec.u, rec.v);

    rec.mat = mat;

    return true;
  }

  aabb bounding_box() const override { return bbox; }
  // 在球面均匀上采样一点的概率(pdf)
  double pdf_value(const point3& o, const vec3& v) const override {
    // This method only works for stationary spheres.

    hit_record rec;
    if (!this->hit(ray(o, v), interval(0.001, infinity), rec)) return 0;

    auto cos_theta_max =
        sqrt(1 - radius * radius / (center1 - o).length_squared());
    auto solid_angle = 2 * pi * (1 - cos_theta_max);

    return 1 / solid_angle;
  }
  // 在球面上均匀采样一点
  vec3 random(const point3& o) const override {
    vec3 direction = center1 - o;
    auto distance_squared = direction.length_squared();
    onb uvw;
    uvw.build_from_w(direction);
    return uvw.local(random_to_sphere(radius, distance_squared));
  }

 private:
  point3 center1;            // 球的0时刻中心点
  double radius;             // 球的半径
  shared_ptr<material> mat;  // 球的表面材料
  bool is_moving;            // 球是否可以运行
  vec3 center_vec;           // 球的运动方向
  aabb bbox;                 // 球的包围盒
  // 计算time时刻的球的中心点
  point3 sphere_center(double time) const {
    return center1 + time * center_vec;
  }
  // 计算球上一点p的纹理坐标(u,v)
  static void get_sphere_uv(const point3& p, double& u, double& v) {
    // p: a given point on the sphere of radius one, centered at the origin.
    // u: returned value [0,1] of angle around the Y axis from X=-1.
    // v: returned value [0,1] of angle from Y=-1 to Y=+1.
    //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
    //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
    //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>
    auto theta = acos(-p.y());
    auto phi = atan2(-p.z(), p.x()) + pi;

    u = phi / (2 * pi);
    v = theta / pi;
  }
  static vec3 random_to_sphere(double radius, double distance_squared) {
    auto r1 = random_double();
    auto r2 = random_double();
    auto z = 1 + r2 * (sqrt(1 - radius * radius / distance_squared) - 1);

    auto phi = 2 * pi * r1;
    auto x = cos(phi) * sqrt(1 - z * z);
    auto y = sin(phi) * sqrt(1 - z * z);

    return vec3(x, y, z);
  }
};

#endif