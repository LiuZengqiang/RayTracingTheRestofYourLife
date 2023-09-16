/**
 * @file camera.h
 * @author Liuzengqiang (12021032@zju.edu.cn)
 * @brief camera class
 * @version 0.1
 * @date 2023-09-06
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef CAMERA_H
#define CAMERA_H
#include "color.h"
#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "pdf.h"
#include "rtweekend.h"
/**
 * @brief class camera
 *
 */
class camera {
 public:
  double aspect_ratio = 16.0 / 9.0;  // 图像的长宽比
  int image_width = 400;             // 图像的宽(像素数)
  int samples_per_pixel = 100;       // 每个像素的采样光线数
  int max_depth = 10;                // 光线的最大深度(反射次数)
  color background = color(0.7, 0.8, 1.0);  // 场景的背景颜色

  double vfov = 90;  // 视场角

  point3 lookfrom = point3(0, 0, -1);  // 相机位置
  point3 lookat = point3(0, 0, 0);     // 相机观察的点
  vec3 vup = vec3(0, 1, 0);            // 相机上方向向量

  double defocus_angle = 0;  // 模拟实际相机的散射角度(以实现景深效果
  double focus_dist = 10;  // 模拟实际相机的理想焦距(以实现景深效果)
  double recip_sqrt_spp;  // 1/sqrt(x), 其中x为一个像素内部的采样数

  /* Public Camera Parameters Here */
  /**
   * @brief
   *
   * @param world
   */
  void render(const hittable_list& world, const hittable_list& lights) {
    initialize();
    // Render
    std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";
    for (int j = 0; j < image_height; ++j) {
      std::clog << "\rScanlines remaining: " << (image_height - j) << ' '
                << std::flush;
      for (int i = 0; i < image_width; ++i) {
        color pixel_color(0, 0, 0);
        // 增加 jit 随机采样
        int sqrt_ssp = sqrt(samples_per_pixel) + 1;
        for (int s_j = 0; s_j < sqrt_ssp; s_j++) {
          for (int s_i = 0; s_i < sqrt_ssp; s_i++) {
            ray r = get_ray(i, j, s_i, s_j);
            pixel_color += ray_color(r, max_depth, world, lights);
          }
        }

        // for (int sample = 0; sample < samples_per_pixel; sample++) {
        //   // 计算像素(i,j)位置处的入射光线
        //   auto r = get_ray(i, j);
        //   // 光线跟踪主程序, 计算入射光线r经过"光线跟踪"后所附带的颜色值
        //   pixel_color += ray_color(r, max_depth, world);
        // }

        write_color(std::cout, pixel_color, samples_per_pixel);
      }
    }

    std::clog << "\rDone.                 \n";
  }

 private:
  int image_height;    // 图像的高(像素数)
  point3 center;       // 相机位置, 与 lookfrom 相同
  point3 pixel00_loc;  // 图像最左上角的像素坐标
  vec3 pixel_delta_u;  // 图像u方向(宽方向)单位像素的坐标delta
  vec3 pixel_delta_v;  // 图像v方向(高方向)单位像素的坐标delta
  vec3 u, v, w;  // 相机局部坐标系的三个单位坐标轴, u:相机的右/左方向,
                 // v:相机的上方向,w:相机的后方向

  vec3 defocus_disk_u;  // u方向散焦半径
  vec3 defocus_disk_v;  // v方向散焦半径

  /* Private Camera Variables Here */
  void initialize() {
    // Calculate the image height, and ensure that it's at least 1.
    image_height = static_cast<int>(image_width / aspect_ratio);
    image_height = (image_height < 1) ? 1 : image_height;
    // Camera
    // 相机参数
    center = lookfrom;

    auto theta = degrees_to_radians(vfov);  // 视场角 弧度
    auto h = tan(theta / 2.0);

    // Viewport widths less than one are ok since they are real valued.
    // viewport 视窗的宽,高(世界坐标系值)
    auto viewport_height = 2 * h * focus_dist;
    auto viewport_width =
        viewport_height * (static_cast<double>(image_width) / image_height);

    // 相机的局部坐标 u,v,w
    w = unit_vector(lookfrom - lookat);
    u = unit_vector(cross(vup, w));
    v = cross(w, u);
    // Calculate the vectors across the horizontal and down the vertical
    // 视窗宽度(世界坐标系值)
    auto viewport_u = viewport_width * u;
    // 视窗高度(世界坐标系值)
    // 这里必须乘以-v，因为图片在v方向的坐标往下为正
    auto viewport_v = viewport_height * -v;

    // Calculate the horizontal and vertical delta vectors from pixel to
    // pixel.
    // u,v方向一个像素所占的世界坐标值大小
    pixel_delta_u = viewport_u / image_width;
    pixel_delta_v = viewport_v / image_height;

    // Calculate the location of the upper left pixel.
    // 视窗最左上角在世界坐标系下的坐标
    auto viewport_upper_left =
        center - focus_dist * w - viewport_u / 2 - viewport_v / 2;

    // 视窗最左上角的像素(即像素(0,0))的中心点的坐标
    pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

    // 散焦半径
    auto defocus_radius =
        focus_dist * tan(degrees_to_radians(defocus_angle / 2.0));
    // u方向的散焦向量
    defocus_disk_u = defocus_radius * u;
    // v方向的散焦向量
    defocus_disk_v = defocus_radius * v;
    // 单位像素内部的光线数
    recip_sqrt_spp = 1.0 / sqrt(samples_per_pixel);
  }

  /**
   * @brief 光线跟踪递归程序
   *
   * @param r 入射光线
   * @param depth 光线传播的深度
   * @param world 世界场景
   * @return color
   */
  color ray_color(const ray& r, int depth, const hittable_list& world,
                  const hittable_list& lights) {
    if (depth <= 0) {
      return color(0, 0, 0);
    }
    hit_record rec;

    // 如果击中场景中的某个物体
    // 忽略距离在[0,0.001)范围内的交点，避免浮点运算误差
    if (world.hit(r, interval(0.001, infinity), rec)) {
      // 物体反射光线
      ray scattered;
      // 物体材质颜色
      color attenuation;
      // 样采的概率 pdf
      double scattering_pdf = 1.0;

      // 物体自发光
      color color_from_emission = rec.mat->emitted(r, rec, rec.u, rec.v, rec.p);

      if (rec.mat->scatter(r, rec, attenuation, scattered, scattering_pdf)) {
        // 如果材料存在反射, 则返回 自发光+物体颜色*反射光

        hittable_pdf light_pdf(lights, rec.p);
        scattered = ray(rec.p, light_pdf.generate(), r.time());

        if (dot(scattered, rec.normal) < 0) return color_from_emission;

        double pdf_val = light_pdf.value(scattered.direction());
        std::clog << pdf_val << " \n";
        if (pdf_val <= 0.0000001) return color_from_emission + background;

        color sample_color = ray_color(scattered, depth - 1, world, lights);

        color color_from_scatter =
            (attenuation * scattering_pdf * sample_color) / pdf_val;
        return color_from_emission + color_from_scatter;
      } else {
        // 如果材料不存在反射, 则返回 自发光
        return color_from_emission;
      }
    } else {
      // 如果没有击中场景中的物体, 则返回场景背景
      return background;
    }

    // 如果没有击中场景中的物体，就假设击中天空，天空的颜色是一个偏向于蓝色的随机值
    vec3 unit_direction = unit_vector(r.direction());
    auto a = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
  }

  /**
   * @brief 得到一条从相机到像素(i,j)的入射光线,
   *        该光线包含"相机镜头内随机采样(散焦)"和"像素内随机采样"两个随机采样
   *
   * @param i
   * @param j
   * @return ray
   */
  ray get_ray(int i, int j, int s_i, int s_j) const {
    auto pixel_center = pixel00_loc + i * pixel_delta_u + j * pixel_delta_v;
    auto pixel_sample = pixel_center + pixel_sample_square(s_i, s_j);

    auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
    auto ray_direction = pixel_sample - ray_origin;
    // 生成有time属性的ray
    auto ray_time = random_double();
    return ray(ray_origin, ray_direction, ray_time);
  }
  /**
   * @brief 在单位正方形内随机采样
   *
   * @return vec3
   */
  vec3 pixel_sample_square(int s_i, int s_j) const {
    auto px = -0.5 + recip_sqrt_spp * (s_i + random_double());
    auto py = -0.5 + recip_sqrt_spp * (s_j + random_double());
    return px * pixel_delta_u + py * pixel_delta_v;
  }
  /**
   * @brief 在单位圆内随机采样
   *
   * @return point3
   */
  point3 defocus_disk_sample() const {
    auto p = random_in_unit_disk();
    return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
  }
};

#endif