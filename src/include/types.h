// types.h
#pragma once

#include <stdlib.h>
#include <stdint.h>

// Unsigned integers
typedef uint8_t u8;
typedef struct { u8 x, y; } u8_2;
typedef struct { u8 x, y, z; } u8_3;
typedef struct { u8 x, y, w, h; } u8_4;

typedef uint16_t u16;
typedef struct { u16 x, y; } u16_2;
typedef struct { u16 x, y, z; } u16_3;
typedef struct { u16 x, y, w, h; } u16_4;

typedef uint32_t u32;
typedef struct { u32 x, y; } u32_2;
typedef struct { u32 x, y, z; } u32_3;
typedef struct { u32 x, y, w, h; } u32_4;

typedef uint64_t u64;
typedef struct { u64 x, y; } u64_2;
typedef struct { u64 x, y, z; } u64_3;
typedef struct { u64 x, y, w, h; } u64_4;

// Signed integers
typedef int8_t i8;
typedef struct { i8 x, y; } i8_2;
typedef struct { i8 x, y, z; } i8_3;
typedef struct { i8 x, y, w, h; } i8_4;

typedef int16_t i16;
typedef struct { i16 x, y; } i16_2;
typedef struct { i16 x, y, z; } i16_3;
typedef struct { i16 x, y, w, h; } i16_4;

typedef int32_t i32;
typedef struct { i32 x, y; } i32_2;
typedef struct { i32 x, y, z; } i32_3;
typedef struct { i32 x, y, w, h; } i32_4;

typedef int64_t i64;
typedef struct { i64 x, y; } i64_2;
typedef struct { i64 x, y, z; } i64_3;
typedef struct { i64 x, y, w, h; } i64_4;

// Floating point
typedef float f32;
typedef struct { f32 x, y; } f32_2;
typedef struct { f32 x, y, z; } f32_3;
typedef struct { f32 x, y, w, h; } f32_4;

typedef double f64;
typedef struct { f64 x, y; } f64_2;
typedef struct { f64 x, y, z; } f64_3;
typedef struct { f64 x, y, w, h; } f64_4;

// Colors
#define RED       ((SDL_Color){255, 0, 0, 255})
#define GREEN     ((SDL_Color){0, 255, 0, 255})
#define BLUE      ((SDL_Color){0, 0, 255, 255})

#define WHITE     ((SDL_Color){255, 255, 255, 255})
#define LIGHTGRAY ((SDL_Color){200, 200, 200, 255})
#define GRAY      ((SDL_Color){128, 128, 128, 255})
#define DARKGRAY  ((SDL_Color){30, 30, 30, 255})
#define BLACK     ((SDL_Color){0, 0, 0, 255})
