/*
 * cir_queue.c
 *
 *  Created on: Apr 20, 2015
 *      Author: nvhie_000
 */
#include "cir_queue.h"

void cir_queue_init(cir_queue_t *cir_queue, uint8_t *queue_p,
		uint16_t queue_size) {
	cir_queue->queue_p = queue_p;
	cir_queue->queue_size = queue_size;

	cir_queue->head = 0;
	cir_queue->tail = -1;
	cir_queue->preview_pos = cir_queue->tail;

	cir_queue->is_full = false;
}

void cir_queue_add_byte(cir_queue_t *cir_queue, uint8_t a_byte) {
	if (cir_queue->is_full) {
		return;
	}

	cir_queue->queue_p[cir_queue->head] = a_byte;

	if (cir_queue->tail == -1) { /* queue is empty */
		cir_queue->tail = cir_queue->head;
	}

	cir_queue->head = (cir_queue->head + 1) % cir_queue->queue_size;

	/* check whether queue is full */
	if (cir_queue->head == cir_queue->tail) {
		cir_queue->is_full = true;
	}
}

void cir_queue_add_data(cir_queue_t *cir_queue, uint8_t* buf, uint16_t size) {
	/* check size */
	if (size < 1) {
		return;
	}

	if (cir_queue->is_full) {
		return;
	}

	/* copy data from buffer to queue */
	uint16_t count = 0;
	for (count = 0; count < size; count++) {
		cir_queue_add_byte(cir_queue, buf[count]);
	}
}

uint8_t cir_queue_preview_byte(cir_queue_t *cir_queue, bool cont) {
	if (cir_queue->tail == -1) {
		return 0;
	}

	if (cont == false) {
		cir_queue->preview_pos = cir_queue->tail;
	}

	uint8_t retval = cir_queue->queue_p[cir_queue->preview_pos];

	cir_queue->preview_pos = (cir_queue->preview_pos + 1)
			% cir_queue->queue_size;

	return retval;
}

uint16_t cir_queue_preview_data(cir_queue_t *cir_queue, uint8_t *buf,
		uint16_t size) {
	if (size < 1) {
		return 0;
	}

	uint16_t data_size = cir_queue_get_data_size(cir_queue);

	uint16_t count = 0;
	if (data_size > size) {
		for (count = 0; count < size; count++) {
			buf[count] = cir_queue_preview_byte(cir_queue, true);
		}

		return size;
	} else {
		for (count = 0; count < data_size; count++) {
			buf[count] = cir_queue_preview_byte(cir_queue, true);
		}

		return data_size;
	}
}

uint8_t cir_queue_get_byte(cir_queue_t *cir_queue) {
	if (cir_queue->tail == -1) {
		return 0;
	}

	uint8_t retdata = cir_queue->queue_p[cir_queue->tail];
	cir_queue->queue_p[cir_queue->tail] = 0;
	cir_queue->tail = (cir_queue->tail + 1) % cir_queue->queue_size;

	if (cir_queue->tail == cir_queue->head) {
		cir_queue->tail = -1;
	}

	cir_queue->is_full = false;

	return retdata;
}

uint16_t cir_queue_get_data(cir_queue_t *cir_queue, uint8_t* buf, uint16_t size) {
	/* check size */
	if (size < 1) {
		return 0;
	}

	uint16_t data_size = cir_queue_get_data_size(cir_queue);

	uint16_t count = 0;
	if (data_size > size) {
		for (count = 0; count < size; count++) {
			buf[count] = cir_queue_get_byte(cir_queue);
		}

		return size;
	} else {
		for (count = 0; count < data_size; count++) {
			buf[count] = cir_queue_get_byte(cir_queue);
		}

		return data_size;
	}
}

uint16_t cir_queue_get_data_size(cir_queue_t *cir_queue) {
	return (cir_queue->tail == -1) ?
			0 :
			(cir_queue->head > cir_queue->tail ?
					cir_queue->head - cir_queue->tail :
					cir_queue->head + cir_queue->queue_size - cir_queue->tail);
}

uint16_t cir_queue_get_available_queue_size(cir_queue_t *cir_queue) {
	return cir_queue->queue_size - cir_queue_get_data_size(cir_queue);
}

bool cir_queue_overflowed_if_adding(cir_queue_t *cir_queue, uint16_t data_size) {
	if (cir_queue->is_full) {
		return true;
	}

	if (cir_queue->tail == -1) {
		if (data_size > cir_queue->queue_size) {
			return true;
		}
		return false;
	}

	if (cir_queue_get_available_queue_size(cir_queue) < data_size) {
		return true;
	}

	return false;
}

void cir_queue_clear(cir_queue_t *cir_queue) {
	cir_queue->head = 0;
	cir_queue->tail = -1;
	cir_queue->preview_pos = cir_queue->tail;

	cir_queue->is_full = false;
}

int32_t cir_queue_get_head(cir_queue_t *cir_queue) {
	return cir_queue->head;
}

int32_t cir_queue_get_tail(cir_queue_t *cir_queue) {
	return cir_queue->tail;
}

bool cir_queue_is_full(cir_queue_t *cir_queue) {
	return cir_queue->is_full;
}
