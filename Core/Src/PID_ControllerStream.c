/*
 * mainFunction->c
 *
 *  Created on: 02.07.2023
 *      Author: Zain
 */


#include <PID_ControllerStream.h>



 struct PID_CONTROLLER {

		float 		timeSpace;
		float 		KP;
		float 		KI;
		float		KD;
		float 		eOld;
		float		eSum;
		uint32_t	in;
		uint32_t 	out;
		uint32_t 	referencePoint;


};

PID_Controller * pid_init(void){

	    PID_Controller* PID = malloc (sizeof( PID_Controller));
			if(PID == NULL) return NULL;
	return PID;
}

void PID_Set(PID_Controller* PID, float kp,float ki,float kd,uint32_t referencePoint,float sampleTime ){

		PID->timeSpace = sampleTime / 1000.0f;
		PID->eOld = 0.0f;
		PID->referencePoint = referencePoint;
		PID->eSum +=  (__e(PID) / PID->timeSpace);
		PID->KD = kd;
		PID->KP =kp;
		PID->KI = ki;
}


static float __e( PID_Controller* pid) {
    return (float)(pid->referencePoint - (pid->in));
}


uint32_t __Proportional( PID_Controller* pid) {
    return (pid->KP * __e(pid));
}

float __Integral( PID_Controller* pid) {
    return pid->KI * pid->eSum;
}

float __Derivative( PID_Controller* pid) {
    return pid->KD * (__e(pid) - pid->eOld) / pid->timeSpace;
}

PID_State pid_Compute( PID_Controller* pid) {

  if(pid != NULL){

      uint32_t res = (uint32_t)(__Proportional(pid) + __Integral(pid) + __Derivative(pid));
      if (res >= PWM_MAX_DUTYCYCLE) {
          (pid->out) = PWM_MAX_DUTYCYCLE;
          pid->eSum -= __e(pid) / pid->timeSpace;
      }
      else if (res <= PWM_MIN_DUTYCYCLE) {
          (pid->out) = PWM_MIN_DUTYCYCLE;
          pid->eSum -= __e(pid) / pid->timeSpace;
      }
      else {
          (pid->out) = res;
          pid->eOld = __e(pid);

          return PID_SUCCESS;
      }
  }

return PID_FAILD;
}


void mallocFree(PID_Controller * pid){

  free(pid);
}

uint32_t  pidOut(PID_Controller *pid){

      if(pid!=NULL){
	  return pid->out;
  }
  return NULL;
}

void pidIn(PID_Controller* pid, uint32_t in){

  if(pid!=NULL)
    pid->in = in;
}

uint32_t ADC_Map(uint32_t x)
{
  if(x >= 0)
    return (x - ADC_MIN_ANALOG_IN) * (ADC_MAX_ANALOG_MAPED_OUT - ADC_MIN_ANALOG_MAPED_OUT) / (ADC_MAX_ANALOG_IN - ADC_MIN_ANALOG_IN) + ADC_MIN_ANALOG_MAPED_OUT;
}



float weightMeasur(uint32_t x){

  float voltageADC = (x*3.3)/4095;
  float shunt_Resistor = 0.05;
  float current = voltageADC/shunt_Resistor;
  float weight =((1.8*current)/9.81)*1000.0f;


  return weight;
  }

void weightPrint(uint32_t adc){

  char weight[100];

  ssd1306_SetCursor(0,0);
  sprintf(weight, "s%.2f%s%% It weights ", weightMeasur(adc), " g");
  ssd1306_WriteString(weight, Font_11x18, White);
  ssd1306_UpdateScreen();
}




