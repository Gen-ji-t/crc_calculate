#include <stdio.h>
#include <string.h>
#include "crc.h"

#define BUF_SIZE 8192

static void usage(const char *prog)
{
    fprintf(stderr, "用法: %s [-8|-16|-32|-64] <文件>\n", prog);
    fprintf(stderr, "  -8   CRC-8/SMBUS\n");
    fprintf(stderr, "  -16  CRC-16/Modbus\n");
    fprintf(stderr, "  -32  CRC-32/ISO-HDLC (默认)\n");
    fprintf(stderr, "  -64  CRC-64/ECMA-182\n");
}

int main(int argc, char *argv[])
{
    const char *path;
    int algo = 32;

    if (argc == 2) {
        path = argv[1];
    } else if (argc == 3) {
        if      (strcmp(argv[1], "-8")  == 0) algo = 8;
        else if (strcmp(argv[1], "-16") == 0) algo = 16;
        else if (strcmp(argv[1], "-32") == 0) algo = 32;
        else if (strcmp(argv[1], "-64") == 0) algo = 64;
        else { usage(argv[0]); return 1; }
        path = argv[2];
    } else {
        usage(argv[0]);
        return 1;
    }

    FILE *fp = fopen(path, "rb");
    if (!fp) { perror(path); return 1; }

    uint8_t buf[BUF_SIZE];
    size_t n;
    uint8_t  c8  = 0x00;
    uint16_t c16 = 0xFFFF;
    uint32_t c32 = 0xFFFFFFFF;
    uint64_t c64 = 0xFFFFFFFFFFFFFFFFULL;

    while ((n = fread(buf, 1, BUF_SIZE, fp)) > 0) {
        if      (algo == 8)  c8  = crc8_update (c8,  buf, n);
        else if (algo == 16) c16 = crc16_update(c16, buf, n);
        else if (algo == 64) c64 = crc64_update(c64, buf, n);
        else                 c32 = crc32_update(c32, buf, n);
    }
    fclose(fp);

    printf("文件: %s\n", path);
    if (algo == 8)
        printf("算法: CRC-8\n结果: 0x%02X\n", c8);
    else if (algo == 16)
        printf("算法: CRC-16\n结果: 0x%04X\n", c16);
    else if (algo == 64)
        printf("算法: CRC-64\n结果: 0x%016llX\n", (unsigned long long)(c64 ^ 0xFFFFFFFFFFFFFFFFULL));
    else
        printf("算法: CRC-32\n结果: 0x%08X\n", c32 ^ 0xFFFFFFFF);

    return 0;
}
