#include <kern/io/memshim.h>


void* malloc_io(int32_t size){
    if (size == 0) { return (void*)NULL; }
    return get_next_page();
}

int free_io(void* address){
    return free_page(address);
}
