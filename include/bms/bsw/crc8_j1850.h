/**
 * @file crc8_j1850.h
 * @brief CRC-8 SAE J1850 (polynomial 0x1D), as used by AUTOSAR E2E Profile 1/2.
 *
 * The interface mirrors AUTOSAR Crc_CalculateCRC8() so the module can be
 * dropped into an AUTOSAR-like stack without changes to the callers.
 */
#ifndef BMS_BSW_CRC8_J1850_H
#define BMS_BSW_CRC8_J1850_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Initial value defined by SAE J1850. */
#define CRC8_J1850_INITIAL_VALUE ((uint8_t)0xFFU)
/** Final XOR value defined by SAE J1850. */
#define CRC8_J1850_XOR_VALUE ((uint8_t)0xFFU)

/**
 * @brief Calculate CRC-8 SAE J1850 over a data block.
 *
 * @param data        Pointer to the data. May be NULL only if @p length is 0.
 * @param length      Number of bytes to process.
 * @param startValue  CRC result of the previous call (ignored if @p isFirstCall).
 * @param isFirstCall true: start with the J1850 initial value.
 *                    false: continue a chained calculation from @p startValue.
 * @return CRC-8 value (final XOR applied). Returns the CRC of an empty block
 *         when @p data is NULL.
 *
 * @satisfies REQ-CRC-001, REQ-CRC-002, REQ-CRC-003
 */
uint8_t Crc_CalculateCRC8(const uint8_t *data, uint32_t length,
                          uint8_t startValue, bool isFirstCall);

#ifdef __cplusplus
}
#endif

#endif /* BMS_BSW_CRC8_J1850_H */
