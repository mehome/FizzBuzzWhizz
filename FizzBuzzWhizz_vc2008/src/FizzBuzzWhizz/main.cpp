
//#include <vld.h>

#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>

#include <FizzBuzzWhizz/common.h>
#include <FizzBuzzWhizz/FizzBuzzWhizz_stl.h>
#include <FizzBuzzWhizz/FizzBuzzWhizz_sys.h>
#include <FizzBuzzWhizz/FizzBuzzWhizz_fast.h>
#include <FizzBuzzWhizz/stop_watch.h>

#include <FizzBuzzWhizz/aligned_malloc.h>

using namespace annlab;

/// <comment>
///
/// �������Ե�IT��˾֮ThoughtWorks������ս����FizzBuzzWhizz��Ϸ
///
/// From: http://www.cnblogs.com/shines77/p/3712642.html
///
/// �ο�: http://www.cnblogs.com/lanxuezaipiao/p/3705313.html
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

/* Ԥ��ʱ������Ҫ����500����, ���������, ��������������СԤ��ʱ�� */
void iso_cpu_warm_up()
{
#ifndef _DEBUG
    stop_watch sw;
    int sum = 0;
    double elapsedTime = 0.0;
    printf("CPU warm up start ...\n");
    do {
        sw.restart();
        // ����д����ı������ܷ�������һ���̶�ֵ��ţ����, Ӧ��û��
        for (int i = 0; i < 100000; ++i) {
            sum += i;
            // ѭ��˳�����ߵ�������
            for (int j = 10000; j >= 0; --j) {
                sum -= j;
            }
        }
        sw.stop();
        elapsedTime += sw.getMillisec();
    } while (elapsedTime < 500.0);
    // ���sum��ֵֻ��Ϊ�˷�ֹ��������ѭ���Ż���
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

    // ���say_word����
    int max_word_type = _countof(say_word_list);

    // say_word��Ϻ������ַ�����
    int max_word_length = 1;
    for (index = 0; index < max_word_type; ++index)
        max_word_length += say_word_list[index].length();

    // ���뵽STRING_ADDR_ALIGNMENT(4�ֽ�)
    max_word_length = (max_word_length + STRING_ADDR_MASK) & (~STRING_ADDR_MASK);

#if defined(DISPLAY_RESULT_TO_SCREEN) && (DISPLAY_RESULT_TO_SCREEN != 0)
    display_to_screen = true;
#endif

    // ���Բ���������
    max_num_list = sizeof(special_num_list) / sizeof(special_num_list[0]);

    if (display_to_screen)
        max_num_list = 1;

    for (index = 0; index < max_num_list; ++index) {
        display_special_num_list(max_word_type, special_num_list[index]);

        // ʹ��stl��std::vector��std::string, �ٶȽ���
        FizzBuzzWhizz_stl_Test(max_number, max_word_type, max_word_length, say_word_list, special_num_list[index], display_to_screen);

        // �Լ������ڴ�ʵ��string array, ����ϵͳ�Դ����ַ���������, �е��ٶ�
        FizzBuzzWhizz_sys_Test(max_number, max_word_type, max_word_length, say_word_list, special_num_list[index], display_to_screen);

        // �Լ������ڴ�ʵ��string array, ʹ���Լ���д���ַ���������, �Ͽ�
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

    // ���say_word����
    int max_word_type = _countof(say_word_list);

    // say_word��Ϻ������ַ�����
    int max_word_length = 1;
    for (index = 0; index < max_word_type; ++index)
        max_word_length += say_word_list[index].length();

    // ���뵽STRING_ADDR_ALIGNMENT(4�ֽ�)
    max_word_length = (max_word_length + STRING_ADDR_MASK) & (~STRING_ADDR_MASK);

#if defined(DISPLAY_RESULT_TO_SCREEN) && (DISPLAY_RESULT_TO_SCREEN != 0)
    display_to_screen = true;
#endif

    // ���Բ���������
    max_num_list = sizeof(special_num_list) / sizeof(special_num_list[0]);

    if (display_to_screen)
        max_num_list = 1;

    for (index = 0; index < max_num_list; ++index) {
        display_special_num_list(max_word_type, special_num_list[index]);

        // ʹ��stl��std::vector��std::string, �ٶȽ���
        FizzBuzzWhizz_stl_Test(max_number, max_word_type, max_word_length, say_word_list, special_num_list[index], display_to_screen);

        // �Լ������ڴ�ʵ��string array, ����ϵͳ�Դ����ַ���������, �е��ٶ�
        FizzBuzzWhizz_sys_Test(max_number, max_word_type, max_word_length, say_word_list, special_num_list[index], display_to_screen);

        // �Լ������ڴ�ʵ��string array, ʹ���Լ���д���ַ���������, �Ͽ�
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

    // ����CPU, ����Ԥ��, ��Ϊ���ںܶ������CPU��֧�ֿ���ʱ�Զ���Ƶ
    iso_cpu_warm_up();

#if 1

#if !defined(DISPLAY_RESULT_TO_SCREEN) || (DISPLAY_RESULT_TO_SCREEN == 0)
    // ����3��������
    FizzBuzzWhizz_Test_Wrapper_3(100, false);

    system("pause");
#endif  /* DISPLAY_RESULT_TO_SCREEN */

    // ����3��������, ��ʾ�������Ļ
    FizzBuzzWhizz_Test_Wrapper_3(100, true);

    system("pause");

#else

    // ����3��������
    FizzBuzzWhizz_Test_Wrapper_3(100, false);

    printf("--------------------------------------------------\n\n");
    system("pause");

    // ����4��������
    FizzBuzzWhizz_Test_Wrapper_4(100, false);

    printf("--------------------------------------------------\n\n");
    system("pause");

#endif

	return 0;
}
