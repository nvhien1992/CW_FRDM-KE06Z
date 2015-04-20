/*
 * array_utils.h
 *
 *  Created on: Jan 30, 2015
 *      Author: Nguyen Van Hien <nvhien1992@gmail.com>
 *  Brief 
 */

#ifndef ARRAY_UTILS_H_
#define ARRAY_UTILS_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct {
	uint8_t *element_list;
	uint16_t max_elements;
	uint16_t num_elements; //The current number of the configuring elements in array.
	uint8_t sizeof_element;
} array_t;

/**
 * @brief Initialize the configuring array.
 * 
 * @param[in/out] arr Pointer to a struct containing the configuring array. 
 * 
 * @return false if arr is null.
 */bool init_array(array_t *arr, uint8_t *given_arr, uint16_t max_elements,
		uint8_t sizeof_element_in_byte);

/**
 * @brief Insert new configuring elements into the configuring array.
 * 
 * @param[in] arr Pointer to a struct containing the configuring array.
 * @param[in] value The value will be stored in conf_arr.
 * @param[in] index Position in array.
 * 
 * @return false if:
 * 			- arrays are full.
 * 			- arr is null.
 * 			- index is greater than the numbers of elements in the array.
 */bool insert_element_at_index(array_t *arr, uint8_t *value, uint16_t index);

/**
 * @brief Delete an elements in the configuring array.
 * 
 * @param[in] arr Pointer to a struct containing the configuring array.
 * @param[in] index Position in arrays.
 * 
 * @return false if:
 * 			- arrays are empty.
 * 			- arr is null.
 * 			- index is greater than the numbers of elements in the array. 
 */bool delete_element_at_index(array_t *arr, uint16_t index);

/**
 * @brief Modify the inserted value in the configuring array.
 * 
 * @param[in] arr Pointer to a struct containing the configuring array.
 * @param[in] new_value New value will be stored.
 * @param[in] index Position in arrays.
 * 
 * @return false if:
 * 			- arr is null.
 * 			- index+1 is greater than the numbers of elements in the array. 
 */bool modify_element_at_index(array_t *arr, uint8_t *new_value, uint16_t index);

/**
 * @brief Get value stored at index in arr
 * 
 * @param[in] array_t
 * @param[out] conf_value
 * @param[in] index
 * 
 * @return 
 */bool get_element_at_index(array_t *arr, uint8_t *out_value, uint8_t index);

/**
 * 
 */
uint16_t get_num_valid_element(array_t *arr);

#endif /* ARRAY_UTILS_H_ */
