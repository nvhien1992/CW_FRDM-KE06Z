/**
 @file sync_buffer.h
 @brief
 @author <b>Nguyen Van Hien</b> <nvhien1992@gmail.com>
 @copyright Copyright &copy; 2015, <b>SMART SENSSING AND INTELLIGENT CONTROL GROUP</b>, All rights reserved. 
 */

#ifndef SYNC_BUFFER_H_
#define SYNC_BUFFER_H_

#include "PE_Types.h"
#include "mqxlite.h"

typedef struct {
	LWSEM_STRUCT read_sem;
	LWSEM_STRUCT write_sem;
	uint8_t *resource;
	uint16_t resource_size;
} sync_buffer_t;

/**
 * @brief Create 2 semaphores: read and write semaphore. Initialize resource.
 * 
 * @param[in,out] sync_buff Contains read/write sem and sharing resource.
 * @param[in] buffer Location of sharing resource.
 * @param[in] buffer_size Size of buffer.
 */
void sync_buffer_init(sync_buffer_t *sync_buff, uint8_t *buffer,
		uint16_t buffer_size);

/**
 * @brief Allow changing the location of sharing resouce.
 * 
 * @param[in,out] sync_buff Contains read/write sem and sharing resource.
 * @param[in] buffer Location of sharing resource.
 * @param[in] buffer_size Size of buffer.
 */
void sync_buffer_set_resource(sync_buffer_t *sync_buff, uint8_t *buffer,
		uint16_t buffer_size);

/**
 * @brief Return the size of resource buffer.
 * 
 * @param[in,out] sync_buff Contains read/write sem and sharing resource.
 * 
 * @return Size of resource buffer.
 */
uint16_t sync_buffer_get_resource_size(sync_buffer_t *sync_buff);

/**
 * @brief Check read sem and get location of resource.
 * 		Call sync_buffer_allow_write function after calling this function.
 * 
 * @param[in,out] sync_buff Contains read/write sem and sharing resource.
 * 
 * @return Pointer to the location of resource.
 */
uint8_t* sync_buffer_read(sync_buffer_t *sync_buff);

/**
 * @brief Check write sem and get location of resource. Allow writing data into resource buffer.
 * 		Call sync_buffer_allow_read function after writing finish.
 * 
 * @param[in,out] sync_buff Contains read/write sem and sharing resource.
 * 
 * @return Pointer to the location of resource.
 */
uint8_t* sync_buffer_get_resource_to_write(sync_buffer_t *sync_buff);

/**
 * @brief Check write sem and write data directly into resource buffer.
 * 		Call sync_buffer_allow_read function after calling this function.
 * 
 * @param[in,out] sync_buff Contains read/write sem and sharing resource.
 * @param[in] in_buff Pointer to data buffer.
 * @param[in] size Size of data in given buffer.
 */
void sync_buffer_write(sync_buffer_t *sync_buff, uint8_t *in_buff,
		uint16_t size);

/**
 * @brief Called after writing data into resource buffer. Allow reading data from resouce buffer.
 * 
 * @param[in,out] sync_buff Contains read/write sem and sharing resource.
 */
void sync_buffer_allow_read(sync_buffer_t *sync_buff);

/**
 * @brief Called after reading data from resource buffer. Allow writing data into resouce buffer.
 * 
 * @param[in,out] sync_buff Contains read/write sem and sharing resource.
 */
void sync_buffer_allow_write(sync_buffer_t *sync_buff);

#endif /* SYNC_BUFFER_H_ */
