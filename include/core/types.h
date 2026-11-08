#pragma once


// ==================================================
// unsigned integers
// ==================================================
using u8 = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;

static_assert(sizeof(u8) == 1);
static_assert(sizeof(u16) == 2);
static_assert(sizeof(u32) == 4);
static_assert(sizeof(u64) == 8);


// ==================================================
// signed integers
// ==================================================
using s8 = signed char;
using s16 = signed short;
using s32 = signed int;
using s64 = signed long long;

static_assert(sizeof(s8) == 1);
static_assert(sizeof(s16) == 2);
static_assert(sizeof(s32) == 4);
static_assert(sizeof(s64) == 8);


// ==================================================
// decimals
// ==================================================
using f32 = float;
using f64 = double;

static_assert(sizeof(f32) == 4);
static_assert(sizeof(f64) == 8);


// ==================================================
// characters
// ==================================================
using c8 = char;
using c16 = wchar_t;

static_assert(sizeof(c8) == 1);
static_assert(sizeof(c16) == 2);


// ==================================================
// boolean
// ==================================================
using b8 = bool;

static_assert(sizeof(b8) == 1);


// ==================================================
// includes
// ==================================================
//TODO:  check for avx2 support
#include <immintrin.h>
#include <avxintrin.h>
#include <cstdio>
#ifndef NDEBUG
#include <cassert>
#endif
#include <vulkan/vulkan.h>
#include <windows.h>
#include <core/memory.h>
#include <core/dll.h>
#include <core/log.h>
#include <core/settings.h>
#include <core/window.h>
#include <core/resource/shader.h>
#include <core/resource/graphics.h>
#include <core/graphics/vulkan.h>
#include <core/graphics/gpu_info.h>
#include <core/graphics/renderer.h>
#include <core/file.h>
