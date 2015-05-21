/**
 @file SIM900A.h
 @brief Declare data structures and functions for SIM900's library. 
 @author <b>Nguyen Van Hien</b> <nvhien1992@gmail.com><br>
 @author <b>Huynh Trung Tin</b> <tinvuong51003405@gmail.com><br>
 @author <b>Bui Van Hieu</b> <bvhieu@cse.hcmut.edu.vn>
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
	char* apn_name; /*!< the name of Access Point Name */
	char* apn_pass; /*!< password for Access Point Name */
	char* apn_user; /*!< username for Access Point Name */
} SIM900_apn_t;

typedef struct {
	char *phone_number; /*!< the phone number that sent or received SMS message */
	char *content; /*!< content of an SMS message */
} SIM900_SMS_msg_t;

typedef enum {
	DEL_READ, /*!< delete all read messages */
	DEL_UNREAD, /*!< delete all unread messages */
	DEL_SENT, /*!< delete all sent messages */
	DEL_UNSENT, /*!< delete all unsent messages */
	DEL_INBOX, /*!< delete all received messages */
	DEL_ALL, /*!< delete all messages */
} SIM900_del_SMS_t;

typedef enum {
	INCOMING_VOICE_CALL, /*!< having incoming voice call */
	INCOMING_DATA_CALL, /*!< having incoming data call */
	RECEIVED_SMS_MSG, /*!< received an SMS message */ 
	URC, /*!<  */
	RI_UNKNOWN, /*!< Unknown Ring Indicator */
} SIM900_RI_result_t;

/*=======================================================================
 ============================DEFINE FUNCTIONS============================
 =======================================================================*/
/**
 * @brief Initialize SIM900. Must be called before calling other functions in this file.
 * 
 * @param[in] defined_pins Contains function pointers created in PE's components.
 */
void sim900_init(SIM900_pins_t *defined_pins);

/**
 * @brief Enable or Disable RI.
 */
void sim900_control_ring_indicator(bool is_enabled);

/**
 * @brief Start SIM900.
 * 
 * @return TRUE if success, FALSE if fail.
 */
bool sim900_start(void);

/**
 * @brief Configure sim900 using AT command.
 * 
 * @return TRUE if success, FALSE if fail. 
 */
bool sim900_default_config(void);

/**
 * @brief Select sleeping mode controlled by DTR pin for SIM900A.
 * 
 * @return TRUE if success, FALSE if fail.
 */
bool sim900_enable_DTR_sleep_mode(void);

/**
 * @brief Disable auto-sleep mode.
 * 
 * @return TRUE if success, FALSE if fail.
 */
bool sim900_disable_sleep_mode(void);

/**
 * @brief Using DTR pin to wake up or allow SIM900 go to sleep.
 * Set sleep mode = 1 (sleeping mode controlled by DTR pin).
 * 
 * @param[in] sleep_sim900 TRUE if allowing sleeping, FALSE if waking SIM900 up.
 */
void sim900_control_sleep_manually(bool sleep_sim900);

/**
 * @brief The callback function called in interrupt's handler.
 */
void sim900_RI_callback(void);

/**
 * @brief Call this function in a task to process Ring Indicator.
 * 
 * @param[out] RI_result Contains phone number if RI indicates incoming call or 
 * 			SMS msg index if RI indicates recv-ed SMS msg. 
 * 
 * @return Incoming call or received SMS msg.
 */
SIM900_RI_result_t sim900_process_RI_task(char* RI_result);

/**
 * @brief Received RI unsolicited response when RI occurs.
 */
void sim900_uart_callback(void);

/**
 * @brief Set APN parameter before connecting internet.
 * 
 * @param[in] apn_name
 * @param[in] apn_user
 * @param[in] apn_pass
 */
void sim900_set_apn_para(char* apn_name, char* apn_user, char* apn_pass);

/**
 * @brief Check SIM inserted or not.
 * 
 * @return TRUE if success, FALSE if fail.
 */
bool sim900_check_SIM_inserted(void);

/**
 * @brief Get Mobile Pervice Provider's name
 * 
 * @return NULL if fail else MSP name will be returned.
 */
char* sim900_get_MSP_name(void);

/**
 * @brief Connect internet using GPRS.
 * 
 * @return TRUE if success, FALSE if fail.
 */
bool sim900_connect_internet(void);

/**
 * @brief Stop SIM900.
 * 
 * @return TRUE if success, FALSE if fail.
 */
bool sim900_stop(void);

/**
 * @brief Create POST session.
 * 
 * @param[in] URL Web address will be accessed.
 * 
 * @return TRUE if success, FALSE if fail.
 */
bool sim900_create_HTTP_POST_session(char *URL);

/**
 * @brief Send the size of HTTP data by UART before sending data.
 * sim900_create_HTTP_POST_session() must be called before calling this function.
 * 
 * @param[in] data_size
 * 
 * @return TRUE if success, FALSE if fail.
 */
bool sim900_send_HTTP_data_size(uint16_t data_size);

/**
 * @brief Write a string data into SIM900's buffer before sending over GPRS.
 * 
 * @param[in] data Pointer to string buffer.
 */
void sim900_uart_send_data(char *data);

/**
 * @brief Send HTTP data to the connected URL. 
 * sim900_create_HTTP_POST_session() and sim900_send_HTTP_data_size() must be exe-ed before calling this function.
 * Call sim900_terminate_HTTP_session() to finish session.
 * 
 * @return NULL if fail else response content will be returned.
 */
char* sim900_HTTP_POST_action(void);

/**
 * @brief Send data to webserver using POST method.
 * Call sim900_terminate_HTTP_session() to finish session. 
 * 
 * @param[in] URL
 * @param[in] data
 * 
 * @return NULL if fail else response content will be returned.
 */
char* sim900_HTTP_POST(char *URL, char *data);

/**
 * @brief Read data from webserver using GET method. 
 * Call sim900_terminate_HTTP_session() to finish session.
 * 
 * @param[in] URL
 * 
 * @return NULL if fail else response content will be returned.
 */
char* sim900_HTTP_GET(char *URL);

/**
 * @brief Terminate an HTTP session.
 * 
 * @return TRUE if success, FALSE if fail.
 */
bool sim900_terminate_HTTP_session(void);

/**
 * @brief Send an SMS message to the specified phone number.
 * 
 * @param[in] phone_number
 * @param[in] msg	
 * 
 * @return TRUE if success, FALSE if fail.
 */
bool sim900_send_sms(char* phone_number, char* msg);

/**
 * @brief Make a missed voice call.
 * 
 * @param[in] phone_number
 * 
 * @return TRUE if success, FALSE if fail.
 */
bool sim900_make_missed_voice_call(char* phone_number);

/**
 * @brief Read SMS msg at index in SIM900's memory.
 * 
 * @param[in] sms_index
 * 
 * @return NULL if fail else raw sms msg will be returned.
 */
char* sim900_read_SMS_msg(uint8_t sms_index);

/**
 * @brief Return phone number that sent the SMS msg.
 * 
 * @param[in] raw_msg Raw content when receiving response by UART.
 * @param[out] phone_number
 * 
 * @return TRUE if success, FALSE if fail.
 */
bool sim900_get_phone_number_from_SMS_msg(char *raw_msg, char *phone_number);

/**
 * @brief Return content in SMS msg.
 * 
 * @param[in] raw_msg
 * @param[out] content
 * 
 * @return TRUE if success, FALSE if fail.
 */
bool sim900_get_content_from_SMS_msg(char *raw_msg, char *content);

/**
 * @brief Normalize the phone number (zero is first character).
 * 
 * @param[in,out] phone_number
 * @param[in] country_phone_code
 * 
 * @return TRUE if success, FALSE if fail.
 */
bool sim900_normalize_phone_number(char *phone_number, char *country_phone_code);

/**
 * @brief Allow receiving phone number when RI occurs.
 * 
 * @return TRUE if success, FALSE if fail.
 */
bool sim900_show_incoming_call_number(void);

/**
 * @brief Not allow receiving phone number when RI occurs.
 * 
 * @return TRUE if success, FALSE if fail.
 */
bool sim900_not_show_incoming_call_number(void);

/**
 * @brief Enable echo mode.
 *
 * @return TRUE if success, FALSE if fail. 
 */
bool sim900_enable_echo_mode(void);

/**
 * @brief Disable echo mode.
 * 
 * @return TRUE if success, FALSE if fail.
 */
bool sim900_disable_echo_mode(void);

/**
 * @brief SMS in text mode.
 * 
 * @return TRUE if success, FALSE if fail.
 */
bool sim900_set_SMS_text_mode(void);

/**
 * @brief SMS in PDU mode.
 * 
 * @return TRUE if success, FALSE if fail.
 */
bool sim900_set_SMS_PDU_mode(void);

/**
 * @brief Hang up a voice call.
 * 
 * @return TRUE if success, FALSE if fail.
 */
bool sim900_hang_up_voice_call(void);

/**
 * @brief Delete a group of SMS msg:
 * 		"DEL ALL"
 * 		"DEL READ"
 * 		"DEL UNREAD"
 * 		"DEL SENT"
 * 		"DEL UNSENT"
 * 		"DEL INBOX"
 * 
 * @param[in] del_type
 * 
 * @return TRUE if success, FALSE if fail.
 */
bool sim900_delete_group_SMS_msg(SIM900_del_SMS_t del_type);

/**
 * @brief Delete an SMS msg at index in SIM900's memory.
 * 
 * @param[in] index 
 * 
 * @return TRUE if success, FALSE if fail.
 */
bool sim900_delete_an_SMS_msg(uint8_t index);

#endif //SIM900A_H_
