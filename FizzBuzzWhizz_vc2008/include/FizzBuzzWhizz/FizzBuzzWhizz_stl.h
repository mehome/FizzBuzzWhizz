
#ifndef _FIZZBUZZWHIZZ_STL_H_
#define _FIZZBUZZWHIZZ_STL_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <FizzBuzzWhizz/common.h>
#include <string>
#include <vector>

using namespace std;

void FizzBuzzWhizz_stl(const int max_number,
                       const int max_word_type, const int max_word_length,
                       const string say_word_list[], int special_num_list[],
                       std::vector<string> &say_word_result);

void FizzBuzzWhizz_stl_Test(const int max_number,
                            const int max_word_type, const int max_word_length,
                            const string say_word_list[], int special_num_list[],
                            bool display_to_screen);

#endif  /* _FIZZBUZZWHIZZ_STL_H_ */
