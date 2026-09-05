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
TEST(cx_buffer_copy_with_offset) {
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
TEST(cx_buffer_copy_with_limit_offset) {
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
TEST(cx_buffer_copy_with_offset_out_of_bounds) {
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
TEST(cx_buffer_copy_with_invalid_offset) {
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