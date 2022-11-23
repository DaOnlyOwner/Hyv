cmake_minimum_required(VERSION 3.8)
project(Hyv)

if(WIN32)
	set(PLATFORM "PLATFORM_WIN32")
elseif(LINUX)
	set(PLATFORM "PLATFORM_LINUX")
elseif(APPLE)
	set(PLATFORM "PLATFORM_MACOS")
else()
	message(FATAL_ERROR "target platform unknown")
endif()

message(STATUS "building for ${PLATFORM}")

file(GLOB_RECURSE SRC src/*.cpp)
file(GLOB_RECURSE INC include/*.h)
	
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)

add_executable(Hyv ${SRC} ${INC} dev/dev.cpp)

conan_basic_setup(Hyv)
target_compile_features(Hyv PUBLIC cxx_std_17)

target_compile_definitions(Hyv PRIVATE ${PLATFORM})
target_link_libraries(Hyv ${CONAN_LIBS})
target_include_directories(Hyv PUBLIC include)