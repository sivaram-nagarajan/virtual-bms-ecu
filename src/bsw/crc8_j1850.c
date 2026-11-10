/**
 * @file crc8_j1850.c
 * @brief Table-free CRC-8 SAE J1850 implementation (small ROM footprint).
 */
#include "bms/bsw/crc8_j1850.h"

#include <stddef.h>

#define CRC8_J1850_POLYNOMIAL ((uint8_t)0x1DU)
#define CRC8_MSB_MASK ((uint8_t)0x80U)
#define BITS_PER_BYTE (8U)

/* The signature is fixed by AUTOSAR SWS CRC, so parameter order is not ours to change. */
/* NOLINTNEXTLINE(bugprone-easily-swappable-parameters) */
uint8_t Crc_CalculateCRC8(const uint8_t *data, uint32_t length,
                          uint8_t startValue, bool isFirstCall)
{
    uint8_t crc = CRC8_J1850_INITIAL_VALUE;

    if (!isFirstCall) {
        /* Undo the final XOR of the previous call to continue the chain. */
        crc = (uint8_t)(startValue ^ CRC8_J1850_XOR_VALUE);
    }

    if (data != NULL) {
        for (uint32_t byteIdx = 0U; byteIdx < length; byteIdx++) {
            crc = (uint8_t)(crc ^ data[byteIdx]);
            for (uint32_t bitIdx = 0U; bitIdx < BITS_PER_BYTE; bitIdx++) {
                if ((crc & CRC8_MSB_MASK) != 0U) {
                    crc = (uint8_t)((uint8_t)(crc << 1U) ^ CRC8_J1850_POLYNOMIAL);
                } else {
                    crc = (uint8_t)(crc << 1U);
                }
            }
        }
    }

    return (uint8_t)(crc ^ CRC8_J1850_XOR_VALUE);
}
