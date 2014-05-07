
#ifndef _FIZZBUZZWHIZZ_COMMON_H_
#define _FIZZBUZZWHIZZ_COMMON_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* 是否把结果输出到屏幕上或输出到字符串数组里, 如果输出到屏幕, 那么我们只计算和显示一次 */
#define DISPLAY_RESULT_TO_SCREEN        0

/* 单次FizzBuzzWhizz计算内部是否使用stop_watch计时, 因为计时也是耗时间的, 如无必要, 需禁用之 */
#define FIZZ_NATIVE_USE_STOPWATCH       1

/* 如果结果不输出到屏幕, 则内部计时是不需要的 */
#if !defined(DISPLAY_RESULT_TO_SCREEN) || (DISPLAY_RESULT_TO_SCREEN == 0)
#undef  FIZZ_NATIVE_USE_STOPWATCH
#define FIZZ_NATIVE_USE_STOPWATCH       0
#endif

/* 测试最大迭代次数 */
#define MAX_LOOP_TIME                   10000

/* 无效的特殊报数 */
#define INVALID_SPECIAL_NUM             (-1)

/* 非特殊报数索引 */
#define NORMAL_NUM_INDEX                0

/* 第一个特殊报数的索引值 */
#define FIRST_SPECIAL_NUM_FIXED_INDEX   1

/* 是否为特殊报数? */
#define IS_SPECIAL_NUM                  2

/* 字符串字节对齐 */
#define STRING_ADDR_ALIGNMENT           4UL
#define STRING_ADDR_MASK                (STRING_ADDR_ALIGNMENT - 1)

/* 定义index_mask的类型, 最大可以支持64bit, 即uint64,  */
/* 如果还需要更大, 可自定义新的类型, 并使其支持位操作. */
typedef unsigned int index_mask_t;

#ifndef _MSC_VER
#define _aligned_malloc(sizes, alignment)   malloc(sizes)
#define _aligned_free(p)                    free(p)
#endif

/* Macros for min/max. */
#ifndef MIN
#define MIN(a, b)       (((a) < (b)) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a, b)       (((a) > (b)) ? (a) : (b))
#endif

#endif  /* _FIZZBUZZWHIZZ_COMMON_H_ */
