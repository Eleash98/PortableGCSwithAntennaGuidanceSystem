/*
 * Control.h
 *
 * Created: 4/20/2021 2:30:12 AM
 *  Author: ahmed
 */ 


#ifndef CONTROL_H_
#define CONTROL_H_

typedef enum {VFD,PWMD} Driver_t;

void Control_voidPIDConfig(unsigned int n, double p[], double i[], double d[], double init_y[], double init_r[], double ts_sec);
void Control_voidSetpoint(double new_r[]);
void Control_voidFeedback(double new_f[]);
void Control_voidChSetpoint(double delta_r[]);
void Control_voidSetDriver(Driver_t driver);
void Control_voidStart();
void Control_voidStop();

#endif /* CONTROL_H_ */