#ifndef CRC_H
#define CRC_H

#include <stdint.h>
#include <stddef.h>

uint8_t  crc8_update (uint8_t  crc, const uint8_t *buf, size_t len);
uint16_t crc16_update(uint16_t crc, const uint8_t *buf, size_t len);
uint32_t crc32_update(uint32_t crc, const uint8_t *buf, size_t len);
uint64_t crc64_update(uint64_t crc, const uint8_t *buf, size_t len);

#endif
