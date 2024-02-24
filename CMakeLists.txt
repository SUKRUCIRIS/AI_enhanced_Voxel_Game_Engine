cmake_minimum_required(VERSION 3.10)

project(opengl_voxel_gsu C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Release)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}$<0:>)

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(GLFW_VULKAN_STATIC OFF CACHE BOOL "" FORCE)

set(CGLM_SHARED OFF CACHE BOOL "" FORCE)
set(CGLM_STATIC ON CACHE BOOL "" FORCE)
set(CGLM_USE_C99 ON CACHE BOOL "" FORCE)
set(CGLM_USE_TEST OFF CACHE BOOL "" FORCE)

set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_NO_EXPORT ON CACHE BOOL "" FORCE)
set(USE_STATIC_CRT ON CACHE BOOL "" FORCE)

if(CMAKE_C_COMPILER_ID STREQUAL "MSVC")

	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")
	set(CMAKE_GENERATOR_PLATFORM x64)
  set(CMAKE_C_FLAGS_RELEASE "/O2 /Ob2 /Oi /Ot /Oy /GT /GL /MT /GS- /W4 /wd4324 /MP")
	set(CMAKE_CXX_FLAGS_RELEASE "/O2 /Ob2 /Oi /Ot /Oy /GT /GL /MT /GS- /W4 /wd4324 /MP")
  set(CMAKE_EXE_LINKER_FLAGS "/LTCG /machine:x64 /INCREMENTAL:NO /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup
	/NODEFAULTLIB:MSVCRT /NODEFAULTLIB:LIBCMT")
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)

	add_subdirectory(third_party/glfw)
	add_subdirectory(third_party/cglm)
	add_subdirectory(third_party/assimp)
	add_subdirectory(third_party/freetype)

	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party/cglm/include)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party/glfw/include)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party/opengl/include)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party/stb)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party/assimp/include)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/build/third_party/assimp/include)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party/freetype/include)

	FILE(GLOB_RECURSE MY_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
	add_executable(opengl_voxel_gsu ${MY_SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/third_party/opengl/src/glad.c)

	find_package(OpenGL REQUIRED)
	target_link_libraries(opengl_voxel_gsu cglm glfw assimp freetype msvcrt OpenGL::GL)
	target_compile_options(opengl_voxel_gsu PUBLIC /MT)

	set_target_properties(opengl_voxel_gsu PROPERTIES WIN32_EXECUTABLE TRUE)

else()

  message(FATAL_ERROR "You can compile this with MSVC on x64 windows computer only.")
	
endif()
