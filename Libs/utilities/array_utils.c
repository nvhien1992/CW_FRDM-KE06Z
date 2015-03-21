/*
 * array_utils.c
 *
 *  Created on: Jan 30, 2015
 *      Author: Nguyen Van Hien <nvhien1992@gmail.com>
 */
#include "array_utils.h"

bool init_conf_array(conf_arr_t *conf_arr) {
	/* check parameter */
	if (!conf_arr) {
		return FALSE;
	}
	uint8_t index = 0;
	for (index = 0; index < conf_arr->max_elements; index++) {
		conf_arr->element_list[index] = 0;
	}
	conf_arr->num_elements = 0;

	return TRUE;
}

bool insert(conf_arr_t *conf_arr, uint16_t value, uint8_t index) {
	/* check parameter */
	if (!conf_arr) {
		return FALSE;
	}

	/* arrays are full */
	if (conf_arr->num_elements == conf_arr->max_elements) {
		return FALSE;
	}
	
	/* out of range */
	if(index >= conf_arr->max_elements) {
		return FALSE;
	}

	/* not continuous */
	if (index > conf_arr->num_elements) {
		return FALSE;
	}

	uint8_t count = 0;
	for (count = conf_arr->num_elements; count > index; count--) {
		conf_arr->element_list[count] = conf_arr->element_list[count - 1];
	}
	conf_arr->element_list[index] = value;
	conf_arr->num_elements++;

	return TRUE;
}

bool delete(conf_arr_t *conf_arr, uint8_t index) {
	/* check parameter */
	if (!conf_arr) {
		return FALSE;
	}

	/* have no the configuring element */
	if (conf_arr->num_elements == 0) {
		return FALSE;
	}

	/* out of range */
	if (index >= conf_arr->num_elements) {
		return FALSE;
	}

	if (index == conf_arr->num_elements - 1) {
		conf_arr->element_list[index] = 0;
	} else {
		uint8_t count = 0;
		for (count = index; count < conf_arr->num_elements; count++) {
			conf_arr->element_list[count] = conf_arr->element_list[count + 1];
		}
	}

	conf_arr->num_elements--;

	return TRUE;
}

bool modify_value(conf_arr_t *conf_arr, uint16_t new_value, uint8_t index) {
	/* check parameter */
	if (!conf_arr) {
		return FALSE;
	}

	/* out of range */
	if (index >= conf_arr->num_elements) {
		return FALSE;
	}

	conf_arr->element_list[index] = new_value;

	return TRUE;
}

bool get_conf_value(conf_arr_t *conf_arr, uint16_t *conf_value, uint8_t index) {
	if (!conf_arr) {
		return FALSE;
	}
	if (index >= conf_arr->num_elements) {
		return FALSE;
	}
	*conf_value = conf_arr->element_list[index];

	return TRUE;
}
