#!/usr/bin/bash

cd ./build/
# 修改 ./RayTracingTheNextWeek 后面的参数选择需要渲染的场景，合法的参数为 [0-9] 共10个场景
cmake .. && make && ./RayTracingTheNextWeek 0 > image.ppm
cd ..

