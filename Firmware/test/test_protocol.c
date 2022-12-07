#ifdef TEST

#include "unity.h"

#include "mock_led.h"
#include "mock_ring_buffer.h"
#include "mock_usb_wrapper.h"
#include "protocol.h"

TEST_FILE("util.c")
TEST_FILE("streaming_data.c")

static bool usb_transmit_len_correct;
static uint8_t usb_transmit_buf[7];

static bool usb_ringbuf_pop_stub_no_stx(uint8_t *buf, uint16_t size, int num_calls);
static bool usb_ringbuf_pop_stub_incomplete_packet(uint8_t *buf, uint16_t size, int num_calls);
static bool usb_ringbuf_pop_stub_complete_packet(uint8_t *buf, uint16_t size, int num_calls);
static void usb_transmit_stub(uint8_t *buf, uint16_t len, int num_calls);

void setUp(void)
{
}

void tearDown(void)
{
}

void test_packet_receive_empty_usb_queue(void)
{
    packet_t packet;

    usb_ringbuf_pop_IgnoreAndReturn(false);

    packet_parser_init();
    TEST_ASSERT_FALSE(packet_receive(&packet));
    TEST_ASSERT_FALSE(packet_receive(&packet));
    TEST_ASSERT_FALSE(packet_receive(&packet));
}

void test_packet_receive_no_stx(void)
{
    packet_t packet;

    usb_ringbuf_pop_Stub(usb_ringbuf_pop_stub_no_stx);

    packet_parser_init();
    TEST_ASSERT_FALSE(packet_receive(&packet));
}

void test_packet_receive_incomplete_packet(void)
{
    packet_t packet;

    usb_ringbuf_pop_Stub(usb_ringbuf_pop_stub_incomplete_packet);

    packet_parser_init();
    TEST_ASSERT_FALSE(packet_receive(&packet));
}

void test_packet_receive_complete_packet(void)
{
    packet_t packet;

    usb_ringbuf_pop_Stub(usb_ringbuf_pop_stub_complete_packet);

    packet_parser_init();
    TEST_ASSERT_FALSE(packet_receive(&packet));
    TEST_ASSERT_TRUE(packet_receive(&packet));
    TEST_ASSERT_EQUAL_UINT8(packet.typ, TYP_CMD);
    TEST_ASSERT_EQUAL_UINT8(packet.len, 0x02);
    TEST_ASSERT_EQUAL_UINT8(packet.dat[0], 0x42);
    TEST_ASSERT_EQUAL_UINT8(packet.dat[1], 0xC9);
    TEST_ASSERT_EQUAL_UINT8(packet.cks, 0x0B);
    TEST_ASSERT_EQUAL_UINT8(packet.etx, ETX);
    TEST_ASSERT_FALSE(packet_receive(&packet));
}

void test_packet_validate_wrong_etx(void)
{
    packet_t packet = { .typ = TYP_CMD, .len = 0x02, .dat = { 0x42, 0xC9 }, .cks = 0x0B, .etx = 0x00 };

    TEST_ASSERT_EQUAL_UINT8(packet_validate(&packet), ERR_ETX_MIS);
}

void test_packet_validate_wrong_cks(void)
{
    packet_t packet = { .typ = TYP_CMD, .len = 0x02, .dat = { 0x42, 0xC9 }, .cks = 0x00, .etx = ETX };

    TEST_ASSERT_EQUAL_UINT8(packet_validate(&packet), ERR_CKS_MIS);
}

void test_packet_validate_wrong_typ(void)
{
    packet_t packet = { .typ = TYP_RESP, .len = 0x02, .dat = { 0x42, 0xC9 }, .cks = 0x0B, .etx = ETX };

    TEST_ASSERT_EQUAL_UINT8(packet_validate(&packet), ERR_TYP_INVAL);
}

void test_packet_validate_no_command(void)
{
    packet_t packet = { .typ = TYP_CMD, .len = 0x00, .cks = 0x00, .etx = ETX };

    TEST_ASSERT_EQUAL_UINT8(packet_validate(&packet), ERR_CMD_INVAL);
}

void test_packet_validate_correct(void)
{
    packet_t packet = { .typ = TYP_CMD, .len = 0x02, .dat = { 0x42, 0xC9 }, .cks = 0x0B, .etx = ETX };

    TEST_ASSERT_EQUAL_UINT8(packet_validate(&packet), ERR_OK);
}

void test_packet_checksum_calc(void)
{
    const uint8_t buf[12] = {0x9D, 0x0E, 0x4C, 0xD6, 0x7F, 0x88, 0x3C, 0x13, 0x12, 0xC5, 0xA4, 0x0A};

    TEST_ASSERT_EQUAL_UINT8(packet_checksum_calc(buf, sizeof(buf)), 0xA8);
}

void test_packet_checksum_calc_empty(void)
{
    TEST_ASSERT_EQUAL_UINT8(packet_checksum_calc(NULL, 0U), 0x00);
}

void test_response_send_ack(void)
{
    usb_transmit_Stub(usb_transmit_stub);

    response_send(ERR_OK);
    TEST_ASSERT_EQUAL_UINT8(usb_transmit_buf[0], STX);
    TEST_ASSERT_EQUAL_UINT8(usb_transmit_buf[1], TYP_RESP);
    TEST_ASSERT_EQUAL_UINT8(usb_transmit_buf[2], 0x02);
    TEST_ASSERT_EQUAL_UINT8(usb_transmit_buf[3], ACK);
    TEST_ASSERT_EQUAL_UINT8(usb_transmit_buf[4], ERR_OK);
    TEST_ASSERT_EQUAL_UINT8(usb_transmit_buf[5], ACK + ERR_OK);
    TEST_ASSERT_EQUAL_UINT8(usb_transmit_buf[6], ETX);
}

void test_response_send_nack(void)
{
    usb_transmit_Stub(usb_transmit_stub);

    response_send(ERR_CMD_INVAL);
    TEST_ASSERT_EQUAL_UINT8(usb_transmit_buf[0], STX);
    TEST_ASSERT_EQUAL_UINT8(usb_transmit_buf[1], TYP_RESP);
    TEST_ASSERT_EQUAL_UINT8(usb_transmit_buf[2], 0x02);
    TEST_ASSERT_EQUAL_UINT8(usb_transmit_buf[3], NACK);
    TEST_ASSERT_EQUAL_UINT8(usb_transmit_buf[4], ERR_CMD_INVAL);
    TEST_ASSERT_EQUAL_UINT8(usb_transmit_buf[5], NACK + ERR_CMD_INVAL);
    TEST_ASSERT_EQUAL_UINT8(usb_transmit_buf[6], ETX);
}

static bool usb_ringbuf_pop_stub_no_stx(uint8_t *buf, uint16_t size, int num_calls)
{
    /* Assume that USB queue has 10 non-STX characters. */
    if (num_calls < 10)
    {
        *buf = 0x00;
        return true;
    }
    else
    {
        return false;
    }
}

static bool usb_ringbuf_pop_stub_incomplete_packet(uint8_t *buf, uint16_t size, int num_calls)
{
    uint8_t ringbuf[5] = { STX, TYP_CMD, 0x02, 0x00, 0x00 };

    if (num_calls < 5)
    {
        *buf = ringbuf[num_calls];
        return true;
    }
    else
    {
        return false;
    }
}

static bool usb_ringbuf_pop_stub_complete_packet(uint8_t *buf, uint16_t size, int num_calls)
{
    uint8_t ringbuf[11] = { 0xFF, 0xFE, STX, TYP_CMD, 0x02, 0x42, 0xC9, 0x0B, ETX, 0x7, 0x4D };

    if (num_calls < 6)
    {
        *buf = ringbuf[num_calls];
        return true;
    }
    else if (num_calls == 6)
    {
        return false;
    }
    else if (num_calls < 12)
    {
        *buf = ringbuf[num_calls - 1];
        return true;
    }
    else
    {
        return false;
    }
}

static void usb_transmit_stub(uint8_t *buf, uint16_t len, int num_calls)
{
    if (len == 7U)
    {
        for (uint16_t i = 0U; i < len; i++)
        {
            usb_transmit_buf[i] = buf[i];
        }
        usb_transmit_len_correct = true;
    }
    else
    {
        usb_transmit_len_correct = false;
    }
}

#endif // TEST
