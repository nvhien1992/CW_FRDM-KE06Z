/*
 * array_utils.c
 *
 *  Created on: Jan 30, 2015
 *      Author: Nguyen Van Hien <nvhien1992@gmail.com>
 */
#include <cstring>
#include "array_utils.h"

bool init_array(array_t *arr, uint8_t *given_arr, uint16_t max_elements,
		uint8_t sizeof_element_in_byte) {
	/* check parameter */
	if (!given_arr) {
		return false;
	}

	arr->element_list = given_arr;
	arr->max_elements = max_elements;
	arr->sizeof_element = sizeof_element_in_byte;
	arr->num_elements = 0;

	memset(arr->element_list, 0, max_elements);

	return true;
}

bool insert_element_at_index(array_t *arr, uint8_t *value, uint16_t index) {
	/* check parameter */
	if (!arr) {
		return false;
	}

	/* arrays are full */
	if (arr->num_elements == arr->max_elements) {
		return false;
	}

	/* out of range */
	if (index >= arr->max_elements) {
		return false;
	}

	/* not continuous */
	if (index > arr->num_elements) {
		return false;
	}

	if (index < arr->num_elements) {
		memmove(&arr->element_list[(index + 1) * arr->sizeof_element],
				&arr->element_list[index * arr->sizeof_element],
				(arr->num_elements - index) * arr->sizeof_element);
	}

	memcpy(&arr->element_list[index * arr->sizeof_element], value,
			arr->sizeof_element);

	arr->num_elements++;

	return true;
}

bool delete_element_at_index(array_t *arr, uint16_t index) {
	/* check parameter */
	if (!arr) {
		return false;
	}

	/* have no the configuring element */
	if (arr->num_elements == 0) {
		return false;
	}

	/* out of range */
	if (index >= arr->num_elements) {
		return false;
	}

	if (index == arr->num_elements - 1) {
		memset(&arr->element_list[index * arr->sizeof_element], 0,
				arr->sizeof_element);
	} else {
		memmove(&arr->element_list[index * arr->sizeof_element],
				&arr->element_list[(index + 1) * arr->sizeof_element],
				(arr->num_elements - index - 1) * arr->sizeof_element);
		memset(&arr->element_list[(arr->num_elements - 1) * arr->sizeof_element],
				0, arr->sizeof_element);
	}

	arr->num_elements--;

	return true;
}

bool modify_element_at_index(array_t *arr, uint8_t *new_value, uint16_t index) {
	/* check parameter */
	if (!arr) {
		return false;
	}

	/* out of range */
	if (index >= arr->num_elements) {
		return false;
	}

	memcpy(&arr->element_list[index * arr->sizeof_element], new_value,
			arr->sizeof_element);

	return true;
}

bool get_element_at_index(array_t *arr, uint8_t *out_value, uint8_t index) {
	if (!arr) {
		return false;
	}
	if (index >= arr->num_elements) {
		return false;
	}

	memcpy(out_value, &arr->element_list[index * arr->sizeof_element],
			arr->sizeof_element);

	return true;
}

uint16_t get_num_valid_element(array_t *arr) {
	if (!arr) {
		return 0;
	}
	return arr->num_elements;
}
