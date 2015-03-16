/*
 * PID.c
 *
 *  Created on: Mar 8, 2015
 *      Author: nvhie_000
 */
#include "PID.h"

#define MAX_TS 360000000 //us
bool PID_reinit(pid_t* pPID, int32_t r, uint32_t Ts, int32_t Kp, int32_t Ki,
		int32_t Kd) {
	if (pPID == NULL || Ts <= 0 || Ts >= MAX_TS) {
		return false;
	}

	pPID->r = r;
	pPID->Ts = Ts;
	pPID->Kp = Kp;
	pPID->Ki = Ki;
	pPID->Kd = Kd;
	pPID->a = (float) Kp + (float) Ki * (float) Ts / 2.0
			+ (float) Kd / (float) Ts;
	pPID->b = -(float) Kp + (float) Ki * (float) Ts / 2.0
			- 2.0 * (float) Kd / (float) Ts;
	pPID->c = (float) Kd / (float) Ts;
	pPID->ek_1 = 0;
	pPID->ek_2 = 0;
	pPID->uk_1 = 0;

	return true;
}
int32_t PID_textbook_step(pid_t* pPID, int32_t s) {
	int32_t uk, ek;

	ek = s - pPID->r;
	uk = pPID->uk_1 + pPID->a * ek + pPID->b * pPID->ek_1 + pPID->c * pPID->ek_2;
	pPID->uk_1 = uk;
//	ek_2 = ek_1;
//	ek_1 = ek;
	pPID->ek_2 = pPID->ek_1;
	pPID->ek_1 = ek;

	return uk;
}

