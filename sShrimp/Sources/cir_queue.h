/**
 @file cir_queue.h
 @brief
 @author <b>Pham Huu Dang Nhat</b> <phamhuudangnhat@gmail.com><br> 
 @author <b>Nguyen Van Hien</b> <nvhien1992@gmail.com>
 @copyright Copyright (C) 2015 <b>SMART SENSSING AND INTELLIGENT CONTROL GROUP</b> , All rights reserved 
 */

#ifndef CIR_QUEUE_H_
#define CIR_QUEUE_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct {
	uint8_t* queue_p;
	uint16_t queue_size;
	int32_t head; /* next pos for new data to be pushed to the queue*/
	int32_t tail; /* next data to be pop from the queue */
	int32_t preview_pos;
	bool is_full;
} cir_queue_t;

/**
 * @brief Initialize circular queue. User must allocate data for the queue.
 *          Init private (head = 0, tail = -1)
 *          
 * @param[in] cir_queue
 * @param[in] queue_p Pointer to buffer for queue.
 * @param[in] queue_size Size of the buffer.
 */
void cir_queue_init(cir_queue_t *cir_queue, uint8_t *queue_p,
		uint16_t queue_size);

/**
 * @brief Add one byte to head of the circular queue.
 *
 * @param[in,out] cir_queue
 * @param[in] a_byte
 */
void cir_queue_add_byte(cir_queue_t *cir_queue, uint8_t a_byte);

/**
 * @brief Add a buffer of bytes to head of the circular queue.
 *
 * @param[in,out] cir_queue
 * @param[in] buf Pointer to data buffer.
 * @param[in] size Size of the data buffer.
 */
void cir_queue_add_data(cir_queue_t *cir_queue, uint8_t* buf, uint16_t size);

/**
 * @brief Preview one byte data from tail of the circular queue.
 *
 * @param[in] cir_queue
 * @param[in] cont True if continue from previous point, otherwise, begin from tail.
 *
 * @return Data of a byte (0 if queue is empty).
 */
uint8_t cir_queue_preview_byte(cir_queue_t *cir_queue, bool cont);

/**
 * @brief Preview a segment of data in circular queue.
 * 
 * @param[in] cir_queue
 * @param[out] buf
 * @param[in] size
 * 
 * @return The actual number of bytes has been preview (<= size).
 */
uint16_t cir_queue_preview_data(cir_queue_t *cir_queue, uint8_t *buf,
		uint16_t size);

/**
 * @brief Get one byte from tail of the circular queue.
 * 
 * @param[in] cir_queue
 *
 * @return Data of a byte (0 if queue is empty).
 */
uint8_t cir_queue_get_byte(cir_queue_t *cir_queue);

/**
 * @brief Get a buffer of bytes from tail of the circular queue.
 * 
 * @param[in] cir_queue
 * @param[out] buf Pointer to data buffer.
 * @param[in] size Size of the data buffer.
 *
 * @return The actual number of bytes has been read (<= size).
 */
uint16_t cir_queue_get_data(cir_queue_t *cir_queue, uint8_t* buf, uint16_t size);

/**
 * @brief Get size of data added into the circular queue.
 * 
 * @param[in] cir_queue
 *
 * @return Size of added data.
 */
uint16_t cir_queue_get_data_size(cir_queue_t *cir_queue);

/**
 * @brief Get available size of the circular queue.
 * 
 * @param[in] cir_queue
 *
 * @return Available size of the circular queue.
 */
uint16_t cir_queue_get_available_queue_size(cir_queue_t *cir_queue);

/**
 * @brief Check whether data added into circular queue makes queue fully.  
 * 
 * @param[in] cir_queue
 * @param[in] data_size
 * 
 * @return TRUE if the circular queue will be full. FALSE if not.
 */bool cir_queue_overflowed_if_adding(cir_queue_t *cir_queue, uint16_t data_size);

/**
 * @brief Clear the circular queue. 
 * 
 * @param[in] cir_queue
 */
void cir_queue_clear(cir_queue_t *cir_queue);

/**
 * @brief Get head of the circular queue.
 * 
 * @param[in] cir_queue
 *
 * @return Head of the circular queue.
 */
int32_t cir_queue_get_head(cir_queue_t *cir_queue);

/**
 * @brief Get tail of the circular queue.
 * 
 * @param[in] cir_queue
 *
 * @return Tail of the circular queue.
 */
int32_t cir_queue_get_tail(cir_queue_t *cir_queue);

/**
 * @brief Check if queue has been full or not.
 * 
 * @param[in] cir_queue
 *
 * @return TRUE if full. Otherwise, FALSE.
 */bool cir_queue_is_full(cir_queue_t *cir_queue);

#endif /* CIR_QUEUE_H_ */
