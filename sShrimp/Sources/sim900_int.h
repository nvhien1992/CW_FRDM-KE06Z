/**
 @file sim900_int.c
 @brief Implementing internal function of SIM900 
 @author   Huynh Trung Tin <tinvuong51003405@gmail.com>
 Bui Van Hieu <bvhieu@cse.hcmut.edu.vn>
 @copyright Copyright (C) 2014 <b>SMART SENSSING AND INTELLIGENT CONTROL GROUP</b> , All rights reserved 
 */

#ifndef SIM900_INT_H_
#define SIM900_INT_H_

//#include "swm_gw_mng.h"
#include "remote_com_resources.h"

/*=======================================================================
 ==============================DEFINE GPIO===============================
 =======================================================================*/
typedef struct {
	struct {
		void (*SetVal)(LDD_TDeviceData *dev_data);
		void (*ClrVal)(LDD_TDeviceData *dev_data);
	} nRST;
	struct {
		void (*SetVal)(LDD_TDeviceData *dev_data);
		void (*ClrVal)(LDD_TDeviceData *dev_data);
	} nPW;
	struct {
		void (*SetVal)(LDD_TDeviceData *dev_data);
		void (*ClrVal)(LDD_TDeviceData *dev_data);
	} DTR;	
	struct {
		bool (*GetVal)(LDD_TDeviceData *dev_data);
	} STATUS;
	struct {
		LDD_TError (*SetEdge)(LDD_TDeviceData *dev_data, uint8_t edge);
		void (*Enable)(LDD_TDeviceData *dev_data);
		void (*Disable)(LDD_TDeviceData *dev_data);
	} RI;
} SIM900_pins_t;

/*=======================================================================
 ================================STEP NUM================================
 =======================================================================*/
#define  SIM900_START_DEVICE_STEP_NUM           3
#define  SIM900_CONFIG_DEVICE_STEP_NUM          4
#define  SIM900_CONNECT_INTERNET_STEP_NUM       9
#define  SIM900_STOP_DEVICE_STEP_NUM			3
#define  SIM900_HTTP_POST_STEP_NUM              9
#define  SIM900_HTTP_GET_STEP_NUM               6
#define  SIM900_SEND_SMS_STEP_NUM               2
#define  SIM900_MAKE_VOICE_CALL_STEP_NUM        3
#define  SIM900_TERM_HTTP_SESSION_STEP_NUM	    1

/*=======================================================================
 ============================DEFINE FUNCTIONS============================
 =======================================================================*/

/**
 * @brief turn on sim900.
 */
void sim900_power_on();

/**
 * @brief turn off sim900.
 */
void sim900_power_off(void);

/**
 * 
 */
char* sim900_get_http_res_content(void);

/**
 * @brief config gpio for sim900 connection.
 * 
 * @param[in] defined_pins 
 */
void sim900_pin_init(SIM900_pins_t *defined_pins);

/**
 * 
 */
void sim900_control_ring_indicator(bool is_enabled);

/**
 * 
 */
void sim900_start_device(RCOM_job_result_t *job_result);

/**
 * @brief  Initialize sim900 using AT command.
 * 
 * @param[out] job_result
 */
void sim900_config_device(RCOM_job_result_t *job_result);

/**
 * @brief Select sleep mode for SIM900A. Only support DTR sleeping.
 * 
 * @param[out] job_result
 * @param[in] sleep_enabled Enable (DTR sleep mode only) and Disable sleeping mode.
 */
void sim900_set_sleep_mode(RCOM_job_result_t *job_result, bool sleep_enabled);

/**
 * 
 */
void sim900_control_sleep_manually(bool sleep_sim900);

/**
 * @brief  sim900 gprs config
 */
void sim900_connect_internet(RCOM_job_result_t *job_result);

/**
 * @brief  stop SIM900
 *	Create a square wave at nRST pin
 *	Power down
 */
void sim900_stop_device(RCOM_job_result_t *job_result);

/*  
 * @description:  send report with gprs			            								
 *    method: http post											  				
 *    Data Format (To send Webserver):													
 *    fN=...&rDN=...&nD=...&n0=...&d0=...&v0=...&sT0=...  v.v. 
 *    fN: farm Name
 *    rDN: received Data Number
 *    nD: Number data of report			
 *    n0: ID of node 0
 *    d0: ID of sensor 0
 *    v0: Value of sensor 0
 *    sT0:Sensing time of sensor 0
 *    n1: ID of node 1
 *    d1: ID of sensor 1
 *    v1: Value of sensor 1
 *    sT1:Sensing time of sensor 1

 *    ...		
 * @para:
 *   domain: domain of webserver 					
 *   pRptPara: pointer of report para structure
 *   curStep: current step in post report operation
 * @return:
 *   1:success
 *   0:fail	
 */
//void sim900_post_report(gw_rpt_para_t* pRptPara, RCOM_job_result_t *job_result);

/*
 * @brief	Get unit (in string format) from unit (in int format)
 */
//void SWM_Get_Unit_Str(ss_unit_t unit, char unit_str[]);

/**
 * @brief   send update with gprs	
 *   -method: http post	
 *   Data Format (To send Webserver):
 *      fN=...&n0=...&aT0=...&d0=...&u0=...&lES0=...&lEC0=...
 *              &lWS0=...&lWC0=...&hWC0=...&hWS0=...&hEC0=...&hES0=... .v.v. 
 *      fN: farm Name
 *      n0: ID of node 0
 *      aT0: actived Time of node 0
 *      d0: ID of sensor 0
 *      u0: unit of sensor 0
 *      lES0: low Emergency Set of sensor 0
 *      lEC0: low Emergency Clear of sensor 0
 *      lWS0: low Warning Set of sensor 0
 *      lWC0: low Warning Clear of sensor 0
 *      hWC0: high Warning Clear of sensor 0
 *      hWS0: high Warning Set of sensor 0
 *      hEC0: high Emergency Clear of sensor 0
 *      hES0: high Emergency Set of sensor 0
 *      ...	
 * @para:	
 *   domain: domain of webserver
 *   curStep: current step in post update operation 	
 * @return:
 *    1: success
 *    0: fail		
 */
//void sim900_post_update(RCOM_job_result_t *job_result);

/**
 * @brief   request Webserver to get time stamp	
 *   method: http get
 * @para:	
 *   domain: domain of webserver
 *   curStep: current step in get timestamp operation
 * @return:
 */
void sim900_get_timestamp(RCOM_job_result_t *job_result);

/**
 * @brief   send a warning SMS message
 * @para:	
 *   phoneNum: phone number which is received message
 *   message: warning message
 *   curStep: current step in send sms operation
 * @return:
 *   1:success
 *   0:fail	
 */
void sim900_send_sms(RCOM_job_result_t *job_result, char* phone_number,
		char* msg);

/**
 * @brief   make a warning voice call
 * @para:	
 *   phoneNumOne: the first phone number
 *   phoneNumSecond: the second phone number
 * @return:
 *   1:success
 *   0:fail	
 * @attention This is emergency situation, block everthing until DONE 
 */
void sim900_make_voice_call(RCOM_job_result_t *job_result, char* phone_number);

#endif //SIM900_INT_H_
