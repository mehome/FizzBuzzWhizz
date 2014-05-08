
#include <FizzBuzzWhizz/FizzBuzzWhizz_stl.h>
#include <FizzBuzzWhizz/stop_watch.h>

#include <vector>

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

void FizzBuzzWhizz_stl(const int max_number,
                       const int max_word_type, const int max_word_length,
                       const string say_word_list[], int special_num_list[],
                       std::vector<string> &say_word_result)
{
    int num;
    int index;
    int special_num;
    int first_special_num;
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

    unsigned int integer_base10_length = _countof(integer_base10) - 1;

#if defined(FIZZ_NATIVE_USE_STOPWATCH) && (FIZZ_NATIVE_USE_STOPWATCH != 0)
    stop_watch sw;
    sw.start();
#endif  /* FIZZ_NATIVE_USE_STOPWATCH */

#if 0
    // 最大数的数字位数(数字最大长度)
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
    string sayword;
    std::vector<string> mask_sayword_list;

    sayword.reserve(max_word_length);
    mask_sayword_list.reserve(max_mask);
    //mask_sayword_list.resize(max_mask);
    for (mask = 0; mask < max_mask; ++mask) {
        sayword = "";
        cur_mask = mask;
        for (index = 0; index < (int)max_word_type; ++index) {
            if ((cur_mask & 1) != 0)
                sayword += say_word_list[index];
            cur_mask >>= 1;
        }
        mask_sayword_list.push_back(sayword);
        //mask_sayword_list[mask] = sayword.c_str();
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

    // 计算(合并)和设置所有特殊数的mask值
    for (index = 0; index < max_word_type; ++index) {
        // 取一个特殊数
        special_num = special_num_list[index];
        // 如果特殊数不在[1, 9]范围内, 则认为是无效特殊数, 跳过
        if (special_num >= 1 && special_num <= 9) {
            // 该特殊数的mask值
            mask = 1 << index;
            for (num = special_num; num <= max_number; num += special_num)
                sayword_index_list[num] |= (index_mask_t)mask;
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
    first_special_num = special_num_list[0];

    if (first_special_num != INVALID_SPECIAL_NUM) {

        // 筛选所有个,十,百,千,万,十万,百万位数等包含first_special_num的数
        for (digital = 0; digital < max_digital; ++digital) {
            right_start_num = first_special_num * integer_base10[digital];
            right_max_num = right_start_num + integer_base10[digital];
            // 右边的步长恒为1
            right_num_step = 1;
            // 这里(right_start_num + 0)虽然已经是first_special_num的倍数, 但是因为还要进行左边(高位)的循环, 所以不能省略
            // 右边循环(该个,十,百,千,万位的右边, 即低位循环)
            for (right_num = right_start_num + 0; (right_num < right_max_num && right_num <= max_number); right_num += right_num_step) {
                sayword_index_list[right_num] = FIRST_SPECIAL_NUM_FIXED_INDEX;

                if (digital < integer_base10_length) {
                    left_num_step = integer_base10[digital + 1];
                    left_start_num = right_num + left_num_step;
                    // 左边循环(该个,十,百,千,万位的左边, 即高位循环)
                    for (left_num = left_start_num; left_num <= max_number; left_num += left_num_step)
                        sayword_index_list[left_num] = FIRST_SPECIAL_NUM_FIXED_INDEX;
                }
            }
        }

    }

#if defined(DISPLAY_RESULT_TO_SCREEN) && (DISPLAY_RESULT_TO_SCREEN != 0)

#if defined(FIZZ_NATIVE_USE_STOPWATCH) && (FIZZ_NATIVE_USE_STOPWATCH != 0)
    sw.stop();
#endif  /* FIZZ_NATIVE_USE_STOPWATCH */

    // 输出结果
    printf("FizzBuzzWhizz_stl() ReportNumber List:\n\nmax_number = %d\n\n", max_number);
    for (num = 1; num <= max_number; ++num) {
        index = sayword_index_list[num];
        // 如果不是特殊数, 则直接输出该数, 否则输出index(mask值)对应的字符串
        if (index == NORMAL_NUM_INDEX)
            printf("%d\n", num);
        else
            printf("%s\n", mask_sayword_list[index].c_str());
    }
    printf("\n");

#if defined(FIZZ_NATIVE_USE_STOPWATCH) && (FIZZ_NATIVE_USE_STOPWATCH != 0)
    printf("FizzBuzzWhizz_stl() elapsed time: %0.3f ms\n\n", sw.getMillisec());
#endif  /* FIZZ_NATIVE_USE_STOPWATCH */

#else  /* !DISPLAY_RESULT_TO_SCREEN */

    char buffer[32];

    // 如果不输出到屏幕, 则输出到字符串数组
    //std::vector<string> say_word_result;
    //say_word_result.clear();
    //say_word_result.reserve(max_number + 1);
    //say_word_result.clear();
    if (say_word_result.size() != (max_number + 1))
        say_word_result.resize(max_number + 1);
    //say_word_result.push_back("");
    //say_word_result[0] = "";
#if 1
    for (num = 1; num <= max_number; ++num) {
        index = sayword_index_list[num];
        // 如果不是特殊数, 则直接输出该数, 否则输出index(mask值)对应的字符串
        if (index == NORMAL_NUM_INDEX) {
            if (_itoa(num, buffer, 10))
                //say_word_result.push_back(buffer);
                say_word_result[num] = buffer;
        }
        else {
            //say_word_result.push_back(mask_sayword_list[index].c_str());
            //say_word_result.push_back(mask_sayword_list[index]);
            //say_word_result[num] = mask_sayword_list[index].c_str();
            say_word_result[num] = mask_sayword_list[index];
        }
    }
#else
    std::vector<string>::iterator iter, iter_first;
    num = 1;
    iter_first = say_word_result.begin();
    for (iter = ++iter_first; iter != say_word_result.end(); ++iter, ++num) {
        index = sayword_index_list[num];
        // 如果不是特殊数, 则直接输出该数, 否则输出index(mask值)对应的字符串
        if (index == NORMAL_NUM_INDEX) {
            if (_itoa(num, buffer, 10))
                //say_word_result.push_back(buffer);
                *iter = buffer;
        }
        else {
            //say_word_result.push_back(mask_sayword_list[index].c_str());
            //say_word_result.push_back(mask_sayword_list[index]);
            //say_word_result[num] = mask_sayword_list[index].c_str();
            *iter = mask_sayword_list[index];
        }
    }
#endif

#if defined(FIZZ_NATIVE_USE_STOPWATCH) && (FIZZ_NATIVE_USE_STOPWATCH != 0)
    sw.stop();
    //printf("elapsed time: %0.3f ms\n\n", sw.getMillisec());

    s_SolveTotalTime += sw.getMillisec();
#endif  /* FIZZ_NATIVE_USE_STOPWATCH */

    /*
    if (out_sayword_result != NULL) {
        // 输出结果
        printf("FizzBuzzWhizz_stl() ReportNumber List:\n\nmax_number = %d\n\n", max_number);
        for (num = 1; num <= max_number; ++num) {
            index = sayword_index_list[num];
            // 如果不是特殊数, 则直接输出该数, 否则输出index(mask值)对应的字符串
            if (index == NORMAL_NUM_INDEX)
                printf("%d\n", num);
            else
                printf("%s\n", mask_sayword_list[index].c_str());
        }
        printf("\n");
    }
    //*/

#endif  /* DISPLAY_RESULT_TO_SCREEN */

    // 释放内存
    if (sayword_index_list)
        free(sayword_index_list);
}

void FizzBuzzWhizz_stl_Test(const int max_number,
                            const int max_word_type, const int max_word_length,
                            const string say_word_list[], int special_num_list[],
                            bool display_to_screen)
{
    stop_watch sw;
    std::vector<string> say_word_result;

    if (display_to_screen) {

        printf("==================================\n\n");
        printf("FizzBuzzWhizz_stl_Test()\n\n");
        printf("==================================\n\n");

        sw.start();
        FizzBuzzWhizz_stl(max_number, max_word_type, max_word_length, say_word_list, special_num_list, say_word_result);
        sw.stop();

#if !defined(DISPLAY_RESULT_TO_SCREEN) || (DISPLAY_RESULT_TO_SCREEN == 0)
        // 输出结果
        printf("ReportNumber List:\n\nmax_number = %d\n\n", max_number);
        for (int num = 1; num <= max_number; ++num) {
            printf("%s\n", say_word_result[num].c_str());
        }
        printf("\n");

        printf("============================================\n\n");

#if defined(FIZZ_NATIVE_USE_STOPWATCH) && (FIZZ_NATIVE_USE_STOPWATCH != 0)
        printf("FizzBuzzWhizz_stl() Solve Time: %0.3f ms\n\n", s_SolveTotalTime);
#endif  /* FIZZ_NATIVE_USE_STOPWATCH */
        printf("FizzBuzzWhizz_stl(): %0.3f ms\n\n", sw.getMillisec());

        printf("============================================\n\n");
#endif  /* DISPLAY_RESULT_TO_SCREEN */

        system("pause");
    }
    else {
        sw.start();
        for (int i = 0; i < MAX_LOOP_TIME; ++i) {
            FizzBuzzWhizz_stl(max_number, max_word_type, max_word_length, say_word_list, special_num_list, say_word_result);
        }
        sw.stop();

        printf("FizzBuzzWhizz_stl_Test():  %0.3f ms\n\n", sw.getMillisec());
    }
}
