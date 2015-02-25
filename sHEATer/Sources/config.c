/*
 * config.c
 *
 *  Created on: Jan 30, 2015
 *      Author: Nguyen Van Hien <nvhien1992@gmail.com>
 */
#include "config.h"

bool init_conf_array(conf_arr_s *conf_arr) {
	/* check parameter */
	if (!conf_arr) {
		return false;
	}

	conf_arr->num_elements = 0;

	return true;
}

bool insert(conf_arr_s *conf_arr, uint16_t temp, uint16_t time, uint8_t index) {
	/* check parameter */
	if (!conf_arr) {
		return false;
	}

	/* arrays are full */
	if (conf_arr->num_elements == max_conf_element - 1) {
		return false;
	}

	/* not continuous */
	if (index > conf_arr->num_elements) {
		return false;
	}

	uint8_t count = 0;
	for (count = conf_arr->num_elements; count > index; count--) {
		conf_arr->temp_list[count] = conf_arr->temp_list[count - 1];
		conf_arr->time_list[count] = conf_arr->time_list[count - 1];
	}
	conf_arr->temp_list[index] = temp;
	conf_arr->time_list[index] = time;
	conf_arr->num_elements++;

	return true;
}

bool delete(conf_arr_s *conf_arr, uint8_t index) {
	/* check parameter */
	if (!conf_arr) {
		return false;
	}

	/* have no the configuring element */
	if (conf_arr->num_elements == 0) {
		return false;
	}

	/* out of range */
	if (index >= conf_arr->num_elements) {
		return false;
	}

	uint8_t count = 0;
	for (count = 0; count < conf_arr->num_elements - 1; count++) {
		conf_arr->temp_list[count] = conf_arr->temp_list[count + 1];
		conf_arr->time_list[count] = conf_arr->time_list[count + 1];
	}
	conf_arr->num_elements--;

	return true;
}

bool modify_temp(conf_arr_s *conf_arr, uint16_t temp, uint8_t index) {
	/* check parameter */
	if (!conf_arr) {
		return false;
	}

	/* out of range */
	if (index >= conf_arr->num_elements) {
		return false;
	}

	conf_arr->temp_list[index] = temp;

	return true;
}

bool modify_time(conf_arr_s *conf_arr, uint16_t time, uint8_t index) {
	/* check parameter */
	if (!conf_arr) {
		return false;
	}

	/* out of range */
	if (index >= conf_arr->num_elements) {
		return false;
	}

	conf_arr->time_list[index] = time;

	return true;
}

bool get_conf_temp(conf_arr_s *conf_arr, uint16_t *conf_temp, uint8_t index) {
	if (!conf_arr) {
		return false;
	}
	if (index >= conf_arr->num_elements) {
		return false;
	}
	*conf_temp = conf_arr->temp_list[index];
	
	return true;
}

bool get_conf_time(conf_arr_s *conf_arr, uint16_t *conf_time, uint8_t index) {
	if (!conf_arr) {
		return false;
	}
	if (index >= conf_arr->num_elements) {
		return false;
	}
	*conf_time = conf_arr->time_list[index];
	
	return true;
}
