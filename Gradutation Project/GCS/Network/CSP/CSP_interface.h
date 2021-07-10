#ifndef CSP_INTERFACE_H_INCLUDED
#define CSP_INTERFACE_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "STD.h"
#include "BIT_MATH.h"
#include "CSP_hmac.h"
#include "CSP_SHA_256_224.h"
#include "SHA_1.h"
#include "CSP_CRC32.h"
#include "CSP_XTEA.h"


/////////////////////////////////////////////////////////////////////
#define CSP_ID_PRIO_SIZE		2 // Bits for priority
#define CSP_ID_HOST_SIZE		5 // Bits for host (destination/source address)
#define CSP_ID_PORT_SIZE		6 // Bits for port (destination/source port)
#define CSP_ID_FLAGS_SIZE		8 // Bits for flags
////////////////////////////////////////////////////////////////////
#define CSP_F_RES1			0x80 //Reserved for future use
#define CSP_F_RES2			0x40 // Reserved for future use
#define CSP_F_RES3			0x20 // Reserved for future use
#define CSP_F_RES4			0x10 // Reserved for future use
#define CSP_F_HMAC			0x08 // Use HMAC verification
#define CSP_F_XTEA			0x04 // Use XTEA encryption
#define CSP_F_RDP			0x02 // Use RDP protocol
#define CSP_F_CRC32			0x01 // Use CRC32 checksum
////////////////////////////////////////////////////////////////////
#define  CSP_PRIO_CRITICAL		 0 // Critical
#define	 CSP_PRIO_HIGH		     1 // High
#define	 CSP_PRIO_NORM			 2 // Normal (default)
#define	 CSP_PRIO_LOW			 3 // Low
////////////////////////////////////////////////////////////////////

//**dest/source ID
#define src_ID  0x00
#define des_ID  0x01
//**dest/source port
#define src_port 0x00
#define des_port 0x01
///////////////////////////////////////////////////////////////////////


typedef struct
{
    u8* data_ptr;
    u8* CSP_packet;
    u8 flags;
    u8 SOURCE_PORT;
    u8 DESTINATION_PORT;
    u8 SOURCE_ID;
    u8 DESTINATION_ID;
    u8 PRIORITY;
    u16 data_length;
    u32 CRC;
    u8* HMAC_ptr;
}CSP_STRUCTURE;


typedef enum {
	CSP_CMP				= 0,   // CSP management, e.g. memory, routes, stats
	CSP_PING			= 1,   // Ping-return ping
	CSP_PS				= 2,   // Current process list
	CSP_MEMFREE			= 3,   // Free memory
	CSP_REBOOT			= 4,   // Reboot, see #CSP_REBOOT_MAGIC and #CSP_REBOOT_SHUTDOWN_MAGIC
	CSP_BUF_FREE	    = 5,   // Free CSP buffers
	CSP_UPTIME			= 6,   // Uptime
} csp_service_port_t;

//**Message priority.
/*
typedef enum {
	CSP_PRIO_CRITICAL		= 0, // Critical
	CSP_PRIO_HIGH			= 1, // High
	CSP_PRIO_NORM			= 2, // Normal (default)
	CSP_PRIO_LOW			= 3, // Low
} csp_prio_t;*/
/*functions declaration*/

void CSP_VOID_TRANSMISSION(CSP_STRUCTURE* CSP_FRAME,u8 hmac_key_length,u8* hmac_key,SHA_256_224_t* sha_struct,SHA_1_t *sha_1_struct,u8 SHA_type,u8 num_rounds,u32* XTEA_key,u8 CSP_priority);
void CSP_VOID_RECEIVING(CSP_STRUCTURE* CSP_FRAME,u8 * CSP_packet,u8 hmac_key_length,u8* hmac_key,SHA_256_224_t* sha_struct,SHA_1_t *sha_1_struct,u8 SHA_type,u8 num_rounds,u32* XTEA_key);

#endif // CSP_INTERFACE_H_INCLUDED
