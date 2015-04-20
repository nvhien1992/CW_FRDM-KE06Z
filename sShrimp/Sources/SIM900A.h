/**
 @file SIM900A.h
 @brief Implementing internal function of SIM900 
 @author   Nguyen Van Hien <nvhien1992@gmail.com>
 @	Huynh Trung Tin <tinvuong51003405@gmail.com>
 @	Bui Van Hieu <bvhieu@cse.hcmut.edu.vn>
 @copyright Copyright (C) 2015 <b>SMART SENSSING AND INTELLIGENT CONTROL GROUP</b> , All rights reserved 
 */

#ifndef SIM900A_H_
#define SIM900A_H_

#include "remote_com_resources.h"

/*=======================================================================
 ==============================DEFINE STRUCTS============================
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
		void (*Enable)(LDD_TDeviceData *dev_data);
		void (*Disable)(LDD_TDeviceData *dev_data);
		bool (*GetVal)(LDD_TDeviceData *dev_data);
	} RI;
} SIM900_pins_t;

typedef struct {
	char* apn_name;
	char* apn_pass;
	char* apn_user;
} SIM900_params_t;

typedef enum {
	DEL_READ, //del all read msgs
	DEL_UNREAD, //del all unread msgs
	DEL_SENT, //del all sent msgs
	DEL_UNSENT, //del all unsent msgs
	DEL_INBOX, //del all received msgs
	DEL_ALL, //del all msgs
} SIM900_del_SMS_t;

typedef struct {
	char phone_number[12];
	char msg_content[150];
} SIM900_SMS_package_t;

typedef enum {
	INCOMING_VOICE_CALL, INCOMING_DATA_CALL, RECEIVED_SMS_MSG,
} SIM900_RI_result_t;

/*=======================================================================
 ============================DEFINE FUNCTIONS============================
 =======================================================================*/
/**
 * @brief config gpio for sim900 connection.
 * 
 * @param[in] defined_pins 
 */
void sim900_init(SIM900_pins_t *defined_pins);

/**
 * 
 */
void sim900_control_ring_indicator(bool is_enabled);

/**
 * 
 */
void sim900_start(RCOM_job_result_t *job_result);

/**
 * @brief  Initialize sim900 using AT command.
 * 
 * @param[out] job_result
 */
void sim900_default_config(RCOM_job_result_t *job_result);

/**
 * @brief Select sleeping mode controlled by DTR pin for SIM900A.
 * 
 * @param[out] job_result
 */
void sim900_enable_DTR_sleep_mode(RCOM_job_result_t *job_result);

/**
 * 
 */
void sim900_disable_sleep_mode(RCOM_job_result_t *job_result);

/**
 * @brief Using DTR pin to wake up or allow SIM900 go to sleep.
 * Set sleep mode = 1 (sleeping mode controlled by DTR pin).
 * 
 * @param[in] sleep_sim900 -TRUE if allowing sleeping, FALSE if waking up SIM900.
 */
void sim900_control_sleep_manually(bool sleep_sim900);

/**
 * 
 */
void sim900_RI_callback(void);

/**
 * 
 */
SIM900_RI_result_t sim900_process_RI_task(RCOM_job_result_t *job_result);

/**
 * 
 */
void sim900_uart_callback(void);

/**
 * 
 */
void sim900_set_apn_para(char* apn_name, char* apn_user, char* apn_pass);

/**
 * 
 */
bool sim900_check_SIM_inserted(RCOM_job_result_t *job_result);

/**
 * 
 */
void sim900_get_MSP_name(RCOM_job_result_t *job_result);

/**
 * @brief  sim900 gprs config
 * 
 * @param[out] job_result
 */
void sim900_connect_internet(RCOM_job_result_t *job_result);

/**
 * @brief  stop SIM900.
 * 
 * @param[out] job_result
 */
void sim900_stop(RCOM_job_result_t *job_result);

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
 * @param[in]
 * @param[out]
 */
//void sim900_post_report(gw_rpt_para_t* pRptPara, RCOM_job_result_t *job_result);
/*
 * @brief	Get unit (in string format) from unit (in int format)
 */
//void SWM_Get_Unit_Str(ss_unit_t unit, char unit_str[]);
/**
 * @brief Send update with gprs	
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
 * @param[out] job_result
 */
//void sim900_post_update(RCOM_job_result_t *job_result);
/**
 * 
 */
void sim900_create_HTTP_POST_session(RCOM_job_result_t *job_result, char *URL);

/**
 * @brief Send the size of HTTP data by UART before sending data.
 * sim900_create_HTTP_GET_session() must be exe-ed before calling this function.
 * 
 * @param[out] job_result
 * @param[in] data_size
 */
void sim900_send_HTTP_data_size(RCOM_job_result_t *job_result,
		uint16_t data_size);

/**
 * 
 */
void sim900_uart_send_data(char *data);

/**
 * @brief Send HTTP data to the connected URL. 
 * sim900_create_HTTP_GET_session() and sim900_send_HTTP_data_size() must be exe-ed before calling this function.
 * Call sim900_terminate_HTTP_session() to finish session.
 * 
 * @param[out] job_result
 * @param[out] out_buffer
 */
void sim900_HTTP_POST_action(RCOM_job_result_t *job_result);

/**
 * 
 */
void sim900_HTTP_POST(RCOM_job_result_t *job_result, char *URL, char *data);

/**
 * @brief Read data from webserver in GET method. 
 * sim900_create_HTTP_GET_session() must be exe-ed before calling this function.
 * Call sim900_terminate_HTTP_session() to finish session.
 * 
 * @param[out] job_result
 * @param[in] URL
 * @param[out] out_buffer
 */
void sim900_HTTP_GET(RCOM_job_result_t *job_result, char *URL);

/**
 * 
 */
void sim900_terminate_HTTP_session(RCOM_job_result_t *job_result);

/**
 * @brief Send an SMS message to the specified phone number.
 * 
 * @param[out] job_result
 * @param[in] phone_number
 * @param[in] msg	
 */
void sim900_send_sms(RCOM_job_result_t *job_result, char* phone_number,
		char* msg);

/**
 * @brief Make a voice call.
 * 
 * @param[out] job_result
 * @param[in] phone_number
 */
void sim900_make_missed_voice_call(RCOM_job_result_t *job_result,
		char* phone_number);

/**
 * 
 */
void sim900_read_SMS_msg(RCOM_job_result_t *job_result, uint8_t sms_index);

/**
 * 
 */
void sim900_show_incoming_call_number(RCOM_job_result_t *job_result);

/**
 * 
 */
void sim900_not_show_incoming_call_number(RCOM_job_result_t *job_result);

/**
 * 
 */
void sim900_enable_echo_mode(RCOM_job_result_t *job_result);

/**
 * 
 */
void sim900_disable_echo_mode(RCOM_job_result_t *job_result);

/**
 * 
 */
void sim900_set_SMS_text_mode(RCOM_job_result_t *job_result);

/**
 * 
 */
void sim900_set_SMS_PDU_mode(RCOM_job_result_t *job_result);

/**
 * 
 */
void sim900_hang_up_voice_call(RCOM_job_result_t *job_result);

/**
 * 
 */
void sim900_delete_group_SMS_message(RCOM_job_result_t *job_result,
		SIM900_del_SMS_t del_type);

/**
 * 
 */
void sim900_delete_an_SMS_message(RCOM_job_result_t *job_result, uint8_t index);

#endif //SIM900A_H_
