
#ifndef _FIZZBUZZWHIZZ_COMMON_H_
#define _FIZZBUZZWHIZZ_COMMON_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* �Ƿ�ѽ���������Ļ�ϻ�������ַ���������, ����������Ļ, ��ô����ֻ�������ʾһ�� */
#define DISPLAY_RESULT_TO_SCREEN        0

/* ����FizzBuzzWhizz�����ڲ��Ƿ�ʹ��stop_watch��ʱ, ��Ϊ��ʱҲ�Ǻ�ʱ���, ���ޱ�Ҫ, �����֮ */
#define FIZZ_NATIVE_USE_STOPWATCH       1

/* ���������������Ļ, ���ڲ���ʱ�ǲ���Ҫ�� */
#if !defined(DISPLAY_RESULT_TO_SCREEN) || (DISPLAY_RESULT_TO_SCREEN == 0)
#undef  FIZZ_NATIVE_USE_STOPWATCH
#define FIZZ_NATIVE_USE_STOPWATCH       0
#endif

/* �������������� */
#define MAX_LOOP_TIME                   10000

/* ��Ч�����ⱨ�� */
#define INVALID_SPECIAL_NUM             (-1)

/* �����ⱨ������ */
#define NORMAL_NUM_INDEX                0

/* ��һ�����ⱨ��������ֵ */
#define FIRST_SPECIAL_NUM_FIXED_INDEX   1

/* �Ƿ�Ϊ���ⱨ��? */
#define IS_SPECIAL_NUM                  2

/* �ַ����ֽڶ��� */
#define STRING_ADDR_ALIGNMENT           4UL
#define STRING_ADDR_MASK                (STRING_ADDR_ALIGNMENT - 1)

/* ����index_mask������, ������֧��64bit, ��uint64,  */
/* �������Ҫ����, ���Զ����µ�����, ��ʹ��֧��λ����. */
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
