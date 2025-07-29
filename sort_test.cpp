#include <stdint.h>

#include "sort_test.hpp"

// --------------------------------------------------------------------------------
extern "C"
{
    void
    bubble_sort_uint64 (uint64_t *, size_t);
    
    void
    shaker_sort_uint64 (uint64_t *, size_t);
    
    void
    insertion_sort_uint64 (uint64_t *, size_t);
    
    void
    selection_sort_uint64 (uint64_t *, size_t);
    
    void
    gnome_sort_uint64 (uint64_t *, size_t);
}

int
main ()
{   
    return (0);
}
