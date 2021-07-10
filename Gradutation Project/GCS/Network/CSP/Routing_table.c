#include "CSP_interface.h"
#include"Routing_table.h"

void CSP_routing_table(CSP_STRUCTURE *csp_frame)
{
    if (csp_frame->DESTINATION_ID == GCS_node)
       {
           printf("frame should be routed to ME \n");
           printf("*************************************************************************\n");
       }

    else if (csp_frame->DESTINATION_ID == MCC_node)
       {
           printf("frame should be routed to Mission Control Centre\n");
           printf("*************************************************************************\n");
       }

    else if (csp_frame->DESTINATION_ID == OBC_node)
       {
           printf("frame should be routed to Satellite OBC subsystem\n");
           printf("*************************************************************************\n");
       }

    else if (csp_frame->DESTINATION_ID == ADC_node)
       {
           printf("frame should be routed to Satellite ADC subsystem \n");
           printf("*************************************************************************\n");
       }

    else if (csp_frame->DESTINATION_ID == communication_node)
       {
           printf("frame should be routed to Satellite communication subsystem\n");
           printf("*************************************************************************\n");
       }

    else if (csp_frame->DESTINATION_ID == payload_camera_node)
       {
           printf("frame should be routed to Satellite payload camera subsystem\n");
           printf("*************************************************************************\n");
       }

}
