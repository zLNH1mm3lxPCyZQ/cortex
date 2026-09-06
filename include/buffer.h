#pragma once

#include <stdbool.h>
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
    CX_BUFFER_SIZE_MISMATCH,
    CX_BUFFER_NOT_FOUND,
    CX_BUFFER_IO_ERROR
} cxBufferStatus;

typedef struct {
    float* data;
    size_t len;
} cxBuffer;

cxBufferStatus cx_buffer_is_valid(const cxBuffer* buf);
cxBufferStatus cx_buffer_print(const cxBuffer* buf);
cxBufferStatus cx_buffer_allocate(cxBuffer** out, size_t len);
cxBufferStatus cx_buffer_deallocate(cxBuffer** buf);
cxBufferStatus cx_buffer_from_array(const float* src, cxBuffer** dst, size_t len);
cxBufferStatus cx_buffer_to_array(const cxBuffer* src, float** dst);
cxBufferStatus cx_buffer_full(cxBuffer** buf, size_t len, float value);
cxBufferStatus cx_buffer_write(cxBuffer* buf, size_t index, float new_value);
cxBufferStatus cx_buffer_fill(cxBuffer* buf, float value);
cxBufferStatus cx_buffer_read(const cxBuffer* buf, size_t index, float* value);
cxBufferStatus cx_buffer_copy(const cxBuffer* src, cxBuffer* dst, size_t start_offset);
cxBufferStatus cx_buffer_clone(const cxBuffer* src, cxBuffer** dst);
cxBufferStatus cx_buffer_swap(cxBuffer** buf, cxBuffer** other);
cxBufferStatus cx_buffer_reverse(cxBuffer* buf);
cxBufferStatus cx_buffer_resize(cxBuffer* buf, size_t new_len);
cxBufferStatus cx_buffer_concat(const cxBuffer* buf1, const cxBuffer* buf2, cxBuffer** dst);
cxBufferStatus cx_buffer_append(cxBuffer* buf, const cxBuffer* other);
cxBufferStatus cx_buffer_insert(cxBuffer* buf, size_t index, const cxBuffer* other);
cxBufferStatus cx_buffer_remove(cxBuffer* buf, size_t index, size_t count);
cxBufferStatus cx_buffer_find(const cxBuffer* buf, float value, size_t* index);
cxBufferStatus cx_buffer_contains(const cxBuffer* buf, float value, bool* found);
cxBufferStatus cx_buffer_equal(const cxBuffer* a, const cxBuffer* b, bool* equal);
cxBufferStatus cx_buffer_slice(const cxBuffer* src, size_t start, size_t len, cxBuffer** dst);
cxBufferStatus cx_buffer_gather(const cxBuffer* src, const size_t* indices, size_t count, cxBuffer** dst);

typedef void (*cx_buffer_visitor)(size_t index, float value, void* ctx);
cxBufferStatus cx_buffer_for_each(const cxBuffer* buf, cx_buffer_visitor visitor, void* ctx);

typedef float (*cx_buffer_mapper)(float value, void* ctx);
cxBufferStatus cx_buffer_map(const cxBuffer* src, cx_buffer_mapper mapper, void* ctx, cxBuffer** dst);

typedef float (*cx_buffer_reducer)(float accumulator, float value, void* ctx);
cxBufferStatus cx_buffer_reduce(const cxBuffer* buf, cx_buffer_reducer reducer, float initial, void* ctx, float* result);

typedef bool (*cx_buffer_predicate)(float value, void* ctx);
cxBufferStatus cx_buffer_filter(const cxBuffer* src, cx_buffer_predicate pred, void* ctx, cxBuffer** dst);
cxBufferStatus cx_buffer_find_if(const cxBuffer* buf, cx_buffer_predicate pred, void* ctx, size_t* index);
cxBufferStatus cx_buffer_count_if(const cxBuffer* buf, cx_buffer_predicate pred, void* ctx, size_t* count);
cxBufferStatus cx_buffer_any(const cxBuffer* buf, cx_buffer_predicate pred, void* ctx, bool* result);
cxBufferStatus cx_buffer_all(const cxBuffer* buf, cx_buffer_predicate pred, void* ctx, bool* result);

cxBufferStatus cx_buffer_save(const cxBuffer* buf, const char* path);
cxBufferStatus cx_buffer_load(cxBuffer** buf, const char* path);
cxBufferStatus cx_buffer_save_txt(const cxBuffer* buf, const char* path);
cxBufferStatus cx_buffer_load_txt(cxBuffer** buf, const char* path);
cxBufferStatus cx_buffer_length(const cxBuffer* buf, size_t* length);
