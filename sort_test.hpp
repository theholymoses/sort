#ifndef SORT_TEST_HPP
#define SORT_TEST_HPP

#include <iostream>
#include <random>
#include <type_traits>

#include <stdlib.h>
#include <time.h>
#include <assert.h>

// -------------------------------------------------------------------------------- RANDOM NUMBERS
template <typename T>
constexpr bool always_false = false;

template <typename T>
static inline T
rndi (T min_v = std::numeric_limits<T>::min(),
      T max_v = std::numeric_limits<T>::max())
{
    static std::random_device rd;
    static std::mt19937_64 engine(rd());
    static std::uniform_int_distribution<> d(min_v, max_v);

    return (d(engine));
}

template <typename T>
static inline T
rndf (T min_v = std::numeric_limits<T>::min(),
      T max_v = std::numeric_limits<T>::max())
{
    static std::random_device rd;
    static std::mt19937_64 engine(rd());
    static std::uniform_real_distribution<> d(min_v, max_v);

    return (d(engine));
}

// -------------------------------------------------------------------------------- ARRAY CLASS
enum array_initial_state
{
    AIS_START,

    AIS_SORTED_DESC,    // array is sorted backwards before sorting (to imitate the worst case)
    AIS_UNSORTED,       // array is not sorted
    AIS_SORTED_ASC,     // array is sorted (best case)

    AIS_END
};

template <typename T, size_t n>
class arr
{
    alignas (64) T a[n];

    enum array_initial_state ais;
    const char *ais_str;

    constexpr static size_t val_per_line = 16;      // value per line to print in debug

    int
    asc(const void *a, const void *b)
    {
        T va = *(T *)a;
        T vb = *(T *)b;

        if      (va < vb)
            return (-1);
        else if (va > vb)
            return (1);
        return (0);
    }

    int
    desc(const void *a, const void *b)
    {
        T va = *(T *)a;
        T vb = *(T *)b;

        if      (va > vb)
            return (-1);
        else if (va < vb)
            return (1);
        return (0);
    }

public:
    arr()
    {
        for (size_t i = 0; i < n; ++i)
        {
            if constexpr (std::is_floating_point_v<T>)
                a[i] = rndf<T>();
            else if constexpr (std::is_integral_v<T>)
                a[i] = rndi<T>();
            else
                static_assert(always_false<T>, "unsupported type");
        }
    }

    arr(arr<T, n> _a, enum array_initial_state _ais)
    : ais(_ais)
    {
        for (size_t i = 0; i < n; ++i)
            a[i] = _a[i];

        switch (ais)
        {
            case AIS_SORTED_DESC:
                ais_str = "sorted backwards";
                break;

            case AIS_SORTED_ASC:
                ais_str = "sorted";
                break;

            case AIS_UNSORTED:
                ais_str = "unsorted";
                break;

            default:
                ais_str = "unknown";
                break;
        }
    }

    T&
    operator[](size_t i) { return a[i]; }

    const T&
    operator[](size_t i) const { return a[i]; }

    T *
    get_arr() { return (a); }

    size_t
    get_len() const { return (n); }

    size_t
    get_sz() const { return (sizeof(T)); } 

    enum array_initial_state
    get_ais() const { return (ais); }

    const char *
    get_ais_str() const { return (ais_str); }

    bool
    sorted() const
    {
        for (size_t i = 0; i < n - 1; ++i)
            if (a[i] > a[i + 1])
                return (false);

        return (true);
    }

    void
    print() const
    {
        size_t full = n / val_per_line;
        size_t rem  = n % val_per_line;

        size_t i = 0;

        while (full && i < n)
        {
            for (size_t j = 0; j < val_per_line - 1; ++j)
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

template <typename function>
class alg
{
protected:
    const char  *name;
    const char  *info;
    function    func_sort;
    double      exec_time;

    virtual void
    call_sort(const void *, size_t) = 0;

public:
    virtual void
    sort(const void *ptr, size_t n)
    {
        struct timespec b;
        struct timespec e;

        clock_gettime(CLOCK_MONOTONIC_RAW, &b);
        call_sort(ptr, n);
        clock_gettime(CLOCK_MONOTONIC_RAW, &e);

        exec_time = (e.tv_nsec - b.tv_nsec) / 1000000000.0 + (e.tv_sec - b.tv_sec);
    }

    virtual const char *
    get_name() const { return (name); }

    virtual const char *
    get_info() const { return (info); }

    virtual double
    get_exec_t() const { return (exec_time); }
};

class strict_type_sort_alg : alg<f_strict_type_sort>
{
public:
    strict_type_sort_alg(const char *_name, const char *_info, f_strict_type_sort _func_sort)
    {
        name = _name;
        info = _info;
        func_sort = _func_sort;
    }

    void
    call_sort(const void *ptr, size_t n) override { func_sort(ptr, n); }
};

class generic_type_sort_alg : alg<f_generic_type_sort>
{
    size_t size;
    cmp    func_cmp;

public:
    generic_type_sort_alg(const char *_name, const char *_info, size_t _size, f_generic_type_sort _func_sort, cmp _func_cmp)
    {
        name = _name;
        info = _info;
        size = _size;
        func_sort = _func_sort;
        func_cmp  = _func_cmp;
    }

    void
    call_sort(const void *ptr, size_t n) override { func_sort(ptr, n, size, func_cmp); }
};

// -------------------------------------------------------------------------------- REPORT CLASS
struct sort_report
{
    const char  *algorithm_name;
    const char  *info;
    const char  *ais_str;
    size_t      len;
    size_t      size;
    double      exec_time;

    enum array_initial_state ais;

    sort_report(auto &al, auto &ar)
    {
        algorithm_name  = al.get_name();
        info            = al.get_info();
        ais_str         = ar.get_ais_str();
        len             = ar.get_len();
        size            = ar.get_size();
        exec_time       = ar.get_exec_time();
    }

    void
    print() const
    {
        printf("| %-15s (%-30s) | %-15s | %5zu | %5zu | | %-9.6f |\n",
                algorithm_name,
                info,
                ais_str,
                len,
                size,
                exec_time);
    }

    inline bool
    operator==(const sort_report& r)
    {
        if      (ais == r.ais)
            return (true);
        else if (len == r.len)
            return (true);
        else if (size == r.size)
            return (true);
        else if (exec_time == r.exec_time)
            return (true);

        return (false);
    }

    inline bool
    operator<(const sort_report& r)
    {
        if      (ais < r.ais)
            return (true);
        else if (ais > r.ais)
            return (false);

        else if (len < r.len)
            return (true);
        else if (len > r.len)
            return (false);

        else if (size < r.size)
            return (true);
        else if (size > r.size)
            return (false);

        else if (exec_time < r.exec_time)
            return (true);

        return (false);
    }

    inline bool
    operator!=(const sort_report& r)
    {
        return !(*this == r);
    }

    inline bool
    operator>(const sort_report& r)
    {
        return !(*this < r);
    }

    inline bool
    operator>=(const sort_report& r)
    {
        return (*this == r || *this > r);
    }

    inline bool
    operator<=(const sort_report& r)
    {
        return (*this == r || *this < r);
    }

};

#endif // SORT_TEST_HPP
