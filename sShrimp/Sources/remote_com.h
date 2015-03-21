/**
 @brief Implementing functions for SWM Project
 @author Huynh Trung Tin <tinvuong51003405@gmail.com>
 	 	 Nguyen Van Hien <nvhien1992@gmail.com>
 @copyright Copyright (C) 2014 <b>SMART SENSSING AND INTELLIGENT CONTROL GROUP</b> , All rights reserved 
 */

#ifndef REMOTE_COM_H_
#define REMOTE_COM_H_

#include "PE_Types.h"

/*======================================================================
 ===========================SHARED DEFINATION============================
 =======================================================================*/

typedef enum {
	START_DEVICE, //
	CONFIG_DEVICE, //new - power off, power one, reset, check availability
	CONNECT_INTERNET, //new connect internet over gprs/wifi
	STOP_DEVICE, /* may be omitted */
	REPORT_DATA,
	UPDATE_DATA,
	POLL_COMMAND,
	GET_TIMESTAMP,
	SEND_SMS,
	MAKE_CALL,
	TURN_OFF_DEVICE,
	TERMINATE_HTTP_SESSION,
} RCOM_job_type_t;

typedef enum {
	JOB_FAIL, //
	JOB_SUCCESS_WITH_DATA, //
	JOB_SUCCESS_WITHOUT_DATA, //
} RCOM_job_result_t;

typedef enum {
	STEP_SUCCESS, //
	STEP_FAIL, //
} RCOM_step_result_t;

typedef struct {
	RCOM_job_type_t job_type;
	union {
		uint32_t int_param;
		void* struct_param;
	} content;
} RCOM_job_t;

typedef struct {
	uint8_t execution_method;
	void* send_command;
	uint32_t timeout;
	void* expected_response;
} RCOM_step_info_t;

typedef struct {
	RCOM_job_result_t result_type;
	union {
		uint32_t int_result; //contain failed_reason if failed
		void* struct_result; //contain data if success
	} content;
} RCOM_job_result_t;

typedef enum {
	DEVICE_HAS_NOTHING, //
	DEVICE_HAS_INIT, //
	DEVICE_HAS_CONNECTION //
} RCOM_device_status_t;

typedef struct {
	bool SM_status;
	void* SM_buffer;
	uint32_t SM_pointer;
	uint8_t device_status;
	void* web_domain;
	void* apn_name;
	void* apn_pass;
	void* apn_user;
	void* phone;
	void* sms;
} RCOM_t;

/*======================================================================
 ==============================SHARED DATA===============================
 =======================================================================*/
extern RCOM_t RCOM_info;

#endif //REMOTE_COM_H_

