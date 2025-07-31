#include <stdint.h>
#include <vector>

#include "sort_test.hpp"

// --------------------------------------------------------------------------------
extern "C"
{
    void
    bubble_sort_uint64 (const void *, size_t);
    
    void
    shaker_sort_uint64 (const void *, size_t);
    
    void
    insertion_sort_uint64 (const void *, size_t);
    
    void
    selection_sort_uint64 (const void *, size_t);
    
    void
    gnome_sort_uint64 (const void *, size_t);
};

int
main ()
{
}
