/*
 * mainFunction.h
 *
 *  Created on: 02.07.2023
 *      Author: Zain
 */

#ifndef INC_PID_CONTROLLERSTREAM_H_
#define INC_PID_CONTROLLERSTREAM_H_

#include "stm32f3xx_hal.h" // Include the appropriate header file for your microcontroller platform
#include <stm32f3xx_hal_adc.h>
#include <stdlib.h>
#include "ssd1306.h"
#include <stdio.h>
#include <math.h>

#define ADC_MAX_ANALOG_IN 4096 - 1
#define ADC_MIN_ANALOG_IN 0
#define ADC_MAX_ANALOG_MAPED_OUT 1024
#define ADC_MIN_ANALOG_MAPED_OUT 0
#define PWM_MAX_DUTYCYCLE 140 - 1 // 3.3 v
#define PWM_MIN_DUTYCYCLE 0 // 0.4 v

typedef enum {

  PID_FAILD = 0,
  PID_SUCCESS = 1,
}PID_State;

typedef struct PID_CONTROLLER PID_Controller;


	PID_Controller * pid_init(void);

	void PID_Set(PID_Controller* PID, float KP,float KI,float KD,uint32_t referencePoint,float timeSpace );

	static float __e(PID_Controller* pid);

	uint32_t __Proportional(PID_Controller* pid);

	float __Integral(PID_Controller* pid);

	float __Derivative(PID_Controller* pid);

	PID_State pid_Compute(PID_Controller* pid);

	void mallocFree(PID_Controller * pid);

	uint32_t  pidOut(PID_Controller *pid);

	void pidIn (PID_Controller *pid, uint32_t in);

	uint32_t ADC_Map(uint32_t x);

	float weightMeasur(uint32_t x);

	void weightPrint( uint32_t adc);


#endif /* INC_PID_CONTROLLERSTREAM_H_ */


