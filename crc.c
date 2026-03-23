#include "crc.h"

static uint8_t  crc8_table[256];
static uint16_t crc16_table[256];
static uint32_t crc32_table[256];
static uint64_t crc64_table[256];
static int tables_ready;

static void init_tables(void)
{
    int i, j;
    uint8_t  c8;
    uint16_t c16;
    uint32_t c32;
    uint64_t c64;

    for (i = 0; i < 256; i++) {
        c8 = (uint8_t)i;
        for (j = 0; j < 8; j++)
            c8 = (c8 & 0x80) ? (c8 << 1) ^ 0x07 : c8 << 1;
        crc8_table[i] = c8;

        c16 = (uint16_t)i;
        for (j = 0; j < 8; j++)
            c16 = (c16 & 1) ? (c16 >> 1) ^ 0xA001 : c16 >> 1;
        crc16_table[i] = c16;

        c32 = (uint32_t)i;
        for (j = 0; j < 8; j++)
            c32 = (c32 & 1) ? (c32 >> 1) ^ 0xEDB88320 : c32 >> 1;
        crc32_table[i] = c32;

        c64 = (uint64_t)i;
        for (j = 0; j < 8; j++)
            c64 = (c64 & 1) ? (c64 >> 1) ^ 0xC96C5795D7870F42ULL : c64 >> 1;
        crc64_table[i] = c64;
    }
    tables_ready = 1;
}

uint8_t crc8_update(uint8_t crc, const uint8_t *buf, size_t len)
{
    if (!tables_ready) init_tables();
    while (len--)
        crc = crc8_table[crc ^ *buf++];
    return crc;
}

uint16_t crc16_update(uint16_t crc, const uint8_t *buf, size_t len)
{
    if (!tables_ready) init_tables();
    while (len--)
        crc = crc16_table[(crc ^ *buf++) & 0xFF] ^ (crc >> 8);
    return crc;
}

uint32_t crc32_update(uint32_t crc, const uint8_t *buf, size_t len)
{
    if (!tables_ready) init_tables();
    while (len--)
        crc = crc32_table[(crc ^ *buf++) & 0xFF] ^ (crc >> 8);
    return crc;
}

uint64_t crc64_update(uint64_t crc, const uint8_t *buf, size_t len)
{
    if (!tables_ready) init_tables();
    while (len--)
        crc = crc64_table[(crc ^ *buf++) & 0xFF] ^ (crc >> 8);
    return crc;
}
