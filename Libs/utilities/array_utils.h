/*
 * array_utils.h
 *
 *  Created on: Jan 30, 2015
 *      Author: Nguyen Van Hien <nvhien1992@gmail.com>
 *  Brief 
 */

#ifndef ARRAY_UTILS_H_
#define ARRAY_UTILS_H_

#include "PE_Types.h"

typedef struct {
	uint16_t *element_list;
	uint8_t max_elements;
	uint8_t num_elements; //The current number of the configuring elements in array.
} conf_arr_t;

/**
 * @brief Initialize the configuring array.
 * 
 * @param[in/out] conf_arr Pointer to a struct containing the configuring array. 
 * 
 * @return false if conf_arr is null.
 */bool init_conf_array(conf_arr_t *conf_arr);

/**
 * @brief Insert new configuring elements into the configuring array.
 * 
 * @param[in] conf_arr Pointer to a struct containing the configuring array.
 * @param[in] value The value will be stored in conf_arr.
 * @param[in] index Position in array.
 * 
 * @return false if:
 * 			- arrays are full.
 * 			- conf_arr is null.
 * 			- index is greater than the numbers of elements in the array.
 */bool insert(conf_arr_t *conf_arr, uint16_t value, uint8_t index);

/**
 * @brief Delete an elements in the configuring array.
 * 
 * @param[in] conf_arr Pointer to a struct containing the configuring array.
 * @param[in] index Position in arrays.
 * 
 * @return false if:
 * 			- arrays are empty.
 * 			- conf_arr is null.
 * 			- index is greater than the numbers of elements in the array. 
 */bool delete(conf_arr_t *conf_arr, uint8_t index);

/**
 * @brief Modify the inserted value in the configuring array.
 * 
 * @param[in] conf_arr Pointer to a struct containing the configuring array.
 * @param[in] new_value New value will be stored.
 * @param[in] index Position in arrays.
 * 
 * @return false if:
 * 			- conf_arr is null.
 * 			- index+1 is greater than the numbers of elements in the array. 
 */bool modify_value(conf_arr_t *conf_arr, uint16_t new_value, uint8_t index);

/**
 * @brief Get value stored at index in conf_arr
 * 
 * @param[in] conf_arr
 * @param[out] conf_value
 * @param[in] index
 * 
 * @return 
 */bool get_conf_value(conf_arr_t *conf_arr, uint16_t *conf_value, uint8_t index);

#endif /* ARRAY_UTILS_H_ */
