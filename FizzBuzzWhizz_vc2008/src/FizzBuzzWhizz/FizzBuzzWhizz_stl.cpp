
#include <FizzBuzzWhizz/FizzBuzzWhizz_stl.h>
#include <FizzBuzzWhizz/stop_watch.h>

#include <vector>

using namespace annlab;

/* ͳ���㷨���õ���ʱ�� */
static double s_SolveTotalTime = 0.0;

/// <comment>
///
/// �������Ե�IT��˾֮ThoughtWorks������ս����FizzBuzzWhizz��Ϸ
///
/// From: http://www.cnblogs.com/shines77/p/3712642.html
///
/// �ο�: http://www.cnblogs.com/lanxuezaipiao/p/3705313.html
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
    // �����������λ��(������󳤶�)
    max_digital = (int)floor(log((double)max_number) / log(10.0) + 0.001) + 1;
#else
    // �����������λ��(������󳤶�), ������log������, �����������ò��, ��Ϊ��������Ǻ���ҲҪ�õ�
    max_digital = 0;
    for (index = 0; index < (int)integer_base10_length; ++index) {
        if (max_number >= integer_base10[index] && max_number < integer_base10[index + 1]) {
            max_digital = index + 1;
            break;
        }
    }
#endif

    // ���mask(bit��ģ)ֵ, = 2^(max_word_type) = 2^3 = 8
    max_mask = 1 << max_word_type;

    // ���mask��Ӧ�����ⱨ���ַ���, ��max_mask��(2^3 = 8��)
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

    // ����sayword_index_list�ڴ�
    sayword_index_list = (index_mask_t *)malloc((max_number + 1) * sizeof(index_mask_t));
    if (sayword_index_list == NULL) {
        printf("Error: alloc array [sayword_index_list] out of memory\n\n");
        return;
    }

    // ����sayword_index_list��Ĭ��ֵ��ΪNORMAL_NUM_INDEX(0), ��Ĭ���Ƿ����ⱨ��
    for (num = 0; num <= max_number; ++num) {
        sayword_index_list[num] = NORMAL_NUM_INDEX;
    }

    // ����(�ϲ�)������������������maskֵ
    for (index = 0; index < max_word_type; ++index) {
        // ȡһ��������
        special_num = special_num_list[index];
        // �������������[1, 9]��Χ��, ����Ϊ����Ч������, ����
        if (special_num >= 1 && special_num <= 9) {
            // ����������maskֵ
            mask = 1 << index;
            for (num = special_num; num <= max_number; num += special_num)
                sayword_index_list[num] |= (index_mask_t)mask;
        }
        else {
            special_num_list[index] = INVALID_SPECIAL_NUM;
        }
    }

    // ����5: ���ݴ˹���, ���������������ְ����˵�һ��������(first)����,
    // ��ɸѡ���и�λ������first����, ��ɸѡ����ʮλ������first����,
    // ��������, ��λ, ǧλ..., ֱ�Ӵﵽmax_number
    // FIRST_SPECIAL_NUM_FIXED_INDEX��ֵ�̶�Ϊ1, ��Ϊ��һ��������(����һ��)��mask����1

    // ��һ��������
    first_special_num = special_num_list[0];

    if (first_special_num != INVALID_SPECIAL_NUM) {

        // ɸѡ���и�,ʮ,��,ǧ,��,ʮ��,����λ���Ȱ���first_special_num����
        for (digital = 0; digital < max_digital; ++digital) {
            right_start_num = first_special_num * integer_base10[digital];
            right_max_num = right_start_num + integer_base10[digital];
            // �ұߵĲ�����Ϊ1
            right_num_step = 1;
            // ����(right_start_num + 0)��Ȼ�Ѿ���first_special_num�ı���, ������Ϊ��Ҫ�������(��λ)��ѭ��, ���Բ���ʡ��
            // �ұ�ѭ��(�ø�,ʮ,��,ǧ,��λ���ұ�, ����λѭ��)
            for (right_num = right_start_num + 0; (right_num < right_max_num && right_num <= max_number); right_num += right_num_step) {
                sayword_index_list[right_num] = FIRST_SPECIAL_NUM_FIXED_INDEX;

                if (digital < integer_base10_length) {
                    left_num_step = integer_base10[digital + 1];
                    left_start_num = right_num + left_num_step;
                    // ���ѭ��(�ø�,ʮ,��,ǧ,��λ�����, ����λѭ��)
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

    // ������
    printf("FizzBuzzWhizz_stl() ReportNumber List:\n\nmax_number = %d\n\n", max_number);
    for (num = 1; num <= max_number; ++num) {
        index = sayword_index_list[num];
        // �������������, ��ֱ���������, �������index(maskֵ)��Ӧ���ַ���
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

    // ������������Ļ, ��������ַ�������
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
        // �������������, ��ֱ���������, �������index(maskֵ)��Ӧ���ַ���
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
        // �������������, ��ֱ���������, �������index(maskֵ)��Ӧ���ַ���
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
        // ������
        printf("FizzBuzzWhizz_stl() ReportNumber List:\n\nmax_number = %d\n\n", max_number);
        for (num = 1; num <= max_number; ++num) {
            index = sayword_index_list[num];
            // �������������, ��ֱ���������, �������index(maskֵ)��Ӧ���ַ���
            if (index == NORMAL_NUM_INDEX)
                printf("%d\n", num);
            else
                printf("%s\n", mask_sayword_list[index].c_str());
        }
        printf("\n");
    }
    //*/

#endif  /* DISPLAY_RESULT_TO_SCREEN */

    // �ͷ��ڴ�
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
        // ������
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
