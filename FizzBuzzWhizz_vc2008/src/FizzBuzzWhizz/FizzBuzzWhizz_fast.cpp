
#include <FizzBuzzWhizz/FizzBuzzWhizz_fast.h>
#include <FizzBuzzWhizz/stop_watch.h>

#include <FizzBuzzWhizz/FastStrUtils.h>

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
    // �����������λ��(������󳤶�), ���Ƽ�
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

    // ����3, 4: ����(�ϲ�)������������������maskֵ
    for (index = 0; index < max_word_type; ++index) {
        // ȡһ��������, �Ӻ���ǰ��
        special_num = special_num_list[index];
        // �������������[1, 9]��Χ��, ����Ϊ����Ч������, ����
        if (special_num >= 1 && special_num <= 9) {
            // ����������maskֵ
            mask = 1 << index;
#if 0
            // ɨ��������������û����ͬ��, �еĻ�һ������mask, �Ժ󲻱��ظ�д��, ���Ч��
            // (����ʵ��֤��Ч������, ��Ȼд���������, ����Ҳ�����˴���, ����������,
            //  ��Ϊ�������ظ���������ǱȽ��ٵ�, ����������б�ܳ�, ���ظ��Ŀ��ܴܺ��ʱ��, ��������ֵ��)
            for (int index2 = index + 1; index2 < max_word_type; ++index2) {
                num = special_num_list[index2];
                // ������ڵ�ǰ������, ��ô���������Ҳ�϶���[1, 9]��Χ��
                if (num == special_num) {
                    // ���뵱ǰ��maskֵ
                    mask |= (1 << index2);
                    // �Ѹ�����������Ϊ��Ч, ��Ϊ�Ѿ�������
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

    // ����5: ���ݴ˹���, ���������������ְ����˵�һ��������(first)����,
    // ��ɸѡ���и�λ������first����, ��ɸѡ����ʮλ������first����,
    // ��������, ��λ, ǧλ..., ֱ�Ӵﵽmax_number
    // FIRST_SPECIAL_NUM_FIXED_INDEX��ֵ�̶�Ϊ1, ��Ϊ��һ��������(����һ��)��mask����1

    // ��һ��������
    special_num = special_num_list[0];

    // ����������Ƿ���[1, 9]��Χ��
    //if (special_num >= 1 && special_num <= 9) {
    if (special_num != INVALID_SPECIAL_NUM) {
#if 1
        if (max_digital <= integer_base10_length) {
            // ɸѡ���и�,ʮ,��,ǧ,��,ʮ��,����λ���Ȱ���first_special_num����
            for (digital = 0; digital < max_digital; ++digital) {
                right_start_num = special_num * integer_base10[digital];
                right_max_num = MIN(right_start_num + integer_base10[digital] - 1, max_number);
                // �ұߵĲ�����Ϊ1
                right_num_step = 1;
                // ����right_start_num��Ȼ�Ѿ���first_special_num�ı���,
                // ������Ϊ��Ҫ�������(��λ)��ѭ��, ���Բ���ʡ��
                // �ұ�ѭ��(�ø�,ʮ,��,ǧ,��λ���ұ�, ����λѭ��)
                for (right_num = right_start_num; right_num <= right_max_num; right_num += right_num_step) {
                    //if (sayword_index_list[right_num] != FIRST_SPECIAL_NUM_FIXED_INDEX)
                        sayword_index_list[right_num] = FIRST_SPECIAL_NUM_FIXED_INDEX;

                    left_num_step = integer_base10[digital + 1];
                    left_start_num = right_num + left_num_step;
                    // ���ѭ��(�ø�,ʮ,��,ǧ,��λ�����, ����λѭ��)
                    for (left_num = left_start_num; left_num <= max_number; left_num += left_num_step) {
                        //if (sayword_index_list[left_num] != FIRST_SPECIAL_NUM_FIXED_INDEX)
                            sayword_index_list[left_num] = FIRST_SPECIAL_NUM_FIXED_INDEX;
                    }
                }
            }
        }
        else {
            // ɸѡ���и�,ʮ,��,ǧ,��,ʮ��,����λ���Ȱ���first_special_num����
            for (digital = 0; digital < max_digital; ++digital) {
                right_start_num = special_num * integer_base10[digital];
                right_max_num = MIN(right_start_num + integer_base10[digital] - 1, max_number);
                // �ұߵĲ�����Ϊ1
                right_num_step = 1;
                // ����right_start_num��Ȼ�Ѿ���first_special_num�ı���,
                // ������Ϊ��Ҫ�������(��λ)��ѭ��, ���Բ���ʡ��
                // �ұ�ѭ��(�ø�,ʮ,��,ǧ,��λ���ұ�, ����λѭ��)
                for (right_num = right_start_num; right_num <= right_max_num; right_num += right_num_step) {
                    //if (sayword_index_list[right_num] != FIRST_SPECIAL_NUM_FIXED_INDEX)
                        sayword_index_list[right_num] = FIRST_SPECIAL_NUM_FIXED_INDEX;

                    if (digital < integer_base10_length) {
                        left_num_step = integer_base10[digital + 1];
                        left_start_num = right_num + left_num_step;
                        // ���ѭ��(�ø�,ʮ,��,ǧ,��λ�����, ����λѭ��)
                        for (left_num = left_start_num; left_num <= max_number; left_num += left_num_step) {
                            //if (sayword_index_list[left_num] != FIRST_SPECIAL_NUM_FIXED_INDEX)
                                sayword_index_list[left_num] = FIRST_SPECIAL_NUM_FIXED_INDEX;
                        }
                    }
                }
            }
        }
#else
        // ɸѡ���и�,ʮ,��,ǧ,��,ʮ��,����λ���Ȱ���first_special_num����

        // �ȴ����λ��, ��Ϊ��λ���Ƚ�����, ����û���ұ�ѭ����, �����ó������⴦��
        // �����и���ֵ�����, ��һ������special_num��ʼѭ����������һ��,
        // ��Ϊ�������ֵ�Ѿ��� FIRST_SPECIAL_NUM_FIXED_INDEX �˵�, ������û��Ҫ��д���
#if 0
        left_start_num = special_num;
        // ���ѭ��(�ø�,ʮ,��,ǧ,��λ�����, ����λѭ��)
        for (left_num = left_start_num; left_num <= max_number; left_num += 10) {
            sayword_index_list[left_num] = FIRST_SPECIAL_NUM_FIXED_INDEX;
        }
#else
        left_num_step = integer_base10[1];
        left_start_num = special_num;
        // ���ѭ��(�ø�,ʮ,��,ǧ,��λ�����, ����λѭ��)
        for (left_num = left_start_num; left_num <= max_number; left_num += left_num_step) {
            sayword_index_list[left_num] = FIRST_SPECIAL_NUM_FIXED_INDEX;
        }
#endif

        // �ٴ���ʮλ, ��λ, ǧλ, ��λ��
        for (digital = 1; digital < max_digital; ++digital) {
            left_num_step = integer_base10[digital + 1];
            left_start_num = special_num * integer_base10[digital];
            // ���ѭ��(�ø�,ʮ,��,ǧ,��λ�����, ����λѭ��)
            for (left_num = left_start_num; left_num <= max_number; left_num += left_num_step) {
                sayword_index_list[left_num] = FIRST_SPECIAL_NUM_FIXED_INDEX;

                right_max_num = MIN(integer_base10[digital] - 1, max_number);
                // �ұ�ѭ������ʼ����Ϊ1, ��Ϊ0��ǰ���Ѿ�������, ����һ��ѭ��
                right_start_num = left_num + 1;
                // �ұ�ѭ���Ĳ�����Ϊ1(�˱�����ʵû�õ�)
                right_num_step = 1;
                // �ұ�ѭ��(�ø�,ʮ,��,ǧ,��λ���ұ�, ����λѭ��)
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

    // ������
    printf("FizzBuzzWhizz_fast() ReportNumber List:\n\nmax_number = %d\n\n", max_number);
    for (num = 1; num <= max_number; ++num) {
        index = sayword_index_list[num];
        // �������������, ��ֱ���������, �������index(maskֵ)��Ӧ���ַ���
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

    // ������������Ļ, ��������ַ�������
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
        // �������������, ��ֱ���������, �������index(maskֵ)��Ӧ���ַ���
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

    // �ͷ��ڴ�
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
            // ������
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
