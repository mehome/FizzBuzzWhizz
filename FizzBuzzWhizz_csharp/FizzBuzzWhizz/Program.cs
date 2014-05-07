//#define NEED_DISPLAY_RESULT

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;

/// <comment>
///
/// 最难面试的IT公司之ThoughtWorks代码挑战——FizzBuzzWhizz游戏
///
/// From: http://www.cnblogs.com/shines77/p/3712642.html
///
/// 参考: http://www.cnblogs.com/lanxuezaipiao/p/3705313.html
///
/// </comment>

namespace FizzBuzzWhizz
{
    class Program
    {
        private const int INVALID_SPECIAL_NUM = -1;
        private const int NORMAL_NUM_INDEX = 0;
        private const int FIRST_SPECIAL_NUM_FIXED_INDEX = 1;

        static Stopwatch swSolve = new Stopwatch();
        static Stopwatch swDisplay = new Stopwatch();

        static void Main(string[] args)
        {
            /*
            int[] special_num_list = null;
            //int[] special_num_list = new int[3] { 3, 5, 7 };
            //int[] special_num_list = new int[3] { 2, 4, 8 };
            //int[] special_num_list = new int[3] { 3, 6, 8 };

            special_num_list = new int[3] { 3, 5, 7 };
            FizzBuzzWhizz_Test_Warpper(special_num_list);

            special_num_list = new int[3] { 2, 4, 8 };
            FizzBuzzWhizz_Test_Warpper(special_num_list);

            special_num_list = new int[3] { 3, 6, 8 };
            FizzBuzzWhizz_Test_Warpper(special_num_list);
            //*/

            Test("3,5,7");
            Test("2,4,8");
            Test("3,6,8");

            Console.Write("\n");
            Console.Write("Press any key to continue...");
            Console.ReadKey();
        }

        static string[] FizzBuzzWhizz_Test_Warpper(int[] special_num_list)
        {
            const int max_number = 100;
            string[] word_list = new string[3] { "Fizz", "Buzz", "Whizz" };
            string[] result = null;

            //FizzBuzzWhizz_Test(max_number, word_list.Length, word_list, special_num_list);

            swSolve.Restart();
            for (int i = 0; i < 10000; i++)
            {
                result = FizzBuzzWhizz_Test(max_number, word_list.Length, word_list, special_num_list);
            }
            swSolve.Stop();

#if false
            Console.WriteLine("FizzBuzzWhizz ReportNumber List:\n\nmax_number = {0}\n", max_number);
            for (int num = 1; num <= max_number; ++num)
            {
                Console.WriteLine("{0}", result[num]);
            }
            Console.WriteLine("");
#endif

            // 计算FizzBuzzWhizz结果所用的时间和ticks
            Console.WriteLine("=================================================\n");
            Console.WriteLine("Solve Elapsed Time:  {0:0.000} ms\n", swSolve.Elapsed.TotalMilliseconds);
            Console.WriteLine("Solve Elapsed Ticks: {0} tick(s)\n", swSolve.ElapsedTicks);
            Console.WriteLine("=================================================\n");

            return result;
        }

        static string[] FizzBuzzWhizz_Test(int max_number, int max_word_type,
                   string[] word_list, int[] special_num_list)
        {
            int max_mask, integer_base10_length;
            int digital, max_digital;
            int num, index;
            int mask, cur_mask;
            int special_num;
            int first_special_num;

            int left_num, right_num;
            int left_start_num, right_start_num;
            int left_num_step;
            int right_num_step;
            int right_max_num;

            string sayword = null;
            string[] sayword_list = null;
            byte[] sayword_index_list = null;

            string[] sayword_result = null;

            int[] integer_base10 = new int[11] {
                1, 10, 100, 1000, 10000, 100000, 1000000,
                10000000, 100000000, 1000000000, int.MaxValue
            };

            /*
            uint[] power_of_2 = {
                0x01,      0x02,      0x04,      0x08,
                0x10,      0x20,      0x40,      0x80,
                0x100,     0x200,     0x400,     0x800,
                0x1000,    0x2000,    0x4000,    0x8000,
                0x10000,   0x20000,   0x40000,   0x80000,
                0x100000,  0x200000,  0x400000,  0x800000,
                0x1000000, 0x2000000, 0x4000000, 0x8000000,
                0x10000000,0x20000000,0x40000000,0x80000000,
            };
            //*/

#if NEED_DISPLAY_RESULT
            swSolve.Restart();
#endif

            // 最大数的数字位数(数字最大长度), 可以用log函数求, 但这里我们用查表法, 因为这个表我们后面也要用的
            max_digital = 0;
            integer_base10_length = integer_base10.Length - 1;
            for (index = 0; index < integer_base10_length; ++index) {
                if (max_number >= integer_base10[index] && max_number < integer_base10[index + 1]) {
                    max_digital = index + 1;
                    break;
                }
            }

            // 最大mask(bit掩模)值, = 2^(max_word_type) = 2^3 = 8
            max_mask = 1 << max_word_type;

            // 求出mask对应的特殊报数字符串, 共max_mask种(2^3 = 8种)
            sayword_list = new string[max_mask];
            for (mask = 0; mask < max_mask; ++mask) {
                sayword = "";
                cur_mask = mask;
                for (index = 0; index < max_word_type; ++index) {
                    if ((cur_mask & 1) != 0)
                        sayword += word_list[index];
                    cur_mask >>= 1;
                }
                sayword_list[mask] = sayword;
            }

            // 分配sayword_index_list内存
            sayword_index_list = new byte[max_number + 1];

            // 所有sayword_index_list的默认值均为NORMAL_NUM_INDEX(0), 即默认是非特殊报数
            for (num = 0; num <= max_number; ++num) {
                sayword_index_list[num] = NORMAL_NUM_INDEX;
            }

            // 计算(合并)和设置所有特殊数的mask值
            for (index = 0; index < max_word_type; ++index) {
                // 取一个特殊数
                special_num = special_num_list[index];
                // 如果特殊数不在[1, 9]范围内, 则认为是无效特殊数, 跳过
                //if (special_num >= 1 && special_num <= 9) {
                    // 该特殊数的mask值
                    //mask = (int)Math.Pow((double)2.0, (double)index);
                    //mask = (int)power_of_2[index];
                    mask = 1 << index;
                    for (num = special_num; num <= max_number; num += special_num)
                        sayword_index_list[num] |= (byte)mask;
                /*
                }
                else {
                    special_num_list[index] = INVALID_SPECIAL_NUM;
                }
                //*/
            }

            // 根据规则5, 设置所有所报数字包含了第一个特殊数(first)的数, 先筛选所有个位数包含first的数,
            // 再筛选所有十位数包含first的数, 依此类推, 百位, 千位..., 直接达到max_number
            // FIRST_SPECIAL_NUM_FIXED_INDEX的值固定为1, 因为第一个特殊数(仅第一个)的mask就是1

            // 第一个特殊数
            first_special_num = special_num_list[0];

            if (first_special_num != INVALID_SPECIAL_NUM) {

                // 筛选所有个,十,百,千,万,十万,百万位数等包含first_special_num的数
                for (digital = 0; digital < max_digital; ++digital) {
                    right_start_num = first_special_num * integer_base10[digital];
                    right_max_num = right_start_num + integer_base10[digital];
                    // 右边的步长横为1
                    right_num_step = 1;
                    // 这里(right_start_num + 0)虽然已经是first_special_num的倍数, 但是因为还要进行左边(高位)的循环, 所以不能省略
                    // 右边循环(该个,十,百,千,万位的右边, 即低位循环)
                    for (right_num = right_start_num + 0; (right_num < right_max_num && right_num <= max_number); right_num += right_num_step) {
                    //for (right_num = right_start_num + 0; (right_num < right_max_num && right_num <= max_number); ++right_num) {
                        if (sayword_index_list[right_num] != FIRST_SPECIAL_NUM_FIXED_INDEX)
                            sayword_index_list[right_num] = FIRST_SPECIAL_NUM_FIXED_INDEX;

                        if (digital < integer_base10_length) {
                            left_num_step = integer_base10[digital + 1];
                            left_start_num = right_num + left_num_step;
                            // 左边循环(该个,十,百,千,万位的左边, 即高位循环)
                            for (left_num = left_start_num; left_num <= max_number; left_num += left_num_step) {
                                if (sayword_index_list[left_num] != FIRST_SPECIAL_NUM_FIXED_INDEX)
                                    sayword_index_list[left_num] = FIRST_SPECIAL_NUM_FIXED_INDEX;
                            }
                        }
                    }
                }

            }

            sayword_result = new string[max_number + 1];
            for (num = 1; num <= max_number; ++num) {
                index = sayword_index_list[num];
                // 如果不是特殊数, 则直接输出该数, 否则输出index(mask值)对应的字符串
                if (index == NORMAL_NUM_INDEX)
                    sayword_result[num] = num.ToString();
                else
                    sayword_result[num] = sayword_list[index];
            }

#if NEED_DISPLAY_RESULT
            swSolve.Stop();
#endif

#if NEED_DISPLAY_RESULT

            // 输出结果
            swDisplay.Restart();
#if true
            Console.WriteLine("FizzBuzzWhizz ReportNumber List:\n\nmax_number = {0}\n", max_number);
            for (num = 1; num <= max_number; ++num) {
                index = sayword_index_list[num];
                // 如果不是特殊数, 则直接输出该数, 否则输出index(mask值)对应的字符串
                if (index == NORMAL_NUM_INDEX)
                    Console.WriteLine("{0}", num);
                else
                    Console.WriteLine("{0}", sayword_list[index]);
            }
            Console.WriteLine("");
#else
            Console.WriteLine("FizzBuzzWhizz ReportNumber List:\n\nmax_number = {0}\n", max_number);
            for (num = 1; num <= max_number; ++num) {
                Console.WriteLine("{0}", sayword_result[num]);
            }
            Console.WriteLine("");
#endif
            swDisplay.Stop();

            // 计算FizzBuzzWhizz结果所用的时间和ticks
            Console.WriteLine("=================================================\n");
            Console.WriteLine("Solve Elapsed Time:  {0:0.000} ms\n", swSolve.Elapsed.TotalMilliseconds);
            Console.WriteLine("Solve Elapsed Ticks: {0} tick(s)\n",  swSolve.ElapsedTicks);

            // 显示结果所用的时间和ticks
            Console.WriteLine("=================================================\n");
            Console.WriteLine("Display Result Elapsed Time:  {0:0.000} ms\n", swDisplay.Elapsed.TotalMilliseconds);
            Console.WriteLine("Display Result Elapsed Ticks: {0} tick(s)\n",  swDisplay.ElapsedTicks);

            Console.WriteLine("=================================================");
#endif
            /* NEED_DISPLAY_RESULT */

            return sayword_result;
        }

        static void Test(string str)
        {
            const int max_number = 100;
            string[] word_list = new string[3] { "Fizz", "Buzz", "Whizz" };

            Console.WriteLine(str);
            string[] s1 = str.Split(',');
            int[] s2 = new int[] { int.Parse(s1[0]), int.Parse(s1[1]), int.Parse(s1[2]) };
            /*
            string[] r1 = Test1(s2);
            string[] r2 = Test2(s2);
            string[] r3 = FizzBuzzWhizz_Test(max_number, word_list.Length, word_list, s2);
            bool flag = true;
            for (int i = 0; i < 100; i++)
            {
                if (r1[i] != r2[i])
                {
                    flag = false;
                    break;
                }
            }
            if (flag)
            {
                Console.WriteLine("结果相等");
            }
            else
            {
                Console.WriteLine("结果不相等");
            }

            flag = true;
            for (int i = 0; i < 100; i++)
            {
                if (r1[i] != r3[i + 1])
                {
                    flag = false;
                    break;
                }
            }
            if (flag)
            {
                Console.WriteLine("结果相等");
            }
            else
            {
                Console.WriteLine("结果不相等");
            }
            //*/

            /*
            DateTime dt = DateTime.Now;
            for (int i = 0; i < 10000; i++)
            {
                Test1(s2);
            }
            TimeSpan ts = DateTime.Now - dt;
            Console.WriteLine("文艺青年耗时：" + ts.TotalMilliseconds);

            DateTime dt2 = DateTime.Now;
            for (int i = 0; i < 10000; i++)
            {
                Test2(s2);
            }
            TimeSpan ts2 = DateTime.Now - dt2;
            Console.WriteLine("屌丝青年耗时：" + ts2.TotalMilliseconds);

            DateTime dt3 = DateTime.Now;
            for (int i = 0; i < 10000; i++)
            {
                FizzBuzzWhizz_Test(max_number, word_list.Length, word_list, s2);
            }
            TimeSpan ts3 = DateTime.Now - dt3;
            Console.WriteLine("失业青年耗时：" + ts3.TotalMilliseconds);

            Console.WriteLine();
            //*/

            swSolve.Restart();
            for (int i = 0; i < 10000; i++)
            {
                Test1(s2);
            }
            swSolve.Stop();
            Console.WriteLine("文艺青年耗时：" + swSolve.Elapsed.TotalMilliseconds + " ms");
            //Console.WriteLine("文艺青年耗时：" + swSolve.ElapsedTicks + " tick(s)");

            swSolve.Restart();
            for (int i = 0; i < 10000; i++)
            {
                Test2(s2);
            }
            swSolve.Stop();
            Console.WriteLine("屌丝青年耗时：" + swSolve.Elapsed.TotalMilliseconds + " ms");
            //Console.WriteLine("屌丝青年耗时：" + swSolve.ElapsedTicks + " tick(s)");

            swSolve.Restart();
            for (int i = 0; i < 10000; i++)
            {
                FizzBuzzWhizz_Test(max_number, word_list.Length, word_list, s2);
            }
            swSolve.Stop();
            Console.WriteLine("失业青年耗时：" + swSolve.Elapsed.TotalMilliseconds + " ms");
            //Console.WriteLine("失业青年耗时：" + swSolve.ElapsedTicks + " tick(s)");

            swSolve.Restart();
            for (int i = 0; i < 10000; i++)
            {
                Test3(s2);
            }
            swSolve.Stop();
            Console.WriteLine("牛比青年耗时：" + swSolve.Elapsed.TotalMilliseconds + " ms");
            //Console.WriteLine("牛比青年耗时：" + swSolve.ElapsedTicks + " tick(s)");

            Console.WriteLine();
        }

        private static string[] Test1(int[] a)
        {
            string[] result = new string[100];
            for (int i = 1; i <= 100; i++)
            {
                result[i - 1] = (i % a[0] == 0 ? "Fizz" : "") + (i % a[1] == 0 ? "Buzz" : "") + (i % a[2] == 0 ? "Whizz" : "") + ((i % a[0] != 0 && i % a[1] != 0 && i % a[2] != 0) ? i.ToString() : "");
            }
            for (int i = 0; i < 10; i++)
            {
                result[a[0] * 10 + i - 1] = result[i * 10 + a[0] - 1] = "Fizz";
            }
            return result;
        }

        private static string[] Test2(int[] a)
        {
            string[] result = new string[100];
            for (int i = 0; i < 10; i++)
            {
                result[a[0] * 10 + i - 1] = result[i * 10 + a[0] - 1] = "Fizz";
            }

            comp(result, nlcm(a, 3), "FizzBuzzWhizz");

            comp(result, lcm(a[0], a[1]), "FizzBuzz");
            comp(result, lcm(a[0], a[2]), "FizzWhizz");
            comp(result, lcm(a[1], a[2]), "BuzzWhizz");

            comp(result, a[0], "Fizz");
            comp(result, a[1], "Buzz");
            comp(result, a[2], "Whizz");

            for (int i = 1; i <= 100; i++)
            {
                if (string.IsNullOrEmpty(result[i - 1]))
                {
                    result[i - 1] = i.ToString();
                }
            }
            return result;
        }

        private static string[] Test3(int[] a)
        {
            string[] result = new string[101];
            //Console.WriteLine("说出三个不同的特殊数,用','隔开 ");
            //string[] s1 = Console.ReadLine().Split(',');
            string[] s1 = new string[3] {
                a[0].ToString(), a[1].ToString(), a[2].ToString()
            };
            for (int i = 1; i < 101; i++)
            {
                if (i.ToString().Contains(s1[0])) {
                    result[i] = "Fizz";
                }
                else {
                    result[i] = (i % int.Parse(s1[0]) == 0 ? "Fizz" : "") + (i % int.Parse(s1[1]) == 0 ? "Buzz" : "") + (i % int.Parse(s1[2]) == 0 ? "Whizz" : "") + ((i % int.Parse(s1[0]) != 0 && i % int.Parse(s1[1]) != 0 && i % int.Parse(s1[2]) != 0) ? i.ToString() : "");
                }
            }
            //Console.ReadKey();
            return result;
        }

        static void comp(string[] result, int temp, string str)
        {
            for (int i = 1; i * temp <= 100; i++)
            {
                if (string.IsNullOrEmpty(result[i * temp - 1]))
                {
                    result[i * temp - 1] = str;
                }
            }
        }

        static int gcd(int a, int b)
        {
            if (a < b)
                swap(ref a, ref b);
            if (b == 0)
                return a;
            else
                return gcd(b, a % b);
        }

        private static void swap(ref int a, ref int b)
        {
            int c = a;
            a = b;
            b = c;
        }

        static int ngcd(int[] a, int n)
        {
            if (n == 1) return a[0];
            return gcd(a[n - 1], ngcd(a, n - 1));
        }

        static int lcm(int a, int b)
        {
            return a * b / gcd(a, b);
        }

        static int nlcm(int[] a, int n)
        {
            if (n == 1)
                return a[0];
            else
                return lcm(a[n - 1], nlcm(a, n - 1));
        }
    }
}
