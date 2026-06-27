
cmake_minimum_required(VERSION 3.22)
project(TreatMeCppEngine CXX)
set(CMAKE_CXX_STANDARD 17)
add_executable(TreatMeCppEngine src/main.cpp)
# تم إزالة ربط المكتبات المعقدة مؤقتاً لتسهيل الـ Build الأولي للمشروع
