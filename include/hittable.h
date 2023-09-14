/**
 * @file hittable.h
 * @author Liuzengqiang (12021032@zju.edu.cn)
 * @brief
 * @version 0.1
 * @date 2023-09-06
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef HITTABLE_H
#define HITTABLE_H

#include "aabb.h"
#include "ray.h"
#include "rtweekend.h"

class material;
/**
 * @brief 记录光线与物体的交点信息
 *
 */
class hit_record {
 public:
  point3 p;  // 交点的坐标
  // 交点处的法向, 与入射光方向相反(可能指向物体外也可能指向物体内)
  vec3 normal;
  shared_ptr<material> mat;  // 交点处材料属性
  double t;                  // 光线的传播距离
  double u;                  // 用于计算纹理的坐标参数(u,v)
  double v;
  bool front_face;  // 该交点是否是物体的外表面
  /**
   * @brief 设置该交点是否是物体的外表面
   *
   * @param r 入射光
   * @param outward_normal 物体的外表面法向(向外)
   */
  void set_face_normal(const ray& r, const vec3& outward_normal) {
    front_face = dot(r.direction(), outward_normal) < 0;
    normal = front_face ? (outward_normal) : (-outward_normal);
  }
};
/**
 * @brief 可与光线相交的类, 所有可与光线作用的物体都必须继承该类并实现其中的
 * hit() 函数
 *
 */
class hittable {
 public:
  // =default 关键字令编译器自动生成默认的构造函数
  virtual ~hittable() = default;
  virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;
  // 该 hittable 的包围盒
  virtual aabb bounding_box() const = 0;
};

/**
 * @brief 对物体进行 translate 的包装类
 *
 */
class translate : public hittable {
 public:
  translate(shared_ptr<hittable> p, const vec3& displacement)
      : object(p), offset(displacement) {
    // 对 包围盒 进行偏移, 包围盒用在构建 BVH 树中,
    // 必须保证包围盒在世界坐标系下是正确的
    bbox = object->bounding_box() + offset;
  }

  bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
    // Move the ray backwards by the offset
    // 物体偏移相当于入射光进行反向偏移
    // 之所以可以这样操作是因为:
    // 此处的 hit() 函数只用于计算(只关心)光线 r 与该物体的碰撞,
    // 不考虑 r 与场景中其他物体的碰撞, 假如该物体的前面存在某个其他物体
    // 遮挡了该物体, 那么要么程序不会到达这里, 要么此处的交点会被更近的交点替换
    ray offset_r(r.origin() - offset, r.direction(), r.time());

    // Determine where (if any) an intersection occurs along the offset ray
    if (!object->hit(offset_r, ray_t, rec)) return false;

    // Move the intersection point forwards by the offset
    rec.p += offset;

    return true;
  }

  aabb bounding_box() const override { return bbox; }

 private:
  shared_ptr<hittable> object;
  vec3 offset;
  aabb bbox;
};

/**
 * @brief 对物体进行 totate 的包装类
 * 在构建场景时 必须先 rotate 再 translate
 *
 */
class rotate_y : public hittable {
 public:
  rotate_y(shared_ptr<hittable> p, double angle) : object(p) {
    // 物体旋转时需要更新 bbox
    auto radians = degrees_to_radians(angle);
    sin_theta = sin(radians);
    cos_theta = cos(radians);
    bbox = object->bounding_box();

    point3 min(infinity, infinity, infinity);
    point3 max(-infinity, -infinity, -infinity);

    for (int i = 0; i < 2; i++) {
      for (int j = 0; j < 2; j++) {
        for (int k = 0; k < 2; k++) {
          auto x = i * bbox.x.max + (1 - i) * bbox.x.min;
          auto y = j * bbox.y.max + (1 - j) * bbox.y.min;
          auto z = k * bbox.z.max + (1 - k) * bbox.z.min;

          auto newx = cos_theta * x + sin_theta * z;
          auto newz = -sin_theta * x + cos_theta * z;

          vec3 tester(newx, y, newz);

          for (int c = 0; c < 3; c++) {
            min[c] = fmin(min[c], tester[c]);
            max[c] = fmax(max[c], tester[c]);
          }
        }
      }
    }

    bbox = aabb(min, max);
  }

  bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
    // Change the ray from world space to object space
    auto origin = r.origin();
    auto direction = r.direction();

    // 将入射光进行旋转
    origin[0] = cos_theta * r.origin()[0] - sin_theta * r.origin()[2];
    origin[2] = sin_theta * r.origin()[0] + cos_theta * r.origin()[2];

    direction[0] = cos_theta * r.direction()[0] - sin_theta * r.direction()[2];
    direction[2] = sin_theta * r.direction()[0] + cos_theta * r.direction()[2];

    ray rotated_r(origin, direction, r.time());

    // Determine where (if any) an intersection occurs in object space
    if (!object->hit(rotated_r, ray_t, rec)) return false;

    // Change the intersection point from object space to world space
    // 将计算得到的交点局部坐标系转到世界坐标系
    auto p = rec.p;
    p[0] = cos_theta * rec.p[0] + sin_theta * rec.p[2];
    p[2] = -sin_theta * rec.p[0] + cos_theta * rec.p[2];

    // Change the normal from object space to world space
    auto normal = rec.normal;
    normal[0] = cos_theta * rec.normal[0] + sin_theta * rec.normal[2];
    normal[2] = -sin_theta * rec.normal[0] + cos_theta * rec.normal[2];

    rec.p = p;
    rec.normal = normal;

    return true;
  }
  aabb bounding_box() const override { return bbox; }

 private:
  shared_ptr<hittable> object;
  double sin_theta;
  double cos_theta;
  aabb bbox;  // hittable 的包围盒
};

#endif