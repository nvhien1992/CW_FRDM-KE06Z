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
#include "remote_com.h"
/////////////////////////////////////////////
#define SIM_UART_DATA_RATE    38400

/*======================================================================
 ==============================DEFINE GPIO===============================
 =======================================================================*/

//MOB_nRST is output_
#define  MOB_NRST_SEL         			 P3SEL
#define  MOB_NRST_DIR         			 P3DIR
#define  MOB_NRST_PIN         			 0x40
#define  MOB_NRST_SET()       			 P3OUT |= 0x40
#define  MOB_NRST_CLEAR()     			 P3OUT &= 0xBF
//MOB_nPW is output
#define  MOB_NPW_SEL          			 P8SEL
#define  MOB_NPW_DIR          			 P8DIR
#define  MOB_NPW_PIN          			 0x08
#define  MOB_NPW_SET()        			 P8OUT |= 0x08
#define  MOB_NPW_CLEAR()      			 P8OUT &= 0xF7
//MOB_STATUS is input
#define  MOB_STATUS_SEL       			 P8SEL
#define  MOB_STATUS_DIR       			 P8DIR
#define  MOB_STATUS_PIN       			 0x04
#define  MOB_STATUS_IN        			 P8IN
//MOB_RI is input
#define  MOB_RI_SEL           			 P2SEL
#define  MOB_RI_DIR           			 P2DIR
#define  MOB_RI_PIN           			 0x10
#define  MOB_RI_IN            			 P2IN
//MOB_DTR is output
#define  MOB_DTR_SEL          			 P7SEL
#define  MOB_DTR_DIR          			 P7DIR
#define  MOB_DTR_PIN          			 0x08
#define  MOB_DTR_SET()        			 P7OUT |= 0x08
#define  MOB_DTR_CLEAR()      			 P7OUT &= 0xF7

/*======================================================================
 ================================STEP NUM================================
 =======================================================================*/
#define  SIM900_START_DEVICE_STEP_NUM           2
#define  SIM900_CONFIG_DEVICE_STEP_NUM          4
#define  SIM900_CONNECT_INTERNET_STEP_NUM       9
#define  SIM900_STOP_DEVICE_STEP_NUM			3
#define  SIM900_HTTP_POST_STEP_NUM              9
#define  SIM900_HTTP_GET_STEP_NUM               6
#define  SIM900_SEND_SMS_STEP_NUM               2
#define  SIM900_MAKE_VOICE_CALL_STEP_NUM        2
#define  SIM900_TERM_HTTP_SESSION_STEP_NUM	    1

/*======================================================================
 ============================DEFINE FUNCTION=============================
 =======================================================================*/
#include "remote_com_resources.h"

/**
 * @brief 
 *   turn on sim900
 * @para:
 *   none
 * @return:
 *   none
 */
void sim900_power_on();

/**
 * @brief 
 *   turn off sim900
 * @para:
 *   none
 * @return:
 *   none
 */
void sim900_power_off(void);

void sim900_term_http_session(void);

/**
 * @brief 
 *   Sending a command to SIM900
 * @param
 *   cmd: comamnd (string format)
 * @return
 */
uint8_t sim900_send_cmd(char* cmd);

/**
 @brief Send more data to the SIM900
 @return None
 @attention This function do not clear RXBuf before sending 
 */
void sim900_send_more(char* data);

/**
 * 
 */
uint8_t sim900_send_http_data(char* data, uint8_t is_sent);

/**
 * 
 */
char* sim900_get_http_res_content(void);

/**
 * @brief 
 *   config gpio for sim900 connection
 * @para   
 *   none
 * @return
 *   none
 */
void sim900_gpio_init(void);

void sim900_start_device(RCOM_job_result_t *job_result);

/**
 * @brief  reinit sim900
 *	Init GPIO, UART
 *	Power up
 *	Create a square wave at nRST pin
 */
void sim900_config_device(RCOM_job_result_t *job_result);

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
void sim900_post_report(gw_rpt_para_t* pRptPara, RCOM_job_result_t *job_result);

/*
 * @brief	Get unit (in string format) from unit (in int format)
 */
void SWM_Get_Unit_Str(ss_unit_t unit, char unit_str[]);

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
void sim900_post_update(RCOM_job_result_t *job_result);

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
void sim900_send_sms(RCOM_job_result_t *job_result);

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
uint8_t sim900_make_voice_call(char* phone_number, uint8_t current_step);

/*
 * @return
 *      1: need to terminate session
 *      0: don't need to terminate session
 */
uint8_t sim900_need_terminate_session(RCOM_job_type_t job_type);

#endif //SIM900_INT_H_

