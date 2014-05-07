
#include <FizzBuzzWhizz/FizzBuzzWhizz_fast.h>
#include <FizzBuzzWhizz/stop_watch.h>

#include <FizzBuzzWhizz/FastStrUtils.h>

using namespace annlab;

/* 统计算法所用的总时间 */
static double s_SolveTotalTime = 0.0;

/// <comment>
///
/// 最难面试的IT公司之ThoughtWorks代码挑战——FizzBuzzWhizz游戏
///
/// From: http://www.cnblogs.com/shines77/p/3712642.html
///
/// 参考: http://www.cnblogs.com/lanxuezaipiao/p/3705313.html
///
/// </comment>

void FizzBuzzWhizz_fast(const int max_number,
                        const int max_word_type, const int max_word_length,
                        const string say_word_list[], int special_num_list[],
                        char **out_sayword_result)
{
    int num;
    int index;
    int special_num;
    unsigned int digital, max_digital;
    unsigned int mask, cur_mask, max_mask;

    int left_num, right_num;
    int left_start_num, right_start_num;
    int left_num_step, right_num_step;
    int right_max_num;

    index_mask_t *sayword_index_list;

    const int integer_base10[] = {
        1, 10, 100, 1000, 10000, 100000, 1000000,
        10000000, 100000000, 1000000000, INT_MAX
    };

    const unsigned int integer_base10_length = _countof(integer_base10) - 1;

#if defined(FIZZ_NATIVE_USE_STOPWATCH) && (FIZZ_NATIVE_USE_STOPWATCH != 0)
    stop_watch sw;
    sw.start();
#endif  /* FIZZ_NATIVE_USE_STOPWATCH */

#if 0
    // 最大数的数字位数(数字最大长度), 不推荐
    max_digital = (int)floor(log((double)max_number) / log(10.0) + 0.001) + 1;
#else
    // 最大数的数字位数(数字最大长度), 可以用log函数求, 但这里我们用查表法, 因为这个表我们后面也要用的
    max_digital = 0;
    for (index = 0; index < (int)integer_base10_length; ++index) {
        if (max_number >= integer_base10[index] && max_number < integer_base10[index + 1]) {
            max_digital = index + 1;
            break;
        }
    }
#endif

    // 最大mask(bit掩模)值, = 2^(max_word_type) = 2^3 = 8
    max_mask = 1 << max_word_type;

    // 求出mask对应的特殊报数字符串, 共max_mask种(2^3 = 8种)
    char *mask_sayword_list, *cur_sayword_list;
    mask_sayword_list = (char *)_aligned_malloc(max_mask * max_word_length * sizeof(char), STRING_ADDR_ALIGNMENT);
    if (mask_sayword_list == NULL) {
        printf("Error: alloc string array [mask_sayword_list] out of memory\n\n");
        return;
    }
    mask_sayword_list[0] = '\0';
    for (mask = 1; mask < max_mask; ++mask) {
        cur_sayword_list = mask_sayword_list + mask * max_word_length;
        cur_sayword_list[0] = '\0';
        cur_mask = mask;
        for (index = 0; index < max_word_type; ++index) {
            if ((cur_mask & 1) != 0)
                //cur_sayword_list = fast_strcat_iter<char>(cur_sayword_list, say_word_list[index].c_str());
                cur_sayword_list = fast_strcat_iter_align4<char>(cur_sayword_list, say_word_list[index].c_str(), say_word_list[index].size());
            cur_mask >>= 1;
            //if (cur_mask == 0)
            //    break;
        }
    }

    // 分配sayword_index_list内存
    sayword_index_list = (index_mask_t *)malloc((max_number + 1) * sizeof(index_mask_t));
    if (sayword_index_list == NULL) {
        printf("Error: alloc array [sayword_index_list] out of memory\n\n");
        return;
    }

    // 所有sayword_index_list的默认值均为NORMAL_NUM_INDEX(0), 即默认是非特殊报数
    for (num = 0; num <= max_number; ++num) {
        sayword_index_list[num] = NORMAL_NUM_INDEX;
    }

    // 规则3, 4: 计算(合并)和设置所有特殊数的mask值
    for (index = 0; index < max_word_type; ++index) {
        // 取一个特殊数, 从后往前读
        special_num = special_num_list[index];
        // 如果特殊数不在[1, 9]范围内, 则认为是无效特殊数, 跳过
        if (special_num >= 1 && special_num <= 9) {
            // 该特殊数的mask值
            mask = 1 << index;
#if 0
            // 扫描后面的特殊数有没有相同的, 有的话一起置入mask, 以后不必重复写入, 提高效率
            // (不过实践证明效果不大, 虽然写入次数少了, 但是也增加了代码, 反而变慢了,
            //  因为特殊数重复的情况还是比较少的, 如果特殊数列表很长, 且重复的可能很大的时候, 这样做才值得)
            for (int index2 = index + 1; index2 < max_word_type; ++index2) {
                num = special_num_list[index2];
                // 如果等于当前特殊数, 那么这个特殊数也肯定在[1, 9]范围内
                if (num == special_num) {
                    // 并入当前的mask值
                    mask |= (1 << index2);
                    // 把该特殊数设置为无效, 因为已经处理了
                    special_num_list[index2] = INVALID_SPECIAL_NUM;
                }
            }
#endif
            for (num = special_num; num <= max_number; num += special_num) {
                sayword_index_list[num] |= (index_mask_t)mask;
            }
        }
        else {
            special_num_list[index] = INVALID_SPECIAL_NUM;
        }
    }

    // 规则5: 根据此规则, 设置所有所报数字包含了第一个特殊数(first)的数,
    // 先筛选所有个位数包含first的数, 再筛选所有十位数包含first的数,
    // 依此类推, 百位, 千位..., 直接达到max_number
    // FIRST_SPECIAL_NUM_FIXED_INDEX的值固定为1, 因为第一个特殊数(仅第一个)的mask就是1

    // 第一个特殊数
    special_num = special_num_list[0];

    // 检查特殊数是否在[1, 9]范围内
    //if (special_num >= 1 && special_num <= 9) {
    if (special_num != INVALID_SPECIAL_NUM) {
#if 1
        if (max_digital <= integer_base10_length) {
            // 筛选所有个,十,百,千,万,十万,百万位数等包含first_special_num的数
            for (digital = 0; digital < max_digital; ++digital) {
                right_start_num = special_num * integer_base10[digital];
                right_max_num = MIN(right_start_num + integer_base10[digital] - 1, max_number);
                // 右边的步长恒为1
                right_num_step = 1;
                // 这里right_start_num虽然已经是first_special_num的倍数,
                // 但是因为还要进行左边(高位)的循环, 所以不能省略
                // 右边循环(该个,十,百,千,万位的右边, 即低位循环)
                for (right_num = right_start_num; right_num <= right_max_num; right_num += right_num_step) {
                    //if (sayword_index_list[right_num] != FIRST_SPECIAL_NUM_FIXED_INDEX)
                        sayword_index_list[right_num] = FIRST_SPECIAL_NUM_FIXED_INDEX;

                    left_num_step = integer_base10[digital + 1];
                    left_start_num = right_num + left_num_step;
                    // 左边循环(该个,十,百,千,万位的左边, 即高位循环)
                    for (left_num = left_start_num; left_num <= max_number; left_num += left_num_step) {
                        //if (sayword_index_list[left_num] != FIRST_SPECIAL_NUM_FIXED_INDEX)
                            sayword_index_list[left_num] = FIRST_SPECIAL_NUM_FIXED_INDEX;
                    }
                }
            }
        }
        else {
            // 筛选所有个,十,百,千,万,十万,百万位数等包含first_special_num的数
            for (digital = 0; digital < max_digital; ++digital) {
                right_start_num = special_num * integer_base10[digital];
                right_max_num = MIN(right_start_num + integer_base10[digital] - 1, max_number);
                // 右边的步长恒为1
                right_num_step = 1;
                // 这里right_start_num虽然已经是first_special_num的倍数,
                // 但是因为还要进行左边(高位)的循环, 所以不能省略
                // 右边循环(该个,十,百,千,万位的右边, 即低位循环)
                for (right_num = right_start_num; right_num <= right_max_num; right_num += right_num_step) {
                    //if (sayword_index_list[right_num] != FIRST_SPECIAL_NUM_FIXED_INDEX)
                        sayword_index_list[right_num] = FIRST_SPECIAL_NUM_FIXED_INDEX;

                    if (digital < integer_base10_length) {
                        left_num_step = integer_base10[digital + 1];
                        left_start_num = right_num + left_num_step;
                        // 左边循环(该个,十,百,千,万位的左边, 即高位循环)
                        for (left_num = left_start_num; left_num <= max_number; left_num += left_num_step) {
                            //if (sayword_index_list[left_num] != FIRST_SPECIAL_NUM_FIXED_INDEX)
                                sayword_index_list[left_num] = FIRST_SPECIAL_NUM_FIXED_INDEX;
                        }
                    }
                }
            }
        }
#else
        // 筛选所有个,十,百,千,万,十万,百万位数等包含first_special_num的数

        // 先处理个位数, 因为个位数比较特殊, 它是没有右边循环的, 所有拿出来特殊处理
        // 这里有个奇怪的问题, 第一个数从special_num开始循环反而还快一点,
        // 因为这个数的值已经是 FIRST_SPECIAL_NUM_FIXED_INDEX 了的, 理论上没必要再写入的
#if 0
        left_start_num = special_num;
        // 左边循环(该个,十,百,千,万位的左边, 即高位循环)
        for (left_num = left_start_num; left_num <= max_number; left_num += 10) {
            sayword_index_list[left_num] = FIRST_SPECIAL_NUM_FIXED_INDEX;
        }
#else
        left_num_step = integer_base10[1];
        left_start_num = special_num;
        // 左边循环(该个,十,百,千,万位的左边, 即高位循环)
        for (left_num = left_start_num; left_num <= max_number; left_num += left_num_step) {
            sayword_index_list[left_num] = FIRST_SPECIAL_NUM_FIXED_INDEX;
        }
#endif

        // 再处理十位, 百位, 千位, 万位等
        for (digital = 1; digital < max_digital; ++digital) {
            left_num_step = integer_base10[digital + 1];
            left_start_num = special_num * integer_base10[digital];
            // 左边循环(该个,十,百,千,万位的左边, 即高位循环)
            for (left_num = left_start_num; left_num <= max_number; left_num += left_num_step) {
                sayword_index_list[left_num] = FIRST_SPECIAL_NUM_FIXED_INDEX;

                right_max_num = MIN(integer_base10[digital] - 1, max_number);
                // 右边循环的起始数恒为1, 因为0在前面已经处理了, 减少一次循环
                right_start_num = left_num + 1;
                // 右边循环的步长恒为1(此变量其实没用到)
                right_num_step = 1;
                // 右边循环(该个,十,百,千,万位的右边, 即低位循环)
                for (right_num = right_start_num; right_num <= right_max_num; ++right_num) {
                    sayword_index_list[right_num] = FIRST_SPECIAL_NUM_FIXED_INDEX;
                }
            }
        }
#endif
    }

#if defined(DISPLAY_RESULT_TO_SCREEN) && (DISPLAY_RESULT_TO_SCREEN != 0)

#if defined(FIZZ_NATIVE_USE_STOPWATCH) && (FIZZ_NATIVE_USE_STOPWATCH != 0)
    sw.stop();
#endif  /* FIZZ_NATIVE_USE_STOPWATCH */

    // 输出结果
    printf("FizzBuzzWhizz_fast() ReportNumber List:\n\nmax_number = %d\n\n", max_number);
    for (num = 1; num <= max_number; ++num) {
        index = sayword_index_list[num];
        // 如果不是特殊数, 则直接输出该数, 否则输出index(mask值)对应的字符串
        if (index == NORMAL_NUM_INDEX)
            printf("%d\n", num);
        else
            printf("%s\n", &mask_sayword_list[index * max_word_length]);
    }
    printf("\n");

#if defined(FIZZ_NATIVE_USE_STOPWATCH) && (FIZZ_NATIVE_USE_STOPWATCH != 0)
    printf("FizzBuzzWhizz_fast() elapsed time: %0.3f ms\n\n", sw.getMillisec());
#endif  /* FIZZ_NATIVE_USE_STOPWATCH */

#else  /* !DISPLAY_RESULT_TO_SCREEN */

    // 如果不输出到屏幕, 则输出到字符串数组
    char *sayword_result, *cur_sayword_result;
    index_mask_t *cur_sayword_index_list;
    sayword_result = (char *)_aligned_malloc((max_number + 1) * max_word_length * sizeof(char), STRING_ADDR_ALIGNMENT);
    if (sayword_result == NULL) {
        printf("Error: alloc string array [sayword_result] out of memory\n");
        return;
    }
    cur_sayword_result = sayword_result + max_word_length;
    cur_sayword_index_list = &sayword_index_list[1];
    for (num = 1; num <= max_number; ++num) {
        //index = sayword_index_list[num];
        index = *cur_sayword_index_list++;
        // 如果不是特殊数, 则直接输出该数, 否则输出index(mask值)对应的字符串
        if (index == NORMAL_NUM_INDEX) {
            fast_itoa_radix_10<int, char>(num, cur_sayword_result);
        }
        else {
            fast_strcpy_void<char>(cur_sayword_result, &mask_sayword_list[index * max_word_length]);
        }
        cur_sayword_result += max_word_length;
    }

    if (out_sayword_result != NULL) {
        *out_sayword_result = sayword_result;
    }
    else {
        if (sayword_result)
            _aligned_free(sayword_result);
    }

#if defined(FIZZ_NATIVE_USE_STOPWATCH) && (FIZZ_NATIVE_USE_STOPWATCH != 0)
    sw.stop();
    //printf("elapsed time: %0.3f ms\n\n", sw.getMillisec());

    s_SolveTotalTime += sw.getMillisec();
#endif  /* FIZZ_NATIVE_USE_STOPWATCH */

#endif  /* DISPLAY_RESULT_TO_SCREEN */

    // 释放内存
    if (mask_sayword_list)
        _aligned_free(mask_sayword_list);
    if (sayword_index_list)
        free(sayword_index_list);
}

void FizzBuzzWhizz_fast_Test(const int max_number,
                             const int max_word_type, const int max_word_length,
                             const string say_word_list[], int special_num_list[],
                             bool display_to_screen)
{
    stop_watch sw;
    char *say_word_result = NULL;

    if (display_to_screen) {
        printf("==================================\n\n");
        printf("FizzBuzzWhizz_fast_Test()\n\n");
        printf("==================================\n\n");

        sw.start();
        FizzBuzzWhizz_fast(max_number, max_word_type, max_word_length, say_word_list, special_num_list, &say_word_result);
        sw.stop();

#if !defined(DISPLAY_RESULT_TO_SCREEN) || (DISPLAY_RESULT_TO_SCREEN == 0)
        if (say_word_result != NULL) {
            // 输出结果
            printf("ReportNumber List:\n\nmax_number = %d\n\n", max_number);
            for (int num = 1; num <= max_number; ++num) {
                printf("%s\n", &say_word_result[num * max_word_length]);
            }
            printf("\n");
        }

        printf("============================================\n\n");

#if defined(FIZZ_NATIVE_USE_STOPWATCH) && (FIZZ_NATIVE_USE_STOPWATCH != 0)
        printf("FizzBuzzWhizz_fast_Test() Solve Time: %0.3f ms\n\n", s_SolveTotalTime);
#endif  /* FIZZ_NATIVE_USE_STOPWATCH */
        printf("FizzBuzzWhizz_fast_Test(): %0.3f ms\n\n", sw.getMillisec());

        printf("============================================\n\n");
#endif  /* DISPLAY_RESULT_TO_SCREEN */

        system("pause");
    }
    else {
        sw.start();
        for (int i = 0; i < MAX_LOOP_TIME; ++i) {
            FizzBuzzWhizz_fast(max_number, max_word_type, max_word_length, say_word_list, special_num_list, &say_word_result);
            if (say_word_result)
                _aligned_free(say_word_result);
        }
        sw.stop();

        say_word_result = NULL;
        FizzBuzzWhizz_fast(max_number, max_word_type, max_word_length, say_word_list, special_num_list, &say_word_result);

        printf("FizzBuzzWhizz_fast_Test(): %0.3f ms\n\n", sw.getMillisec());
    }

    if (say_word_result) {
        _aligned_free(say_word_result);
        say_word_result = NULL;
    }
}
