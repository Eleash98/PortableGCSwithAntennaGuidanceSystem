/*
 * Timer_program.c
 *
 *  Created on: Jan 21, 2021
 *      Author: MohammedGamalEleish
 */
#include "STD_TYPES.h"
#include "BIT_MATH.h"
#include <stdio.h>

#include "TIMER_general.h"
//#include "PRINT_interface.h"

//#include "PH_interface.h"
//#include "DL_interface.h"
//#include "MDL_interface.h"

#include "Timer_interface.h"
#include "Timer_flags.h"



typedef struct{
    size_t Timer;
    u32 Interval;
    u8 Status;
}AX25Timer_t;

AX25Timer_t AX25Timers[4];
void (*AX25TimerCallBacks[4])(void);


void T1_CallBack(void){
#if DEBUGGING == 1
    printf("Timer T1 Expired\n");
#endif // DEBUGGING
    Timer.T1_EXPIRE = TIMER_FLAG_SET;
    printf("Timer T1 Expired\n");
    AX25Timer_voidStop(AX25_TIMER_T1);
    //SM_voidDataLinkLayer();
}

void T2_CallBack(void){
#if DEBUGGING == 1
    printf("Timer T2 Expired\n");
#endif // DEBUGGING
    Timer.T2_EXPIRE = TIMER_FLAG_SET;
    AX25Timer_voidStop(AX25_TIMER_T2);
    //SM_voidManagmentDataLink();
}

void T3_CallBack(void){
#if DEBUGGING == 1
    printf("Timer T3 Expired\n");
#endif // DEBUGGING
    Timer.T3_EXPIRE = TIMER_FLAG_SET;
    AX25Timer_voidStop(AX25_TIMER_T3);
    //SM_voidDataLinkLayer();
}

/*
void T100_CallBack(void){
#if DEBUGGING == 1
    printf("Timer T100 Expired\n");
#endif // DEBUGGING
    Timer.T100_EXPIRE = TIMER_FLAG_SET;
    AX25Timer_voidStop(AX25_TIMER_T100);
    //SM_voidSimplexPhysicalLayer();
}


void T101_CallBack(void){
#if DEBUGGING == 1
    printf("Timer T101 Expired\n");
#endif // DEBUGGING
    Timer.T101_EXPIRE = TIMER_FLAG_SET;
    AX25Timer_voidStop(AX25_TIMER_T101);
    //SM_voidSimplexPhysicalLayer();
}


void T102_CallBack(void){
#if DEBUGGING == 1
    printf("Timer T102 Expired\n");
#endif // DEBUGGING
    Timer.T102_EXPIRE = TIMER_FLAG_SET;
    AX25Timer_voidStop(AX25_TIMER_T102);
    //SM_voidSimplexPhysicalLayer();
}


void T103_CallBack(void){
#if DEBUGGING == 1
    printf("Timer T103 Expired\n");
#endif // DEBUGGING
    Timer.T103_EXPIRE = TIMER_FLAG_SET;
    printf("Timer T103 Expired\n");
    AX25Timer_voidStop(AX25_TIMER_T103);
    //SM_voidSimplexPhysicalLayer();
}

void T104_CallBack(void){
#if DEBUGGING == 1
    printf("Timer T104 Expired\n");
#endif // DEBUGGING
    Timer.T104_EXPIRE = TIMER_FLAG_SET;
    printf("Timer T104 Expired\n");
    AX25Timer_voidStop(AX25_TIMER_T104);
    //SM_voidSimplexPhysicalLayer();
}


void T105_CallBack(void){
#if DEBUGGING == 1
    printf("Timer T105 Expired\n");
#endif // DEBUGGING
    Timer.T105_EXPIRE = TIMER_FLAG_SET;
    printf("Timer T105 Expired\n");
    AX25Timer_voidStop(AX25_TIMER_T105);
    //SM_voidSimplexPhysicalLayer();
}


void T106_CallBack(void){
#if DEBUGGING == 1
    printf("Timer T106 Expired\n");
#endif // DEBUGGING
    Timer.T106_EXPIRE = TIMER_FLAG_SET;
    printf("Timer T106 Expired\n");
    AX25Timer_voidStop(AX25_TIMER_T106);
    //SM_voidSimplexPhysicalLayer();
}


void T107_CallBack(void){
#if DEBUGGING == 1
    printf("Timer T107 Expired\n");
#endif // DEBUGGING
    Timer.T107_EXPIRE = TIMER_FLAG_SET;
    printf("Timer T107 Expired\n");
    AX25Timer_voidStop(AX25_TIMER_T107);
    //SM_voidSimplexPhysicalLayer();
}


void T108_CallBack(void){
#if DEBUGGING == 1
    printf("Timer T108 Expired\n");
#endif // DEBUGGING
    Timer.T108_EXPIRE = TIMER_FLAG_SET;
    printf("Timer T108 Expired\n");
    AX25Timer_voidStop(AX25_TIMER_T108);
    //SM_voidSimplexPhysicalLayer();
}

*/
void TR210_CallBack(void){
#if DEBUGGING == 1
    printf("Timer TR210 Expired\n");
#endif // DEBUGGING
    Timer.TR210_EXPIRE = TIMER_FLAG_SET;
    AX25Timer_voidStop(AX25_TIMER_TR210);

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
/*****Timer Default value******/

u32 T1Default       =3000;
u32 T2Default       = 200000;
u32 T3Default       = 4000;
u32 T100Default     = 20;
u32 T101Default     = 1;
u32 T102Default     = 1;
u32 T103Default     = 1;
u32 T104Default     = 1;
u32 T105Default     = 1;
u32 T106Default     = 60000;
u32 T107Default     = 10;
u32 T108Default     = 10;
u32 TR210Default    = 5000;

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

void AX25Timer_voidInit(void){

    T1Value = T1Default;

    AX25TimerCallBacks[0]   = T1_CallBack;
    AX25TimerCallBacks[1]   = T2_CallBack;
    AX25TimerCallBacks[2]   = T3_CallBack;
    AX25TimerCallBacks[3]   = TR210_CallBack;
    /*AX25TimerCallBacks[4]   = T101_CallBack;
    AX25TimerCallBacks[5]   = T102_CallBack;
    AX25TimerCallBacks[6]   = T103_CallBack;
    AX25TimerCallBacks[7]   = T104_CallBack;
    AX25TimerCallBacks[8]   = T105_CallBack;
    AX25TimerCallBacks[9]   = T106_CallBack;
    AX25TimerCallBacks[10]  = T107_CallBack;
    AX25TimerCallBacks[11]  = T108_CallBack;
    AX25TimerCallBacks[12]  = TR210_CallBack;*/

    AX25Timers[0].Interval = T1Default;
    AX25Timers[1].Interval = T2Default;
    AX25Timers[2].Interval = T3Default;
    AX25Timers[3].Interval = TR210Default;
    /*AX25Timers[4].Interval = T101Default;
    AX25Timers[5].Interval = T102Default;
    AX25Timers[6].Interval = T103Default;
    AX25Timers[7].Interval = T104Default;
    AX25Timers[8].Interval = T105Default;
    AX25Timers[9].Interval = T106Default;
    AX25Timers[10].Interval = T107Default;
    AX25Timers[11].Interval = T108Default;
    AX25Timers[12].Interval = TR210Default;
*/

    timer_initialize();

    for(u8 i = 0; i<4;i++){
       // AX25Timers[i].Timer = start_timer(AX25Timers[i].Interval,(void (*)(unsigned int, void*))AX25TimerCallBacks[i],TIMER_SINGLE_SHOT,0);
        //stop_timer(AX25Timers[i].Timer);
        AX25Timers[i].Status = TIMER_STATUS_STOP;
    }
}



void AX25Timer_voidSetTime(u8 TimerID,u32 TimerValue){

    AX25Timers[TimerID].Interval = TimerValue;
}
//void AX25Timer_voidSetExpireTime(u8 TimerID,u32 ExpireValue){}


void AX25Timer_voidStop(u8 TimerID)
{
    if(AX25Timers[TimerID].Status != TIMER_STATUS_STOP){
        stop_timer(AX25Timers[TimerID].Timer);
        AX25Timers[TimerID].Status = TIMER_STATUS_STOP;
    }
#if DEBUGGING == 1
    printf("Stopping Timer ");
    PrintTimer(TimerID);
#endif
}
void AX25Timer_voidStart(u8 TimerID)
{

    if(AX25Timers[TimerID].Status != TIMER_STATUS_RUN){
        AX25Timers[TimerID].Timer = start_timer(AX25Timers[TimerID].Interval,(void (*)(size_t, void*))AX25TimerCallBacks[TimerID],TIMER_SINGLE_SHOT,NULL);
        AX25Timers[TimerID].Status = TIMER_STATUS_RUN;
    }
#if DEBUGGING == 1
    printf("Starting Timer ");
    PrintTimer(TimerID);
#endif
}
void AX25Timer_voidClear(u8 TimerID)
{
    if(AX25Timers[TimerID].Status != TIMER_STATUS_RUN){
        AX25Timers[TimerID].Timer = start_timer(AX25Timers[TimerID].Interval,(void (*)(size_t, void*))AX25TimerCallBacks[TimerID],TIMER_SINGLE_SHOT,0);
        AX25Timers[TimerID].Status = TIMER_STATUS_RUN;
    }
#if DEBUGGING == 1
    printf("Clearing Timer ");
    PrintTimer(TimerID);
#endif
}
u32 AX25Timer_u32GetTime(u8 TimerID){
        return AX25Timers[TimerID].Interval;
}

u32 AX25Timer_u32GetExpireTime(u8 TimerID){

    if(AX25Timers[TimerID].Status == TIMER_STATUS_RUN)
        return get_time(AX25Timers[TimerID].Timer);
    else
        return 0;
}

u8 AX25Timer_u8GetStatus(u8 TimerID){

    return AX25Timers[TimerID].Status;
}

void AX25Timer_voidExit(void){

    timer_finalize();

}
