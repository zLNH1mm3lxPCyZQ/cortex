#include "test.h"
#include "buffer.h"

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