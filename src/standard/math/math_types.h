#pragma once
#include "../common.h"

/*
** Scalar
*/

#define PI_F32 3.14159265358979323846f
#define PI_F64 3.14159265358979323846

/*
** Vector
*/

typedef struct f32vec2 f32vec2;
typedef struct f64vec2 f64vec2;
typedef struct s8vec2 s8vec2;
typedef struct s16vec2 s16vec2;
typedef struct s32vec2 s32vec2;
typedef struct s64vec2 s64vec2;
typedef struct u8vec2 u8vec2;
typedef struct u16vec2 u16vec2;
typedef struct u32vec2 u32vec2;
typedef struct u64vec2 u64vec2;

typedef struct f32vec3 f32vec3;
typedef struct f64vec3 f64vec3;
typedef struct s8vec3 s8vec3;
typedef struct s16vec3 s16vec3;
typedef struct s32vec3 s32vec3;
typedef struct s64vec3 s64vec3;
typedef struct u8vec3 u8vec3;
typedef struct u16vec3 u16vec3;
typedef struct u32vec3 u32vec3;
typedef struct u64vec3 u64vec3;

typedef struct f32vec4 f32vec4;
typedef struct f64vec4 f64vec4;
typedef struct s8vec4 s8vec4;
typedef struct s16vec4 s16vec4;
typedef struct s32vec4 s32vec4;
typedef struct s64vec4 s64vec4;
typedef struct u8vec4 u8vec4;
typedef struct u16vec4 u16vec4;
typedef struct u32vec4 u32vec4;
typedef struct u64vec4 u64vec4;

#define f32vec2(x, y)			((f32vec2){ x, y })
#define f64vec2(x, y)			((f64vec2){ x, y })
#define s8vec2(x, y)			((s8vec2){ x, y })
#define s16vec2(x, y)			((s16vec2){ x, y })
#define s32vec2(x, y)			((s32vec2){ x, y })
#define s64vec2(x, y)			((s64vec2){ x, y })
#define u8vec2(x, y)			((u8vec2){ x, y })
#define u16vec2(x, y)			((u16vec2){ x, y })
#define u32vec2(x, y)			((u32vec2){ x, y })
#define u64vec2(x, y)			((u64vec2){ x, y })

#define f32vec3(x, y, z)		((f32vec3){ x, y, z })
#define f64vec3(x, y, z)		((f64vec3){ x, y, z })
#define s8vec3(x, y, z)			((s8vec3){ x, y, z })
#define s16vec3(x, y, z)		((s16vec3){ x, y, z })
#define s32vec3(x, y, z)		((s32vec3){ x, y, z })
#define s64vec3(x, y, z)		((s64vec3){ x, y, z })
#define u8vec3(x, y, z)			((u8vec3){ x, y, z })
#define u16vec3(x, y, z)		((u16vec3){ x, y, z })
#define u32vec3(x, y, z)		((u32vec3){ x, y, z })
#define u64vec3(x, y, z)		((u64vec3){ x, y, z })

#define f32vec4(x, y, z, w)		((f32vec4){ x, y, z, w })
#define f64vec4(x, y, z, w)		((f64vec4){ x, y, z, w })
#define s8vec4(x, y, z, w)		((s8vec4){ x, y, z, w })
#define s16vec4(x, y, z, w)		((s16vec4){ x, y, z, w })
#define s32vec4(x, y, z, w)		((s32vec4){ x, y, z, w })
#define s64vec4(x, y, z, w)		((s64vec4){ x, y, z, w })
#define u8vec4(x, y, z, w)		((u8vec4){ x, y, z, w })
#define u16vec4(x, y, z, w)		((u16vec4){ x, y, z, w })
#define u32vec4(x, y, z, w)		((u32vec4){ x, y, z, w })
#define u64vec4(x, y, z, w)		((u64vec4){ x, y, z, w })

struct f32vec2 {
	union {
		struct { f32 x; f32 y; };
		struct { f32 r; f32 g; };
		struct { f32 u; f32 v; };
	};
};

struct f64vec2 {
	union {
		struct { f64 x; f64 y; };
		struct { f64 r; f64 g; };
		struct { f64 u; f64 v; };
	};
};

struct s8vec2 {
	union {
		struct { s8 x; s8 y; };
		struct { s8 r; s8 g; };
		struct { s8 u; s8 v; };
	};
};

struct s16vec2 {
	union {
		struct { s16 x; s16 y; };
		struct { s16 r; s16 g; };
		struct { s16 u; s16 v; };
	};
};

struct s32vec2 {
	union {
		struct { s32 x; s32 y; };
		struct { s32 r; s32 g; };
		struct { s32 u; s32 v; };
	};
};

struct s64vec2 {
	union {
		struct { s64 x; s64 y; };
		struct { s64 r; s64 g; };
		struct { s64 u; s64 v; };
	};
};

struct u8vec2 {
	union {
		struct { u8 x; u8 y; };
		struct { u8 r; u8 g; };
		struct { u8 u; u8 v; };
	};
};

struct u16vec2 {
	union {
		struct { u16 x; u16 y; };
		struct { u16 r; u16 g; };
		struct { u16 u; u16 v; };
	};
};

struct u32vec2 {
	union {
		struct { u32 x; u32 y; };
		struct { u32 r; u32 g; };
		struct { u32 u; u32 v; };
	};
};

struct u64vec2 {
	union {
		struct { u64 x; u64 y; };
		struct { u64 r; u64 g; };
		struct { u64 u; u64 v; };
	};
};


struct f32vec3 {
	union {
		struct { f32 x; f32 y; f32 z; };
		struct { f32 r; f32 g; f32 b; };
		struct { f32 u; f32 v; f32 w; };
        f32vec2 xy;
        f32vec2 rg;
        f32vec2 uv;
	};
};

struct f64vec3 {
	union {
		struct { f64 x; f64 y; f64 z; };
		struct { f64 r; f64 g; f64 b; };
		struct { f64 u; f64 v; f64 w; };
        f64vec2 xy;
        f64vec2 rg;
        f64vec2 uv;
	};
};

struct s8vec3 {
	union {
		struct { s8 x; s8 y; s8 z; };
		struct { s8 r; s8 g; s8 b; };
		struct { s8 u; s8 v; s8 w; };
        s8vec2 xy;
        s8vec2 rg;
        s8vec2 uv;
	};
};

struct s16vec3 {
	union {
		struct { s16 x; s16 y; s16 z; };
		struct { s16 r; s16 g; s16 b; };
		struct { s16 u; s16 v; s16 w; };
        s16vec2 xy;
        s16vec2 rg;
        s16vec2 uv;
	};
};

struct s32vec3 {
	union {
		struct { s32 x; s32 y; s32 z; };
		struct { s32 r; s32 g; s32 b; };
		struct { s32 u; s32 v; s32 w; };
        s32vec2 xy;
        s32vec2 rg;
        s32vec2 uv;
	};
};

struct s64vec3 {
	union {
		struct { s64 x; s64 y; s64 z; };
		struct { s64 r; s64 g; s64 b; };
		struct { s64 u; s64 v; s64 w; };
        s64vec2 xy;
        s64vec2 rg;
        s64vec2 uv;
	};
};

struct u8vec3 {
	union {
		struct { u8 x; u8 y; u8 z; };
		struct { u8 r; u8 g; u8 b; };
		struct { u8 u; u8 v; u8 w; };
        u8vec2 xy;
        u8vec2 rg;
        u8vec2 uv;
	};
};

struct u16vec3 {
	union {
		struct { u16 x; u16 y; u16 z; };
		struct { u16 r; u16 g; u16 b; };
		struct { u16 u; u16 v; u16 w; };
        u16vec2 xy;
        u16vec2 rg;
        u16vec2 uv;
	};
};

struct u32vec3 {
	union {
		struct { u32 x; u32 y; u32 z; };
		struct { u32 r; u32 g; u32 b; };
		struct { u32 u; u32 v; u32 w; };
        u32vec2 xy;
        u32vec2 rg;
        u32vec2 uv;
	};
};

struct u64vec3 {
	union {
		struct { u64 x; u64 y; u64 z; };
		struct { u64 r; u64 g; u64 b; };
		struct { u64 u; u64 v; u64 w; };
        u64vec2 xy;
        u64vec2 rg;
        u64vec2 uv;
	};
};


struct f32vec4 {
	union {
		struct { f32 x; f32 y; f32 z; f32 w; };
		struct { f32 r; f32 g; f32 b; f32 a; };
		struct { f32vec2 xy; f32vec2 zw; };
		struct { f32vec2 rg; f32vec2 ba; };
		f32vec3 xyz;
		f32vec3 rgb;
	};
};

struct f64vec4 {
	union {
		struct { f64 x; f64 y; f64 z; f64 w; };
		struct { f64 r; f64 g; f64 b; f64 a; };
		struct { f64vec2 xy; f64vec2 zw; };
		struct { f64vec2 rg; f64vec2 ba; };
		f64vec3 xyz;
		f64vec3 rgb;
	};
};

struct s8vec4 {
	union {
		struct { s8 x; s8 y; s8 z; s8 w; };
		struct { s8 r; s8 g; s8 b; s8 a; };
		struct { s8vec2 xy; s8vec2 zw; };
		struct { s8vec2 rg; s8vec2 ba; };
		s8vec3 xyz;
		s8vec3 rgb;
	};
};

struct s16vec4 {
	union {
		struct { s16 x; s16 y; s16 z; s16 w; };
		struct { s16 r; s16 g; s16 b; s16 a; };
		struct { s16vec2 xy; s16vec2 zw; };
		struct { s16vec2 rg; s16vec2 ba; };
		s16vec3 xyz;
		s16vec3 rgb;
	};
};

struct s32vec4 {
	union {
		struct { s32 x; s32 y; s32 z; s32 w; };
		struct { s32 r; s32 g; s32 b; s32 a; };
		struct { s32vec2 xy; s32vec2 zw; };
		struct { s32vec2 rg; s32vec2 ba; };
		s32vec3 xyz;
		s32vec3 rgb;
	};
};

struct s64vec4 {
	union {
		struct { s64 x; s64 y; s64 z; s64 w; };
		struct { s64 r; s64 g; s64 b; s64 a; };
		struct { s64vec2 xy; s64vec2 zw; };
		struct { s64vec2 rg; s64vec2 ba; };
		s64vec3 xyz;
		s64vec3 rgb;
	};
};

struct u8vec4 {
	union {
		struct { u8 x; u8 y; u8 z; u8 w; };
		struct { u8 r; u8 g; u8 b; u8 a; };
		struct { u8vec2 xy; u8vec2 zw; };
		struct { u8vec2 rg; u8vec2 ba; };
		u8vec3 xyz;
		u8vec3 rgb;
	};
};

struct u16vec4 {
	union {
		struct { u16 x; u16 y; u16 z; u16 w; };
		struct { u16 r; u16 g; u16 b; u16 a; };
		struct { u16vec2 xy; u16vec2 zw; };
		struct { u16vec2 rg; u16vec2 ba; };
		u16vec3 xyz;
		u16vec3 rgb;
	};
};

struct u32vec4 {
	union {
		struct { u32 x; u32 y; u32 z; u32 w; };
		struct { u32 r; u32 g; u32 b; u32 a; };
		struct { u32vec2 xy; u32vec2 zw; };
		struct { u32vec2 rg; u32vec2 ba; };
		u32vec3 xyz;
		u32vec3 rgb;
	};
};

struct u64vec4 {
	union {
		struct { u64 x; u64 y; u64 z; u64 w; };
		struct { u64 r; u64 g; u64 b; u64 a; };
		struct { u64vec2 xy; u64vec2 zw; };
		struct { u64vec2 rg; u64vec2 ba; };
		u64vec3 xyz;
		u64vec3 rgb;
	};
};



/*
** Matrix (GLSL style column-major order)
*/

typedef struct f32vec2x2 f32vec2x2;
typedef struct f64vec2x2 f64vec2x2;
typedef struct f32vec2x3 f32vec2x3;
typedef struct f64vec2x3 f64vec2x3;
typedef struct f32vec2x4 f32vec2x4;
typedef struct f64vec2x4 f64vec2x4;
typedef struct f32vec3x2 f32vec3x2;
typedef struct f64vec3x2 f64vec3x2;
typedef struct f32vec3x3 f32vec3x3;
typedef struct f64vec3x3 f64vec3x3;
typedef struct f32vec3x4 f32vec3x4;
typedef struct f64vec3x4 f64vec3x4;
typedef struct f32vec4x2 f32vec4x2;
typedef struct f64vec4x2 f64vec4x2;
typedef struct f32vec4x3 f32vec4x3;
typedef struct f64vec4x3 f64vec4x3;
typedef struct f32vec4x4 f32vec4x4;
typedef struct f64vec4x4 f64vec4x4;

typedef f32vec2x2 f32mat2;
typedef f32vec3x3 f32mat3;
typedef f32vec4x4 f32mat4;
typedef f64vec2x2 f64mat2;
typedef f64vec3x3 f64mat3;
typedef f64vec4x4 f64mat4;

struct f32vec2x2 { f32vec2 column[2]; };
struct f64vec2x2 { f64vec2 column[2]; };
struct f32vec2x3 { f32vec2 column[3]; };
struct f64vec2x3 { f64vec2 column[3]; };
struct f32vec2x4 { f32vec2 column[4]; };
struct f64vec2x4 { f64vec2 column[4]; };
struct f32vec3x2 { f32vec3 column[2]; };
struct f64vec3x2 { f64vec3 column[2]; };
struct f32vec3x3 { f32vec3 column[3]; };
struct f64vec3x3 { f64vec3 column[3]; };
struct f32vec3x4 { f32vec3 column[4]; };
struct f64vec3x4 { f64vec3 column[4]; };
struct f32vec4x2 { f32vec4 column[2]; };
struct f64vec4x2 { f64vec4 column[2]; };
struct f32vec4x3 { f32vec4 column[3]; };
struct f64vec4x3 { f64vec4 column[3]; };
struct f32vec4x4 { f32vec4 column[4]; };
struct f64vec4x4 { f64vec4 column[4]; };

#define IDENTITY_f32vec2x2 ((f32vec2x2) { .column[0].x = 1.f, .column[1].y = 1.f })
#define IDENTITY_f64vec2x2 ((f64vec2x2) { .column[0].x = 1.f, .column[1].y = 1.f })
#define IDENTITY_f32vec2x3 ((f32vec2x3) { .column[0].x = 1.f, .column[1].y = 1.f })
#define IDENTITY_f64vec2x3 ((f64vec2x3) { .column[0].x = 1.f, .column[1].y = 1.f })
#define IDENTITY_f32vec2x4 ((f32vec2x4) { .column[0].x = 1.f, .column[1].y = 1.f })
#define IDENTITY_f64vec2x4 ((f64vec2x4) { .column[0].x = 1.f, .column[1].y = 1.f })
#define IDENTITY_f32vec3x2 ((f32vec3x2) { .column[0].x = 1.f, .column[1].y = 1.f })
#define IDENTITY_f64vec3x2 ((f64vec3x2) { .column[0].x = 1.f, .column[1].y = 1.f })
#define IDENTITY_f32vec3x3 ((f32vec3x3) { .column[0].x = 1.f, .column[1].y = 1.f, .column[2].z = 1.f })
#define IDENTITY_f64vec3x3 ((f64vec3x3) { .column[0].x = 1.f, .column[1].y = 1.f, .column[2].z = 1.f })
#define IDENTITY_f32vec3x4 ((f32vec3x4) { .column[0].x = 1.f, .column[1].y = 1.f, .column[2].z = 1.f })
#define IDENTITY_f64vec3x4 ((f64vec3x4) { .column[0].x = 1.f, .column[1].y = 1.f, .column[2].z = 1.f })
#define IDENTITY_f32vec4x2 ((f32vec4x2) { .column[0].x = 1.f, .column[1].y = 1.f })
#define IDENTITY_f64vec4x2 ((f64vec4x2) { .column[0].x = 1.f, .column[1].y = 1.f })
#define IDENTITY_f32vec4x3 ((f32vec4x3) { .column[0].x = 1.f, .column[1].y = 1.f, .column[2].z = 1.f })
#define IDENTITY_f64vec4x3 ((f64vec4x3) { .column[0].x = 1.f, .column[1].y = 1.f, .column[2].z = 1.f })
#define IDENTITY_f32vec4x4 ((f32vec4x4) { .column[0].x = 1.f, .column[1].y = 1.f, .column[2].z = 1.f, .column[3].w = 1.f })
#define IDENTITY_f64vec4x4 ((f64vec4x4) { .column[0].x = 1.f, .column[1].y = 1.f, .column[2].z = 1.f, .column[3].w = 1.f })

#define IDENTITY_f32mat2 IDENTITY_f32vec2x2
#define IDENTITY_f32mat3 IDENTITY_f32vec3x3
#define IDENTITY_f32mat4 IDENTITY_f32vec4x4
#define IDENTITY_f64mat2 IDENTITY_f64vec2x2
#define IDENTITY_f64mat3 IDENTITY_f64vec3x3
#define IDENTITY_f64mat4 IDENTITY_f64vec4x4
