#include <stdio.h>
#include <stdint.h>
#include "buffer.h"

int main() {
    printf("Hello World!\n");
    cxBuffer* buf;
    cx_buffer_allocate(&buf, 100);
    cx_buffer_write(buf, 99, 100.0);
    cx_buffer_print(buf);
    cx_buffer_deallocate(&buf);
    return 0;
}