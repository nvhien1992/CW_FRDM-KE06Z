/*
 * sync_buffer.c
 *
 *  Created on: May 9, 2015
 *      Author: nvhie_000
 */
#include <cstring> 
#include "sync_buffer.h"

#define NOTIFY_EN (1)
#define DEBUG_EN (1)
#include "debug.h"

void sync_buffer_init(sync_buffer_t *sync_buff, uint8_t *buffer,
		uint16_t buffer_size) {
	sync_buff->resource = buffer;
	sync_buff->resource_size = buffer_size;
	if (_lwsem_create(&sync_buff->read_sem, 0) != MQX_OK) {
		DEBUG("ERR: Can't create read sem\n");
	}
	if (_lwsem_create(&sync_buff->write_sem, 1) != MQX_OK) {
		DEBUG("ERR: Can't create write sem\n");
	}
}

void sync_buffer_set_resource(sync_buffer_t *sync_buff, uint8_t *buffer,
		uint16_t buffer_size) {
	sync_buff->resource = buffer;
	sync_buff->resource_size = buffer_size;
}

uint16_t sync_buffer_get_resource_size(sync_buffer_t *sync_buff) {
	return sync_buff->resource_size;
}

uint8_t* sync_buffer_read(sync_buffer_t *sync_buff) {
	if (_lwsem_wait(&sync_buff->read_sem) != MQX_OK) {
		DEBUG("ERR: Waiting Read sem failed\n");
	}

	return sync_buff->resource;
}

uint8_t* sync_buffer_get_resource_to_write(sync_buffer_t *sync_buff) {
	if (_lwsem_wait(&sync_buff->write_sem) != MQX_OK) {
		DEBUG("ERR: Waiting Write sem failed\n");
	}

	return sync_buff->resource;
}

void sync_buffer_write(sync_buffer_t *sync_buff, uint8_t *in_buff,
		uint16_t size) {
	if (_lwsem_wait(&sync_buff->write_sem) != MQX_OK) {
		DEBUG("ERR: Waiting Write sem failed\n");
	}

	memcpy(sync_buff->resource, in_buff, size);
}

void sync_buffer_allow_read(sync_buffer_t *sync_buff) {
	_lwsem_post(&sync_buff->read_sem);
}

void sync_buffer_allow_write(sync_buffer_t *sync_buff) {
	_lwsem_post(&sync_buff->write_sem);
}
