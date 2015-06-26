/**
 @file remote_com_resources.h
 @brief 
 @author <b>Nguyen Van Hien</b> <nvhien1992@gmail.com>
 @copyright Copyright &copy; 2015, <b>SMART SENSSING AND INTELLIGENT CONTROL GROUP</b>, All rights reserved.
 */

#ifndef REMOTE_COM_RESOURCES_H_
#define REMOTE_COM_RESOURCES_H_

#include "PE_Types.h"
#include "uart_ll.h"

/*=======================================================================
 ===========================PUBLIC RCOM DEFINITIONS======================
 =======================================================================*/
typedef enum {
	RCOM_HAS_NOTHING, 			/*!< rcom didn't start successfully */
	RCOM_STARTED, 				/*!< rcom started successfully */
	RCOM_CONFIGURED, 			/*!< rcom configured successfully */
	RCOM_INTERNET_CONNECTED 	/*!< rcom connected internet successfully */
} RCOM_status_t;

/*=======================================================================
 ==========================PRIVATE RCOM DEFINITIONS======================
 =======================================================================*/
typedef enum {
	SM_AND_TIMEOUT, /*!< using simple state machine to check SIM900A's response after sending cmd */
	TIMEOUT_ONLY, 	/*!< only waiting for SIM900A's execution after sending cmd */
	NOTHING, 		/*!< not waiting after sending cmd */
} RCOM_exe_method_t;

typedef enum {
	STEP_SUCCESS, 	/*!< step executed successfully */
	STEP_FAIL, 		/*!< step executed fail */
} RCOM_step_result_t;

typedef struct {
	RCOM_exe_method_t execution_method; /*!< SM_AND_TIMEOUT, TIMEOUT_ONLY or NOTHING */
	char* command; 						/*!< AT cmd will be sent to SIM900A */
	uint32_t timeout; 					/*!< available if using SM_AND_TIMEOUT, TIMEOUT_ONLY exe-method */
	char* expected_response; 			/*!< pattern used to compare with SIM900A's response in SM_AND_TIMEOUT exe-method */
} RCOM_step_info_t;

typedef struct {
	bool SM_status; 		/*!< state machine is enable or disable */
	char* SM_buffer; 		/*!< contains the expected response */
	uint32_t SM_pointer; 	/*!< position of pointer on SM_buffer while checking response */
} RCOM_SM_t;

typedef struct {
	char* buffer_ptr; 			/*!< pointer to the given buffer */
	uint16_t buffer_max_size;	/*!< size of the given buffer */
	uint16_t buffer_counter;	/*!< num of bytes of data in the given buffer */
	bool enable_buffer;			/*!< enable or disable buffer */
} RCOM_buff_t;

/*=======================================================================
 ==========================PUBLIC RCOM FUNCTIONS=========================
 =======================================================================*/
/**
 * @public
 * @brief Initialize RCOM before starting device (SIM900, wifi).
 * 
 * @param[in] systick_period_in_ms The period of system tick in milisecond.
 */
void RCOM_init(uint8_t systick_period_in_ms);

/**
 * @public
 * @brief Set status for RCOM device (SIM900, wifi,...).
 * 
 * @param[in] new_status Present status of rcom device.
 */
void RCOM_set_status(RCOM_status_t new_status);

/**
 * @public
 * @brief Get status of RCOM device.
 * 
 * @return Status of rcom device.
 */
RCOM_status_t RCOM_get_status(void);

/**
 * @public
 * @brief Callback function processing the received charater.
 */
void RCOM_uart_rx_callback(void);

/**
 * @public
 * @brief Set UART's function pointers for RCOM.
 * 
 * @param[in] defined_uart UART's function pointers. 
 */
void RCOM_set_uart(uart_t *defined_uart);

/**
 * @public
 * @brief Allocate memory for receving response from RCOM device.
 * 
 * @param[in] buf_addr Pointer to a allocated buffer.
 * @param[in] buf_max_size Size of the buffer.
 */
void RCOM_set_rx_buf(char* buf_addr, uint16_t buf_max_size);

/*=======================================================================
 ========================PROTECTED RCOM FUNCTIONS========================
 =======================================================================*/
/**
 * @protected
 * @brief Send a command to SIM900 and process responses.
 * 
 * @param[in] step_info Contains information about a step of the RCOM job.
 * 
 * @return SUCCESS if success, otherwise FAIL.
 */
RCOM_step_result_t RCOM_step_excution(RCOM_step_info_t* step_info);

/**
 * @protected
 * @brief Write a string frame into SIM900 using UART.
 * 
 * @param[in] frame Data in string.
 */
void RCOM_uart_writef(char* frame);

/**
 * @protected
 * @brief Register to receive the char response from SIM900 using UART.
 */
void RCOM_uart_get_char(void);

/**
 * @protected
 * @brief Store the received char response into the buffer.
 * 
 * @param[in] a_buffer The buffer containing response from SIM900.
 */
void RCOM_store_rchar_into_buffer(RCOM_buff_t *a_buffer);

/**
 * @protected
 * @brief Get the RCOM's RX buffer.
 * 
 * @return Pointer to RX buffer.
 */
char* RCOM_get_rx_buf(void);

/**
 * @protected
 * @brief Clear RCOM's RX buffer.
 */
void RCOM_clear_rx_buf(void);

/**
 * @protected
 * @brief Enable RCOM's RX buffer to write response into.
 */
void RCOM_enable_rx_buf(void);

/**
 * @protected
 * @brief Disable RCOM's RX buffer to write response into.
 */
void RCOM_disable_rx_buf(void);

/**
 * @protected
 * @brief Get the length of data stored in RCOM's RX buffer.
 * 
 * @return The length of data.
 */
uint16_t RCOM_get_rx_buf_data_len(void);

/**
 * @protected
 * @brief Get system tick saved when initialize.
 * 
 * @return System tick in milisecond.
 */
uint8_t RCOM_get_systick(void);

#endif //REMOTE_COM_RESOURCES_H_
