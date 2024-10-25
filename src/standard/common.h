#pragma once

#include <stdint.h> // common types


/*
 * TYPES
 */
typedef int8_t      s8;
typedef int16_t     s16;
typedef int32_t     s32;
typedef int64_t     s64;

typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;
typedef uint64_t    u64;

typedef float       f32;
typedef double      f64;

typedef int32_t     b32;

static_assert(sizeof(u8) == 1, "Expected u8 to be 1 byte.");
static_assert(sizeof(u16) == 2, "Expected u16 to be 2 bytes.");
static_assert(sizeof(u32) == 4, "Expected u32 to be 4 bytes.");
static_assert(sizeof(u64) == 8, "Expected u64 to be 8 bytes.");

static_assert(sizeof(s8) == 1, "Expected s8 to be 1 byte.");
static_assert(sizeof(s16) == 2, "Expected s16 to be 2 bytes.");
static_assert(sizeof(s32) == 4, "Expected s32 to be 4 bytes.");
static_assert(sizeof(s64) == 8, "Expected s64 to be 8 bytes.");

static_assert(sizeof(f32) == 4, "Expected f32 to be 4 bytes.");
static_assert(sizeof(f64) == 8, "Expected f64 to be 8 bytes.");

#define s8_MIN  INT8_MIN
#define s16_MIN INT16_MIN
#define s32_MIN INT32_MIN
#define s64_MIN INT64_MIN
#define s8_MAX  INT8_MAX
#define s16_MAX INT16_MAX
#define s32_MAX INT32_MAX
#define s64_MAX INT64_MAX
#define U8_MAX  UINT8_MAX
#define U16_MAX UINT16_MAX
#define U32_MAX UINT32_MAX
#define U64_MAX UINT64_MAX

#define TiB(amount) ((amount) * 1024ULL * 1024ULL * 1024ULL * 1024ULL)
#define GiB(amount) ((amount) * 1024ULL * 1024ULL * 1024ULL)
#define MiB(amount) ((amount) * 1024ULL * 1024ULL)
#define KiB(amount) ((amount) * 1024ULL)

#define TB(amount) ((amount) * 1000ULL * 1000ULL * 1000ULL * 1000ULL)
#define GB(amount) ((amount) * 1000ULL * 1000ULL * 1000ULL)
#define MB(amount) ((amount) * 1000ULL * 1000ULL)
#define KB(amount) ((amount) * 1000ULL)

#define BIT(x) (1 << x)

/*
 * PLATFORM PAIN
 */
// trap pain
#ifdef STANDARD_PLATFORM_WINDOWS
    #define STANDARD_TRAP *(int*)0 = 0
#endif
#ifdef STANDARD_PLATFORM_LINUX
    #define STANDARD_TRAP __builtin_trap()
#endif

// inline pain
#ifdef STANDARD_PLATFORM_WINDOWS
    #define STANDARD_INLINE __forceinline
#endif
#ifdef STANDARD_PLATFORM_LINUX
    #define STANDARD_INLINE __attribute__((always_inline)) inline
#endif


/*
 * RESULT
 */
enum STANDARD_RESULT : u64{
    STANDARD_RESULT_SUCCESS = 0,
    STANDARD_RESULT_FAILURE = 1,
    STANDARD_RESULT_OS_ALLOCATION_FAILURE = 100,
    STANDARD_RESULT_OS_WINDOWING_FAILURE = 200,

    _STANDARD_RESULT_MAX
};


/*
 * ASSERTS
 */
#ifdef STANDARD_DEBUG_ASSERTS
    #define die() STANDARD_TRAP

    void assert_print(const char* expression, const char* message, const char* file, int line);

    #define STANDARD_ASSERT(expr)                                  \
        {                                                       \
            if (expr) {                                         \
            } else {                                            \
                assert_print(#expr, "", __FILE__, __LINE__);    \
                die();                                          \
            }                                                   \
        }

    #define STANDARD_ASSERT_MSG(expr, message)                         \
        {                                                           \
            if (expr) {                                             \
            } else {                                                \
                assert_print(#expr, message, __FILE__, __LINE__);   \
                die();                                              \
            }                                                       \
        }

    #else
        #define STANDARD_ASSERT(expr)
        #define STANDARD_ASSERT_MSG(expr, message)

#endif
