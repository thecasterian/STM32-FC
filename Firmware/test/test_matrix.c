#ifdef TEST

#include "unity.h"

#include "matrix.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test_matrix_transpose(void)
{
    float m[6] = {
        1.f, 2.f, 3.f,
        4.f, 5.f, 6.f,
    };
    float res[6];
    float expected[6] = {
        1.f, 4.f,
        2.f, 5.f,
        3.f, 6.f,
    };

    matrix_transpose(m, 2, 3, res);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, res, 6);
}

void test_matrix_cholesky(void)
{
    float m[16] = {
          9.f,   0.f, -27.f,  18.f,
          0.f,   9.f,  -9.f, -27.f,
        -27.f,  -9.f,  99.f, -27.f,
         18.f, -27.f, -27.f, 121.f,
    };
    float res[16];
    float expected[16] = {
         3.f,  0.f,  0.f,  0.f,
         0.f,  3.f,  0.f,  0.f,
        -9.f, -3.f,  3.f,  0.f,
         6.f, -9.f,  0.f,  2.f,
    };

    matrix_cholesky(m, 4, res);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, res, 16);
}

void test_matrix_inv_sym(void)
{
    float m[16] = {
          9.f,   0.f, -27.f,  18.f,
          0.f,   9.f,  -9.f, -27.f,
        -27.f,  -9.f,  99.f, -27.f,
         18.f, -27.f, -27.f, 121.f,
    };
    float res[16];
    float expected[16] = {
         76.f, -42.f,  12.f, -18.f,
        -42.f,  89.f,   4.f,  27.f,
         12.f,   4.f,   4.f,   0.f,
        -18.f,  27.f,   0.f,   9.f,
    };

    for (int i = 0; i < 16; i++)
    {
        expected[i] /= 36.f;
    }

    matrix_inv_sym(m, 4, res);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, res, 16);
}

void test_matrix_add_scalar(void)
{
    float m[6] = {
        1.f, 2.f, 3.f,
        4.f, 5.f, 6.f,
    };
    float res[6];
    float expected[6] = {
        2.f, 3.f, 4.f,
        5.f, 6.f, 7.f,
    };

    matrix_add_scalar(m, 1.f, 2, 3, res);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, res, 6);
}

void test_matrix_add_eye(void)
{
    float m[9] = {
        1.f, 2.f, 3.f,
        4.f, 5.f, 6.f,
        7.f, 8.f, 9.f,
    };
    float res[9];
    float expected[9] = {
        -1.f,  2.f,  3.f,
         4.f,  3.f,  6.f,
         7.f,  8.f,  7.f,
    };

    matrix_add_eye(m, -2.f, 3, res);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, res, 9);
}

void test_matrix_mul_scalar(void)
{
    float m[6] = {
        1.f, 2.f, 3.f,
        4.f, 5.f, 6.f,
    };
    float res[6];
    float expected[6] = {
        2.f, 4.f, 6.f,
        8.f, 10.f, 12.f,
    };

    matrix_mul_scalar(m, 2.f, 2, 3, res);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, res, 6);
}

void test_matrix_add(void)
{
    float a[6] = {
        1.f, 2.f, 3.f,
        4.f, 5.f, 6.f,
    };
    float b[6] = {
        3.f, 1.f, 9.f,
        2.f, 8.f, 4.f,
    };
    float res[6];
    float expected[6] = {
         4.f,  3.f, 12.f,
         6.f, 13.f, 10.f,
    };

    matrix_add(a, b, 2, 3, res);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, res, 6);
}

void test_matrix_sub(void)
{
    float a[6] = {
        1.f, 2.f, 3.f,
        4.f, 5.f, 6.f,
    };
    float b[6] = {
        3.f, 1.f, 9.f,
        2.f, 8.f, 4.f,
    };
    float res[6];
    float expected[6] = {
        -2.f,  1.f, -6.f,
         2.f, -3.f,  2.f,
    };

    matrix_sub(a, b, 2, 3, res);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, res, 6);
}

void test_matrix_mul(void)
{
    float a[6] = {
        1.f, 2.f, 3.f,
        4.f, 5.f, 6.f,
    };
    float b[12] = {
        5.f, 6.f, 0.f, 1.f,
        0.f, 3.f, 2.f, 0.f,
        1.f, 0.f, 4.f, 3.f,
    };
    float res[8];
    float expected[8] = {
         8.f, 12.f, 16.f, 10.f,
        26.f, 39.f, 34.f, 22.f,
    };

    matrix_mul(a, b, 2, 3, 4, res);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, res, 8);
}

#endif // TEST
