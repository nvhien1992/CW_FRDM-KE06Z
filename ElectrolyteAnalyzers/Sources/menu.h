/**
 @file menu.h
 @brief Implementing functions for menu of the electrolyte analyzer.
 @author <b>Nguyen Van Hien</b> <nvhien1992@gmail.com>
 @copyright Copyright &copy; 2015, <b>SMART SENSSING AND INTELLIGENT CONTROL GROUP</b>, All rights reserved. 
 */
#ifndef MENU_H_
#define MENU_H_

#include "PE_Types.h"
#include "mqxlite.h"

/**
 * @brief
 */
void analyzer_init(void);

/**
 * @brief
 */
void yes_no_btn_processing(void);

/**
 * @brief
 */
void enable_button(void);

/**
 * @brief
 */
void disable_button(void);

/**
 * @brief
 */
void main_menu(void);

/**
 * @brief
 */
void measurement_menu(void);

/**
 * @brief
 */
void print_functions_menu(void);

/**
 * @brief
 */
void qc_std_dialysate_urine_sample_menu(void);

/**
 * @brief
 */
void daily_maintenance_menu(void);

/**
 * @brief
 */
void operator_functions_menu(void);

/**
 * @brief
 */
void program_instrument_menu(void);

/**
 * @brief
 */
void service_functions_menu(void);

/**
 * @brief
 */
void calibration_menu(void);

/**
 * @brief
 */
void power_up_sequence(void);

#endif /* MENU_H_ */
