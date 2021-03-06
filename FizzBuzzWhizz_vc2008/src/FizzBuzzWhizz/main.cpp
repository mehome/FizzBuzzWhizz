
//#include <vld.h>

#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>

#include <FizzBuzzWhizz/common.h>
#include <FizzBuzzWhizz/FizzBuzzWhizz_stl.h>
#include <FizzBuzzWhizz/FizzBuzzWhizz_stl2.h>
#include <FizzBuzzWhizz/FizzBuzzWhizz_sys.h>
#include <FizzBuzzWhizz/FizzBuzzWhizz_fast.h>
#include <FizzBuzzWhizz/stop_watch.h>

#include <FizzBuzzWhizz/aligned_malloc.h>

using namespace annlab;

/// <comment>
///
/// 最难面试的IT公司之ThoughtWorks代码挑战——FizzBuzzWhizz游戏
///
/// From: http://www.cnblogs.com/shines77/p/3712642.html
///
/// 参考: http://www.cnblogs.com/lanxuezaipiao/p/3705313.html
///
/// </comment>

void display_special_num_list(const int max_word_type, const int special_num_list[])
{
    printf("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n\n");
    printf("  special num: ( ");
    for (int index = 0; index < max_word_type; ++index) {
        if (index < max_word_type - 1)
            printf("%d, ", special_num_list[index]);
        else
            printf("%d", special_num_list[index]);
    }
    printf(" )\n\n");
    printf("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n\n");
}

/* 预热时间至少要大于500毫秒, 如果还不够, 可以自行增加最小预热时间 */
void iso_cpu_warm_up()
{
#ifndef _DEBUG
    stop_watch sw;
    volatile int sum = 0;
    double elapsedTime = 0.0;
    printf("CPU warm up start ...\n");
    do {
        sw.restart();
        // 如果有聪明的编译器能发现这是一个固定值就牛比了, 应该没有
        for (int i = 0; i < 10000; ++i) {
            sum += i;
            // 循环顺序故意颠倒过来的
            for (int j = 5000; j >= 0; --j) {
                sum -= j;
            }
        }
        sw.stop();
        elapsedTime += sw.getMillisec();
    } while (elapsedTime < 500.0);
    // 输出sum的值只是为了防止编译器把循环优化掉
    printf("sum = %u, time: %0.3f ms\n", sum, elapsedTime);
    printf("CPU warm up done ... \n\n");
#endif
}

void FizzBuzzWhizz_Test_Wrapper_3(const int max_number, bool display_to_screen)
{
    int index, max_num_list;
    const string say_word_list[] = { "Fizz", "Buzz", "Whizz" };
    int special_num_list[][3] = {
        { 3, 5, 7 },
        { 2, 4, 8 },
        { 3, 6, 8 },
        //{ 2, 2, 2 },
    };

    // 最大say_word类型
    int max_word_type = _countof(say_word_list);

    // say_word组合后的最大字符长度
    int max_word_length = 1;
    for (index = 0; index < max_word_type; ++index)
        max_word_length += say_word_list[index].length();

    // 对齐到STRING_ADDR_ALIGNMENT(4字节)
    max_word_length = (max_word_length + STRING_ADDR_MASK) & (~STRING_ADDR_MASK);

#if defined(DISPLAY_RESULT_TO_SCREEN) && (DISPLAY_RESULT_TO_SCREEN != 0)
    display_to_screen = true;
#endif

    // 测试参数总组数
    max_num_list = sizeof(special_num_list) / sizeof(special_num_list[0]);

    if (display_to_screen)
        max_num_list = 1;

    for (index = 0; index < max_num_list; ++index) {
        display_special_num_list(max_word_type, special_num_list[index]);

        // 使用stl的std::vector和std::string, 速度较慢
        FizzBuzzWhizz_stl_Test(max_number, max_word_type, max_word_length, say_word_list, special_num_list[index], display_to_screen);

        // 使用stl的std::string数组, 速度比前者快一点
        FizzBuzzWhizz_stl2_Test(max_number, max_word_type, max_word_length, say_word_list, special_num_list[index], display_to_screen);

        // 自己分配内存实现string array, 采用系统自带的字符串处理函数, 中等速度
        FizzBuzzWhizz_sys_Test(max_number, max_word_type, max_word_length, say_word_list, special_num_list[index], display_to_screen);

        // 自己分配内存实现string array, 使用自己编写的字符串处理函数, 较快
        FizzBuzzWhizz_fast_Test(max_number, max_word_type, max_word_length, say_word_list, special_num_list[index], display_to_screen);
    }

#if !defined(DISPLAY_RESULT_TO_SCREEN) || (DISPLAY_RESULT_TO_SCREEN == 0)
    if (!display_to_screen)
        printf("===============================================\n\n");
#else
    printf("\n");
#endif  /* DISPLAY_RESULT_TO_SCREEN */
}

void FizzBuzzWhizz_Test_Wrapper_4(const int max_number, bool display_to_screen)
{
    int index, max_num_list;
    const string say_word_list[] = { "Fizz", "Buzz", "Whizz", "Zoozz" };
    int special_num_list[][4] = {
        { 3, 5, 7, 9 },
        { 2, 4, 8, 9 },
        { 3, 6, 8, 9 },
        //{ 2, 2, 2, 2 },
    };   

    // 最大say_word类型
    int max_word_type = _countof(say_word_list);

    // say_word组合后的最大字符长度
    int max_word_length = 1;
    for (index = 0; index < max_word_type; ++index)
        max_word_length += say_word_list[index].length();

    // 对齐到STRING_ADDR_ALIGNMENT(4字节)
    max_word_length = (max_word_length + STRING_ADDR_MASK) & (~STRING_ADDR_MASK);

#if defined(DISPLAY_RESULT_TO_SCREEN) && (DISPLAY_RESULT_TO_SCREEN != 0)
    display_to_screen = true;
#endif

    // 测试参数总组数
    max_num_list = sizeof(special_num_list) / sizeof(special_num_list[0]);

    if (display_to_screen)
        max_num_list = 1;

    for (index = 0; index < max_num_list; ++index) {
        display_special_num_list(max_word_type, special_num_list[index]);

        // 使用stl的std::vector和std::string, 速度较慢
        FizzBuzzWhizz_stl_Test(max_number, max_word_type, max_word_length, say_word_list, special_num_list[index], display_to_screen);

        // 使用stl的std::string数组, 速度比前者快一点
        FizzBuzzWhizz_stl2_Test(max_number, max_word_type, max_word_length, say_word_list, special_num_list[index], display_to_screen);

        // 自己分配内存实现string array, 采用系统自带的字符串处理函数, 中等速度
        FizzBuzzWhizz_sys_Test(max_number, max_word_type, max_word_length, say_word_list, special_num_list[index], display_to_screen);

        // 自己分配内存实现string array, 使用自己编写的字符串处理函数, 较快
        FizzBuzzWhizz_fast_Test(max_number, max_word_type, max_word_length, say_word_list, special_num_list[index], display_to_screen);
    }

#if !defined(DISPLAY_RESULT_TO_SCREEN) || (DISPLAY_RESULT_TO_SCREEN == 0)
    if (!display_to_screen)
        printf("===============================================\n\n");
#else
    printf("\n");
#endif  /* DISPLAY_RESULT_TO_SCREEN */
}

void aligned_malloc_test()
{
    void *ptr;
    size_t offset, alignment;
    alignment = 256;
    offset = 13;
    ptr = (void *)_aligned_offset_malloc(32, alignment, offset);
    if (ptr != NULL) {
        if ((((uint32_t)ptr + offset) % alignment) == 0)
            printf("This pointer: 0x%08X, is offset by %d on alignment of %d\n", ptr, offset, alignment);
        else
            printf("This pointer: 0x%08X, does not satisfy offset %d and alignment %d\n", ptr, offset, alignment);
    }
    if (ptr)
        _aligned_free(ptr);

    ptr = (void *)_aligned_malloc(64, alignment);
    if (ptr)
        _aligned_free(ptr);

    ptr = (void *)iso_aligned_offset_malloc(32, alignment, offset);
    if (ptr != NULL) {
        if ((((uint32_t)ptr + offset) % alignment) == 0)
            printf("This pointer: 0x%08X, is offset by %d on alignment of %d\n", ptr, offset, alignment);
        else
            printf("This pointer: 0x%08X, does not satisfy offset %d and alignment %d\n", ptr, offset, alignment);
    }
    if (ptr)
        iso_aligned_free(ptr);

    ptr = (void *)iso_aligned_malloc(64, alignment);
    if (ptr)
        iso_aligned_free(ptr); 
}

int main(int argc, char* argv[])
{
#ifdef _DEBUG
    aligned_malloc_test();
    //return 0;
#endif

    // 唤醒CPU, 进行预热, 因为现在很多主板和CPU都支持空闲时自动降频
    iso_cpu_warm_up();

#if 1

#if !defined(DISPLAY_RESULT_TO_SCREEN) || (DISPLAY_RESULT_TO_SCREEN == 0)
    // 测试3个特殊数
    FizzBuzzWhizz_Test_Wrapper_3(100, false);

    system("pause");
#endif  /* DISPLAY_RESULT_TO_SCREEN */

    // 测试3个特殊数, 显示结果到屏幕
    FizzBuzzWhizz_Test_Wrapper_3(100, true);

    system("pause");

#else

    // 测试3个特殊数
    FizzBuzzWhizz_Test_Wrapper_3(100, false);

    printf("--------------------------------------------------\n\n");
    system("pause");

    // 测试4个特殊数
    FizzBuzzWhizz_Test_Wrapper_4(100, false);

    printf("--------------------------------------------------\n\n");
    system("pause");

#endif

	return 0;
}
