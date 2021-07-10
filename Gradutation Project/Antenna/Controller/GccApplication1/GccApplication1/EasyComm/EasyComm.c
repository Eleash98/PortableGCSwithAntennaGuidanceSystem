/*
 * EasyComm.c
 *
 * Created: 4/19/2021 1:55:31 AM
 *  Author: ahmed
 */ 
#include <stdlib.h>
#include "std-types.h"
#include "EasyComm.h"
#include <ctype.h>
#include "DIO_INT.h"
#include "../Encoder_interface.h"
#include "Control.h"
#define IS_NUM(x) ((x)<='9' && (x)>='0')

#define MAX_CMD_SIZE 100
#define MAX_BUF_SIZE 50
#define LSA1 PA, PIN0
#define LSA2 PA, PIN1
#define LSE1 PA, PIN2
#define LSE2 PA, PIN3
static u8 buffer[MAX_BUF_SIZE];
static float az;
static float el;
static u32   uplink_rate, downlink_rate;
static u8 uplink_mode[4], downlink_mode[4];
static u8 azimuth_analog_speed=0;
static u8 elevation_analog_speed=0;
static Encoder_t EA={1,0x5,ENCODER_BAUD_9600,ENCODER_MODE_SINGLE_TURN,ENCODER_CONVERSION_CONT,4000,ENCODER_DIRECTION_CLOCKWISE,{0,0,0,0}},
				 EE={2,0x5,ENCODER_BAUD_9600,ENCODER_MODE_SINGLE_TURN,ENCODER_CONVERSION_CONT,4000,ENCODER_DIRECTION_CLOCKWISE,{0,0,0,0}};
static Reading_t RA,RE;
typedef struct {
	u32 time_l;
	u32 time_h;
	} _time_t;
typedef struct {
	u16 az;
	u16 el;
	_time_t time;
	} _entry;
_time_t base_time={0,0};
_entry *table;
static _entry *cursor;
u16 tsz;

void EasyComm_voidParseCmd(u8 cmd[], u8 *resp)
{
	u8 i=0;
	u8 sz=0;
	u8 rsz=0;
	buffer[0]=0, resp[0]=0;
	u8 flg=0;
	while (cmd[i] && i < MAX_CMD_SIZE)
	{
		if (isblank(cmd[i]))
		{
			i++;
			continue;
		}
		sz=0;
		while(!(isblank(cmd[i])||!cmd[i]))
			buffer[sz++]=cmd[i++];
		buffer[sz]=0;
		if (buffer[0]=='A' && buffer[1]=='Z')
		{
			if (sz!=8 && sz!=2)
			{
				resp[0]=0;
				return;
			}
			if (sz==2)
			{
				resp[rsz++]='A';
				resp[rsz++]='Z';
				HEncoder_voidGetPosition(&EE,&RE);
				float az1=RE.Angle*1.25;
				//float az1=123.45;
				resp[rsz++]=(int)az1/100+'0';
				resp[rsz++]=(int)az1%100/10+'0';
				resp[rsz++]=(int)az1%10+'0';
				resp[rsz++]='.';
				resp[rsz++]=(int)(az1*10.0)%10+'0';
				resp[rsz++]=(int)(az1*100.0)%10+'0';
				resp[rsz++]=(int)' ';
				flg=1;
				continue;
			}
			az=0.0;
			for (int k = 2; k < 8; k ++)
			{
				if (k==5)
				{
					if (buffer[k]!='.')
					{
						resp[0]=0;
						return;
					}
					continue;
				}
				if (!IS_NUM(buffer[k]))
				{
					resp[0]=0;
					return;
				}
				az*=10.0;
				az+=(buffer[k]-'0')/100.0;
			}
			flg=1;
			continue;
		}
		if (buffer[0]=='E' && buffer[1]=='L')
		{
			if (sz!=8 && sz!=2)
			{
				resp[0]=0;
				return;
			}
			if (sz==2)
			{
				resp[rsz++]='E';
				resp[rsz++]='L';
				HEncoder_voidGetPosition(&EE,&RE);
				float el1=RE.Angle;
				//float el1=123.45;
				resp[rsz++]=(int)el1/100+'0';
				resp[rsz++]=(int)el1%100/10+'0';
				resp[rsz++]=(int)el1%10+'0';
				resp[rsz++]='.';
				resp[rsz++]=(int)(el1*10.0)%10+'0';
				resp[rsz++]=(int)(el1*100.0)%10+'0';
				resp[rsz++]=(int)' ';
				flg=1;
				continue;
			}
			el=0.0;
			for (int k = 2; k < 8; k ++)
			{
				if (k==5)
				{
					if (buffer[k]!='.')
					{
						resp[0]=0;
						return;
					}
					continue;
				}
				if (!IS_NUM(buffer[k]))
				{
					resp[0]=0;
					return;
				}
				el*=10.0;
				el+=(buffer[k]-'0')/100.0;
			}
			flg=1;
			continue;
		}
		if (buffer[0]=='U' && buffer[1]=='P')
		{
			if (sz!=11)
			{
				resp[0]=0;
				return;
			}
			uplink_rate=0;
			for (int k = 2; k < 11; k ++)
			{
				if (!IS_NUM(buffer[k]))
				{
					resp[0]=0;
					return;
				}
				uplink_rate*=10;
				uplink_rate+=buffer[k]-'0';
			}
			continue;
		}
		if (buffer[0]=='D' && buffer[1]=='N')
		{
			if (sz!=11)
			{
				resp[0]=0;
				return;
			}
			downlink_rate=0;
			for (int k = 2; k < 11; k ++)
			{
				if (!IS_NUM(buffer[k]))
				{
					resp[0]=0;
					return;
				}
				downlink_rate*=10;
				downlink_rate+=buffer[k]-'0';
			}
			continue;
		}
		if (buffer[0]=='U' && buffer[1]=='M')
		{
			if (sz!=5)
			{
				resp[0]=0;
				return;
			}
			for (int k = 2; k < 5; k ++)
			{
				uplink_mode[k-2]=buffer[k];
			}
			uplink_mode[3]=0;
			continue;
		}
		if (buffer[0]=='D' && buffer[1]=='M')
		{
			if (sz!=5)
			{
				resp[0]=0;
				return;
			}
			for (int k = 2; k < 5; k ++)
			{
				downlink_mode[k-2]=buffer[k];
			}
			downlink_mode[3]=0;
			continue;
		}
		if (buffer[0]=='M' && buffer[1]=='U')
		{
			if (sz!=2)
			{
				resp[0]=0;
				return;
			}
			double delta_r[]={3,3};
			Control_voidChSetpoint(delta_r);
			continue;
		}
		if (buffer[0]=='M' && buffer[1]=='D')
		{
			if (sz!=2)
			{
				resp[0]=0;
				return;
			}
			//TODO
			continue;
		}
		if (buffer[0]=='M' && buffer[1]=='L')
		{
			if (sz!=2)
			{
				resp[0]=0;
				return;
			}
			//TODO
			continue;
		}
		if (buffer[0]=='M' && buffer[1]=='R')
		{
			if (sz!=2)
			{
				resp[0]=0;
				return;
			}
			//TODO
			continue;
		}
		if (buffer[0]=='V' && buffer[1]=='A')
		{
			if (sz!=3)
			{
				resp[0]=0;
				return;
			}
			if (buffer[2]<'0'||buffer[2]>'2')
			{
				resp[0]=0;
				return;
			}
			azimuth_analog_speed=buffer[2]-'0';
			continue;
		}
		if (buffer[0]=='V' && buffer[1]=='E')
		{
			if (sz!=3)
			{
				resp[0]=0;
				return;
			}
			if (buffer[2]<'0'||buffer[2]>'2')
			{
				resp[0]=0;
				return;
			}
			elevation_analog_speed=buffer[2]-'0';
			continue;
		}
		if (buffer[0]=='P'&&buffer[1]=='N')
		{
			if (sz!=2)
			{
				resp[0]=0;
				return;
			}
			resp[rsz++]='P';
			resp[rsz++]='N';
			resp[rsz++]=' ';
			continue;
		}
		if (buffer[0]=='T'&&buffer[1]=='S')
		{
			if (sz!=5)
			{
				resp[0]=0;
				return;
			}
			if (!IS_NUM(buffer[2]) ||
				!IS_NUM(buffer[3]) ||
				!IS_NUM(buffer[2]))
			{
				resp[0]=0;
				return;
			}
			tsz=(buffer[2]-'0')*100+(buffer[3]-'0')*10+(buffer[4]-'0');
			free(table);
			table=(_entry *)malloc(tsz*sizeof(_entry));
			cursor=table;
			continue;
		}
		if (buffer[0]=='T'&&buffer[1]=='L')
		{
			if (sz!=26)
			{
				resp[0]=0;
				return;
			}
			_entry e={0};
			for (u8 k=2; k<26; k++)
			{
				if (k==5||k==12)
				{
					if (buffer[k]!='.')
					{
						resp[0]=0;
						return;
					}
					continue;
				}
				if (k==8||k==15)
				{
					if (buffer[k]!=';')
					{
						resp[0]=0;
						return;
					}
					continue;
				}
				if (!IS_NUM(buffer[k]))
				{
					resp[0]=0;
					return;
				}
				if (k<8)
				{
					e.az*=10;
					e.az+=buffer[k]-'0';
					continue;
				}
				if (k<15)
				{
					e.el*=10;
					e.el+=buffer[k]-'0';
					continue;
				}
				//e.time.time_h*=10;
				//e.time.time_h+=e.time.time_l>>31+e.time.time_l>>29+(e.time.time_l<<1+e.time.time_l<<3 < e.time.time_l<<1&&e.time.time_l<<1+e.time.time_l<<3 < e.time.time_l<<3);
				//e.time+=buffer[k]-'0';
			}
			*(cursor++)=e;
			continue;
		}
		if (buffer[0]=='S'&&buffer[1]=='A')
		{
			if (sz!=2)
			{
				resp[0]=0;
				return;
			}
			HEncoder_voidSetOrigin(&EA);
			continue;
		}
		if (buffer[0]=='S'&&buffer[1]=='E')
		{
			if (sz!=2)
			{
				resp[0]=0;
				return;
			}
			HEncoder_voidSetOrigin(&EE);
			continue;
		}
		resp[0]=0;
		return;
	}
	if (flg)
	{
		if (IO_u8ReadPin(LSA1))
		{
			resp[rsz++]='A';
			resp[rsz++]='L';
			resp[rsz++]='0';
			resp[rsz++]=' ';
		}
		if (IO_u8ReadPin(LSA2))
		{
			resp[rsz++]='A';
			resp[rsz++]='L';
			resp[rsz++]='1';
			resp[rsz++]=' ';
		}
		if (IO_u8ReadPin(LSE1))
		{
			resp[rsz++]='A';
			resp[rsz++]='L';
			resp[rsz++]='2';
			resp[rsz++]=' ';
		}
		if (IO_u8ReadPin(LSE2))
		{
			resp[rsz++]='A';
			resp[rsz++]='L';
			resp[rsz++]='3';
			resp[rsz++]=' ';
		}
	}
	resp[rsz]=0;
}

