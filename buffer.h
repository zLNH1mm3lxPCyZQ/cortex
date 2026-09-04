#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef enum {
    CX_BUFFER_OK,
    CX_BUFFER_ERR_NULL_POINTER,
    CX_BUFFER_ALLOCATION_FAILED,
    CX_BUFFER_OUT_OF_BOUNDS,
    CX_BUFFER_INVALID_ARG,
    CX_BUFFER_INVALID_BUFFER,
    CX_BUFFER_SIZE_MISMATCH
} cxBufferStatus;

typedef struct {
    float* data;
    size_t len;
} cxBuffer;

static inline int cx_buffer_is_valid(const cxBuffer* buf) {
    return buf && buf->len <= SIZE_MAX / sizeof(*buf->data) &&
           (buf->len == 0 || buf->data);
}

static inline cxBufferStatus cx_buffer_allocate(cxBuffer** out, size_t len) {
    if (!out) return CX_BUFFER_ERR_NULL_POINTER;
    if (len == 0) return CX_BUFFER_INVALID_ARG;
    if (len > SIZE_MAX / sizeof(float)) return CX_BUFFER_INVALID_ARG;

    cxBuffer* buf = malloc(sizeof(cxBuffer));
    if (!buf) return CX_BUFFER_ALLOCATION_FAILED;

    buf->data = malloc(sizeof(float) * len);
    if (!buf->data) {
        free(buf);
        return CX_BUFFER_ALLOCATION_FAILED;
    }

    buf->len = len;
    *out = buf;
    return CX_BUFFER_OK;
}

static inline cxBufferStatus cx_buffer_deallocate(cxBuffer** buf) {
    if (!buf || !*buf) return CX_BUFFER_ERR_NULL_POINTER;
    free((*buf)->data);
    free(*buf);
    *buf = NULL;
    return CX_BUFFER_OK;
}

static inline cxBufferStatus cx_buffer_write(cxBuffer* buf, size_t index, float new_value) {
    if (!buf) return CX_BUFFER_ERR_NULL_POINTER;
    if (!cx_buffer_is_valid(buf)) return CX_BUFFER_INVALID_BUFFER;
    if (index >= buf->len) return CX_BUFFER_OUT_OF_BOUNDS;
    buf->data[index] = new_value;
    return CX_BUFFER_OK;
}

static inline cxBufferStatus cx_buffer_read(cxBuffer* buf, size_t index, float* value) {
    if (!buf) return CX_BUFFER_ERR_NULL_POINTER;
    if (!value) return CX_BUFFER_ERR_NULL_POINTER;
    if (!cx_buffer_is_valid(buf)) return CX_BUFFER_INVALID_BUFFER;
    if (index >= buf->len) return CX_BUFFER_OUT_OF_BOUNDS;
    *value = buf->data[index];
    return CX_BUFFER_OK;
}

static inline cxBufferStatus cx_buffer_copy(const cxBuffer* src, cxBuffer* dst, size_t start_offset) {
    if (!src || !dst) return CX_BUFFER_ERR_NULL_POINTER;
    if (!cx_buffer_is_valid(src) || !cx_buffer_is_valid(dst)) return CX_BUFFER_INVALID_BUFFER;
    if (start_offset > src->len) return CX_BUFFER_INVALID_ARG;
    if (dst->len > src->len - start_offset) return CX_BUFFER_SIZE_MISMATCH;

    memmove(dst->data, src->data + start_offset, dst->len * sizeof(*dst->data));
    return CX_BUFFER_OK;
}

static inline cxBufferStatus cx_buffer_full(cxBuffer** buf, size_t len, float value) {
    if (!buf) return CX_BUFFER_ERR_NULL_POINTER;
    cxBufferStatus status = cx_buffer_allocate(buf, len);
    if (status) return status;
    for (size_t i = 0; i < (*buf)->len; i++) {
        (*buf)->data[i] = value;
    }
    return CX_BUFFER_OK;
}

static inline cxBufferStatus cx_buffer_swap(cxBuffer** buf, cxBuffer** other) {
    if (!buf || !*buf) return CX_BUFFER_ERR_NULL_POINTER;
    if (!other || !*other) return CX_BUFFER_ERR_NULL_POINTER;
    if (!cx_buffer_is_valid(*buf) || !cx_buffer_is_valid(*other)) return CX_BUFFER_INVALID_BUFFER;
    cxBuffer* temp = *buf;
    *buf = *other;
    *other = temp;
    return CX_BUFFER_OK;
}

static inline cxBufferStatus cx_buffer_reverse(cxBuffer* buf) {
    if (!buf) return CX_BUFFER_ERR_NULL_POINTER;
    if (!cx_buffer_is_valid(buf)) return CX_BUFFER_INVALID_BUFFER;
    if (buf->len == 0) return CX_BUFFER_OK;
    size_t start = 0;
    size_t end = buf->len - 1;
    while (start < end) {
        float temp = buf->data[start];
        buf->data[start] = buf->data[end];
        buf->data[end] = temp;
        start++;
        end--;
    }
    return CX_BUFFER_OK;
}

static inline cxBufferStatus cx_buffer_print(const cxBuffer* buf) {
    if (!buf) return CX_BUFFER_ERR_NULL_POINTER;
    if (!cx_buffer_is_valid(buf)) return CX_BUFFER_INVALID_BUFFER;
    
    printf("cxBuffer(len=%zu) [ ", buf->len);
    for (size_t i = 0; i < buf->len; i++) {
        printf("%g", buf->data[i]);
        if (i != buf->len - 1) printf(", ");
    }
    printf(" ]\n");
    return CX_BUFFER_OK;
}