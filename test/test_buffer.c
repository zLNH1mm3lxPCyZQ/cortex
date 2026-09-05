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
TEST(buffer_deallocation_already_null) {
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