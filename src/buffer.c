#include "buffer.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

cxBufferStatus cx_buffer_is_valid(const cxBuffer* buf) {
    if (!buf || !buf->data || buf->len > SIZE_MAX / sizeof(*buf->data)) {
        return CX_BUFFER_INVALID_BUFFER;
    }

    return CX_BUFFER_OK;
}

cxBufferStatus cx_buffer_print(const cxBuffer* buf) {
    if (!buf) return CX_BUFFER_ERR_NULL_POINTER;
    if (cx_buffer_is_valid(buf) != CX_BUFFER_OK) return CX_BUFFER_INVALID_BUFFER;

    printf("cxBuffer(len=%zu) [ ", buf->len);
    for (size_t index = 0; index < buf->len; index++) {
        printf("%g", buf->data[index]);
        if (index + 1 < buf->len) printf(", ");
    }
    printf(" ]\n");
    return CX_BUFFER_OK;
}

cxBufferStatus cx_buffer_allocate(cxBuffer** out, size_t len) {
    if (!out) return CX_BUFFER_ERR_NULL_POINTER;
    if (*out) return CX_BUFFER_ALREADY_ALLOCATED;
    if (len == 0 || len > SIZE_MAX / sizeof(float)) return CX_BUFFER_INVALID_LENGTH;

    cxBuffer* buf = malloc(sizeof(*buf));
    if (!buf) return CX_BUFFER_ALLOCATION_FAILED;

    buf->data = malloc(sizeof(*buf->data) * len);
    if (!buf->data) {
        free(buf);
        return CX_BUFFER_ALLOCATION_FAILED;
    }

    buf->len = len;
    *out = buf;
    return CX_BUFFER_OK;
}

cxBufferStatus cx_buffer_deallocate(cxBuffer** buf) {
    if (!buf || !*buf) return CX_BUFFER_ERR_NULL_POINTER;

    free((*buf)->data);
    free(*buf);
    *buf = NULL;
    return CX_BUFFER_OK;
}

cxBufferStatus cx_buffer_from_array(const float* src, cxBuffer** dst, size_t len) {
    if (!src || !dst) return CX_BUFFER_ERR_NULL_POINTER;
    if (*dst) return CX_BUFFER_ALREADY_ALLOCATED;
    if (len == 0 || len > SIZE_MAX / sizeof(float)) return CX_BUFFER_INVALID_LENGTH;

    cxBufferStatus status = cx_buffer_allocate(dst, len);
    if (status != CX_BUFFER_OK) return status;

    memcpy((*dst)->data, src, len * sizeof(float));
    return CX_BUFFER_OK;
}

cxBufferStatus cx_buffer_to_array(const cxBuffer* src, float** dst) {
    if (!src || !dst) return CX_BUFFER_ERR_NULL_POINTER;
    if (cx_buffer_is_valid(src) != CX_BUFFER_OK) return CX_BUFFER_INVALID_BUFFER;

    *dst = malloc(src->len * sizeof(float));
    if (!*dst) return CX_BUFFER_ALLOCATION_FAILED;

    memcpy(*dst, src->data, src->len * sizeof(float));
    return CX_BUFFER_OK;
}

cxBufferStatus cx_buffer_write(cxBuffer* buf, size_t index, float new_value) {
    if (!buf) return CX_BUFFER_ERR_NULL_POINTER;
    if (cx_buffer_is_valid(buf) != CX_BUFFER_OK) return CX_BUFFER_INVALID_BUFFER;
    if (index >= buf->len) return CX_BUFFER_OUT_OF_BOUNDS;

    buf->data[index] = new_value;
    return CX_BUFFER_OK;
}

cxBufferStatus cx_buffer_read(cxBuffer* buf, size_t index, float* value) {
    if (!buf || !value) return CX_BUFFER_ERR_NULL_POINTER;
    if (cx_buffer_is_valid(buf) != CX_BUFFER_OK) return CX_BUFFER_INVALID_BUFFER;
    if (index >= buf->len) return CX_BUFFER_OUT_OF_BOUNDS;

    *value = buf->data[index];
    return CX_BUFFER_OK;
}

cxBufferStatus cx_buffer_copy(const cxBuffer* src, cxBuffer* dst, size_t start_offset) {
    if (!src || !dst) return CX_BUFFER_ERR_NULL_POINTER;
    if (src == dst) return CX_BUFFER_INVALID_ARG;
    if (cx_buffer_is_valid(src) != CX_BUFFER_OK ||
        cx_buffer_is_valid(dst) != CX_BUFFER_OK) return CX_BUFFER_INVALID_BUFFER;
    if (start_offset >= src->len) return CX_BUFFER_INVALID_ARG;
    if (dst->len > src->len - start_offset) return CX_BUFFER_SIZE_MISMATCH;

    memmove(dst->data, src->data + start_offset, dst->len * sizeof(*dst->data));
    return CX_BUFFER_OK;
}

cxBufferStatus cx_buffer_clone(const cxBuffer* src, cxBuffer** dst) {
    if (!src || !dst) return CX_BUFFER_ERR_NULL_POINTER;
    if (cx_buffer_is_valid(src) != CX_BUFFER_OK) return CX_BUFFER_INVALID_BUFFER;
    if (*dst) return CX_BUFFER_ALREADY_ALLOCATED;

    cxBufferStatus status = cx_buffer_allocate(dst, src->len);
    if (status != CX_BUFFER_OK) return status;

    return cx_buffer_copy(src, *dst, 0);
}

cxBufferStatus cx_buffer_swap(cxBuffer** buf, cxBuffer** other) {
    if (!buf || !*buf || !other || !*other) return CX_BUFFER_ERR_NULL_POINTER;
    if (buf == other || *buf == *other) return CX_BUFFER_INVALID_ARG;
    if (cx_buffer_is_valid(*buf) != CX_BUFFER_OK ||
        cx_buffer_is_valid(*other) != CX_BUFFER_OK) return CX_BUFFER_INVALID_BUFFER;

    cxBuffer* temp = *buf;
    *buf = *other;
    *other = temp;
    return CX_BUFFER_OK;
}

cxBufferStatus cx_buffer_reverse(cxBuffer* buf) {
    if (!buf) return CX_BUFFER_ERR_NULL_POINTER;
    if (cx_buffer_is_valid(buf) != CX_BUFFER_OK) return CX_BUFFER_INVALID_BUFFER;

    for (size_t start = 0, end = buf->len; start < --end; start++) {
        float temp = buf->data[start];
        buf->data[start] = buf->data[end];
        buf->data[end] = temp;
    }
    return CX_BUFFER_OK;
}

cxBufferStatus cx_buffer_full(cxBuffer** buf, size_t len, float value) {
    if (!buf) return CX_BUFFER_ERR_NULL_POINTER;

    cxBufferStatus status = cx_buffer_allocate(buf, len);
    if (status != CX_BUFFER_OK) return status;

    for (size_t index = 0; index < (*buf)->len; index++) {
        (*buf)->data[index] = value;
    }
    return CX_BUFFER_OK;
}

cxBufferStatus cx_buffer_fill(cxBuffer* buf, float value) {
    if (!buf) return CX_BUFFER_ERR_NULL_POINTER;
    if (cx_buffer_is_valid(buf) != CX_BUFFER_OK) return CX_BUFFER_INVALID_BUFFER;

    for (size_t index = 0; index < buf->len; index++) {
        buf->data[index] = value;
    }
    return CX_BUFFER_OK;
}

cxBufferStatus cx_buffer_resize(cxBuffer* buf, size_t new_len) {
    if (!buf) return CX_BUFFER_ERR_NULL_POINTER;
    if (cx_buffer_is_valid(buf) != CX_BUFFER_OK) return CX_BUFFER_INVALID_BUFFER;
    if (new_len == 0) { return CX_BUFFER_INVALID_LENGTH; }
    if (new_len > SIZE_MAX / sizeof(*buf->data)) return CX_BUFFER_INVALID_LENGTH;

    float* new_data = (float*)realloc(buf->data, new_len * sizeof(*buf->data));
    if (!new_data) return CX_BUFFER_ALLOCATION_FAILED;

    buf->data = new_data;
    buf->len = new_len;
    return CX_BUFFER_OK;
}

cxBufferStatus cx_buffer_concat(const cxBuffer* buf1, const cxBuffer* buf2, cxBuffer** dst) {
    if (!buf1 || !buf2 || !dst) return CX_BUFFER_ERR_NULL_POINTER;
    if (cx_buffer_is_valid(buf1) != CX_BUFFER_OK || cx_buffer_is_valid(buf2) != CX_BUFFER_OK) return CX_BUFFER_INVALID_BUFFER;
    if (*dst) return CX_BUFFER_ALREADY_ALLOCATED;
    if (buf1->len > SIZE_MAX - buf2->len) return CX_BUFFER_INVALID_LENGTH;

    cxBufferStatus status = cx_buffer_allocate(dst, buf1->len + buf2->len);
    if (status != CX_BUFFER_OK) return status;

    for (size_t i = 0; i < buf1->len; i++) {
        (*dst)->data[i] = buf1->data[i];
    }
    for (size_t i = 0; i < buf2->len; i++) {
        (*dst)->data[buf1->len + i] = buf2->data[i];
    }
    return CX_BUFFER_OK;
}

cxBufferStatus cx_buffer_append(cxBuffer* buf, const cxBuffer* other) {
    if (!buf || !other) return CX_BUFFER_ERR_NULL_POINTER;
    if (buf == other) return CX_BUFFER_INVALID_ARG;
    if (cx_buffer_is_valid(buf) != CX_BUFFER_OK || cx_buffer_is_valid(other) != CX_BUFFER_OK) return CX_BUFFER_INVALID_BUFFER;
    if (buf->len > SIZE_MAX - other->len) return CX_BUFFER_INVALID_LENGTH;

    cxBufferStatus status = cx_buffer_resize(buf, buf->len + other->len);
    if (status != CX_BUFFER_OK) return status;

    for (size_t i = 0; i < other->len; i++) {
        buf->data[buf->len - other->len + i] = other->data[i];
    }
    return CX_BUFFER_OK;
}

cxBufferStatus cx_buffer_insert(cxBuffer* buf, size_t index, const cxBuffer* other) {
    if (!buf || !other) return CX_BUFFER_ERR_NULL_POINTER;
    if (buf == other) return CX_BUFFER_INVALID_ARG;
    if (cx_buffer_is_valid(buf) != CX_BUFFER_OK || cx_buffer_is_valid(other) != CX_BUFFER_OK) return CX_BUFFER_INVALID_BUFFER;
    if (index > buf->len) return CX_BUFFER_OUT_OF_BOUNDS;
    if (buf->len > SIZE_MAX - other->len) return CX_BUFFER_INVALID_LENGTH;

    cxBufferStatus status = cx_buffer_resize(buf, buf->len + other->len);
    if (status != CX_BUFFER_OK) return status;

    for (size_t i = buf->len - other->len; i > index; i--) {
        buf->data[i + other->len - 1] = buf->data[i - 1];
    }
    for (size_t i = 0; i < other->len; i++) {
        buf->data[index + i] = other->data[i];
    }
    return CX_BUFFER_OK;
}

cxBufferStatus cx_buffer_remove(cxBuffer* buf, size_t index, size_t count) {
    if (!buf) return CX_BUFFER_ERR_NULL_POINTER;
    if (cx_buffer_is_valid(buf) != CX_BUFFER_OK) return CX_BUFFER_INVALID_BUFFER;
    if (index >= buf->len) return CX_BUFFER_OUT_OF_BOUNDS;
    if (count == 0 || count > buf->len - index) return CX_BUFFER_INVALID_LENGTH;
    if (count == buf->len) return CX_BUFFER_INVALID_LENGTH;

    for (size_t i = index; i < buf->len - count; i++) {
        buf->data[i] = buf->data[i + count];
    }
    return cx_buffer_resize(buf, buf->len - count);
}