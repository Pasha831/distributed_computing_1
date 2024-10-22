cmake_minimum_required(VERSION 3.23)
project(PeplinLabs)

set(CMAKE_CXX_STANDARD 23)

add_executable(PeplinLabs mandelbrot.cpp monte_carlo.cpp second_task.cpp my_rand.cpp my_rand.h
        timer.h rwlock_readers.h rwlock_writers.h pth_ll_rwl.cpp)
