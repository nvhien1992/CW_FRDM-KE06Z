/**
  @author   Huynh Trung Tin <tinvuong51003405@gmail.com>
  @copyright Copyright (C) 2014 <b>SMART SENSSING AND INTELLIGENT CONTROL GROUP</b> , All rights reserved 
*/
#include "remote_com_resources.h"
#include "remote_com.h"
/*======================================================================
=============================DEFINE FUNCTION============================
=======================================================================*/
/*
 *
 */
void RCOM_get_step_info(job_type_t job_type, uint8_t current_step, RCOM_step_info_t* step_info) {
  switch (job_type) {
    case START_DEVICE:
      switch (current_step) {
        case 0:
          step_info->execution_method = 1;
          strcpy(step_info->send_command, '');          
          step_info->timeout = 3;
          break;
        case 1:
          step_info->execution_method = 1;
          strcpy(step_info->send_command, '');          
          step_info->timeout = 15;
          break;
        default:
          break;
      }
      break;
    case CONFIG_DEVICE:
      switch (current_step) {
        case 0:
          step_info->execution_method = 1;
          strcpy(step_info->send_command, 'AT\r\n');
          step_info->timeout = 1;
          break;
        case 1:
          step_info->execution_method = 1;
          strcpy(step_info->send_command, 'AT\r\n');
          step_info->timeout = 1;
          break;
        case 2:
          step_info->execution_method = 0;
          strcpy(step_info->send_command, 'ATE0\r\n');
          step_info->timeout = 1;
          strcpy(step_info->expected_response, '\r\nOK\r\n');
          break;
        case 3:
          step_info->execution_method = 0;
          strcpy(step_info->send_command, 'AT+CMGF=1\r\n');
          step_info->timeout = 5;
          strcpy(step_info->expected_response, '\r\nOK\r\n');
          break;
        default:
          break;
      }
      break;  // end CONFIG_DEVICE
    case CONNECT_INTERNET:
      switch (current_step) {
        case 0:
          step_info->execution_method = 0;
          strcpy(step_info->send_command, 'AT+CGATT=0\r\n');
          step_info->timeout = 10;
          strcpy(step_info->expected_response, '\r\nOK\r\n');
          break;
        case 1:
          step_info->execution_method = 0;
          strcpy(step_info->send_command, 'AT+CGATT=1\r\n');
          step_info->timeout = 5;
          strcpy(step_info->expected_response, '\r\nOK\r\n');
          break;
        case 2:
          step_info->execution_method = 1;
          strcpy(step_info->send_command, 'AT+SAPBR=0,1\r\n');
          step_info->timeout = 1;
          break;
        case 3: // Set CONTYPE
          step_info->execution_method = 0;
          strcpy(step_info->send_command, 'AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\r\n');
          step_info->timeout = 1;
          strcpy(step_info->expected_response, '\r\nOK\r\n');
          break;
        case 4: //Set APN_NAME
          step_info->execution_method = 0;
          sprintf(step_info->send_command, 'AT+SAPBR=3,1,\"APN\",\"%s\"\r\n', (char*)RCOM_info.apn_name);
          step_info->timeout = 1;
          strcpy(step_info->expected_response, '\r\nOK\r\n');
          break;
        case 5: //Set APN_USER
          step_info->execution_method = 0;
          sprintf(step_info->send_command, 'AT+SAPBR=3,1,\"USER\",\"%s\"\r\n', (char*)RCOM_info.apn_user);
          step_info->timeout = 1;
          strcpy(step_info->expected_response, '\r\nOK\r\n');
          break;
        case 6: //Set APN_PASSWORD
          step_info->execution_method = 0;
          sprintf(step_info->send_command, 'AT+SAPBR=3,1,\"PWD\",\"%s\"\r\n', (char*)RCOM_info.apn_pass);
          step_info->timeout = 1;
          strcpy(step_info->expected_response, '\r\nOK\r\n');
          break;
        case 7: //Query IP
          step_info->execution_method = 0;
          strcpy(step_info->send_command, 'AT+SAPBR=2,1\r\n');
          step_info->timeout = 1;
          strcpy(step_info->expected_response, '\r\n+SAPBR: 1');
          break;
        case 8: //Open bearer
          step_info->execution_method = 0;
          strcpy(step_info->send_command, 'AT+SAPBR=1,1\r\n');
          step_info->timeout = 60;
          strcpy(step_info->expected_response, '\r\nOK\r\n');
          break;
        default:
          break;
      }
      break;  // end CONNECT_INTERNET
    case STOP_DEVICE:
      switch (current_step) {
        case 0:
          step_info->execution_method = 1;
          strcpy(step_info->send_command, '');          
          step_info->timeout = 3;
          break;
        case 1:
          step_info->execution_method = 1;
          strcpy(step_info->send_command, '');          
          step_info->timeout = 3;
          break;
        case 2:
          step_info->execution_method = 1;
          strcpy(step_info->send_command, '');          
          step_info->timeout = 1;
          break;
        default:
          break;
      }
      break;  // end STEP_DEVICE
    case REPORT_DATA:   //Use HTTP POST
    case UPDATE_DATA: //Use HTTP POST
      switch (current_step) {
        case 0:
          step_info->execution_method = 0;
          strcpy(step_info->send_command, 'AT+HTTPINIT\r\n');
          step_info->timeout = 1;
          strcpy(step_info->expected_response, '\r\nOK\r\n');
          break;
        case 1:
          step_info->execution_method = 0;
          strcpy(step_info->send_command, 'AT+HTTPPARA=\"CID\",1\r\n');
          step_info->timeout = 1;
          strcpy(step_info->expected_response, '\r\nOK\r\n');
          break;
        case 2:
          step_info->execution_method = 0;
          sprintf(step_info->send_command, 'AT+HTTPPARA=\"URL\",\"%s/local-com/index.php\"\r\n', RCOM_info.web_domain);
          step_info->timeout = 1;
          strcpy(step_info->expected_response, '\r\nOK\r\n');
          break;
        case 3:
          step_info->execution_method = 0;
          strcpy(step_info->send_command, 'AT+HTTPPARA=\"CONTENT\",\"application/x-www-form-urlencoded\"\r\n');
          step_info->timeout = 1;
          strcpy(step_info->expected_response, '\r\nOK\r\n');
          break;
        case 4:
          step_info->execution_method = 0;
          strcpy(step_info->send_command, '');
          step_info->timeout = 1;
          strcpy(step_info->expected_response, '\r\nDOWNLOAD\r\n');
          break;
        case 5:
          step_info->execution_method = 0;
          strcpy(step_info->send_command, '');
          step_info->timeout = 1;
          strcpy(step_info->expected_response, '\r\nOK\r\n');
          break;
        case 6:
          step_info->execution_method = 0;
          strcpy(step_info->send_command, 'AT+HTTPACTION=1\r\n');
          step_info->timeout = 20;
          strcpy(step_info->expected_response, 'HTTPACTION:1,200');
          break;
        case 7:
          step_info->execution_method = 1;
          strcpy(step_info->send_command, 'AT+HTTPREAD\r\n');
          step_info->timeout = 5;
          break;
        case 8:   // Terminate HTTP session
          step_info->execution_method = 1;
          strcpy(step_info->send_command, '');
          step_info->timeout = 1;
          break;
        case 9:
        default:
          break;
      }
      break; 
    case POLL_COMMAND:  //Use HTTP GET
    case GET_TIMESTAMP: //Use HTTP GET
      switch (current_step) {
        case 0:
          step_info->execution_method = 0;
          strcpy(step_info->send_command, 'AT+HTTPINIT\r\n');
          step_info->timeout = 1;
          strcpy(step_info->expected_response, '\r\nOK\r\n');
          break;
        case 1:
          step_info->execution_method = 0;
          strcpy(step_info->send_command, 'AT+HTTPPARA=\"CID\",1\r\n');
          step_info->timeout = 1;
          strcpy(step_info->expected_response, '\r\nOK\r\n');
          break;
        case 2:
          step_info->execution_method = 0;
          sprintf(step_info->send_command, 'AT+HTTPPARA=\"URL\",\"%s/local-com/index.php?comType=timestamp\"\r\n', RCOM_info.web_domain);
          step_info->timeout = 1;
          strcpy(step_info->expected_response, '\r\nOK\r\n');
          break;
        case 3:
          step_info->execution_method = 0;
          strcpy(step_info->send_command, 'AT+HTTPACTION=0\r\n');
          step_info->timeout = 20;
          strcpy(step_info->expected_response, 'HTTPACTION:0,200');
          break;
        case 4:
          step_info->execution_method = 1;
          strcpy(step_info->send_command, 'AT+HTTPREAD\r\n');
          step_info->timeout = 5;
          break;
        case 5:   //Terminate HTTP session
          step_info->execution_method = 1;
          strcpy(step_info->send_command, '');
          step_info->timeout = 1;
          break;
        case 6:
        default:
          break;
      }
      break;
    case SEND_SMS:
      switch (current_step) {
        case 0:
          step_info->execution_method = 0;
          sprintf(step_info->send_command, 'AT+CMGS=\"%s\"\r', RCOM_info.phone);
          step_info->timeout = 2;
          strcpy(step_info->expected_response, '>');
          break;
        case 1:
          step_info->execution_method = 0;
          sprintf(step_info->send_command, '%s\x1A\r', RCOM_info.sms);
          step_info->timeout =10;
          strcpy(step_info->expected_response, '\r\nOK\r\n');
          break;
        default:
          break;
      }
      break;  // end SEND_SMS
    case MAKE_CALL:
      switch (current_step) {
        case 0:
          step_info->execution_method = 0;
          sprintf(step_info->send_command, 'ATD%s;\r\n', RCOM_info.phone);
          step_info->timeout = 30;
          strcpy(step_info->expected_response, '\r\nBUSY\r\n');
          break;
        case 1:
          step_info->execution_method = 0;
          strcpy(step_info->send_command, 'ATH\r\n');
          step_info->timeout = 1;
          strcpy(step_info->expected_response, '\r\nBUSY\r\n');
          break;
        default:
          break;
      } 
      break;  // end MAKE_CALL
    case TURN_OFF_DEVICE:
      break;
    case TERMINATE_HTTP_SESSION:
      break;
    default:
      break;
  }
}
/*
 *
 */
RCOM_step_result_t RCOM_step_excution(RCOM_step_info_t* step_info) {
  switch (ext_method) {
    
    // execution method: timeout and state machine
    case 0:
      // copy expected response to RCOM-state-machine-buffer
      RCOM_info.SM_buffer = (void*)step_info->expected_response;
      
      // Enable RCOM-state-machine
      RCOM_SM_enable();

      // send command to UART
      uart_writef(step_info->send_command);

      //TODO: wait message with timeout = $timeout
      // WAITMESSAGE(step_info->timeout);
      // if isTimeout
      //   return STEP_FAIL;
      // return STEP_SUCCESS;  //Exit WAITMESSAGE by message from UART interrupt handler 
      break;
    
    // execution method: timeout only
    case 1:

      // send command to UART
      uart_writef(step_info->send_command);

      //TODO: wait message with timeout = $timeout
      // WAITMESSAGE(step_info->timeout);
      // return STEP_SUCCESS;
    
      break;
    
    // execution method: nothing
    case 2:
      // send command to UART
      uart_writef(step_info->send_command);
      // return STEP_SUCCESS;
      break;

    default:
      // return STEP_SUCCESS;
      break;
  }
  // return STEP_SUCCESS;
}
/*
 * @description: uart callback function
 */
int uart_rx_callback(unsigned char c){
  if (RCOM_SM_is_enable()){
    if (c == RCOM_info.SM_buffer[RCOM_info.SM_pointer]) {
      // Increase RCOM state machine pointer
      RCOM_info.SM_pointer++;
      // Check end of buffer
      if (RCOM_info.SM_buffer[RCOM_info.SM_pointer] == '\0') {
        
        //TODO: Send e notification to function RCOM_step_excution (success notification)
        
        //Disable RCOM state machine
        RCOM_SM_disable();
      }
    } else {
      //Resend RCOM state machine pointere when receive incorrect character
      RCOM_info.SM_pointer = 0;
    }
  }
}
/*
 * @description: send frame by UART
 */
void uart_writef(char* frame){
  while (*frame != '\0'){
    uart1_writeb((unsigned char)*frame);
    frame++;
  }
}
/*
 * @description: check state machine of RCOM is enabled or not
 */
bool RCOM_SM_is_enable(void) {
  return RCOM_info.SM_status;
} 

void RCOM_SM_enable(void) {
  RCOM_info.SM_status = true;
  RCOM_info.SM_pointer = 0;
}

void RCOM_SM_disable(void) {
  RCOM_info.SM_status = false;
  RCOM_info.SM_pointer = 0;
}

void RCOM_EnableRxBuf(void){
  enRecv = true;
}


void RCOM_DisableRxBuf(void){
  enRecv = false;
}


void RCOM_SetDomain(char* newDomain){
  DOMAIN = newDomain;
}

void RCOM_SetRxBuf(char* bufAddr){
  rx_buf = bufAddr;
}


char* RCOM_GetRxBuf(void){
  return rx_buf;
}

uint16_t RCOM_GetRxBufLen(void){
  return rx_buf_counter;
}
