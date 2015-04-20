/*
 * cir_queue.h
 *
 *  Created on: Apr 20, 2015
 *      Author: nvhie_000
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
 * @brief   constructor, user must allocate data for the queue.
 *          Init private (head = 0, tail = -1)
 *
 * @param[in]   queue_p, pointer to buffer for queue.
 * @param[in]   queue_size, size of the buffer.
 *
 * @return  none
 */
void cir_queue_init(cir_queue_t *cir_queue, uint8_t *queue_p, uint16_t queue_size);

/**
 * @brief   add one byte to head of the circular queue.
 *
 * @param[in]  a_byte
 *
 * @return  none
 */
void cir_queue_add_byte(cir_queue_t *cir_queue, uint8_t a_byte);

/**
 * @brief   add a buffer of bytes to head of the circular queue.
 *
 * @param[in]  buf, a buffer of bytes.
 * @param[in]  size, size of the buffer.
 *
 * @return  none
 */
void cir_queue_add_data(cir_queue_t *cir_queue, uint8_t* buf, uint16_t size);

/**
 * @brief   preview one byte data from tail of the circular queue.
 *
 * @param[in] cont, true if continue from previous point, otherwise, begin from tail.
 *
 * @return  data of a byte (0 if queue is empty).
 */
uint8_t cir_queue_preview_data(cir_queue_t *cir_queue, bool cont);

/**
 * @brief   get one byte from tail of the circular queue.
 *
 * @return  data of a byte (0 if queue is empty).
 */
uint8_t cir_queue_get_byte(cir_queue_t *cir_queue);

/**
 * @brief   get a buffer of bytes from tail of the circular queue.
 *
 * @param[out]  buf, a buffer of bytes.
 * @param[in]  size, size of the buffer.
 *
 * @return  actual number of byte has been read (<= size).
 */
uint16_t cir_queue_get_data(cir_queue_t *cir_queue, uint8_t* buf, uint16_t size);

/**
 * @brief   get size of the circular queue.
 *
 * @return  size
 */
uint16_t cir_queue_get_data_size(cir_queue_t *cir_queue);

/**
 * @brief   get head of the circular queue.
 *
 * @return  head
 */
int32_t cir_queue_get_head(cir_queue_t *cir_queue);

/**
 * @brief   get tail of the circular queue.
 *
 * @return  tail
 */
int32_t cir_queue_get_tail(cir_queue_t *cir_queue);

/**
 * @brief   check if queue has been full or not.
 *
 * @return  true if full. Otherwise, false.
 */bool cir_queue_is_full(cir_queue_t *cir_queue);

#endif /* CIR_QUEUE_H_ */
