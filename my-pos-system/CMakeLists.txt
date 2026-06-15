
cmake_minimum_required(VERSION 3.10)
project(EnterprisePOS)

set(CMAKE_CXX_STANDARD 17)

# تضمين مجلد ملفات الرأس
include_directories(include)

# تجميع كافة ملفات التنفيذ المصدرية
file(GLOB SOURCES "src/*.cpp")

add_executable(EnterprisePOS ${SOURCES})

# ربط نظام التشغيل بمكتبة SQLite3 المتواجدة بالجهاز
find_package(SQLite3 REQUIRED)
target_link_libraries(EnterprisePOS PRIVATE SQLite3::SQLite3)
