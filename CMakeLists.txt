cmake_minimum_required(VERSION 3.8)
project(Hyv)


file(GLOB_RECURSE SRC src/*.cpp)
file(GLOB_RECURSE INC include/*.h)
	
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)

add_executable(Hyv ${SRC} ${INC} dev/dev.cpp)

conan_basic_setup(Hyv)
target_compile_features(Hyv PUBLIC cxx_std_17)
target_link_libraries(Hyv ${CONAN_LIBS})