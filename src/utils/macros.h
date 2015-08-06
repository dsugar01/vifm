/* vifm
 * Copyright (C) 2011 xaizek.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifndef VIFM__UTILS__MACROS_H__
#define VIFM__UTILS__MACROS_H__

#include <stddef.h> /* size_t */

/* some useful macros */

/* for portable use of GNUC extensions (see Robert Love's book about system
 * programming in Linux) */
#if __GNUC__ > 3
#undef inline
#define inline inline     __attribute__ ((always_inline))
#define _gnuc_noinline    __attribute__ ((noinline))
#define _gnuc_pure        __attribute__ ((pure))
#define _gnuc_const       __attribute__ ((const))
#define _gnuc_noreturn    __attribute__ ((noreturn))
#define _gnuc_malloc      __attribute__ ((malloc))
#define _gnuc_must_check  __attribute__ ((must_check))
#define _gnuc_deprecated  __attribute__ ((deprecated))
#define _gnuc_used        __attribute__ ((used))
#define _gnuc_unused      __attribute__ ((unused))
#define _gnuc_packed      __attribute__ ((packed))
#define _gnuc_align(x)    __attribute__ ((aligned(x)))
#define _gnuc_align_max   __attribute__ ((aligned))
#define _gnuc_likely(x)   __builtin_expect (!!(x), 1)
#define _gnuc_unlikely(x) __builtin_expect (!!(x), 0)
#else
#define _gnuc_noinline
#define _gnuc_pure
#define _gnuc_const
#define _gnuc_noreturn
#define _gnuc_malloc
#define _gnuc_must_check
#define _gnuc_deprecated
#define _gnuc_used
#define _gnuc_unused
#define _gnuc_packed
#define _gnuc_align
#define _gnuc_align_max
#define _gnuc_likely(x)   (x)
#define _gnuc_unlikely(x) (x)
#endif

#define ARRAY_LEN(x) (sizeof(x)/sizeof((x)[0]))
#define ARRAY_GUARD(x, len) \
    typedef int x##_array_guard[(ARRAY_LEN(x) == (len)) ? 1 : -1]; \
    /* Fake use to suppress "Unused local variable" warning. */ \
    enum { x##_array_guard_fake_use = (size_t)(x##_array_guard*)0 }

#define MIN(a,b) ({ \
										typeof(a) _a = (a); \
										typeof(b) _b = (b); \
										(_a < _b) ? _a : _b; \
									})

#define MAX(a,b) ({ \
										typeof(a) _a = (a); \
										typeof(b) _b = (b); \
										(_a > _b) ? _a : _b; \
									})

#define DIV_ROUND_UP(a,b)  ({ \
															typeof(a) _a = (a); \
															typeof(b) _b = (b); \
															(_a + (_b - 1))/_b; \
														})

#define ROUND_DOWN(a,b)  ({ \
														typeof(a) _a = (a); \
														typeof(b) _b = (b); \
														_a - _a%_b; \
													})

/* Evaluates to "true" if all expressions are true.  Examples:
 * ALL(isdigit, '1', '9') == 1
 * ALL(isdigit, 'z', '9') == 0 */
#define ALL(f, ...) ALL_(f, COUNT(__VA_ARGS__), __VA_ARGS__)
#define ALL_(f, n, ...) ALL__(f, n, __VA_ARGS__)
#define ALL__(f, n, ...) comb##n((f), !!, &&, __VA_ARGS__)

/* Evaluates to "true" if any of expressions is true.  Examples:
 * ANY(isdigit, 'z', '9') == 1
 * ANY(isdigit, 'z', 'a') == 0 */
#define ANY(f, ...) ({ ANY_(f, COUNT(__VA_ARGS__), __VA_ARGS__); })
#define ANY_(f, n, ...) ANY__(f, n, __VA_ARGS__)
#define ANY__(f, n, ...) comb##n((f), !!, ||, __VA_ARGS__)

/* Evaluates to "true" if none of expressions is true.  Examples:
 * NONE(isdigit, 'z', 'a') == 1
 * NONE(isdigit, '1', '9') == 0 */
#define NONE(f, ...) ({ NONE_(f, COUNT(__VA_ARGS__), __VA_ARGS__); })
#define NONE_(f, n, ...) NONE__(f, n, __VA_ARGS__)
#define NONE__(f, n, ...) comb##n((f), !, &&, __VA_ARGS__)

/* Makes bit mask from a list of bits. */
#define MASK(...) MASK_(COUNT(__VA_ARGS__), __VA_ARGS__)
#define MASK_(n, ...) MASK__(n, __VA_ARGS__)
#define MASK__(n, ...) comb##n(<<, 1, |, __VA_ARGS__)

/* Neat trick to obtain number number of variadic arguments. */
#define COUNT(...) COUNT_(__VA_ARGS__, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define COUNT_(_1, _2, _3, _4, _5, _6, _7, _8, _9, n, ...) n

/* Helper macros to process variable arguments of macros. */
#define comb9(f, n, op, val, ...) (n f(val) op comb8(f, n, op, __VA_ARGS__))
#define comb8(f, n, op, val, ...) (n f(val) op comb7(f, n, op, __VA_ARGS__))
#define comb7(f, n, op, val, ...) (n f(val) op comb6(f, n, op, __VA_ARGS__))
#define comb6(f, n, op, val, ...) (n f(val) op comb5(f, n, op, __VA_ARGS__))
#define comb5(f, n, op, val, ...) (n f(val) op comb4(f, n, op, __VA_ARGS__))
#define comb4(f, n, op, val, ...) (n f(val) op comb3(f, n, op, __VA_ARGS__))
#define comb3(f, n, op, val, ...) (n f(val) op comb2(f, n, op, __VA_ARGS__))
#define comb2(f, n, op, val, ...) (n f(val) op comb1(f, n, op, __VA_ARGS__))
#define comb1(f, n, op, val)      (n f(val))

#endif /* VIFM__UTILS__MACROS_H__ */

/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 noexpandtab cinoptions-=(0 : */
/* vim: set cinoptions+=t0 filetype=c : */
