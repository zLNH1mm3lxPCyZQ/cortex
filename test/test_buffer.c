#include "test.h"
#include "buffer.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// a null pointer should always be considered as invalid
TEST(buffer_is_valid_null) {
    ASSERT_EQ(cx_buffer_is_valid(NULL), CX_BUFFER_INVALID_BUFFER);
}

// valid buffer common use
TEST(buffer_is_valid_valid_buffer) {
    cxBuffer buf;
    float data[10];
    buf.data = data;
    buf.len = 10;
    ASSERT_EQ(cx_buffer_is_valid(&buf), CX_BUFFER_OK);
}

// an empty buffer is considered invalid even if len is zero
TEST(buffer_is_valid_zero_len_with_null_data) {
    cxBuffer buf;
    buf.data = NULL;
    buf.len = 0;
    ASSERT_EQ(cx_buffer_is_valid(&buf), CX_BUFFER_INVALID_BUFFER);
}

// an empty buffer with non zero len is considered invalid
TEST(buffer_is_valid_non_zero_len_with_null_data) {
    cxBuffer buf;
    buf.data = NULL;
    buf.len = 10;
    ASSERT_EQ(cx_buffer_is_valid(&buf), CX_BUFFER_INVALID_BUFFER);
}

// buffer allocation common use
TEST(buffer_allocation) {
    cxBuffer* buf = NULL;
    cxBufferStatus status = cx_buffer_allocate(&buf, 1000);
    ASSERT_NOT_NULL(buf);
    ASSERT_EQ(status, CX_BUFFER_OK);
    ASSERT_EQ(cx_buffer_deallocate(&buf), CX_BUFFER_OK);
}

// cannot allocate a buffer that is already allocated
TEST(buffer_allocation_already_allocated) {
    cxBuffer* buf = NULL;
    cxBufferStatus status = cx_buffer_allocate(&buf, 1000);
    ASSERT_NOT_NULL(buf);
    ASSERT_EQ(status, CX_BUFFER_OK);

    status = cx_buffer_allocate(&buf, 100);
    ASSERT_EQ(status, CX_BUFFER_ALREADY_ALLOCATED);
    ASSERT_EQ(cx_buffer_deallocate(&buf), CX_BUFFER_OK);
}

// cannot allocate a zero length buffer
TEST(buffer_allocation_zero_length) {
    cxBuffer* buf = NULL;
    cxBufferStatus status = cx_buffer_allocate(&buf, 0);
    ASSERT_NULL(buf);
    ASSERT_EQ(status, CX_BUFFER_INVALID_LENGTH);
}

// buffer deallocation common use
TEST(buffer_deallocation) {
    cxBuffer* buf = NULL;
    cxBufferStatus status = cx_buffer_allocate(&buf, 10);
    ASSERT_NOT_NULL(buf);
    ASSERT_EQ(status, CX_BUFFER_OK);
    status = cx_buffer_deallocate(&buf);
    ASSERT_NULL(buf);
    ASSERT_EQ(status, CX_BUFFER_OK);
}

// cannot deallocate a buffer pointer thats already null
TEST(buffer_deallocation_null_pointer) {
    cxBuffer* buf = NULL;
    cxBufferStatus status = cx_buffer_deallocate(&buf);
    ASSERT_EQ(status, CX_BUFFER_ERR_NULL_POINTER);
}

// buffer from array common use
TEST(buffer_from_array) {
    float src[10];
    for (size_t i = 0; i < 10; i++) {
        src[i] = (float)i;
    }

    cxBuffer* buf = NULL;
    cxBufferStatus status = cx_buffer_from_array(src, &buf, 10);
    ASSERT_EQ(status, CX_BUFFER_OK);
    ASSERT_NOT_NULL(buf);
    for (size_t i = 0; i < 10; i++) {
        ASSERT_FLOAT_EQ(buf->data[i], (float)i, 1e-5);
    }
    ASSERT_EQ(cx_buffer_deallocate(&buf), CX_BUFFER_OK);
}

// cannot create a buffer from a null array
TEST(buffer_from_array_null_pointer) {
    cxBuffer* buf = NULL;
    cxBufferStatus status = cx_buffer_from_array(NULL, &buf, 10);
    ASSERT_EQ(status, CX_BUFFER_ERR_NULL_POINTER);
    ASSERT_NULL(buf);
}

// cannot create a buffer from a null destination pointer
TEST(buffer_from_array_null_destination) {
    float src[10];
    for (size_t i = 0; i < 10; i++) {
        src[i] = (float)i;
    }

    cxBufferStatus status = cx_buffer_from_array(src, NULL, 10);
    ASSERT_EQ(status, CX_BUFFER_ERR_NULL_POINTER);
}

// buffer to array common use
TEST(buffer_to_array) {
    cxBuffer* buf = NULL;
    cxBufferStatus status = cx_buffer_allocate(&buf, 10);
    ASSERT_NOT_NULL(buf);
    ASSERT_EQ(status, CX_BUFFER_OK);

    for (size_t i = 0; i < 10; i++) {
        buf->data[i] = (float)i;
    }

    float* arr = NULL;
    status = cx_buffer_to_array(buf, &arr);
    ASSERT_EQ(status, CX_BUFFER_OK);
    ASSERT_NOT_NULL(arr);
    for (size_t i = 0; i < 10; i++) {
        ASSERT_FLOAT_EQ(arr[i], (float)i, 1e-5);
    }
    free(arr);
    ASSERT_EQ(cx_buffer_deallocate(&buf), CX_BUFFER_OK);
}

// cannot convert a null buffer to an array
TEST(buffer_to_array_null_pointer) {
    float* arr = NULL;
    cxBufferStatus status = cx_buffer_to_array(NULL, &arr);
    ASSERT_EQ(status, CX_BUFFER_ERR_NULL_POINTER);
}

// cannot convert a buffer to an array with a null destination pointer
TEST(buffer_to_array_null_destination) {
    cxBuffer* buf = NULL;
    cxBufferStatus status = cx_buffer_allocate(&buf, 10);
    ASSERT_NOT_NULL(buf);
    ASSERT_EQ(status, CX_BUFFER_OK);

    status = cx_buffer_to_array(buf, NULL);
    ASSERT_EQ(status, CX_BUFFER_ERR_NULL_POINTER);

    ASSERT_EQ(cx_buffer_deallocate(&buf), CX_BUFFER_OK);
}

// buffer write common use
TEST(buffer_write) {
    cxBuffer* buf = NULL;
    cxBufferStatus status = cx_buffer_allocate(&buf, 10);
    ASSERT_NOT_NULL(buf);
    ASSERT_EQ(status, CX_BUFFER_OK);

    status = cx_buffer_write(buf, 0, 10.0);
    ASSERT_EQ(status, CX_BUFFER_OK);
    status = cx_buffer_write(buf, 9, 10.0);
    ASSERT_EQ(status, CX_BUFFER_OK);
    
    ASSERT_EQ(buf->data[0], 10.0);
    ASSERT_EQ(buf->data[9], 10.0);

    ASSERT_EQ(cx_buffer_deallocate(&buf), CX_BUFFER_OK);
}

// cannot write into a null buffer pointer
TEST(buffer_write_null_pointer) {
    cxBuffer* buf = NULL;

    cxBufferStatus status = cx_buffer_write(buf, 10, 10.0);
    ASSERT_EQ(status, CX_BUFFER_ERR_NULL_POINTER);
}

// cannot write out of bounds on a buffer
TEST(buffer_write_out_of_bounds) {
    cxBuffer* buf = NULL;
    cxBufferStatus status = cx_buffer_allocate(&buf, 10);
    ASSERT_NOT_NULL(buf);
    ASSERT_EQ(status, CX_BUFFER_OK);

    status = cx_buffer_write(buf, 10, 10.0);
    ASSERT_EQ(status, CX_BUFFER_OUT_OF_BOUNDS);

    status = cx_buffer_write(buf, 1999, 10.0);
    ASSERT_EQ(status, CX_BUFFER_OUT_OF_BOUNDS);
    
    ASSERT_EQ(cx_buffer_deallocate(&buf), CX_BUFFER_OK);
}

// buffer read common use
TEST(buffer_read) {
    cxBuffer* buf = NULL;
    cxBufferStatus status = cx_buffer_allocate(&buf, 10);
    ASSERT_NOT_NULL(buf);
    ASSERT_EQ(status, CX_BUFFER_OK);

    buf->data[0] = 9.6;
    buf->data[4] = 3.0;
    buf->data[9] = -8.2;

    float res;
    status = cx_buffer_read(buf, 0, &res);
    ASSERT_EQ(status, CX_BUFFER_OK);
    ASSERT_FLOAT_EQ(res, 9.6, 1e-5);
    status = cx_buffer_read(buf, 4, &res);
    ASSERT_EQ(status, CX_BUFFER_OK);
    ASSERT_FLOAT_EQ(res, 3.0, 1e-5);
    status = cx_buffer_read(buf, 9, &res);
    ASSERT_EQ(status, CX_BUFFER_OK);
    ASSERT_FLOAT_EQ(res, -8.2, 1e-5);

    ASSERT_EQ(cx_buffer_deallocate(&buf), CX_BUFFER_OK);
}

// cannot read from a null buffer pointer
TEST(buffer_read_null_pointer) {
    cxBuffer* buf = NULL;
    float res;
    cxBufferStatus status = cx_buffer_read(buf, 0, &res);
    ASSERT_EQ(status, CX_BUFFER_ERR_NULL_POINTER);
}

// cannot read out of bounds on a buffer
TEST(buffer_read_out_of_bounds) {
    cxBuffer* buf = NULL;
    cxBufferStatus status = cx_buffer_allocate(&buf, 10);
    ASSERT_NOT_NULL(buf);
    ASSERT_EQ(status, CX_BUFFER_OK);

    float res;
    status = cx_buffer_read(buf, 10, &res);
    ASSERT_EQ(status, CX_BUFFER_OUT_OF_BOUNDS);

    status = cx_buffer_read(buf, 1999, &res);
    ASSERT_EQ(status, CX_BUFFER_OUT_OF_BOUNDS);

    ASSERT_EQ(cx_buffer_deallocate(&buf), CX_BUFFER_OK);
}

// copy buffer contents from source to destination starting at offset 0
TEST(buffer_copy) {
    cxBuffer* src = NULL;
    cxBufferStatus status = cx_buffer_allocate(&src, 10);
    ASSERT_NOT_NULL(src);
    ASSERT_EQ(status, CX_BUFFER_OK);

    for (int i = 0; i < 10; i++) {
        src->data[i] = (float)i;
    }

    cxBuffer* dest = NULL;
    status = cx_buffer_allocate(&dest, 10);
    ASSERT_NOT_NULL(dest);
    ASSERT_EQ(status, CX_BUFFER_OK);

    status = cx_buffer_copy(src, dest, 0);
    ASSERT_EQ(status, CX_BUFFER_OK);

    for (int i = 0; i < 10; i++) {
        ASSERT_FLOAT_EQ(dest->data[i], (float)i, 1e-5);
    }

    ASSERT_EQ(cx_buffer_deallocate(&src), CX_BUFFER_OK);
    ASSERT_EQ(cx_buffer_deallocate(&dest), CX_BUFFER_OK);
}

// copy buffer contents from source to destination starting at a non-zero offset
TEST(buffer_copy_with_offset) {
    cxBuffer* src = NULL;
    cxBufferStatus status = cx_buffer_allocate(&src, 10);
    ASSERT_NOT_NULL(src);
    ASSERT_EQ(status, CX_BUFFER_OK);

    for (int i = 0; i < 10; i++) {
        src->data[i] = (float)i;
    }

    cxBuffer* dest = NULL;
    status = cx_buffer_allocate(&dest, 6);
    ASSERT_NOT_NULL(dest);
    ASSERT_EQ(status, CX_BUFFER_OK);

    status = cx_buffer_copy(src, dest, 3);
    ASSERT_EQ(status, CX_BUFFER_OK);

    for (int i = 0; i < 6; i++) {
        ASSERT_FLOAT_EQ(dest->data[i], (float)(i+3), 1e-5);
    }

    ASSERT_EQ(cx_buffer_deallocate(&src), CX_BUFFER_OK);
    ASSERT_EQ(cx_buffer_deallocate(&dest), CX_BUFFER_OK);
}

// copy buffer contents from source to destination starting at the last valid offset
TEST(buffer_copy_with_limit_offset) {
    cxBuffer* src = NULL;
    cxBufferStatus status = cx_buffer_allocate(&src, 10);
    ASSERT_NOT_NULL(src);
    ASSERT_EQ(status, CX_BUFFER_OK);

    for (int i = 0; i < 10; i++) {
        src->data[i] = (float)i;
    }

    cxBuffer* dest = NULL;
    status = cx_buffer_allocate(&dest, 1);
    ASSERT_NOT_NULL(dest);
    ASSERT_EQ(status, CX_BUFFER_OK);

    status = cx_buffer_copy(src, dest, src->len-1);
    ASSERT_EQ(status, CX_BUFFER_OK);

    for (int i = 0; i < 1; i++) {
        ASSERT_FLOAT_EQ(dest->data[i], 9.0, 1e-5);
    }

    ASSERT_EQ(cx_buffer_deallocate(&src), CX_BUFFER_OK);
    ASSERT_EQ(cx_buffer_deallocate(&dest), CX_BUFFER_OK);
}

// copy buffer contents from source to destination starting at an offset that causes out-of-bounds error
TEST(buffer_copy_with_offset_out_of_bounds) {
    cxBuffer* src = NULL;
    cxBufferStatus status = cx_buffer_allocate(&src, 10);
    ASSERT_NOT_NULL(src);
    ASSERT_EQ(status, CX_BUFFER_OK);

    for (int i = 0; i < 10; i++) {
        src->data[i] = (float)i;
    }

    cxBuffer* dest = NULL;
    status = cx_buffer_allocate(&dest, 10);
    ASSERT_NOT_NULL(dest);
    ASSERT_EQ(status, CX_BUFFER_OK);

    status = cx_buffer_copy(src, dest, 3);
    ASSERT_EQ(status, CX_BUFFER_SIZE_MISMATCH);

    ASSERT_EQ(cx_buffer_deallocate(&src), CX_BUFFER_OK);
    ASSERT_EQ(cx_buffer_deallocate(&dest), CX_BUFFER_OK);
}

// copy buffer contents from source to destination starting at an invalid offset
TEST(buffer_copy_with_invalid_offset) {
    cxBuffer* src = NULL;
    cxBufferStatus status = cx_buffer_allocate(&src, 10);
    ASSERT_NOT_NULL(src);
    ASSERT_EQ(status, CX_BUFFER_OK);

    for (int i = 0; i < 10; i++) {
        src->data[i] = (float)i;
    }

    cxBuffer* dest = NULL;
    status = cx_buffer_allocate(&dest, 10);
    ASSERT_NOT_NULL(dest);
    ASSERT_EQ(status, CX_BUFFER_OK);

    status = cx_buffer_copy(src, dest, 10);
    ASSERT_EQ(status, CX_BUFFER_INVALID_ARG);
    status = cx_buffer_copy(src, dest, 11);
    ASSERT_EQ(status, CX_BUFFER_INVALID_ARG);

    ASSERT_EQ(cx_buffer_deallocate(&src), CX_BUFFER_OK);
    ASSERT_EQ(cx_buffer_deallocate(&dest), CX_BUFFER_OK);
}

// copy buffer contents from source to itself should return invalid argument
TEST(buffer_copy_with_self) {
    cxBuffer* buf = NULL;
    cxBufferStatus status = cx_buffer_allocate(&buf, 5);
    ASSERT_NOT_NULL(buf);
    ASSERT_EQ(status, CX_BUFFER_OK);

    for (int i = 0; i < 5; i++) {
        buf->data[i] = (float)i;
    }

    status = cx_buffer_copy(buf, buf, 0);
    ASSERT_EQ(status, CX_BUFFER_INVALID_ARG);

    for (int i = 0; i < 5; i++) {
        ASSERT_FLOAT_EQ(buf->data[i], (float)i, 1e-5);
    }

    ASSERT_EQ(cx_buffer_deallocate(&buf), CX_BUFFER_OK);
}

// clone a buffer into a new buffer
TEST(buffer_clone) {
    cxBuffer* src = NULL;
    cxBufferStatus status = cx_buffer_allocate(&src, 5);
    ASSERT_NOT_NULL(src);
    ASSERT_EQ(status, CX_BUFFER_OK);

    for (int i = 0; i < 5; i++) {
        src->data[i] = (float)i;
    }

    cxBuffer* dst = NULL;
    status = cx_buffer_clone(src, &dst);
    ASSERT_EQ(status, CX_BUFFER_OK);
    ASSERT_NOT_NULL(dst);

    for (int i = 0; i < 5; i++) {
        ASSERT_FLOAT_EQ(dst->data[i], (float)i, 1e-5);
    }

    ASSERT_EQ(cx_buffer_deallocate(&src), CX_BUFFER_OK);
    ASSERT_EQ(cx_buffer_deallocate(&dst), CX_BUFFER_OK);
}

// clone a buffer into an already allocated destination buffer should return invalid argument
TEST(buffer_clone_into_allocated) {
    cxBuffer* src = NULL;
    cxBufferStatus status = cx_buffer_allocate(&src, 5);
    ASSERT_NOT_NULL(src);
    ASSERT_EQ(status, CX_BUFFER_OK);

    for (int i = 0; i < 5; i++) {
        src->data[i] = (float)i;
    }

    cxBuffer* dst = NULL;
    status = cx_buffer_allocate(&dst, 5);
    ASSERT_NOT_NULL(dst);
    ASSERT_EQ(status, CX_BUFFER_OK);

    status = cx_buffer_clone(src, &dst);
    ASSERT_EQ(status, CX_BUFFER_ALREADY_ALLOCATED);

    ASSERT_EQ(cx_buffer_deallocate(&src), CX_BUFFER_OK);
    ASSERT_EQ(cx_buffer_deallocate(&dst), CX_BUFFER_OK);
}

// clone a buffer with a null source should return error
TEST(buffer_clone_with_null_source) {
    cxBuffer* dst = NULL;
    cxBufferStatus status = cx_buffer_clone(NULL, &dst);
    ASSERT_EQ(status, CX_BUFFER_ERR_NULL_POINTER);
    ASSERT_NULL(dst);
}

// clone a buffer with a null destination pointer should return error
TEST(buffer_clone_with_null_destination) {
    cxBuffer* src = NULL;
    cxBufferStatus status = cx_buffer_allocate(&src, 5);
    ASSERT_NOT_NULL(src);
    ASSERT_EQ(status, CX_BUFFER_OK);

    status = cx_buffer_clone(src, NULL);
    ASSERT_EQ(status, CX_BUFFER_ERR_NULL_POINTER);

    ASSERT_EQ(cx_buffer_deallocate(&src), CX_BUFFER_OK);
}

// swap the contents of two buffers
TEST(buffer_swap) {
    cxBuffer* buf1 = NULL;
    cxBuffer* buf2 = NULL;
    cxBufferStatus status = cx_buffer_allocate(&buf1, 5);
    ASSERT_NOT_NULL(buf1);
    ASSERT_EQ(status, CX_BUFFER_OK);

    status = cx_buffer_allocate(&buf2, 5);
    ASSERT_NOT_NULL(buf2);
    ASSERT_EQ(status, CX_BUFFER_OK);

    for (int i = 0; i < 5; i++) {
        buf1->data[i] = (float)i;
        buf2->data[i] = (float)(i + 10);
    }

    status = cx_buffer_swap(&buf1, &buf2);
    ASSERT_EQ(status, CX_BUFFER_OK);

    for (int i = 0; i < 5; i++) {
        ASSERT_FLOAT_EQ(buf1->data[i], (float)(i + 10), 1e-5);
        ASSERT_FLOAT_EQ(buf2->data[i], (float)i, 1e-5);
    }

    ASSERT_EQ(cx_buffer_deallocate(&buf1), CX_BUFFER_OK);
    ASSERT_EQ(cx_buffer_deallocate(&buf2), CX_BUFFER_OK);
}

// swap buffers when one or both pointers are null should return error
TEST(buffer_swap_with_null_pointers) {
    cxBuffer* buf1 = NULL;
    cxBuffer* buf2 = NULL;

    cxBufferStatus status = cx_buffer_swap(&buf1, &buf2);
    ASSERT_EQ(status, CX_BUFFER_ERR_NULL_POINTER);

    status = cx_buffer_allocate(&buf1, 5);
    ASSERT_NOT_NULL(buf1);
    ASSERT_EQ(status, CX_BUFFER_OK);

    status = cx_buffer_swap(&buf1, &buf2);
    ASSERT_EQ(status, CX_BUFFER_ERR_NULL_POINTER);

    ASSERT_EQ(cx_buffer_deallocate(&buf1), CX_BUFFER_OK);

    status = cx_buffer_allocate(&buf2, 5);
    ASSERT_NOT_NULL(buf2);
    ASSERT_EQ(status, CX_BUFFER_OK);

    status = cx_buffer_swap(&buf1, &buf2);
    ASSERT_EQ(status, CX_BUFFER_ERR_NULL_POINTER);

    ASSERT_EQ(cx_buffer_deallocate(&buf2), CX_BUFFER_OK);
}

// swap buffers of different sizes should still succeed
TEST(buffer_swap_with_different_sizes) {
    cxBuffer* buf1 = NULL;
    cxBuffer* buf2 = NULL;
    cxBufferStatus status = cx_buffer_allocate(&buf1, 5);
    ASSERT_NOT_NULL(buf1);
    ASSERT_EQ(status, CX_BUFFER_OK);

    status = cx_buffer_allocate(&buf2, 10);
    ASSERT_NOT_NULL(buf2);
    ASSERT_EQ(status, CX_BUFFER_OK);

    status = cx_buffer_swap(&buf1, &buf2);
    ASSERT_EQ(status, CX_BUFFER_OK);

    ASSERT_EQ(cx_buffer_deallocate(&buf1), CX_BUFFER_OK);
    ASSERT_EQ(cx_buffer_deallocate(&buf2), CX_BUFFER_OK);
}

// swap a buffer with itself should return invalid argument
TEST(buffer_swap_with_self) {
    cxBuffer* buf = NULL;
    cxBufferStatus status = cx_buffer_allocate(&buf, 5);
    ASSERT_NOT_NULL(buf);
    ASSERT_EQ(status, CX_BUFFER_OK);

    status = cx_buffer_swap(&buf, &buf);
    ASSERT_EQ(status, CX_BUFFER_INVALID_ARG);
    ASSERT_NOT_NULL(buf);

    ASSERT_EQ(cx_buffer_deallocate(&buf), CX_BUFFER_OK);
}

// reverse the contents of a buffer
TEST(buffer_reverse) {
    cxBuffer* buf = NULL;
    cxBufferStatus status = cx_buffer_allocate(&buf, 5);
    ASSERT_NOT_NULL(buf);
    ASSERT_EQ(status, CX_BUFFER_OK);

    for (int i = 0; i < 5; i++) {
        buf->data[i] = (float)i;
    }

    status = cx_buffer_reverse(buf);
    ASSERT_EQ(status, CX_BUFFER_OK);

    for (int i = 0; i < 5; i++) {
        ASSERT_FLOAT_EQ(buf->data[i], (float)(4 - i), 1e-5);
    }

    ASSERT_EQ(cx_buffer_deallocate(&buf), CX_BUFFER_OK);
}

// reverse a buffer when the pointer is null should return error
TEST(buffer_reverse_with_null_pointer) {
    cxBuffer* buf = NULL;
    cxBufferStatus status = cx_buffer_reverse(buf);
    ASSERT_EQ(status, CX_BUFFER_ERR_NULL_POINTER);
}

// allocate a buffer with a specific value
TEST(buffer_full) {
    cxBuffer* buf = NULL;
    cxBufferStatus status = cx_buffer_full(&buf, 5, 3.14f);
    ASSERT_NOT_NULL(buf);
    ASSERT_EQ(status, CX_BUFFER_OK);

    for (int i = 0; i < 5; i++) {
        ASSERT_FLOAT_EQ(buf->data[i], 3.14f, 1e-5);
    }

    ASSERT_EQ(cx_buffer_deallocate(&buf), CX_BUFFER_OK);
}

// allocate a buffer with a specific value when the pointer is null should return error
TEST(buffer_full_with_null_pointer) {
    cxBufferStatus status = cx_buffer_full(NULL, 5, 3.14f);
    ASSERT_EQ(status, CX_BUFFER_ERR_NULL_POINTER);
}

// fill an existing buffer with a specific value
TEST(buffer_fill) {
    cxBuffer* buf = NULL;
    cxBufferStatus status = cx_buffer_allocate(&buf, 5);
    ASSERT_NOT_NULL(buf);
    ASSERT_EQ(status, CX_BUFFER_OK);

    status = cx_buffer_fill(buf, 3.14f);
    ASSERT_EQ(status, CX_BUFFER_OK);

    for (int i = 0; i < 5; i++) {
        ASSERT_FLOAT_EQ(buf->data[i], 3.14f, 1e-5);
    }

    ASSERT_EQ(cx_buffer_deallocate(&buf), CX_BUFFER_OK);
}

TEST(buffer_fill_with_null_pointer) {
    cxBufferStatus status = cx_buffer_fill(NULL, 3.14f);
    ASSERT_EQ(status, CX_BUFFER_ERR_NULL_POINTER);
}

// grow a buffer, preserving existing contents
TEST(buffer_resize_grow) {
    cxBuffer* buf = NULL;
    cxBufferStatus status = cx_buffer_allocate(&buf, 5);
    ASSERT_NOT_NULL(buf);
    ASSERT_EQ(status, CX_BUFFER_OK);

    for (int i = 0; i < 5; i++) {
        buf->data[i] = (float)i;
    }

    status = cx_buffer_resize(buf, 10);
    ASSERT_EQ(status, CX_BUFFER_OK);
    ASSERT_EQ(buf->len, (size_t)10);

    for (int i = 0; i < 5; i++) {
        ASSERT_FLOAT_EQ(buf->data[i], (float)i, 1e-5);
    }

    ASSERT_EQ(cx_buffer_deallocate(&buf), CX_BUFFER_OK);
}

// shrink a buffer, preserving the remaining contents
TEST(buffer_resize_shrink) {
    cxBuffer* buf = NULL;
    cxBufferStatus status = cx_buffer_allocate(&buf, 5);
    ASSERT_NOT_NULL(buf);
    ASSERT_EQ(status, CX_BUFFER_OK);

    for (int i = 0; i < 5; i++) {
        buf->data[i] = (float)i;
    }

    status = cx_buffer_resize(buf, 2);
    ASSERT_EQ(status, CX_BUFFER_OK);
    ASSERT_EQ(buf->len, (size_t)2);

    for (int i = 0; i < 2; i++) {
        ASSERT_FLOAT_EQ(buf->data[i], (float)i, 1e-5);
    }

    ASSERT_EQ(cx_buffer_deallocate(&buf), CX_BUFFER_OK);
}

// cannot resize a null buffer pointer
TEST(buffer_resize_null_pointer) {
    cxBufferStatus status = cx_buffer_resize(NULL, 5);
    ASSERT_EQ(status, CX_BUFFER_ERR_NULL_POINTER);
}

// cannot resize a buffer to zero length
TEST(buffer_resize_zero_length) {
    cxBuffer* buf = NULL;
    cxBufferStatus status = cx_buffer_allocate(&buf, 5);
    ASSERT_NOT_NULL(buf);
    ASSERT_EQ(status, CX_BUFFER_OK);

    status = cx_buffer_resize(buf, 0);
    ASSERT_EQ(status, CX_BUFFER_INVALID_LENGTH);
    ASSERT_EQ(buf->len, (size_t)5);

    ASSERT_EQ(cx_buffer_deallocate(&buf), CX_BUFFER_OK);
}

// cannot resize a buffer past SIZE_MAX/sizeof(float) elements
TEST(buffer_resize_length_overflow) {
    cxBuffer* buf = NULL;
    cxBufferStatus status = cx_buffer_allocate(&buf, 5);
    ASSERT_NOT_NULL(buf);
    ASSERT_EQ(status, CX_BUFFER_OK);

    status = cx_buffer_resize(buf, SIZE_MAX);
    ASSERT_EQ(status, CX_BUFFER_INVALID_LENGTH);
    ASSERT_EQ(buf->len, (size_t)5);

    ASSERT_EQ(cx_buffer_deallocate(&buf), CX_BUFFER_OK);
}

// concatenate two buffers into a freshly allocated destination
TEST(buffer_concat) {
    cxBuffer* buf1 = NULL;
    cxBuffer* buf2 = NULL;
    cxBufferStatus status = cx_buffer_allocate(&buf1, 3);
    ASSERT_NOT_NULL(buf1);
    ASSERT_EQ(status, CX_BUFFER_OK);
    status = cx_buffer_allocate(&buf2, 2);
    ASSERT_NOT_NULL(buf2);
    ASSERT_EQ(status, CX_BUFFER_OK);

    for (int i = 0; i < 3; i++) buf1->data[i] = (float)i;
    for (int i = 0; i < 2; i++) buf2->data[i] = (float)(i + 10);

    cxBuffer* dst = NULL;
    status = cx_buffer_concat(buf1, buf2, &dst);
    ASSERT_EQ(status, CX_BUFFER_OK);
    ASSERT_NOT_NULL(dst);
    ASSERT_EQ(dst->len, (size_t)5);

    ASSERT_FLOAT_EQ(dst->data[0], 0.0f, 1e-5);
    ASSERT_FLOAT_EQ(dst->data[1], 1.0f, 1e-5);
    ASSERT_FLOAT_EQ(dst->data[2], 2.0f, 1e-5);
    ASSERT_FLOAT_EQ(dst->data[3], 10.0f, 1e-5);
    ASSERT_FLOAT_EQ(dst->data[4], 11.0f, 1e-5);

    ASSERT_EQ(cx_buffer_deallocate(&buf1), CX_BUFFER_OK);
    ASSERT_EQ(cx_buffer_deallocate(&buf2), CX_BUFFER_OK);
    ASSERT_EQ(cx_buffer_deallocate(&dst), CX_BUFFER_OK);
}

// cannot concat with a null source or destination pointer
TEST(buffer_concat_null_pointer) {
    cxBuffer* buf1 = NULL;
    cxBuffer* dst = NULL;
    cxBufferStatus status = cx_buffer_allocate(&buf1, 3);
    ASSERT_NOT_NULL(buf1);
    ASSERT_EQ(status, CX_BUFFER_OK);

    status = cx_buffer_concat(NULL, buf1, &dst);
    ASSERT_EQ(status, CX_BUFFER_ERR_NULL_POINTER);
    status = cx_buffer_concat(buf1, NULL, &dst);
    ASSERT_EQ(status, CX_BUFFER_ERR_NULL_POINTER);
    status = cx_buffer_concat(buf1, buf1, NULL);
    ASSERT_EQ(status, CX_BUFFER_ERR_NULL_POINTER);

    ASSERT_EQ(cx_buffer_deallocate(&buf1), CX_BUFFER_OK);
}

// cannot concat into a destination pointer that is already allocated
TEST(buffer_concat_already_allocated) {
    cxBuffer* buf1 = NULL;
    cxBuffer* buf2 = NULL;
    cxBuffer* dst = NULL;
    ASSERT_EQ(cx_buffer_allocate(&buf1, 3), CX_BUFFER_OK);
    ASSERT_EQ(cx_buffer_allocate(&buf2, 2), CX_BUFFER_OK);
    ASSERT_EQ(cx_buffer_allocate(&dst, 1), CX_BUFFER_OK);

    cxBufferStatus status = cx_buffer_concat(buf1, buf2, &dst);
    ASSERT_EQ(status, CX_BUFFER_ALREADY_ALLOCATED);

    ASSERT_EQ(cx_buffer_deallocate(&buf1), CX_BUFFER_OK);
    ASSERT_EQ(cx_buffer_deallocate(&buf2), CX_BUFFER_OK);
    ASSERT_EQ(cx_buffer_deallocate(&dst), CX_BUFFER_OK);
}

// a buffer with len == SIZE_MAX is rejected by is_valid before lengths are summed
TEST(buffer_concat_length_overflow) {
    cxBuffer buf1;
    cxBuffer buf2;
    float data = 0.0f;
    buf1.data = &data;
    buf1.len = SIZE_MAX;
    buf2.data = &data;
    buf2.len = 1;

    cxBuffer* dst = NULL;
    cxBufferStatus status = cx_buffer_concat(&buf1, &buf2, &dst);
    ASSERT_EQ(status, CX_BUFFER_INVALID_BUFFER);
    ASSERT_NULL(dst);
}

// append the contents of one buffer onto another
TEST(buffer_append) {
    cxBuffer* buf = NULL;
    cxBuffer* other = NULL;
    ASSERT_EQ(cx_buffer_allocate(&buf, 3), CX_BUFFER_OK);
    ASSERT_EQ(cx_buffer_allocate(&other, 2), CX_BUFFER_OK);

    for (int i = 0; i < 3; i++) buf->data[i] = (float)i;
    for (int i = 0; i < 2; i++) other->data[i] = (float)(i + 10);

    cxBufferStatus status = cx_buffer_append(buf, other);
    ASSERT_EQ(status, CX_BUFFER_OK);
    ASSERT_EQ(buf->len, (size_t)5);

    ASSERT_FLOAT_EQ(buf->data[0], 0.0f, 1e-5);
    ASSERT_FLOAT_EQ(buf->data[1], 1.0f, 1e-5);
    ASSERT_FLOAT_EQ(buf->data[2], 2.0f, 1e-5);
    ASSERT_FLOAT_EQ(buf->data[3], 10.0f, 1e-5);
    ASSERT_FLOAT_EQ(buf->data[4], 11.0f, 1e-5);

    ASSERT_EQ(cx_buffer_deallocate(&buf), CX_BUFFER_OK);
    ASSERT_EQ(cx_buffer_deallocate(&other), CX_BUFFER_OK);
}

// cannot append with a null buffer or source pointer
TEST(buffer_append_null_pointer) {
    cxBuffer* buf = NULL;
    ASSERT_EQ(cx_buffer_allocate(&buf, 3), CX_BUFFER_OK);

    cxBufferStatus status = cx_buffer_append(NULL, buf);
    ASSERT_EQ(status, CX_BUFFER_ERR_NULL_POINTER);
    status = cx_buffer_append(buf, NULL);
    ASSERT_EQ(status, CX_BUFFER_ERR_NULL_POINTER);

    ASSERT_EQ(cx_buffer_deallocate(&buf), CX_BUFFER_OK);
}

// appending a buffer to itself is rejected since resizing may relocate its data
TEST(buffer_append_with_self) {
    cxBuffer* buf = NULL;
    ASSERT_EQ(cx_buffer_allocate(&buf, 3), CX_BUFFER_OK);
    for (int i = 0; i < 3; i++) buf->data[i] = (float)i;

    cxBufferStatus status = cx_buffer_append(buf, buf);
    ASSERT_EQ(status, CX_BUFFER_INVALID_ARG);
    ASSERT_EQ(buf->len, (size_t)3);

    ASSERT_EQ(cx_buffer_deallocate(&buf), CX_BUFFER_OK);
}

// a buffer with len == SIZE_MAX is rejected by is_valid before lengths are summed
TEST(buffer_append_length_overflow) {
    cxBuffer* buf = NULL;
    ASSERT_EQ(cx_buffer_allocate(&buf, 3), CX_BUFFER_OK);

    cxBuffer other;
    float data = 0.0f;
    other.data = &data;
    other.len = SIZE_MAX;

    cxBufferStatus status = cx_buffer_append(buf, &other);
    ASSERT_EQ(status, CX_BUFFER_INVALID_BUFFER);
    ASSERT_EQ(buf->len, (size_t)3);

    ASSERT_EQ(cx_buffer_deallocate(&buf), CX_BUFFER_OK);
}

// insert the contents of one buffer into the middle of another
TEST(buffer_insert) {
    cxBuffer* buf = NULL;
    cxBuffer* other = NULL;
    ASSERT_EQ(cx_buffer_allocate(&buf, 4), CX_BUFFER_OK);
    ASSERT_EQ(cx_buffer_allocate(&other, 2), CX_BUFFER_OK);

    for (int i = 0; i < 4; i++) buf->data[i] = (float)i;
    other->data[0] = 100.0f;
    other->data[1] = 101.0f;

    cxBufferStatus status = cx_buffer_insert(buf, 2, other);
    ASSERT_EQ(status, CX_BUFFER_OK);
    ASSERT_EQ(buf->len, (size_t)6);

    float expected[6] = { 0.0f, 1.0f, 100.0f, 101.0f, 2.0f, 3.0f };
    for (int i = 0; i < 6; i++) {
        ASSERT_FLOAT_EQ(buf->data[i], expected[i], 1e-5);
    }

    ASSERT_EQ(cx_buffer_deallocate(&buf), CX_BUFFER_OK);
    ASSERT_EQ(cx_buffer_deallocate(&other), CX_BUFFER_OK);
}

// insert at the very start of the buffer
TEST(buffer_insert_at_start) {
    cxBuffer* buf = NULL;
    cxBuffer* other = NULL;
    ASSERT_EQ(cx_buffer_allocate(&buf, 3), CX_BUFFER_OK);
    ASSERT_EQ(cx_buffer_allocate(&other, 1), CX_BUFFER_OK);

    for (int i = 0; i < 3; i++) buf->data[i] = (float)i;
    other->data[0] = 100.0f;

    cxBufferStatus status = cx_buffer_insert(buf, 0, other);
    ASSERT_EQ(status, CX_BUFFER_OK);

    float expected[4] = { 100.0f, 0.0f, 1.0f, 2.0f };
    for (int i = 0; i < 4; i++) {
        ASSERT_FLOAT_EQ(buf->data[i], expected[i], 1e-5);
    }

    ASSERT_EQ(cx_buffer_deallocate(&buf), CX_BUFFER_OK);
    ASSERT_EQ(cx_buffer_deallocate(&other), CX_BUFFER_OK);
}

// insert at the very end of the buffer
TEST(buffer_insert_at_end) {
    cxBuffer* buf = NULL;
    cxBuffer* other = NULL;
    ASSERT_EQ(cx_buffer_allocate(&buf, 3), CX_BUFFER_OK);
    ASSERT_EQ(cx_buffer_allocate(&other, 1), CX_BUFFER_OK);

    for (int i = 0; i < 3; i++) buf->data[i] = (float)i;
    other->data[0] = 100.0f;

    cxBufferStatus status = cx_buffer_insert(buf, 3, other);
    ASSERT_EQ(status, CX_BUFFER_OK);

    float expected[4] = { 0.0f, 1.0f, 2.0f, 100.0f };
    for (int i = 0; i < 4; i++) {
        ASSERT_FLOAT_EQ(buf->data[i], expected[i], 1e-5);
    }

    ASSERT_EQ(cx_buffer_deallocate(&buf), CX_BUFFER_OK);
    ASSERT_EQ(cx_buffer_deallocate(&other), CX_BUFFER_OK);
}

// cannot insert with a null buffer or source pointer
TEST(buffer_insert_null_pointer) {
    cxBuffer* buf = NULL;
    ASSERT_EQ(cx_buffer_allocate(&buf, 3), CX_BUFFER_OK);

    cxBufferStatus status = cx_buffer_insert(NULL, 0, buf);
    ASSERT_EQ(status, CX_BUFFER_ERR_NULL_POINTER);
    status = cx_buffer_insert(buf, 0, NULL);
    ASSERT_EQ(status, CX_BUFFER_ERR_NULL_POINTER);

    ASSERT_EQ(cx_buffer_deallocate(&buf), CX_BUFFER_OK);
}

// cannot insert at an index past the end of the buffer
TEST(buffer_insert_out_of_bounds) {
    cxBuffer* buf = NULL;
    cxBuffer* other = NULL;
    ASSERT_EQ(cx_buffer_allocate(&buf, 3), CX_BUFFER_OK);
    ASSERT_EQ(cx_buffer_allocate(&other, 1), CX_BUFFER_OK);

    cxBufferStatus status = cx_buffer_insert(buf, 4, other);
    ASSERT_EQ(status, CX_BUFFER_OUT_OF_BOUNDS);
    ASSERT_EQ(buf->len, (size_t)3);

    ASSERT_EQ(cx_buffer_deallocate(&buf), CX_BUFFER_OK);
    ASSERT_EQ(cx_buffer_deallocate(&other), CX_BUFFER_OK);
}

// inserting a buffer into itself is rejected since resizing may relocate its data
TEST(buffer_insert_with_self) {
    cxBuffer* buf = NULL;
    ASSERT_EQ(cx_buffer_allocate(&buf, 3), CX_BUFFER_OK);
    for (int i = 0; i < 3; i++) buf->data[i] = (float)i;

    cxBufferStatus status = cx_buffer_insert(buf, 1, buf);
    ASSERT_EQ(status, CX_BUFFER_INVALID_ARG);
    ASSERT_EQ(buf->len, (size_t)3);

    ASSERT_EQ(cx_buffer_deallocate(&buf), CX_BUFFER_OK);
}

// a buffer with len == SIZE_MAX is rejected by is_valid before lengths are summed
TEST(buffer_insert_length_overflow) {
    cxBuffer* buf = NULL;
    ASSERT_EQ(cx_buffer_allocate(&buf, 3), CX_BUFFER_OK);

    cxBuffer other;
    float data = 0.0f;
    other.data = &data;
    other.len = SIZE_MAX;

    cxBufferStatus status = cx_buffer_insert(buf, 0, &other);
    ASSERT_EQ(status, CX_BUFFER_INVALID_BUFFER);
    ASSERT_EQ(buf->len, (size_t)3);

    ASSERT_EQ(cx_buffer_deallocate(&buf), CX_BUFFER_OK);
}

// remove a range from the middle of a buffer
TEST(buffer_remove) {
    cxBuffer* buf = NULL;
    ASSERT_EQ(cx_buffer_allocate(&buf, 5), CX_BUFFER_OK);
    for (int i = 0; i < 5; i++) buf->data[i] = (float)i;

    cxBufferStatus status = cx_buffer_remove(buf, 1, 2);
    ASSERT_EQ(status, CX_BUFFER_OK);
    ASSERT_EQ(buf->len, (size_t)3);

    float expected[3] = { 0.0f, 3.0f, 4.0f };
    for (int i = 0; i < 3; i++) {
        ASSERT_FLOAT_EQ(buf->data[i], expected[i], 1e-5);
    }

    ASSERT_EQ(cx_buffer_deallocate(&buf), CX_BUFFER_OK);
}

// remove a single element from a buffer
TEST(buffer_remove_single) {
    cxBuffer* buf = NULL;
    ASSERT_EQ(cx_buffer_allocate(&buf, 3), CX_BUFFER_OK);
    for (int i = 0; i < 3; i++) buf->data[i] = (float)i;

    cxBufferStatus status = cx_buffer_remove(buf, 1, 1);
    ASSERT_EQ(status, CX_BUFFER_OK);
    ASSERT_EQ(buf->len, (size_t)2);

    ASSERT_FLOAT_EQ(buf->data[0], 0.0f, 1e-5);
    ASSERT_FLOAT_EQ(buf->data[1], 2.0f, 1e-5);

    ASSERT_EQ(cx_buffer_deallocate(&buf), CX_BUFFER_OK);
}

// remove up to the last element of a buffer
TEST(buffer_remove_to_end) {
    cxBuffer* buf = NULL;
    ASSERT_EQ(cx_buffer_allocate(&buf, 5), CX_BUFFER_OK);
    for (int i = 0; i < 5; i++) buf->data[i] = (float)i;

    cxBufferStatus status = cx_buffer_remove(buf, 3, 2);
    ASSERT_EQ(status, CX_BUFFER_OK);
    ASSERT_EQ(buf->len, (size_t)3);

    for (int i = 0; i < 3; i++) {
        ASSERT_FLOAT_EQ(buf->data[i], (float)i, 1e-5);
    }

    ASSERT_EQ(cx_buffer_deallocate(&buf), CX_BUFFER_OK);
}

// cannot remove from a null buffer pointer
TEST(buffer_remove_null_pointer) {
    cxBufferStatus status = cx_buffer_remove(NULL, 0, 1);
    ASSERT_EQ(status, CX_BUFFER_ERR_NULL_POINTER);
}

// cannot remove starting at an index past the end of the buffer
TEST(buffer_remove_out_of_bounds) {
    cxBuffer* buf = NULL;
    ASSERT_EQ(cx_buffer_allocate(&buf, 3), CX_BUFFER_OK);

    cxBufferStatus status = cx_buffer_remove(buf, 3, 1);
    ASSERT_EQ(status, CX_BUFFER_OUT_OF_BOUNDS);
    status = cx_buffer_remove(buf, 10, 1);
    ASSERT_EQ(status, CX_BUFFER_OUT_OF_BOUNDS);
    ASSERT_EQ(buf->len, (size_t)3);

    ASSERT_EQ(cx_buffer_deallocate(&buf), CX_BUFFER_OK);
}

// cannot remove a zero-length range
TEST(buffer_remove_zero_count) {
    cxBuffer* buf = NULL;
    ASSERT_EQ(cx_buffer_allocate(&buf, 3), CX_BUFFER_OK);

    cxBufferStatus status = cx_buffer_remove(buf, 0, 0);
    ASSERT_EQ(status, CX_BUFFER_INVALID_LENGTH);
    ASSERT_EQ(buf->len, (size_t)3);

    ASSERT_EQ(cx_buffer_deallocate(&buf), CX_BUFFER_OK);
}

// cannot remove a range that extends past the end of the buffer
TEST(buffer_remove_count_out_of_bounds) {
    cxBuffer* buf = NULL;
    ASSERT_EQ(cx_buffer_allocate(&buf, 5), CX_BUFFER_OK);

    cxBufferStatus status = cx_buffer_remove(buf, 3, 3);
    ASSERT_EQ(status, CX_BUFFER_INVALID_LENGTH);
    ASSERT_EQ(buf->len, (size_t)5);

    ASSERT_EQ(cx_buffer_deallocate(&buf), CX_BUFFER_OK);
}

// remove refuses a count that would wrap size_t when added to index
TEST(buffer_remove_count_wraparound) {
    cxBuffer* buf = NULL;
    ASSERT_EQ(cx_buffer_allocate(&buf, 5), CX_BUFFER_OK);

    cxBufferStatus status = cx_buffer_remove(buf, 2, SIZE_MAX - 1);
    ASSERT_EQ(status, CX_BUFFER_INVALID_LENGTH);
    ASSERT_EQ(buf->len, (size_t)5);

    ASSERT_EQ(cx_buffer_deallocate(&buf), CX_BUFFER_OK);
}

// cannot remove every element via remove; use deallocate instead
TEST(buffer_remove_entire_buffer) {
    cxBuffer* buf = NULL;
    ASSERT_EQ(cx_buffer_allocate(&buf, 3), CX_BUFFER_OK);

    cxBufferStatus status = cx_buffer_remove(buf, 0, 3);
    ASSERT_EQ(status, CX_BUFFER_INVALID_LENGTH);
    ASSERT_EQ(buf->len, (size_t)3);

    ASSERT_EQ(cx_buffer_deallocate(&buf), CX_BUFFER_OK);
}

// find the index of a value present in the buffer
TEST(buffer_find) {
    cxBuffer* buf = NULL;
    ASSERT_EQ(cx_buffer_allocate(&buf, 5), CX_BUFFER_OK);
    for (int i = 0; i < 5; i++) buf->data[i] = (float)i;

    size_t index = 0;
    cxBufferStatus status = cx_buffer_find(buf, 3.0f, &index);
    ASSERT_EQ(status, CX_BUFFER_OK);
    ASSERT_EQ(index, (size_t)3);

    ASSERT_EQ(cx_buffer_deallocate(&buf), CX_BUFFER_OK);
}

// searching for a value that is not present returns not found
TEST(buffer_find_not_found) {
    cxBuffer* buf = NULL;
    ASSERT_EQ(cx_buffer_allocate(&buf, 5), CX_BUFFER_OK);
    for (int i = 0; i < 5; i++) buf->data[i] = (float)i;

    size_t index = 0;
    cxBufferStatus status = cx_buffer_find(buf, 99.0f, &index);
    ASSERT_EQ(status, CX_BUFFER_NOT_FOUND);

    ASSERT_EQ(cx_buffer_deallocate(&buf), CX_BUFFER_OK);
}

// cannot find with a null buffer or output pointer
TEST(buffer_find_null_pointer) {
    cxBuffer* buf = NULL;
    ASSERT_EQ(cx_buffer_allocate(&buf, 5), CX_BUFFER_OK);

    size_t index = 0;
    cxBufferStatus status = cx_buffer_find(NULL, 1.0f, &index);
    ASSERT_EQ(status, CX_BUFFER_ERR_NULL_POINTER);
    status = cx_buffer_find(buf, 1.0f, NULL);
    ASSERT_EQ(status, CX_BUFFER_ERR_NULL_POINTER);

    ASSERT_EQ(cx_buffer_deallocate(&buf), CX_BUFFER_OK);
}

// contains reports true when the value is present
TEST(buffer_contains_true) {
    cxBuffer* buf = NULL;
    ASSERT_EQ(cx_buffer_allocate(&buf, 5), CX_BUFFER_OK);
    for (int i = 0; i < 5; i++) buf->data[i] = (float)i;

    bool found = false;
    cxBufferStatus status = cx_buffer_contains(buf, 4.0f, &found);
    ASSERT_EQ(status, CX_BUFFER_OK);
    ASSERT_TRUE(found);

    ASSERT_EQ(cx_buffer_deallocate(&buf), CX_BUFFER_OK);
}

// contains reports false when the value is absent
TEST(buffer_contains_false) {
    cxBuffer* buf = NULL;
    ASSERT_EQ(cx_buffer_allocate(&buf, 5), CX_BUFFER_OK);
    for (int i = 0; i < 5; i++) buf->data[i] = (float)i;

    bool found = true;
    cxBufferStatus status = cx_buffer_contains(buf, 99.0f, &found);
    ASSERT_EQ(status, CX_BUFFER_OK);
    ASSERT_FALSE(found);

    ASSERT_EQ(cx_buffer_deallocate(&buf), CX_BUFFER_OK);
}

// cannot check contains with a null buffer or output pointer
TEST(buffer_contains_null_pointer) {
    cxBuffer* buf = NULL;
    ASSERT_EQ(cx_buffer_allocate(&buf, 5), CX_BUFFER_OK);

    bool found = false;
    cxBufferStatus status = cx_buffer_contains(NULL, 1.0f, &found);
    ASSERT_EQ(status, CX_BUFFER_ERR_NULL_POINTER);
    status = cx_buffer_contains(buf, 1.0f, NULL);
    ASSERT_EQ(status, CX_BUFFER_ERR_NULL_POINTER);

    ASSERT_EQ(cx_buffer_deallocate(&buf), CX_BUFFER_OK);
}

// two buffers with identical contents are equal
TEST(buffer_equal_true) {
    cxBuffer* a = NULL;
    cxBuffer* b = NULL;
    ASSERT_EQ(cx_buffer_allocate(&a, 3), CX_BUFFER_OK);
    ASSERT_EQ(cx_buffer_allocate(&b, 3), CX_BUFFER_OK);
    for (int i = 0; i < 3; i++) { a->data[i] = (float)i; b->data[i] = (float)i; }

    bool equal = false;
    cxBufferStatus status = cx_buffer_equal(a, b, &equal);
    ASSERT_EQ(status, CX_BUFFER_OK);
    ASSERT_TRUE(equal);

    ASSERT_EQ(cx_buffer_deallocate(&a), CX_BUFFER_OK);
    ASSERT_EQ(cx_buffer_deallocate(&b), CX_BUFFER_OK);
}

// buffers with different lengths are not equal
TEST(buffer_equal_different_lengths) {
    cxBuffer* a = NULL;
    cxBuffer* b = NULL;
    ASSERT_EQ(cx_buffer_allocate(&a, 3), CX_BUFFER_OK);
    ASSERT_EQ(cx_buffer_allocate(&b, 4), CX_BUFFER_OK);

    bool equal = true;
    cxBufferStatus status = cx_buffer_equal(a, b, &equal);
    ASSERT_EQ(status, CX_BUFFER_OK);
    ASSERT_FALSE(equal);

    ASSERT_EQ(cx_buffer_deallocate(&a), CX_BUFFER_OK);
    ASSERT_EQ(cx_buffer_deallocate(&b), CX_BUFFER_OK);
}

// buffers with the same length but different contents are not equal
TEST(buffer_equal_different_contents) {
    cxBuffer* a = NULL;
    cxBuffer* b = NULL;
    ASSERT_EQ(cx_buffer_allocate(&a, 3), CX_BUFFER_OK);
    ASSERT_EQ(cx_buffer_allocate(&b, 3), CX_BUFFER_OK);
    for (int i = 0; i < 3; i++) { a->data[i] = (float)i; b->data[i] = (float)i; }
    b->data[2] = 99.0f;

    bool equal = true;
    cxBufferStatus status = cx_buffer_equal(a, b, &equal);
    ASSERT_EQ(status, CX_BUFFER_OK);
    ASSERT_FALSE(equal);

    ASSERT_EQ(cx_buffer_deallocate(&a), CX_BUFFER_OK);
    ASSERT_EQ(cx_buffer_deallocate(&b), CX_BUFFER_OK);
}

// a buffer is always equal to itself
TEST(buffer_equal_with_self) {
    cxBuffer* buf = NULL;
    ASSERT_EQ(cx_buffer_allocate(&buf, 3), CX_BUFFER_OK);
    for (int i = 0; i < 3; i++) buf->data[i] = (float)i;

    bool equal = false;
    cxBufferStatus status = cx_buffer_equal(buf, buf, &equal);
    ASSERT_EQ(status, CX_BUFFER_OK);
    ASSERT_TRUE(equal);

    ASSERT_EQ(cx_buffer_deallocate(&buf), CX_BUFFER_OK);
}

// cannot compare with a null buffer or output pointer
TEST(buffer_equal_null_pointer) {
    cxBuffer* buf = NULL;
    ASSERT_EQ(cx_buffer_allocate(&buf, 3), CX_BUFFER_OK);

    bool equal = false;
    cxBufferStatus status = cx_buffer_equal(NULL, buf, &equal);
    ASSERT_EQ(status, CX_BUFFER_ERR_NULL_POINTER);
    status = cx_buffer_equal(buf, NULL, &equal);
    ASSERT_EQ(status, CX_BUFFER_ERR_NULL_POINTER);
    status = cx_buffer_equal(buf, buf, NULL);
    ASSERT_EQ(status, CX_BUFFER_ERR_NULL_POINTER);

    ASSERT_EQ(cx_buffer_deallocate(&buf), CX_BUFFER_OK);
}

// slice a sub-range from the middle of a buffer
TEST(buffer_slice) {
    cxBuffer* src = NULL;
    ASSERT_EQ(cx_buffer_allocate(&src, 5), CX_BUFFER_OK);
    for (int i = 0; i < 5; i++) src->data[i] = (float)i;

    cxBuffer* dst = NULL;
    cxBufferStatus status = cx_buffer_slice(src, 1, 3, &dst);
    ASSERT_EQ(status, CX_BUFFER_OK);
    ASSERT_NOT_NULL(dst);
    ASSERT_EQ(dst->len, (size_t)3);

    float expected[3] = { 1.0f, 2.0f, 3.0f };
    for (int i = 0; i < 3; i++) {
        ASSERT_FLOAT_EQ(dst->data[i], expected[i], 1e-5);
    }

    ASSERT_EQ(cx_buffer_deallocate(&src), CX_BUFFER_OK);
    ASSERT_EQ(cx_buffer_deallocate(&dst), CX_BUFFER_OK);
}

// slice the entire buffer
TEST(buffer_slice_full_range) {
    cxBuffer* src = NULL;
    ASSERT_EQ(cx_buffer_allocate(&src, 3), CX_BUFFER_OK);
    for (int i = 0; i < 3; i++) src->data[i] = (float)i;

    cxBuffer* dst = NULL;
    cxBufferStatus status = cx_buffer_slice(src, 0, 3, &dst);
    ASSERT_EQ(status, CX_BUFFER_OK);

    for (int i = 0; i < 3; i++) {
        ASSERT_FLOAT_EQ(dst->data[i], (float)i, 1e-5);
    }

    ASSERT_EQ(cx_buffer_deallocate(&src), CX_BUFFER_OK);
    ASSERT_EQ(cx_buffer_deallocate(&dst), CX_BUFFER_OK);
}

// cannot slice with a null source or destination pointer
TEST(buffer_slice_null_pointer) {
    cxBuffer* src = NULL;
    ASSERT_EQ(cx_buffer_allocate(&src, 3), CX_BUFFER_OK);

    cxBuffer* dst = NULL;
    cxBufferStatus status = cx_buffer_slice(NULL, 0, 1, &dst);
    ASSERT_EQ(status, CX_BUFFER_ERR_NULL_POINTER);
    status = cx_buffer_slice(src, 0, 1, NULL);
    ASSERT_EQ(status, CX_BUFFER_ERR_NULL_POINTER);

    ASSERT_EQ(cx_buffer_deallocate(&src), CX_BUFFER_OK);
}

// cannot slice into a destination pointer that is already allocated
TEST(buffer_slice_already_allocated) {
    cxBuffer* src = NULL;
    cxBuffer* dst = NULL;
    ASSERT_EQ(cx_buffer_allocate(&src, 3), CX_BUFFER_OK);
    ASSERT_EQ(cx_buffer_allocate(&dst, 1), CX_BUFFER_OK);

    cxBufferStatus status = cx_buffer_slice(src, 0, 1, &dst);
    ASSERT_EQ(status, CX_BUFFER_ALREADY_ALLOCATED);

    ASSERT_EQ(cx_buffer_deallocate(&src), CX_BUFFER_OK);
    ASSERT_EQ(cx_buffer_deallocate(&dst), CX_BUFFER_OK);
}

// cannot slice a zero length range
TEST(buffer_slice_zero_length) {
    cxBuffer* src = NULL;
    ASSERT_EQ(cx_buffer_allocate(&src, 3), CX_BUFFER_OK);

    cxBuffer* dst = NULL;
    cxBufferStatus status = cx_buffer_slice(src, 0, 0, &dst);
    ASSERT_EQ(status, CX_BUFFER_INVALID_LENGTH);
    ASSERT_NULL(dst);

    ASSERT_EQ(cx_buffer_deallocate(&src), CX_BUFFER_OK);
}

// cannot slice with a start index past the end of the buffer
TEST(buffer_slice_start_out_of_bounds) {
    cxBuffer* src = NULL;
    ASSERT_EQ(cx_buffer_allocate(&src, 3), CX_BUFFER_OK);

    cxBuffer* dst = NULL;
    cxBufferStatus status = cx_buffer_slice(src, 4, 1, &dst);
    ASSERT_EQ(status, CX_BUFFER_OUT_OF_BOUNDS);
    ASSERT_NULL(dst);

    ASSERT_EQ(cx_buffer_deallocate(&src), CX_BUFFER_OK);
}

// cannot slice a range that extends past the end of the buffer
TEST(buffer_slice_length_out_of_bounds) {
    cxBuffer* src = NULL;
    ASSERT_EQ(cx_buffer_allocate(&src, 3), CX_BUFFER_OK);

    cxBuffer* dst = NULL;
    cxBufferStatus status = cx_buffer_slice(src, 1, 3, &dst);
    ASSERT_EQ(status, CX_BUFFER_OUT_OF_BOUNDS);
    ASSERT_NULL(dst);

    ASSERT_EQ(cx_buffer_deallocate(&src), CX_BUFFER_OK);
}

static void test__sum_visitor(size_t index, float value, void* ctx) {
    (void)index;
    float* sum = (float*)ctx;
    *sum += value;
}

// for_each visits every element in order
TEST(buffer_for_each) {
    cxBuffer* buf = NULL;
    ASSERT_EQ(cx_buffer_allocate(&buf, 5), CX_BUFFER_OK);
    for (int i = 0; i < 5; i++) buf->data[i] = (float)i;

    float sum = 0.0f;
    cxBufferStatus status = cx_buffer_for_each(buf, test__sum_visitor, &sum);
    ASSERT_EQ(status, CX_BUFFER_OK);
    ASSERT_FLOAT_EQ(sum, 10.0f, 1e-5);

    ASSERT_EQ(cx_buffer_deallocate(&buf), CX_BUFFER_OK);
}

// cannot iterate with a null buffer or visitor
TEST(buffer_for_each_null_pointer) {
    cxBuffer* buf = NULL;
    ASSERT_EQ(cx_buffer_allocate(&buf, 3), CX_BUFFER_OK);

    float sum = 0.0f;
    cxBufferStatus status = cx_buffer_for_each(NULL, test__sum_visitor, &sum);
    ASSERT_EQ(status, CX_BUFFER_ERR_NULL_POINTER);
    status = cx_buffer_for_each(buf, NULL, &sum);
    ASSERT_EQ(status, CX_BUFFER_ERR_NULL_POINTER);

    ASSERT_EQ(cx_buffer_deallocate(&buf), CX_BUFFER_OK);
}

static float test__sum_reducer(float accumulator, float value, void* ctx) {
    (void)ctx;
    return accumulator + value;
}

// reduce accumulates every element starting from the initial value
TEST(buffer_reduce) {
    cxBuffer* buf = NULL;
    ASSERT_EQ(cx_buffer_allocate(&buf, 5), CX_BUFFER_OK);
    for (int i = 0; i < 5; i++) buf->data[i] = (float)i;

    float result = 0.0f;
    cxBufferStatus status = cx_buffer_reduce(buf, test__sum_reducer, 100.0f, NULL, &result);
    ASSERT_EQ(status, CX_BUFFER_OK);
    ASSERT_FLOAT_EQ(result, 110.0f, 1e-5);

    ASSERT_EQ(cx_buffer_deallocate(&buf), CX_BUFFER_OK);
}

// reduce over an empty single-element buffer returns the initial value combined with it
TEST(buffer_reduce_single_element) {
    cxBuffer* buf = NULL;
    ASSERT_EQ(cx_buffer_allocate(&buf, 1), CX_BUFFER_OK);
    buf->data[0] = 42.0f;

    float result = 0.0f;
    cxBufferStatus status = cx_buffer_reduce(buf, test__sum_reducer, 0.0f, NULL, &result);
    ASSERT_EQ(status, CX_BUFFER_OK);
    ASSERT_FLOAT_EQ(result, 42.0f, 1e-5);

    ASSERT_EQ(cx_buffer_deallocate(&buf), CX_BUFFER_OK);
}

// cannot reduce with a null buffer, reducer, or output pointer
TEST(buffer_reduce_null_pointer) {
    cxBuffer* buf = NULL;
    ASSERT_EQ(cx_buffer_allocate(&buf, 3), CX_BUFFER_OK);

    float result = 0.0f;
    cxBufferStatus status = cx_buffer_reduce(NULL, test__sum_reducer, 0.0f, NULL, &result);
    ASSERT_EQ(status, CX_BUFFER_ERR_NULL_POINTER);
    status = cx_buffer_reduce(buf, NULL, 0.0f, NULL, &result);
    ASSERT_EQ(status, CX_BUFFER_ERR_NULL_POINTER);
    status = cx_buffer_reduce(buf, test__sum_reducer, 0.0f, NULL, NULL);
    ASSERT_EQ(status, CX_BUFFER_ERR_NULL_POINTER);

    ASSERT_EQ(cx_buffer_deallocate(&buf), CX_BUFFER_OK);
}

static bool test__even_predicate(float value, void* ctx) {
    (void)ctx;
    long v = (long)value;
    return v % 2 == 0;
}

// filter keeps only the elements matching the predicate
TEST(buffer_filter) {
    cxBuffer* src = NULL;
    ASSERT_EQ(cx_buffer_allocate(&src, 5), CX_BUFFER_OK);
    for (int i = 0; i < 5; i++) src->data[i] = (float)i;

    cxBuffer* dst = NULL;
    cxBufferStatus status = cx_buffer_filter(src, test__even_predicate, NULL, &dst);
    ASSERT_EQ(status, CX_BUFFER_OK);
    ASSERT_NOT_NULL(dst);
    ASSERT_EQ(dst->len, (size_t)3);

    float expected[3] = { 0.0f, 2.0f, 4.0f };
    for (int i = 0; i < 3; i++) {
        ASSERT_FLOAT_EQ(dst->data[i], expected[i], 1e-5);
    }

    ASSERT_EQ(cx_buffer_deallocate(&src), CX_BUFFER_OK);
    ASSERT_EQ(cx_buffer_deallocate(&dst), CX_BUFFER_OK);
}

// filter that matches nothing is rejected since it would need a zero length buffer
TEST(buffer_filter_empty_result) {
    cxBuffer* src = NULL;
    ASSERT_EQ(cx_buffer_allocate(&src, 3), CX_BUFFER_OK);
    src->data[0] = 1.0f;
    src->data[1] = 3.0f;
    src->data[2] = 5.0f;

    cxBuffer* dst = NULL;
    cxBufferStatus status = cx_buffer_filter(src, test__even_predicate, NULL, &dst);
    ASSERT_EQ(status, CX_BUFFER_INVALID_LENGTH);
    ASSERT_NULL(dst);

    ASSERT_EQ(cx_buffer_deallocate(&src), CX_BUFFER_OK);
}

// cannot filter with a null source, predicate, or destination pointer
TEST(buffer_filter_null_pointer) {
    cxBuffer* src = NULL;
    ASSERT_EQ(cx_buffer_allocate(&src, 3), CX_BUFFER_OK);

    cxBuffer* dst = NULL;
    cxBufferStatus status = cx_buffer_filter(NULL, test__even_predicate, NULL, &dst);
    ASSERT_EQ(status, CX_BUFFER_ERR_NULL_POINTER);
    status = cx_buffer_filter(src, NULL, NULL, &dst);
    ASSERT_EQ(status, CX_BUFFER_ERR_NULL_POINTER);
    status = cx_buffer_filter(src, test__even_predicate, NULL, NULL);
    ASSERT_EQ(status, CX_BUFFER_ERR_NULL_POINTER);

    ASSERT_EQ(cx_buffer_deallocate(&src), CX_BUFFER_OK);
}

// cannot filter into a destination pointer that is already allocated
TEST(buffer_filter_already_allocated) {
    cxBuffer* src = NULL;
    cxBuffer* dst = NULL;
    ASSERT_EQ(cx_buffer_allocate(&src, 3), CX_BUFFER_OK);
    ASSERT_EQ(cx_buffer_allocate(&dst, 1), CX_BUFFER_OK);

    cxBufferStatus status = cx_buffer_filter(src, test__even_predicate, NULL, &dst);
    ASSERT_EQ(status, CX_BUFFER_ALREADY_ALLOCATED);

    ASSERT_EQ(cx_buffer_deallocate(&src), CX_BUFFER_OK);
    ASSERT_EQ(cx_buffer_deallocate(&dst), CX_BUFFER_OK);
}

#define TEST_SAVE_LOAD_PATH "target/test_buffer_save_load.bin"

// round trip a buffer to disk and back
TEST(buffer_save_load_round_trip) {
    cxBuffer* buf = NULL;
    ASSERT_EQ(cx_buffer_allocate(&buf, 4), CX_BUFFER_OK);
    for (int i = 0; i < 4; i++) buf->data[i] = (float)i + 0.5f;

    cxBufferStatus status = cx_buffer_save(buf, TEST_SAVE_LOAD_PATH);
    ASSERT_EQ(status, CX_BUFFER_OK);

    cxBuffer* loaded = NULL;
    status = cx_buffer_load(&loaded, TEST_SAVE_LOAD_PATH);
    ASSERT_EQ(status, CX_BUFFER_OK);
    ASSERT_NOT_NULL(loaded);

    bool equal = false;
    ASSERT_EQ(cx_buffer_equal(buf, loaded, &equal), CX_BUFFER_OK);
    ASSERT_TRUE(equal);

    ASSERT_EQ(cx_buffer_deallocate(&buf), CX_BUFFER_OK);
    ASSERT_EQ(cx_buffer_deallocate(&loaded), CX_BUFFER_OK);
    remove(TEST_SAVE_LOAD_PATH);
}

// cannot save with a null buffer or path
TEST(buffer_save_null_pointer) {
    cxBuffer* buf = NULL;
    ASSERT_EQ(cx_buffer_allocate(&buf, 3), CX_BUFFER_OK);

    cxBufferStatus status = cx_buffer_save(NULL, TEST_SAVE_LOAD_PATH);
    ASSERT_EQ(status, CX_BUFFER_ERR_NULL_POINTER);
    status = cx_buffer_save(buf, NULL);
    ASSERT_EQ(status, CX_BUFFER_ERR_NULL_POINTER);

    ASSERT_EQ(cx_buffer_deallocate(&buf), CX_BUFFER_OK);
}

// saving to a path that cannot be opened returns an io error
TEST(buffer_save_invalid_path) {
    cxBuffer* buf = NULL;
    ASSERT_EQ(cx_buffer_allocate(&buf, 3), CX_BUFFER_OK);

    cxBufferStatus status = cx_buffer_save(buf, "target/nonexistent_dir/out.bin");
    ASSERT_EQ(status, CX_BUFFER_IO_ERROR);

    ASSERT_EQ(cx_buffer_deallocate(&buf), CX_BUFFER_OK);
}

// cannot load with a null destination pointer or path
TEST(buffer_load_null_pointer) {
    cxBuffer* buf = NULL;
    cxBufferStatus status = cx_buffer_load(NULL, TEST_SAVE_LOAD_PATH);
    ASSERT_EQ(status, CX_BUFFER_ERR_NULL_POINTER);
    status = cx_buffer_load(&buf, NULL);
    ASSERT_EQ(status, CX_BUFFER_ERR_NULL_POINTER);
}

// cannot load into a destination pointer that is already allocated
TEST(buffer_load_already_allocated) {
    cxBuffer* buf = NULL;
    ASSERT_EQ(cx_buffer_allocate(&buf, 3), CX_BUFFER_OK);

    cxBufferStatus status = cx_buffer_load(&buf, TEST_SAVE_LOAD_PATH);
    ASSERT_EQ(status, CX_BUFFER_ALREADY_ALLOCATED);

    ASSERT_EQ(cx_buffer_deallocate(&buf), CX_BUFFER_OK);
}

// loading from a path that does not exist returns an io error
TEST(buffer_load_nonexistent_path) {
    cxBuffer* buf = NULL;
    cxBufferStatus status = cx_buffer_load(&buf, "target/does_not_exist.bin");
    ASSERT_EQ(status, CX_BUFFER_IO_ERROR);
    ASSERT_NULL(buf);
}

// loading a truncated file must not leave a partially populated buffer
TEST(buffer_load_truncated_file) {
    size_t claimed_len = 10;
    FILE* file = fopen(TEST_SAVE_LOAD_PATH, "wb");
    ASSERT_NOT_NULL(file);
    fwrite(&claimed_len, sizeof(claimed_len), 1, file);
    float partial = 1.0f;
    fwrite(&partial, sizeof(partial), 1, file);
    fclose(file);

    cxBuffer* buf = NULL;
    cxBufferStatus status = cx_buffer_load(&buf, TEST_SAVE_LOAD_PATH);
    ASSERT_EQ(status, CX_BUFFER_IO_ERROR);
    ASSERT_NULL(buf);

    remove(TEST_SAVE_LOAD_PATH);
}

// loading a file that claims a zero length is rejected
TEST(buffer_load_zero_length) {
    size_t claimed_len = 0;
    FILE* file = fopen(TEST_SAVE_LOAD_PATH, "wb");
    ASSERT_NOT_NULL(file);
    fwrite(&claimed_len, sizeof(claimed_len), 1, file);
    fclose(file);

    cxBuffer* buf = NULL;
    cxBufferStatus status = cx_buffer_load(&buf, TEST_SAVE_LOAD_PATH);
    ASSERT_EQ(status, CX_BUFFER_IO_ERROR);
    ASSERT_NULL(buf);

    remove(TEST_SAVE_LOAD_PATH);
}

#undef TEST_SAVE_LOAD_PATH

// length reports the number of elements in a buffer
TEST(buffer_length) {
    cxBuffer* buf = NULL;
    ASSERT_EQ(cx_buffer_allocate(&buf, 7), CX_BUFFER_OK);

    size_t length = 0;
    cxBufferStatus status = cx_buffer_length(buf, &length);
    ASSERT_EQ(status, CX_BUFFER_OK);
    ASSERT_EQ(length, (size_t)7);

    ASSERT_EQ(cx_buffer_deallocate(&buf), CX_BUFFER_OK);
}

// cannot get the length with a null buffer or output pointer
TEST(buffer_length_null_pointer) {
    cxBuffer* buf = NULL;
    ASSERT_EQ(cx_buffer_allocate(&buf, 3), CX_BUFFER_OK);

    size_t length = 0;
    cxBufferStatus status = cx_buffer_length(NULL, &length);
    ASSERT_EQ(status, CX_BUFFER_ERR_NULL_POINTER);
    status = cx_buffer_length(buf, NULL);
    ASSERT_EQ(status, CX_BUFFER_ERR_NULL_POINTER);

    ASSERT_EQ(cx_buffer_deallocate(&buf), CX_BUFFER_OK);
}