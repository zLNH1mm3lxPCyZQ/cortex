#pragma once

#include <stddef.h>

typedef enum {
    CX_BUFFER_OK,
    CX_BUFFER_ERR_NULL_POINTER,
    CX_BUFFER_ALLOCATION_FAILED,
    CX_BUFFER_OUT_OF_BOUNDS,
    CX_BUFFER_INVALID_ARG,
    CX_BUFFER_INVALID_LENGTH,
    CX_BUFFER_ALREADY_ALLOCATED,
    CX_BUFFER_INVALID_BUFFER,
    CX_BUFFER_SIZE_MISMATCH
} cxBufferStatus;

typedef struct {
    float* data;
    size_t len;
} cxBuffer;

cxBufferStatus cx_buffer_is_valid(const cxBuffer* buf);
cxBufferStatus cx_buffer_allocate(cxBuffer** out, size_t len);
cxBufferStatus cx_buffer_deallocate(cxBuffer** buf);
cxBufferStatus cx_buffer_write(cxBuffer* buf, size_t index, float new_value);
cxBufferStatus cx_buffer_read(cxBuffer* buf, size_t index, float* value);
cxBufferStatus cx_buffer_copy(const cxBuffer* src, cxBuffer* dst, size_t start_offset);
cxBufferStatus cx_buffer_full(cxBuffer** buf, size_t len, float value);
cxBufferStatus cx_buffer_swap(cxBuffer** buf, cxBuffer** other);
cxBufferStatus cx_buffer_reverse(cxBuffer* buf);
cxBufferStatus cx_buffer_print(const cxBuffer* buf);