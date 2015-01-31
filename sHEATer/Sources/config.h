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

bool init_conf_array(conf_arr_s *conf_arr);
bool insert(conf_arr_s *conf_arr, uint16_t temp, uint16_t time, uint8_t index);
bool delete(conf_arr_s *conf_arr, uint8_t index);
bool modify_temp(conf_arr_s *conf_arr, uint16_t temp, uint8_t index);
bool modify_time(conf_arr_s *conf_arr, uint16_t time, uint8_t index);

#endif /* CONFIG_H_ */
