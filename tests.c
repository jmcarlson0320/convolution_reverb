#include "unit_test.h"

int test1()
{
    return FAIL;
}

int test2()
{
    return FAIL;
}

TESTS = {
    {"test 1", test1},
    {"test 2", test2},
    {0}
};
