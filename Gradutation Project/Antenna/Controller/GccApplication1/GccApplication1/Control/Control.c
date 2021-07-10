/*
 * Control.c
 *
 * Created: 4/20/2021 2:25:38 AM
 *  Author: ahmed
 */ 
#include "std-types.h"
#include "Control.h"
#include "TIM_INT.h"
#include "INT_INT.h"
#include "PWMD.h"
#include <stdlib.h>

static double sampling_time;
static unsigned int dim;
static double *pid_p, *pid_i, *pid_d;
static double *prev_y;
static double *prev_r;
static double *y_sum;
static double *r_sum;
static double *u;
static double *r;
static double *f;
static Driver_t driver_type=PWMD;
static void Control_voidPIDProceed()
{
	for (int k=0; k<dim; k++)
	{
		y_sum[k]+=f[k];
		r_sum[k]+=r[k];
		u[k]=pid_p[k]*(r[k]-f[k])+pid_i[k]*(r_sum[k]-y_sum[k])*sampling_time+pid_d[k]*((r[k]-f[k])-(prev_r[k]-prev_y[k]))/sampling_time;
		if (u[k]>100.) u[k]=100.;
		if (driver_type==PWMD) PWMD_voidSetAmp(k,u[k]);
		prev_r[k]=r[k];
		prev_y[k]=f[k];
	}
	TIM_voidSetCompare(TIM1,OCHA,sampling_time/8e-6);
	TIM_voidTIMxClrFlag(TIM1,TIM_INT_CMPA);
}
void Control_voidPIDConfig(unsigned int n, double p[], double i[], double d[], double init_y[], double init_r[], double ts_sec)
{
	sampling_time=ts_sec;
	dim=n;
	free(pid_p);
	free(pid_i);
	free(pid_d);
	free(prev_y);
	free(prev_r);
	free(y_sum);
	free(r_sum);
	free(u);
	free(r);
	free(f);
	pid_p=(double *)malloc(n*sizeof(double));
	pid_i=(double *)malloc(n*sizeof(double));
	pid_d=(double *)malloc(n*sizeof(double));
	prev_y=(double *)malloc(n*sizeof(double));
	prev_r=(double *)malloc(n*sizeof(double));
	
	y_sum=(double *)malloc(n*sizeof(double));
	r_sum=(double *)malloc(n*sizeof(double));
	u=(double *)malloc(n*sizeof(double));
	r=(double *)malloc(n*sizeof(double));
	f=(double *)malloc(n*sizeof(double));
	for (int k=0; k<dim; k++)
	{
		pid_p[k]=p[k];
		pid_i[k]=i[k];
		pid_d[k]=d[k];
		prev_y[k]=f[k]=init_y[k];
		prev_r[k]=r[k]=init_r[k];
		y_sum[k]=0;
		r_sum[k]=0;
	}
	TIM_voidTIMConfig();
	TIM_voidSetCompare(TIM1,OCHA,sampling_time/8e-6);
	TIM_voidTIMxSetCallback(TIM1,TIM_INT_CMPA,&Control_voidPIDProceed);
	TIM_voidTIMxIntUnmask(TIM1,TIM_INT_CMPA);
	INT_voidGlobEnable();
}

void Control_voidSetpoint(double new_r[])
{
	for (int k=0; k<dim; k++)
	{
		r[k]=new_r[k];
	}
}
void Control_voidFeedback(double new_f[])
{
	for (int k=0; k<dim; k++)
	{
		f[k]=new_f[k];
	}
}
void Control_voidChSetpoint(double delta_r[])
{
	for (int k=0; k<dim; k++)
	{
		r[k]+=delta_r[k];
	}
}
void Control_voidSetDriver(Driver_t driver)
{
	driver_type=driver;
}
/*
void Control_voidGetCtrlSgnl(double *buffer)
{
	for (int k=0; k<dim; k++)
	{
		buffer[k]+=u[k];
	}
}*/
void Control_voidStart()
{
		TIM_voidTIMxIntUnmask(TIM1,TIM_INT_CMPA);
}
void Control_voidStop()
{
		TIM_voidTIMxIntMask(TIM1,TIM_INT_CMPA);
}

