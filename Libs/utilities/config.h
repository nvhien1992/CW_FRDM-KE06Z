/*
 * config.h
 *
 *  Created on: Jan 30, 2015
 *      Author: Nguyen Van Hien <nvhien1992@gmail.com>
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include <stdint.h>
#include <stdbool.h>

#define max_conf_element 64

typedef struct {
	uint16_t temp_list[max_conf_element];
	uint16_t time_list[max_conf_element];
	uint8_t num_elements; //The number of the configuring elements in arrays.
} conf_arr_s;

/**
 * @brief Initialize the configuring arrays.
 * 
 * @param[in/out] conf_arr Pointer to a struct containing the configuring arrays. 
 * 
 * @return false if conf_arr is null.
 */bool init_conf_array(conf_arr_s *conf_arr);

/**
 * @brief Insert new configuring elements into the configuring arrs.
 * 
 * @param[in] conf_arr Pointer to a struct containing the configuring arrays.
 * @param[in] temp Target temperature.
 * @param[in] time Target time.
 * @param[in] index Position in arrays.
 * 
 * @return false if:
 * 			- arrays are full.
 * 			- conf_arr is null.
 * 			- index is greater than the numbers of elements in the arrays.
 */bool insert(conf_arr_s *conf_arr, uint16_t temp, uint16_t time, uint8_t index);

/**
 * @brief Delete an elements in the configuring arrays.
 * 
 * @param[in] conf_arr Pointer to a struct containing the configuring arrays.
 * @param[in] index Position in arrays.
 * 
 * @return false if:
 * 			- arrays are empty.
 * 			- conf_arr is null.
 * 			- index is greater than the numbers of elements in the arrays. 
 */bool delete(conf_arr_s *conf_arr, uint8_t index);

/**
 * @brief Modify the inserted temperature value in the configuring arrays.
 * 
 * @param[in] conf_arr Pointer to a struct containing the configuring arrays.
 * @param[in] temp New temperature value.
 * @param[in] index Position in arrays.
 * 
 * @return false if:
 * 			- conf_arr is null.
 * 			- index+1 is greater than the numbers of elements in the arrays. 
 */bool modify_temp(conf_arr_s *conf_arr, uint16_t temp, uint8_t index);

/**
 * @brief Modify the inserted time value in the configuring arrays.
 * 
 * @param[in] conf_arr Pointer to a struct containing the configuring arrays.
 * @param[in] time New time value.
 * @param[in] index Position in arrays.
 * 
 * @return false if:
 * 			- conf_arr is null.
 * 			- index+1 is greater than the numbers of elements in the arrays. 
 */bool modify_time(conf_arr_s *conf_arr, uint16_t time, uint8_t index);
 
 bool get_conf_temp(conf_arr_s *conf_arr, uint16_t *conf_temp, uint8_t index);
 
 bool get_conf_time(conf_arr_s *conf_arr, uint16_t *conf_time, uint8_t index);

#endif /* CONFIG_H_ */
