#ifndef __MAIN_RELAY__
#define __MAIN_RELAY__

#include "ns_turn_defs.h"
#include "turn_ports.h"

#ifdef __cplusplus
extern "C" {
#endif

int turn_server_run(int argc, char** argv);
void turn_server_stop();

#ifdef __cplusplus
}
#endif

#endif //__MAIN_RELAY__
