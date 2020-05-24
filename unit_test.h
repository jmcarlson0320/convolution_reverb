/***************************************************************************
Author: J. M. Carlson (jmc34@pdx.edu)
Date: 4/24/2020

A very basic unit testing framework.

To use:
1. Create a *.c file that includes this header. Inside that file:
2. Write unit tests with the function signature "int test(void)" that
return PASS or FAIL.
3. Define the TESTS array to contain the unit test names and function
pointers and ends with a NULL struct:
TESTS = {
{"name of test 1", test_function_1},
{"name of test 1", test_function_1},
{"name of test 1", test_function_1},
.
.
.
{0}
};
4. Compile the *.c file as normal:
$ gcc -o run_tests my_unit_tests.c
5. Run the tests:
$ ./run_tests

 ***************************************************************************/
#ifndef UNIT_TESTS_H
#define UNIT_TESTS_H

#include <stdio.h>

#define PASS 1
#define FAIL 0

typedef int (*func_ptr)(void);

struct unit_test {
        char *name;
        func_ptr function;
};

#define TESTS struct unit_test test_list[]
extern struct unit_test test_list[];

static void run_tests(void)
{
        int i = 0;
        while (test_list[i].name != NULL) {
                printf("%s: ", test_list[i].name);
                if ((*test_list[i].function)() == PASS) {
                        printf("passed\n");
                } else
                        printf("failed\n");
                i++;
        }
}

int main()
{
        run_tests();
        return 0;
}

#endif // UNIT_TESTS_H
