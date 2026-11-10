/**
 * @file can_socketcan.c
 * @brief CanDrv implementation on Linux SocketCAN (works with vcan and real interfaces).
 */

#include <errno.h>
#include <fcntl.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include "bms/platform/can_drv.h"

static int canDrv_socket = -1;

Can_ReturnType CanDrv_Init(const char *interfaceName)
{
    struct ifreq ifr;
    struct sockaddr_can addr;
    Can_ReturnType result = CAN_NOT_OK;

    if ((interfaceName != NULL) && (strlen(interfaceName) < sizeof(ifr.ifr_name))) {
        canDrv_socket = socket(PF_CAN, SOCK_RAW, CAN_RAW);
        if (canDrv_socket >= 0) {
            (void)memset(&ifr, 0, sizeof(ifr));
            (void)strncpy(ifr.ifr_name, interfaceName, sizeof(ifr.ifr_name) - 1U);
            (void)memset(&addr, 0, sizeof(addr));
            addr.can_family = AF_CAN;

            if ((ioctl(canDrv_socket, SIOCGIFINDEX, &ifr) == 0) &&
                (fcntl(canDrv_socket, F_SETFL, O_NONBLOCK) == 0)) {
                addr.can_ifindex = ifr.ifr_ifindex;
                if (bind(canDrv_socket, (struct sockaddr *)&addr, sizeof(addr)) == 0) {
                    result = CAN_OK;
                }
            }
            if (result != CAN_OK) {
                perror("CanDrv_Init");
                (void)close(canDrv_socket);
                canDrv_socket = -1;
            }
        }
    }
    return result;
}

Can_ReturnType CanDrv_Write(const Can_FrameType *frame)
{
    struct can_frame cf;
    Can_ReturnType result = CAN_NOT_OK;

    if ((canDrv_socket >= 0) && (frame != NULL) && (frame->dlc <= CANDRV_MAX_DLC)) {
        (void)memset(&cf, 0, sizeof(cf));
        cf.can_id = frame->id & CAN_SFF_MASK;
        cf.can_dlc = frame->dlc;
        (void)memcpy(cf.data, frame->data, frame->dlc);
        if (write(canDrv_socket, &cf, sizeof(cf)) == (ssize_t)sizeof(cf)) {
            result = CAN_OK;
        }
    }
    return result;
}

Can_ReturnType CanDrv_Read(Can_FrameType *frame)
{
    struct can_frame cf;
    Can_ReturnType result = CAN_NOT_OK;

    if ((canDrv_socket >= 0) && (frame != NULL)) {
        const ssize_t n = read(canDrv_socket, &cf, sizeof(cf));
        if (n == (ssize_t)sizeof(cf)) {
            /* Extended, RTR and error frames are not used by this ECU. */
            if ((cf.can_id & (CAN_EFF_FLAG | CAN_RTR_FLAG | CAN_ERR_FLAG)) == 0U) {
                frame->id = cf.can_id & CAN_SFF_MASK;
                frame->dlc = (cf.can_dlc <= CANDRV_MAX_DLC) ? cf.can_dlc : (uint8_t)CANDRV_MAX_DLC;
                (void)memcpy(frame->data, cf.data, CANDRV_MAX_DLC);
                result = CAN_OK;
            } else {
                result = CAN_NO_DATA;
            }
        } else if ((n < 0) && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {
            result = CAN_NO_DATA;
        } else {
            result = CAN_NOT_OK;
        }
    }
    return result;
}

void CanDrv_DeInit(void)
{
    if (canDrv_socket >= 0) {
        (void)close(canDrv_socket);
        canDrv_socket = -1;
    }
}
