#ifdef TEST

#include "unity.h"

#include "util.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test_pack2(void)
{
    TEST_ASSERT_EQUAL_UINT16(0x0000U, pack2(0x00U, 0x00U));
    TEST_ASSERT_EQUAL_UINT16(0x0001U, pack2(0x00U, 0x01U));
    TEST_ASSERT_EQUAL_UINT16(0x0100U, pack2(0x01U, 0x00U));
    TEST_ASSERT_EQUAL_UINT16(0x0101U, pack2(0x01U, 0x01U));
    TEST_ASSERT_EQUAL_UINT16(0x1234U, pack2(0x12U, 0x34U));
    TEST_ASSERT_EQUAL_UINT16(0xFFFFU, pack2(0xFFU, 0xFFU));
}

void test_to_int16(void)
{
    TEST_ASSERT_EQUAL_INT16(0x0000, to_int16(0x0000U));
    TEST_ASSERT_EQUAL_INT16(0x0001, to_int16(0x0001U));
    TEST_ASSERT_EQUAL_INT16(0x7FFF, to_int16(0x7FFFU));
    TEST_ASSERT_EQUAL_INT16(-0x8000, to_int16(0x8000U));
    TEST_ASSERT_EQUAL_INT16(-0x0001, to_int16(0xFFFFU));
}

void test_is_boolean_u8(void)
{
    TEST_ASSERT_TRUE(is_boolean_u8(0U));
    TEST_ASSERT_TRUE(is_boolean_u8(1U));
    TEST_ASSERT_FALSE(is_boolean_u8(2U));
    TEST_ASSERT_FALSE(is_boolean_u8(255U));
}

void test_memcpy_to_volatile(void)
{
    uint8_t src[255];
    volatile uint8_t dst[255] = {0};

    for (uint16_t i = 0U; i < 255; i++)
    {
        src[i] = i;
    }

    memcpy_to_volatile(dst, src, 0U);
    for (uint16_t i = 0U; i < 255; i++)
    {
        TEST_ASSERT_EQUAL_UINT8(dst[i], 0U);
    }

    memcpy_to_volatile(dst, src, 255U);
    for (uint16_t i = 0U; i < 255; i++)
    {
        TEST_ASSERT_EQUAL_UINT8(dst[i], i);
    }
}

void test_memcpy_from_volatile(void)
{
    volatile uint8_t src[255];
    uint8_t dst[255] = {0};

    for (uint16_t i = 0U; i < 255; i++)
    {
        src[i] = i;
    }

    memcpy_from_volatile(dst, src, 0U);
    for (uint16_t i = 0U; i < 255; i++)
    {
        TEST_ASSERT_EQUAL_UINT8(dst[i], 0U);
    }

    memcpy_from_volatile(dst, src, 255U);
    for (uint16_t i = 0U; i < 255; i++)
    {
        TEST_ASSERT_EQUAL_UINT8(dst[i], i);
    }
}

#endif // TEST
