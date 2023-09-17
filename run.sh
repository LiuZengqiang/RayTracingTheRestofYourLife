#!/usr/bin/bash

cd ./build/
cmake .. && make && ./RayTracingTheRestOfYourLife > image.ppm
cd ..
