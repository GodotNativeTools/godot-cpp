#ifndef DEFS_H
#define DEFS_H

#include <GodotGlobal.hpp>

// alloca() is non-standard. When using MSVC, it's in malloc.h.
#if defined(__linux__) || defined(__APPLE__)
#include <alloca.h>
#else
#include <malloc.h>
#endif

#ifdef REAL_T_IS_DOUBLE
typedef double real_t;
#else
typedef float real_t;
#endif

// This epsilon should match the one used by Godot for consistency.
// Using `f` when `real_t` is float.
#define CMP_EPSILON 0.00001f
#define CMP_EPSILON2 (CMP_EPSILON * CMP_EPSILON)

// This epsilon is for values related to a unit size (scalar or vector len).
#ifdef PRECISE_MATH_CHECKS
#define UNIT_EPSILON 0.00001
#else
// Tolerate some more floating point error normally.
#define UNIT_EPSILON 0.001
#endif

#define Math_SQRT12 0.7071067811865475244008443621048490
#define Math_SQRT2 1.4142135623730950488016887242
#define Math_LN2 0.6931471805599453094172321215
#define Math_PI 3.1415926535897932384626433833
#define Math_TAU 6.2831853071795864769252867666
#define Math_E 2.7182818284590452353602874714
#define Math_INF INFINITY
#define Math_NAN NAN

#define _PLANE_EQ_DOT_EPSILON 0.999
#define _PLANE_EQ_D_EPSILON 0.0001

#ifdef __GNUC__
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
#else
#define likely(x) x
#define unlikely(x) x
#endif

// Generic swap template.
#ifndef SWAP
#define SWAP(m_x, m_y) __swap_tmpl((m_x), (m_y))
template <class T>
inline void __swap_tmpl(T &x, T &y) {
	T aux = x;
	x = y;
	y = aux;
}
#endif // SWAP

// Don't use this directly; instead, use any of the CRASH_* macros
#ifdef _MSC_VER
#define GENERATE_TRAP                       \
	__debugbreak();                         \
	/* Avoid warning about control paths */ \
	for (;;) {                              \
	}
#else
#define GENERATE_TRAP __builtin_trap();
#endif

// ERR/WARN macros
#ifndef WARN_PRINT
#define WARN_PRINT(msg) godot::Godot::print_warning(msg, __func__, __FILE__, __LINE__)
#endif

#ifndef WARN_PRINTS
#define WARN_PRINTS(msg) WARN_PRINT((msg).utf8().get_data())
#endif

#ifndef ERR_PRINT
#define ERR_PRINT(msg) godot::Godot::print_error(msg, __func__, __FILE__, __LINE__)
#endif

#ifndef ERR_PRINTS
#define ERR_PRINTS(msg) ERR_PRINT((msg).utf8().get_data())
#endif

#ifndef FATAL_PRINT
#define FATAL_PRINT(msg) ERR_PRINT(godot::String("FATAL: ") + (msg))
#endif

#ifndef ERR_MSG_INDEX
#define ERR_MSG_INDEX(index, size) (godot::String("Index ") + #index + "=" + godot::String((int64_t)index) + " out of size (" + #size + "=" + godot::String((int64_t)size) + ")")
#endif

#ifndef ERR_MSG_NULL
#define ERR_MSG_NULL(param) (godot::String("Parameter '") + #param + "' is null.")
#endif

#ifndef ERR_MSG_COND
#define ERR_MSG_COND(cond) (godot::String("Condition '") + #cond + "' is true.")
#endif

#ifndef ERR_FAIL_INDEX
#define ERR_FAIL_INDEX(index, size)                       \
	do {                                                  \
		if (unlikely((index) < 0 || (index) >= (size))) { \
			ERR_PRINT(ERR_MSG_INDEX(index, size));        \
			return;                                       \
		}                                                 \
	} while (0)
#endif

#ifndef ERR_FAIL_INDEX_V
#define ERR_FAIL_INDEX_V(index, size, ret)                \
	do {                                                  \
		if (unlikely((index) < 0 || (index) >= (size))) { \
			ERR_PRINT(ERR_MSG_INDEX(index, size));        \
			return ret;                                   \
		}                                                 \
	} while (0)
#endif

#ifndef ERR_FAIL_UNSIGNED_INDEX_V
#define ERR_FAIL_UNSIGNED_INDEX_V(index, size, ret) \
	do {                                            \
		if (unlikely((index) >= (size))) {          \
			ERR_PRINT(ERR_MSG_INDEX(index, size));  \
			return ret;                             \
		}                                           \
	} while (0)
#endif

#ifndef CRASH_BAD_INDEX
#define CRASH_BAD_INDEX(index, size)                      \
	do {                                                  \
		if (unlikely((index) < 0 || (index) >= (size))) { \
			FATAL_PRINT(ERR_MSG_INDEX(index, size));      \
			GENERATE_TRAP;                                \
		}                                                 \
	} while (0)
#endif

#ifndef ERR_FAIL_NULL
#define ERR_FAIL_NULL(param)                \
	do {                                    \
		if (unlikely(!param)) {             \
			ERR_PRINT(ERR_MSG_NULL(param)); \
			return;                         \
		}                                   \
	} while (0)
#endif

#ifndef ERR_FAIL_NULL_V
#define ERR_FAIL_NULL_V(param, ret)         \
	do {                                    \
		if (unlikely(!param)) {             \
			ERR_PRINT(ERR_MSG_NULL(param)); \
			return ret;                     \
		}                                   \
	} while (0)
#endif

#ifndef ERR_FAIL_COND
#define ERR_FAIL_COND(cond)                \
	do {                                   \
		if (unlikely(cond)) {              \
			ERR_PRINT(ERR_MSG_COND(cond)); \
			return;                        \
		}                                  \
	} while (0)
#endif

#ifndef CRASH_COND
#define CRASH_COND(cond)                     \
	do {                                     \
		if (unlikely(cond)) {                \
			FATAL_PRINT(ERR_MSG_COND(cond)); \
			return;                          \
		}                                    \
	} while (0)
#endif

#ifndef ERR_FAIL_COND_V
#define ERR_FAIL_COND_V(cond, ret)         \
	do {                                   \
		if (unlikely(cond)) {              \
			ERR_PRINT(ERR_MSG_COND(cond)); \
			return ret;                    \
		}                                  \
	} while (0)
#endif

#ifndef ERR_CONTINUE
#define ERR_CONTINUE(cond)                 \
	{                                      \
		if (unlikely(cond)) {              \
			ERR_PRINT(ERR_MSG_COND(cond)); \
			continue;                      \
		}                                  \
	}
#endif

#ifndef ERR_BREAK
#define ERR_BREAK(cond)                    \
	{                                      \
		if (unlikely(cond)) {              \
			ERR_PRINT(ERR_MSG_COND(cond)); \
			break;                         \
		}                                  \
	}
#endif

#ifndef ERR_FAIL
#define ERR_FAIL()                            \
	do {                                      \
		ERR_PRINT("Method/Function Failed."); \
		return;                               \
	} while (0)
#endif

#ifndef ERR_FAIL_V
#define ERR_FAIL_V(ret)                       \
	do {                                      \
		ERR_PRINT("Method/Function Failed."); \
		return ret;                           \
	} while (0)
#endif

#ifndef CRASH_NOW
#define CRASH_NOW()                             \
	do {                                        \
		FATAL_PRINT("Method/Function Failed."); \
		GENERATE_TRAP;                          \
	} while (0)
#endif

#endif // DEFS_H
