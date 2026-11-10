/**
 * @file main.c
 * @brief Entry point of the virtual BMS ECU.
 *
 * Usage: bms_ecu [-i <can-interface>]   (default: vcan0)
 */

#include <inttypes.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>

#include "bms/bsw/canif.h"
#include "bms/platform/can_drv.h"
#include "bms/platform/os.h"
#include "bms_tasks.h"

#define BMS_DEFAULT_CAN_IF "vcan0"

static const Os_ConfigType bms_osConfig = {Bms_Task10ms, Bms_Task100ms};

static void bms_onSignal(int sig)
{
    (void)sig;
    Os_RequestShutdown();
}

int main(int argc, char *argv[])
{
    const char *canIf = BMS_DEFAULT_CAN_IF;
    struct sigaction sa;
    CanIf_StatisticsType stats;

    if ((argc == 3) && (strcmp(argv[1], "-i") == 0)) {
        canIf = argv[2];
    } else if (argc != 1) {
        (void)fprintf(stderr, "usage: %s [-i <can-interface>]\n", argv[0]);
        return 2;
    }

    if (CanDrv_Init(canIf) != CAN_OK) {
        (void)fprintf(stderr, "cannot open CAN interface '%s' (see README: vcan setup)\n", canIf);
        return 1;
    }

    (void)memset(&sa, 0, sizeof(sa));
    sa.sa_handler = bms_onSignal;
    (void)sigaction(SIGINT, &sa, NULL);
    (void)sigaction(SIGTERM, &sa, NULL);

    Bms_InitAll();
    Os_Init(&bms_osConfig, Os_GetTimeMs());
    (void)printf("virtual-bms-ecu running on %s (10 ms tick). Ctrl+C to stop.\n", canIf);

    Os_Run();

    CanIf_GetStatistics(&stats);
    (void)printf("stopped after %" PRIu32 " ticks, %" PRIu32 " overruns; rx routed %" PRIu32
                 ", unknown id %" PRIu32 ", bad dlc %" PRIu32 "\n",
                 Os_GetTickCount(), Os_GetOverrunCount(), stats.routed, stats.unknownId,
                 stats.badDlc);
    CanDrv_DeInit();
    return 0;
}
