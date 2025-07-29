#ifndef SORT_TEST_HPP
#define SORT_TEST_HPP

#include <iostream>
#include <random>

#include <stdlib.h>
#include <time.h>
#include <assert.h>

// -------------------------------------------------------------------------------- RANDOM NUMBERS
template <class array_element>
static void
fill_int_arr (array_element *arr, size_t n, array_element min, array_element max)
{
    std::random_device rd;
    std::mt19937 gen (rd ());
    std::uniform_int_distribution <array_element> dist(min, max);

    for (size_t i = 0; i < n; ++i)
        arr[i] = dist (gen);
}

template <class array_element>
static void
fill_flt_arr (array_element *arr, size_t n, array_element min, array_element max)
{
    static std::random_device rd;
    static std::mt19937 gen (rd ());
    static std::uniform_real_distribution <array_element> dist(min, max);

    for (size_t i = 0; i < n; ++i)
        arr[i] = dist (gen);
}

// -------------------------------------------------------------------------------- ARRAY CLASS
enum array_initial_state
{
    AIS_SORTED_DESC,    // array is sorted backwards before sorting (to imitate the worst case)
    AIS_UNSORTED,       // array is not sorted
    AIS_SORTED_ASC      // array is sorted (best case)
};

const char *array_initial_state_str[] =
{
    "sorted backwards",
    "unsorted",
    "sorted"
};

// 'e' stands for count of elements to write at most on the line in the case of debugging
template <class array_element, size_t n, size_t e>
class arr
{
private:
    array_element               a[n];
    enum array_initial_state    ais;

    int
    asc (const void *a, const void *b)
    {
        array_element va = *(array_element *)a;
        array_element vb = *(array_element *)b;

        if      (va < vb)
            return (-1);
        else if (va > vb)
            return (1);
        return (0);
    }

    int
    desc (const void *a, const void *b)
    {
        array_element va = *(array_element *)a;
        array_element vb = *(array_element *)b;

        if      (va > vb)
            return (-1);
        else if (va < vb)
            return (1);
        return (0);
    }

public:
    arr () { }

    arr (array_element _arr[n], enum array_initial_state _ais)
    {
        for (size_t i = 0; i < n; ++i)
            a[i] = _arr[i];

        ais = _ais;

        switch (ais)
        {
            case AIS_SORTED_DESC:
                qsort (a, n, sizeof (array_element), desc);
                break;

            case AIS_SORTED_ASC:
                qsort (a, n, sizeof (array_element), asc);
                break;

            default:
                break;
        }
    }

    array_element *
    get_arr () { return (a); }

    size_t
    get_len () { return (n); }

    size_t
    get_sz  () { return (sizeof (array_element)); } 

    enum array_initial_state
    get_ais () { return (ais); }

    bool
    sorted ()
    {
        for (size_t i = 0; i < n - 1; ++i)
            if (a[i] > a[i + 1])
                return (false);
        return (true);
    }


    void
    print ()
    {
        assert (e);

        size_t full = n / e;
        size_t rem  = n % e;

        size_t i = 0;

        while (full && i < n)
        {
            for (size_t j = 0; j < e - 1; ++j)
                std::cout << a[i++] << ", ";
            std::cout << a[i++] << "\n";

            --full;
        }
        while (rem && i < n - 1)
        {
            std::cout << a[i++] << ", ";
            --rem;
        }
        while (rem && i < n)
        {
            std::cout << a[i++] << "\n";
            --rem;
        }
    }
};

// -------------------------------------------------------------------------------- ALGORITHM CLASS
extern "C"
{
    // sorting algorithm written for specific type
    // args:
    //  address (nevermind void)
    //  length
    typedef void (*f_strict_type_sort) (const void *, size_t);

    // sorting algorithm capable of sorting any type
    // args:
    //  address
    //  length
    //  size of one element in bytes
    //  compare function
    typedef void (*f_generic_type_sort) (const void *, size_t, size_t, int (*)(const void *, const void *));

    // compare function
    typedef int (*cmp) (const void *, const void *);
};

template <class function>
class alg
{
protected:
    const char  *name;          // algorithm name
    const char  *specifics;     // additional info about algorithm
    function    f;              // sorting function
    double      exec_time;

    virtual void
    call_sort (void *, size_t) = 0;

public:
    virtual void
    sort (void *ptr, size_t n)
    {
        struct timespec b;
        struct timespec e;

        clock_gettime (CLOCK_MONOTONIC_RAW, &b);
        call_sort (ptr, n);
        clock_gettime (CLOCK_MONOTONIC_RAW, &e);

        exec_time = (e.tv_nsec - b.tv_nsec) / 1000000000.0 + (e.tv_sec - b.tv_sec);
    }

    virtual const char *
    get_name ()     { return (name); }

    virtual const char *
    get_spec ()     { return (specifics); }

    virtual double
    get_exec_t ()   { return (exec_time); }
};

class strict_type_sort_alg : protected alg <f_strict_type_sort>
{
public:
    strict_type_sort_alg (const char *_n, const char *_spec, f_strict_type_sort _f)
    {
        name = _n;
        specifics = _spec;
        f = _f;
    }
    void
    call_sort (void *ptr, size_t n) override { f (ptr, n); }
};

class generic_type_sort_alg : protected alg <f_generic_type_sort>
{
private:
    size_t s; // size of element in bytes
    cmp    c; // compare function
public:
    generic_type_sort_alg (const char *_n, const char *_spec, size_t _s, f_generic_type_sort _f, cmp _c)
    {
        name = _n;
        specifics = _spec;
        s = _s;
        f = _f;
        c = _c;
    }
    void
    call_sort (void *ptr, size_t n) override { f (ptr, n, s, c); }
};

#endif // SORT_TEST_HPP
