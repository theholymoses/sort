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
}

f_strict_type_sort alg_strict[]  =
{
    bubble_sort_uint64,
    shaker_sort_uint64,
    insertion_sort_uint64,
    selection_sort_uint64,
    gnome_sort_uint64,
    0
};

const char *alg_name_strict[] =
{
    "Bubble sort",
    "Shaker sort",
    "Insertion sort",
    "Selection sort",
    "Gnome_sort",
    0
};

const char *alg_info_strict[] =
{
    "strict",
    "strict, one loop",
    "strict",
    "strict",
    "strict",
    0
};

constexpr size_t MAX_LEN = (1 << 16);

int
main ()
{
    std::vector<sort_report> srv;
    arr <uint64_t, MAX_LEN> a64;

    for (size_t i_alg = 0; alg_strict[i_alg]; ++i_alg)
    {
        for (int i_ais = AIS_START + 1; i_ais < AIS_END; ++i_ais)
        {
            arr <uint64_t, MAX_LEN> t64(a64, static_cast<enum array_initial_state>(i_ais));
            strict_type_sort_alg talg(alg_name_strict[i_alg],
                                      alg_info_strict[i_alg],
                                      alg_strict[i_alg]);

            talg.sort(t64.get_arr(), t64.get_len());
            if (!t64.sorted())
            {
                printf("Erroneous sorting algorithm: %s\n", alg_name_strict[i_alg]);
                t64.print();
                exit(1);
            }
            struct sort_report sr{t64, talg};
            srv.push_back(sr);
        }
    }
}
