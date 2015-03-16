/*
 * PID.h
 *
 *  Created on: Mar 8, 2015
 *  Author: Bui Van Hieu <bvhieu@cse.hcmut.edu.vn>
 *  Brief Define general data structure and functions implementing general PID controller
 *	Copyright (C) 2015 <b>INTELLIGENT SENSSING AND SMART SYSTEM</b> , All rights reserved 
 */

#ifndef PID_H_
#define PID_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct {
	int32_t r; /**! Reference point or set point */
	uint32_t Ts; /**! Sampling duty, in term of micro second (us) */
	int32_t Kp; /**! Kp factor */
	int32_t Ki; /**! Ki factor */
	int32_t Kd; /**! Kd factor */
	/* other local variable for implementing textbook-pid algorithm */
	float a; /**! The derived a factor */
	float b; /**! The derived b factor */
	float c; /**! The derived c factor */
	int32_t ek_1; /**! The error e[k-1] */
	int32_t ek_2; /**! The error e[k-2] */
	int32_t uk_1; /**! The output u[k-1] */
} pid_t;

/**
 * @brief Reinit the PID algorithm with new parameter. Reset all local variable
 * 
 * @param[in|out] pPID Pointer to the PID data structure
 * @param[in] r New reference point
 * @param[in] Ts New sampling duty
 * @param[in] Kp New Kp factor
 * @param[in] Ki New Ki factor
 * @param[in] Kd New Kd factor
 * 
 * @return 	false if pPID is NULL; or Ts <= 0; or Ts >= 360 000 000 (do not support too large Ts)
 * 			true if The PID is reinited OK
 */
bool PID_reinit(pid_t* pPID, int32_t r, uint32_t Ts, int32_t Kp, int32_t Ki,
		int32_t Kd);

/**
 * @brief Calculate the next output using textbook-digitalized algorithm
 * 
 * @param[in|out] pPID Pointer to the PID data structure 
 * @param[in] s The new sensing output value (e[k] = s - r)
 * 
 * @return The new output value
 */
int32_t PID_textbook_step(pid_t* pPID, int32_t s);

#endif /* PID_H_ */
