cmake_minimum_required(VERSION 4.2)
project(Delta VERSION 1.0 LANGUAGES CXX)


# ==================================================
# C++ CONFIGURATION
# ==================================================
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# ==================================================
# SOURCE FILES
# ==================================================
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_executable(Delta ${SOURCES}
        include/core/types.h
        src/core/graphics/vulkan.cpp
        src/core/window.cpp
        include/core/settings.h
        include/core/graphics/renderer.h
        include/core/graphics/vulkan.h
        include/core/log.h
        src/core/log.cpp
        include/core/dll.h
        src/core/dll.cpp
        src/core/graphics/renderer.cpp
        src/core/settings.cpp
        include/core/graphics/gpu_info.h
        src/core/graphics/gpu_info.cpp
        src/core/graphics/gpu.cpp
        include/core/memory.h
        src/core/memory/linear_block.cpp
        src/core/memory/memory.cpp
        src/core/graphics/render_target.cpp
        include/core/window.h
        src/core/graphics/frame.cpp
        src/core/memory/resource_pool.cpp
        include/core/resource/graphics.h
        src/core/resource/image.cpp
        src/core/resource/shader.cpp
        src/core/resource/image_sampler.cpp
        src/core/memory/gpu_block.cpp
        src/core/memory/array.cpp
        src/core/resource/pipeline.cpp
        src/core/resource/buffer.cpp
        src/core/resource/pipeline_layout.cpp
        src/core/resource/descriptor_set_layout.cpp
        include/core/resource/shader.h
        include/core/file.h
        src/core/file/base.cpp
        include/core/math.h
        src/core/resource/material.cpp
        src/core/math/matrix4x4.cpp
        src/core/math/vector4.cpp
        src/core/math/quaternion.cpp
)


target_include_directories(Delta PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)


# ==================================================
# VULKAN
# ==================================================
find_package(Vulkan REQUIRED)


target_link_libraries(Delta PRIVATE
        Vulkan::Vulkan
)


# ==================================================
# WINDOWS API
# ==================================================
target_compile_definitions(Delta PRIVATE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
        UNICODE
        _UNICODE
        VK_USE_PLATFORM_WIN32_KHR
        VK_NO_PROTOTYPES
)


target_link_libraries(Delta PRIVATE
)


# ==================================================
# COMPILER SETTINGS
# ==================================================
target_compile_options(Delta PRIVATE
        -Wall
        -Wextra
        -Wconversion
        -Wshadow
        -Wno-unused-variable
        -Werror=return-type

        -fno-rtti
        -fno-exceptions
        -fno-threadsafe-statics

        -fno-use-cxa-atexit

        -ffunction-sections
        -fdata-sections
)


target_compile_options(Delta PRIVATE -mavx2 -msse4.1)


target_compile_options(Delta PRIVATE
        $<$<CONFIG:Debug>:-g>
        $<$<CONFIG:Debug>:-O0>
)


target_compile_options(Delta PRIVATE
        $<$<CONFIG:Release>:-O3>
        $<$<CONFIG:Release>:-fomit-frame-pointer>
)


target_link_options(Delta PRIVATE
        $<$<CONFIG:Release>:-Wl,--gc-sections>
)


# ==================================================
# BUILD CONFIGURATIONS
# ==================================================
target_compile_definitions(Delta PRIVATE
        $<$<CONFIG:Debug>:DEBUG_MODE>
        $<$<CONFIG:Release>:NDEBUG>
)


# ==================================================
# OUTPUT DIRECTORIES
# ==================================================
set_target_properties(Delta PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/Debug"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/Release"
)