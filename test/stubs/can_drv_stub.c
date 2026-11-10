#include "can_drv_stub.h"

#include <stddef.h>
#include <string.h>

static Can_FrameType stub_rx[CAN_STUB_QUEUE_LEN];
static uint32_t stub_rxHead;
static uint32_t stub_rxCount;
static Can_FrameType stub_tx[CAN_STUB_QUEUE_LEN];
static uint32_t stub_txCount;

void CanDrvStub_Reset(void)
{
    (void)memset(stub_rx, 0, sizeof(stub_rx));
    (void)memset(stub_tx, 0, sizeof(stub_tx));
    stub_rxHead = 0U;
    stub_rxCount = 0U;
    stub_txCount = 0U;
}

void CanDrvStub_InjectRx(uint32_t id, const uint8_t *data, uint8_t dlc)
{
    if (stub_rxCount < CAN_STUB_QUEUE_LEN) {
        Can_FrameType *f = &stub_rx[(stub_rxHead + stub_rxCount) % CAN_STUB_QUEUE_LEN];
        f->id = id;
        f->dlc = dlc;
        (void)memset(f->data, 0, sizeof(f->data));
        if (data != NULL) {
            (void)memcpy(f->data, data, dlc);
        }
        stub_rxCount++;
    }
}

uint32_t CanDrvStub_TxCount(void)
{
    return stub_txCount;
}

const Can_FrameType *CanDrvStub_TxFrame(uint32_t index)
{
    return (index < stub_txCount) ? &stub_tx[index] : NULL;
}

Can_ReturnType CanDrv_Init(const char *interfaceName)
{
    (void)interfaceName;
    CanDrvStub_Reset();
    return CAN_OK;
}

Can_ReturnType CanDrv_Write(const Can_FrameType *frame)
{
    if ((frame == NULL) || (stub_txCount >= CAN_STUB_QUEUE_LEN)) {
        return CAN_NOT_OK;
    }
    stub_tx[stub_txCount] = *frame;
    stub_txCount++;
    return CAN_OK;
}

Can_ReturnType CanDrv_Read(Can_FrameType *frame)
{
    if (stub_rxCount == 0U) {
        return CAN_NO_DATA;
    }
    *frame = stub_rx[stub_rxHead];
    stub_rxHead = (stub_rxHead + 1U) % CAN_STUB_QUEUE_LEN;
    stub_rxCount--;
    return CAN_OK;
}

void CanDrv_DeInit(void) {}
