#include "CSP_interface.h"

#ifndef ROUTING_TABLE_H_INCLUDED
#define ROUTING_TABLE_H_INCLUDED

#define GCS_node                    1     // GCS network node address
#define MCC_node                    2     // Mission Control Centre(MCC) network node address
#define OBC_node                    5     // OBC subsystem network node address
#define ADC_node                    3     // ADC subsystem network node address
#define communication_node          4     // communication subsystem network node address
#define payload_camera_node         6     // payload camera network node address


void CSP_routing_table(CSP_STRUCTURE*  frame);

#endif // ROUTING_TABLE_H_INCLUDED
