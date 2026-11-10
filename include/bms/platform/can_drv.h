/**
 * @file can_drv.h
 * @brief CAN driver interface (platform layer, replaces the AUTOSAR Can MCAL driver).
 *
 * The Linux implementation uses SocketCAN (src/platform/can_socketcan.c).
 * Host unit tests link a stub instead (test/stubs/can_drv_stub.c).
 */
#ifndef BMS_PLATFORM_CAN_DRV_H
#define BMS_PLATFORM_CAN_DRV_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CANDRV_MAX_DLC (8U)

/** Classic CAN frame, standard 11-bit identifier. */
typedef struct {
    uint32_t id;
    uint8_t dlc;
    uint8_t data[CANDRV_MAX_DLC];
} Can_FrameType;

typedef enum {
    CAN_OK = 0,
    CAN_NOT_OK,
    CAN_NO_DATA /**< Read: receive queue is empty. */
} Can_ReturnType;

/** Opens the CAN interface (e.g. "vcan0"). */
Can_ReturnType CanDrv_Init(const char *interfaceName);

/** Queues one frame for transmission. Never blocks. */
Can_ReturnType CanDrv_Write(const Can_FrameType *frame);

/** Reads one received frame if available. Never blocks. */
Can_ReturnType CanDrv_Read(Can_FrameType *frame);

void CanDrv_DeInit(void);

#ifdef __cplusplus
}
#endif

#endif /* BMS_PLATFORM_CAN_DRV_H */
