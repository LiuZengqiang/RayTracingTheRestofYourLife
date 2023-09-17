#include <float.h>
#include <time.h>

#include <iostream>

#include "bvh.h"
#include "camera.h"
#include "color.h"
#include "constant_medium.h"
#include "hittable_list.h"
#include "interval.h"
#include "material.h"
#include "quad.h"
#include "ray.h"
#include "rtweekend.h"
#include "sphere.h"
#include "texture.h"
#include "vec3.h"

// void random_spheres() {
//   /* 生成场景 */
//   hittable_list world;
//   auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
//   world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

//   for (int a = -11; a < 11; a++) {
//     for (int b = -11; b < 11; b++) {
//       auto choose_mat = random_double();
//       point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 *
//       random_double());
//       // 根据随机数 choose_mat 的取值大小随机生成 diffuse, metal, glass
//       材质的球 if ((center - point3(4, 0.2, 0)).length() > 0.9) {
//         shared_ptr<material> sphere_material;

//         if (choose_mat < 0.8) {
//           // diffuse, 漫反射材质,
//           反射光线在交点法线方向存在散射(漫反射)和衰减 auto albedo =
//           color::random() * color::random(); sphere_material =
//           make_shared<lambertian>(albedo);
//           // 增加可以移动的球
//           auto center2 = center + vec3(0, random_double(0, 0.5), 0);
//           world.add(make_shared<sphere>(center, center2, 0.2,
//           sphere_material));

//         } else if (choose_mat < 0.95) {
//           // metal, 金属材质,
//           反射光线在理想反射光线方向存在散射(漫反射)和衰减 auto albedo =
//           color::random(0.5, 1); auto fuzz = random_double(0, 0.5);
//           sphere_material = make_shared<metal>(albedo, fuzz);
//           world.add(make_shared<sphere>(center, 0.2, sphere_material));
//         } else {
//           // glass, 玻璃材质, 反射光线存在折射,反射和衰减
//           sphere_material = make_shared<dielectric>(1.5);
//           world.add(make_shared<sphere>(center, 0.2, sphere_material));
//         }
//       }
//     }
//   }

//   auto material1 = make_shared<dielectric>(1.5);
//   world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

//   auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
//   world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

//   auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
//   world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

//   /* 设置相机和输出图像的属性 */
//   camera cam;
//   cam.aspect_ratio = 16.0 / 9.0;  // 图像的长宽比
//   cam.image_width = 400;          // 图像的宽(像素数)
//   cam.samples_per_pixel = 100;    // 每个像素的采样光线数
//   cam.max_depth = 50;             // 光线的最大深度

//   cam.vfov = 20;                    // 视场角
//   cam.lookfrom = point3(13, 2, 3);  // 相机位置
//   cam.lookat = point3(0, 0, 0);     // 相机观察的点
//   cam.vup = vec3(0, 1, 0);          // 相机上方向向量

//   cam.defocus_angle = 0.6;  // 模拟实际相机的散射角度(以实现景深效果)
//   cam.focus_dist = 10.0;  // 模拟实际相机的理想焦距(以实现景深效果)
//   auto start = clock();
//   cam.render(world);
//   auto finish = clock();
//   std::clog << "Elapsed:" << (double)(finish - start) / (CLOCKS_PER_SEC)
//             << "\n";
// }

// void two_spheres() {
//   hittable_list world;

//   auto checker =
//       make_shared<checker_texture>(0.8, color(.2, .3, .1), color(.9, .9,
//       .9));

//   world.add(make_shared<sphere>(point3(0, -10, 0), 10,
//                                 make_shared<lambertian>(checker)));
//   world.add(make_shared<sphere>(point3(0, 10, 0), 10,
//                                 make_shared<lambertian>(checker)));

//   camera cam;

//   cam.aspect_ratio = 16.0 / 9.0;
//   cam.image_width = 400;
//   cam.samples_per_pixel = 100;
//   cam.max_depth = 50;

//   cam.vfov = 20;
//   cam.lookfrom = point3(13, 2, 3);
//   cam.lookat = point3(0, 0, 0);
//   cam.vup = vec3(0, 1, 0);

//   cam.defocus_angle = 0;

//   cam.render(world);
// }
// void earth() {
//   auto earth_texture = make_shared<image_texture>("earthmap.jpg");

//   auto earth_surface = make_shared<lambertian>(earth_texture);
//   auto globe = make_shared<sphere>(point3(0, 0, 0), 2, earth_surface);

//   camera cam;

//   cam.aspect_ratio = 16.0 / 9.0;
//   cam.image_width = 400;
//   cam.samples_per_pixel = 100;
//   cam.max_depth = 50;

//   cam.vfov = 20;
//   // 这里的 lookfrom 应该为 (12,0,0) 才能和网站上的结果相同
//   cam.lookfrom = point3(12, 0, 0);
//   cam.lookat = point3(0, 0, 0);
//   cam.vup = vec3(0, 1, 0);

//   cam.defocus_angle = 0;

//   cam.render(hittable_list(globe));
// }

// void two_perlin_spheres() {
//   hittable_list world;

//   auto pertext = make_shared<noise_texture>(4);
//   world.add(make_shared<sphere>(point3(0, -1000, 0), 1000,
//                                 make_shared<lambertian>(pertext)));
//   world.add(make_shared<sphere>(point3(0, 2, 0), 2,
//                                 make_shared<lambertian>(pertext)));

//   camera cam;

//   cam.aspect_ratio = 16.0 / 9.0;
//   cam.image_width = 400;
//   cam.samples_per_pixel = 100;
//   cam.max_depth = 50;

//   cam.vfov = 20;
//   cam.lookfrom = point3(13, 2, 3);
//   cam.lookat = point3(0, 0, 0);
//   cam.vup = vec3(0, 1, 0);

//   cam.defocus_angle = 0;

//   cam.render(world);
// }

// void quads() {
//   hittable_list world;

//   // Materials
//   auto left_red = make_shared<lambertian>(color(1.0, 0.2, 0.2));
//   auto back_green = make_shared<lambertian>(color(0.2, 1.0, 0.2));
//   auto right_blue = make_shared<lambertian>(color(0.2, 0.2, 1.0));
//   auto upper_orange = make_shared<lambertian>(color(1.0, 0.5, 0.0));
//   auto lower_teal = make_shared<lambertian>(color(0.2, 0.8, 0.8));

//   // Quads
//   world.add(make_shared<quad>(point3(-3, -2, 5), vec3(0, 0, -4), vec3(0, 4,
//   0),
//                               left_red));
//   world.add(make_shared<quad>(point3(-2, -2, 0), vec3(4, 0, 0), vec3(0, 4,
//   0),
//                               back_green));
//   world.add(make_shared<quad>(point3(3, -2, 1), vec3(0, 0, 4), vec3(0, 4, 0),
//                               right_blue));
//   world.add(make_shared<quad>(point3(-2, 3, 1), vec3(4, 0, 0), vec3(0, 0, 4),
//                               upper_orange));
//   world.add(make_shared<quad>(point3(-2, -3, 5), vec3(4, 0, 0), vec3(0, 0,
//   -4),
//                               lower_teal));

//   camera cam;

//   cam.aspect_ratio = 1.0;
//   cam.image_width = 400;
//   cam.samples_per_pixel = 100;
//   cam.max_depth = 50;

//   cam.vfov = 80;
//   cam.lookfrom = point3(0, 0, 9);
//   cam.lookat = point3(0, 0, 0);
//   cam.vup = vec3(0, 1, 0);

//   cam.defocus_angle = 0;

//   cam.render(world);
// }
// void simple_light() {
//   hittable_list world;
//   // perlin noise 纹理
//   auto pertext = make_shared<noise_texture>(4);
//   // 增加两个使用 perlin noise 纹理的球
//   world.add(make_shared<sphere>(point3(0, -1000, 0), 1000,
//                                 make_shared<lambertian>(pertext)));
//   world.add(make_shared<sphere>(point3(0, 2, 0), 2,
//                                 make_shared<lambertian>(pertext)));

//   // 增加一个自发光的球
//   auto difflight = make_shared<diffuse_light>(color(4, 4, 4));
//   world.add(make_shared<sphere>(point3(0, 7, 0), 2, difflight));
//   // 增加一个自发光的四边形
//   world.add(make_shared<quad>(point3(3, 1, -2), vec3(2, 0, 0), vec3(0, 2, 0),
//                               difflight));

//   camera cam;

//   cam.aspect_ratio = 16.0 / 9.0;
//   cam.image_width = 400;
//   cam.samples_per_pixel = 100;
//   cam.max_depth = 50;
//   cam.background = color(0, 0, 0);

//   cam.vfov = 20;
//   cam.lookfrom = point3(26, 3, 6);
//   cam.lookat = point3(0, 2, 0);
//   cam.vup = vec3(0, 1, 0);

//   cam.defocus_angle = 0;

//   cam.render(world);
// }
// void cornell_box() {
//   // Cornell Box 场景
//   hittable_list world;

//   auto red = make_shared<lambertian>(color(.65, .05, .05));
//   auto white = make_shared<lambertian>(color(.73, .73, .73));
//   auto green = make_shared<lambertian>(color(.12, .45, .15));
//   auto light = make_shared<diffuse_light>(color(15, 15, 15));

//   // 增加 Cornel Box 的墙壁
//   world.add(make_shared<quad>(point3(555, 0, 0), vec3(0, 555, 0),
//                               vec3(0, 0, 555), green));
//   world.add(make_shared<quad>(point3(0, 0, 0), vec3(0, 555, 0), vec3(0, 0,
//   555),
//                               red));
//   world.add(make_shared<quad>(point3(343, 554, 332), vec3(-130, 0, 0),
//                               vec3(0, 0, -105), light));
//   world.add(make_shared<quad>(point3(0, 0, 0), vec3(555, 0, 0), vec3(0, 0,
//   555),
//                               white));
//   world.add(make_shared<quad>(point3(555, 555, 555), vec3(-555, 0, 0),
//                               vec3(0, 0, -555), white));
//   world.add(make_shared<quad>(point3(0, 0, 555), vec3(555, 0, 0),
//                               vec3(0, 555, 0), white));

//   // 在场景中增加两个四面体
//   // 必须先 rotate 再 translate
//   shared_ptr<hittable> box1 =
//       box(point3(0, 10, 0), point3(165, 330, 165), white);
//   box1 = make_shared<rotate_y>(box1, 15);
//   box1 = make_shared<translate>(box1, vec3(265, 0, 295));
//   world.add(box1);

//   shared_ptr<hittable> box2 =
//       box(point3(0, 0, 0), point3(165, 165, 165), white);
//   box2 = make_shared<rotate_y>(box2, -18);
//   box2 = make_shared<translate>(box2, vec3(130, 0, 65));
//   world.add(box2);
//   camera cam;

//   cam.aspect_ratio = 1.0;
//   cam.image_width = 400;
//   cam.samples_per_pixel = 100;
//   cam.max_depth = 50;
//   cam.background = color(0, 0, 0);

//   cam.vfov = 40;
//   cam.lookfrom = point3(278, 278, -800);
//   cam.lookat = point3(278, 278, 0);
//   cam.vup = vec3(0, 1, 0);

//   cam.defocus_angle = 0;

//   cam.render(world);
// }

// void cornell_smoke() {
//   hittable_list world;

//   auto red = make_shared<lambertian>(color(.65, .05, .05));
//   auto white = make_shared<lambertian>(color(.73, .73, .73));
//   auto green = make_shared<lambertian>(color(.12, .45, .15));
//   auto light = make_shared<diffuse_light>(color(7, 7, 7));

//   world.add(make_shared<quad>(point3(555, 0, 0), vec3(0, 555, 0),
//                               vec3(0, 0, 555), green));
//   world.add(make_shared<quad>(point3(0, 0, 0), vec3(0, 555, 0), vec3(0, 0,
//   555),
//                               red));
//   world.add(make_shared<quad>(point3(113, 554, 127), vec3(330, 0, 0),
//                               vec3(0, 0, 305), light));
//   world.add(make_shared<quad>(point3(0, 555, 0), vec3(555, 0, 0),
//                               vec3(0, 0, 555), white));
//   world.add(make_shared<quad>(point3(0, 0, 0), vec3(555, 0, 0), vec3(0, 0,
//   555),
//                               white));
//   world.add(make_shared<quad>(point3(0, 0, 555), vec3(555, 0, 0),
//                               vec3(0, 555, 0), white));

//   shared_ptr<hittable> box1 =
//       box(point3(0, 0, 0), point3(165, 330, 165), white);
//   box1 = make_shared<rotate_y>(box1, 15);
//   box1 = make_shared<translate>(box1, vec3(265, 0, 295));

//   shared_ptr<hittable> box2 =
//       box(point3(0, 0, 0), point3(165, 165, 165), white);
//   box2 = make_shared<rotate_y>(box2, -18);
//   box2 = make_shared<translate>(box2, vec3(130, 0, 65));

//   world.add(make_shared<constant_medium>(box1, 0.01, color(0, 0, 0)));
//   world.add(make_shared<constant_medium>(box2, 0.01, color(1, 1, 1)));

//   camera cam;

//   cam.aspect_ratio = 1.0;
//   cam.image_width = 600;
//   cam.samples_per_pixel = 200;
//   cam.max_depth = 50;
//   cam.background = color(0, 0, 0);

//   cam.vfov = 40;
//   cam.lookfrom = point3(278, 278, -800);
//   cam.lookat = point3(278, 278, 0);
//   cam.vup = vec3(0, 1, 0);

//   cam.defocus_angle = 0;

//   cam.render(world);
// }
// void final_scene(int image_width, int samples_per_pixel, int max_depth) {
//   hittable_list boxes1;
//   auto ground = make_shared<lambertian>(color(0.48, 0.83, 0.53));

//   int boxes_per_side = 20;
//   for (int i = 0; i < boxes_per_side; i++) {
//     for (int j = 0; j < boxes_per_side; j++) {
//       auto w = 100.0;
//       auto x0 = -1000.0 + i * w;
//       auto z0 = -1000.0 + j * w;
//       auto y0 = 0.0;
//       auto x1 = x0 + w;
//       auto y1 = random_double(1, 101);
//       auto z1 = z0 + w;

//       boxes1.add(box(point3(x0, y0, z0), point3(x1, y1, z1), ground));
//     }
//   }

//   hittable_list world;

//   world.add(make_shared<bvh_node>(boxes1));

//   auto light = make_shared<diffuse_light>(color(7, 7, 7));
//   world.add(make_shared<quad>(point3(123, 554, 147), vec3(300, 0, 0),
//                               vec3(0, 0, 265), light));

//   auto center1 = point3(400, 400, 200);
//   auto center2 = center1 + vec3(30, 0, 0);
//   auto sphere_material = make_shared<lambertian>(color(0.7, 0.3, 0.1));
//   world.add(make_shared<sphere>(center1, center2, 50, sphere_material));

//   world.add(make_shared<sphere>(point3(260, 150, 45), 50,
//                                 make_shared<dielectric>(1.5)));
//   world.add(make_shared<sphere>(point3(0, 150, 145), 50,
//                                 make_shared<metal>(color(0.8, 0.8,
//                                 0.9), 1.0)));

//   auto boundary = make_shared<sphere>(point3(360, 150, 145), 70,
//                                       make_shared<dielectric>(1.5));
//   world.add(boundary);
//   world.add(make_shared<constant_medium>(boundary, 0.2, color(0.2, 0.4,
//   0.9))); boundary =
//       make_shared<sphere>(point3(0, 0, 0), 5000,
//       make_shared<dielectric>(1.5));
//   world.add(make_shared<constant_medium>(boundary, .0001, color(1, 1, 1)));

//   auto emat =
//       make_shared<lambertian>(make_shared<image_texture>("earthmap.jpg"));
//   world.add(make_shared<sphere>(point3(400, 200, 400), 100, emat));
//   auto pertext = make_shared<noise_texture>(0.1);
//   world.add(make_shared<sphere>(point3(220, 280, 300), 80,
//                                 make_shared<lambertian>(pertext)));

//   hittable_list boxes2;
//   auto white = make_shared<lambertian>(color(.73, .73, .73));
//   int ns = 1000;
//   for (int j = 0; j < ns; j++) {
//     boxes2.add(make_shared<sphere>(point3::random(0, 165), 10, white));
//   }

//   world.add(make_shared<translate>(
//       make_shared<rotate_y>(make_shared<bvh_node>(boxes2), 15),
//       vec3(-100, 270, 395)));

//   camera cam;

//   cam.aspect_ratio = 1.0;
//   cam.image_width = image_width;
//   cam.samples_per_pixel = samples_per_pixel;
//   cam.max_depth = max_depth;
//   cam.background = color(0, 0, 0);

//   cam.vfov = 40;
//   cam.lookfrom = point3(478, 278, -600);
//   cam.lookat = point3(278, 278, 0);
//   cam.vup = vec3(0, 1, 0);

//   cam.defocus_angle = 0;

//   cam.render(world);
// }

// int main(int argc, char** argv) {
//   if (argc < 1) {
//     std::clog << "请输入要生成的场景参数id[0-9]."
//               << "\n"
//               << "例如: ./RayTracingTheNextWeek 0 > image.ppm"
//               << "\n";
//     return -1;
//   }
//   int scene_id = int(argv[1][0] - '0');
//   switch (scene_id) {
//     case 1:
//       // 随机场景
//       random_spheres();
//       break;
//     case 2:
//       // 网格纹理
//       two_spheres();
//       break;
//     case 3:
//       // 地球image纹理
//       earth();
//       break;
//     case 4:
//       // perlin noise纹理
//       two_perlin_spheres();
//       break;
//     case 5:
//       // 四边形
//       quads();
//       break;
//     case 6:
//       // 光源
//       simple_light();
//       break;
//     case 7:
//       // 基础 cornell box
//       cornell_box();
//       break;
//     case 8:
//       // 带有烟雾材质的 cornell box
//       cornell_smoke();
//       break;
//     case 9:
//       // 最终的场景
//       final_scene(800, 10000, 40);
//       break;
//     default:
//       // 最终的场景
//       final_scene(400, 250, 4);
//       break;
//   }
// }

int main() {
  // Cornell Box 场景
  hittable_list world;

  auto red = make_shared<lambertian>(color(.65, .05, .05));
  auto white = make_shared<lambertian>(color(.73, .73, .73));
  auto green = make_shared<lambertian>(color(.12, .45, .15));

  // 增加 Cornel Box 的墙壁
  world.add(make_shared<quad>(point3(555, 0, 0), vec3(0, 555, 0),
                              vec3(0, 0, 555), green));
  world.add(make_shared<quad>(point3(0, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555),
                              red));
  world.add(make_shared<quad>(point3(0, 0, 0), vec3(555, 0, 0), vec3(0, 0, 555),
                              white));
  world.add(make_shared<quad>(point3(555, 555, 555), vec3(-555, 0, 0),
                              vec3(0, 0, -555), white));
  world.add(make_shared<quad>(point3(0, 0, 555), vec3(555, 0, 0),
                              vec3(0, 555, 0), white));

  // 在场景中增加两个四面体
  // 必须先 rotate 再 translate
  // shared_ptr<hittable> box1 =
  //     box(point3(0, 10, 0), point3(165, 330, 165), white);

  // shared_ptr<material> aluminum =
  //     make_shared<metal>(color(0.8, 0.85, 0.88), 0.0);
  // shared_ptr<hittable> box1 =
  //     box(point3(0, 0, 0), point3(165, 330, 165), aluminum);
  // box1 = make_shared<rotate_y>(box1, 15);
  // box1 = make_shared<translate>(box1, vec3(265, 0, 295));
  // world.add(box1);

  // shared_ptr<hittable> box2 =
  //     box(point3(0, 0, 0), point3(165, 165, 165), white);
  // box2 = make_shared<rotate_y>(box2, -18);
  // box2 = make_shared<translate>(box2, vec3(130, 0, 65));
  // world.add(box2);

  // Box
  shared_ptr<hittable> box1 =
      box(point3(0, 0, 0), point3(165, 330, 165), white);
  box1 = make_shared<rotate_y>(box1, 15);
  box1 = make_shared<translate>(box1, vec3(265, 0, 295));
  world.add(box1);

  // Glass Sphere
  auto glass = make_shared<dielectric>(1.5);
  auto light = make_shared<diffuse_light>(color(15, 15, 15));
  world.add(make_shared<sphere>(point3(190, 90, 190), 90, glass));

  hittable_list lights;
  lights.add(make_shared<quad>(point3(343, 554, 332), vec3(-130, 0, 0),
                               vec3(0, 0, -105), light));
  // lights.add(make_shared<sphere>(point3(190, 90, 190), 90, light));

  world.add(make_shared<quad>(point3(343, 554, 332), vec3(-130, 0, 0),
                              vec3(0, 0, -105), light));
  // world.add(make_shared<sphere>(point3(190, 90, 190), 90, light));
  camera cam;

  cam.aspect_ratio = 1.0;
  cam.image_width = 400;
  cam.samples_per_pixel = 50;
  cam.max_depth = 5;
  cam.background = color(0, 0, 0);

  cam.vfov = 40;
  cam.lookfrom = point3(278, 278, -800);
  cam.lookat = point3(278, 278, 0);
  cam.vup = vec3(0, 1, 0);

  cam.defocus_angle = 0;

  cam.render(world, lights);
  return 0;
}