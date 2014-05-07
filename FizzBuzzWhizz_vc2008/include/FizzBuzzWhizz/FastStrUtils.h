
#ifndef _FAST_STR_UTILS_H_
#define _FAST_STR_UTILS_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <FizzBuzzWhizz/common.h>
#include <math.h>

// 数组长度为11
static const int s_integer_base10[] = {
    1, 10, 100, 1000, 10000, 100000, 1000000,
    10000000, 100000000, 1000000000, INT_MAX
};

template <typename value_type, typename char_type>
inline char_type * __cdecl fast_itoa_radix_10(value_type val, char_type *buf)
{
    char_type *dest = buf;
    const int array_max_length = _countof(s_integer_base10) - 1;
    const int first_check_index = array_max_length / 2;
    int is_neg;
    int digval, index, cur_index, max_digital;

#if 0
    cur_index = 0;
    is_neg = 0;
#else
    if (val >= 0) {
        cur_index = 0;
        is_neg = 0;
    }
    else {
        val = (value_type)((unsigned)(-(value_type)val));
        cur_index = 1;
        is_neg = 1;
        buf[0] = (char_type)'-';
    }
#endif

    if (val < 10) {
        if (!is_neg) {
            buf[0] = (char_type)(val + '0');
            buf[1] = (char_type)'\0';
        }
        else {
            //buf[0] = (char_type)'-';
            buf[1] = (char_type)(val + '0');
            buf[2] = (char_type)'\0';
        }
    }
    else if (val < 100) {
#if 1
        digval = val % 10;
        val /= 10;
#else
        digval = 0;
        do {
            digval++;
            val -= 10;
        } while (val >= 10);
#endif

        if (!is_neg) {
            buf[0] = (char_type)(val + '0');
            buf[1] = (char_type)(digval + '0');
            buf[2] = (char_type)'\0';
        }
        else {
            //buf[0] = (char_type)'-';
            buf[1] = (char_type)(val + '0');
            buf[2] = (char_type)(digval + '0');
            buf[3] = (char_type)'\0';
        }
    }
    else {
#if 1
        max_digital = 0;
        for (index = 0; index < array_max_length; ++index) {
            if (val >= s_integer_base10[index] && val < s_integer_base10[index + 1]) {
                max_digital = index;
                break;
            }
        }
#elif 0
        max_digital = (int)floor(log((double)val) / log(10.0) + 0.001);
#else
        value_type cur_base10;
        int head, tail;

        head = 0;
        tail = array_max_length - 1;
        index = first_check_index;
        do {
            cur_base10 = (value_type)(s_integer_base10[index]);
            if (val > cur_base10) {
                head  = index;
                index = head + (tail - head + 1) / 2;
            }
            else if (val < cur_base10) {
                tail  = index;
                index = head + (tail - head + 1) / 2;
            }
            else {
                // val是10的幂次方, 直接输出
                max_digital = head;
                break;
            }
            if ((tail - head) <= 1) {
                max_digital = head;
                break;
            }
        } while (index >= 0 && index < array_max_length);
#endif

#if 1
        cur_index += max_digital;
        buf[cur_index + 1] = (char_type)'\0';

        do {
            digval = val % 10;
            val /= 10;

            buf[cur_index--] = (char_type)(digval + '0');

            max_digital--;
        } while (max_digital >= 0);
#else
        value_type cur_base10;

        do {
            digval = 0;
            cur_base10 = s_integer_base10[max_digital];

            while (val >= cur_base10) {
                digval++;
                val -= cur_base10;
            };

            buf[cur_index++] = (char_type)(digval + '0');

            max_digital--;
        } while (max_digital >= 0);

        buf[cur_index] = (char_type)'\0';
#endif
    }

    return (dest);                    /* return dest */
}

template <typename char_type>
inline char_type * __cdecl fast_strcpy(char_type *dest, const char_type *src)
{
    while (*dest++ = *src++) ;        /* Copy src to end of dest */

    return (dest);                    /* return dest */
}

template <typename char_type>
inline void __cdecl fast_strcpy_void(char_type *dest, const char_type *src)
{
    while (*dest++ = *src++) ;        /* Copy src to end of dest */
}

template <typename char_type>
inline char_type * __cdecl fast_strcat(char_type *dest, const char_type *src)
{
    char_type *cp = dest;

    while (*cp)
        cp++;                       /* find end of dst */

    while (*cp++ = *src++) ;        /* Copy src to end of dest */

    return (dest);                  /* return dest */
}

template <typename char_type>
inline char_type * __cdecl fast_strcat_iter(char_type *dest, const char_type *src)
{
    char_type *cp = dest;

    while (*cp++ = *src++) ;        /* Copy src to end of dest */

    return (--cp);
}

/* 要求: dest地址可以不对齐到4字节, 但src的地址必需保证是4字节对齐的, 而且src的复制长度也是4字节对齐的! */

template <typename char_type>
inline char_type * __cdecl fast_strcat_iter_align4(char_type *dest, const char_type *src)
{
    char_type *cp;
    unsigned int *idest;
    unsigned int *isrc;
    size_t src_len, tail_len;
    int i, copy_steps;

    if (((unsigned int)dest & 3UL) == 0) {
        idest = reinterpret_cast<unsigned int *>(dest);
        isrc  = reinterpret_cast<unsigned int *>(const_cast<char_type *>(src));
        src_len = ::strlen(src);
        copy_steps = (src_len + sizeof(unsigned int) - 1) / sizeof(unsigned int) + 0;
        for (i = copy_steps; i > 0; --i) {
            *idest++ = *isrc++;         /* One time copy 4 bytes src to end of dest */
        }
        cp = reinterpret_cast<char_type *>(idest);
        tail_len = copy_steps * sizeof(unsigned int) - src_len;
        // 复制未包含'\0'字符
        if (tail_len == 0)
            *cp = '\0';
        else
            cp -= (tail_len);
    }
    else {
        cp = dest;
        while (*cp++ = *src++) ;        /* Copy src to end of dest */
        --cp;
    }

    return (cp);
}

/* 要求: dest地址可以不对齐到4字节, 但src的地址必需保证是4字节对齐的, 而且src的复制长度也是4字节对齐的! */

template <typename char_type>
inline char_type * __cdecl fast_strcat_iter_align4(char_type *dest, const char_type *src, size_t src_len)
{
    char_type *cp;
    unsigned int *idest;
    unsigned int *isrc;
    size_t tail_len;
    int i, copy_steps;

    if (((unsigned int)dest & 3UL) == 0) {
        idest = reinterpret_cast<unsigned int *>(dest);
        isrc  = reinterpret_cast<unsigned int *>(const_cast<char_type *>(src));
        copy_steps = (src_len + 1 + sizeof(unsigned int) - 1) / sizeof(unsigned int) + 0;
        for (i = copy_steps; i > 0; --i) {
            *idest++ = *isrc++;         /* One time copy 4 bytes src to end of dest */
        }
        cp = reinterpret_cast<char_type *>(idest);
        tail_len = copy_steps * sizeof(unsigned int) - src_len;
        // 复制过程是已经包含'\0'字符的, 无需再写'\0'
        cp -= (tail_len);
    }
    else {
        cp = dest;
        while (*cp++ = *src++) ;        /* Copy src to end of dest */
        --cp;
    }

    return (cp);
}

#endif  /* _FAST_STR_UTILS_H_ */
