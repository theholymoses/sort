#ifndef SORT_TEST_HPP
#define SORT_TEST_HPP

#include <iostream>
#include <iomanip>
#include <random>
#include <type_traits>
#include <algorithm>

#include <time.h>

// --------------------------------------------------------------------------------
template <typename T>
constexpr bool always_false = false;

template <typename T>
static inline T
random_value (T min_v = std::numeric_limits<T>::min(),
              T max_v = std::numeric_limits<T>::max())
{
    static std::random_device rd;
    static std::mt19937_64 engine(rd());

    if constexpr (std::is_integral<T>::value)
    {
        static std::uniform_int_distribution<> d(min_v, max_v);
        return (d(engine));
    }
    else if constexpr (std::is_floating_point<T>::value)
    {
        static std::uniform_real_distribution<> d(min_v, max_v);
        return (d(engine));
    }
    else
    {
        static_assert(always_false<T>, "unsupported type");
    }

}

// --------------------------------------------------------------------------------
// В какоe состояниe привести массив перед тестированием сортировки(от лучшего к худшему):
enum array_initial_state
{
    AIS_SORTED_ASC,         // массив отсортирован
    AIS_UNSORTED,           // массив не отсортирован
    AIS_SORTED_DESC         // массив отсортирован в обратном порядке
};

// Сколько значений массива вывести на одной строке так, чтобы это было читаемо
// (опираясь на размер в байтах, подразумевая, что T - число)
template <typename T>
constexpr size_t
get_val_per_line()
{
    if constexpr (sizeof(T) >= 8)
        return (4);
    else if constexpr (sizeof(T) >= 4)
        return (8);
    else if constexpr (sizeof(T) >= 2)
        return (12);
    else
        return (16);
}

// Сколько байт в выводе на экран будет занимать одно число
// (опираясь на размер в байтах, подразумевая, что T - число)
template <typename T>
constexpr size_t
get_val_width()
{
    if constexpr (sizeof(T) >= 8)
        return (21);
    else if constexpr (sizeof(T) >= 4)
        return (11);
    else if constexpr (sizeof(T) >= 2)
        return (6);
    else
        return (4);
}

template <typename T, size_t n>
class test_array
{
    T a[n];
    enum array_initial_state ais = AIS_UNSORTED;
    const char *ais_str;

    constexpr static size_t val_per_line = get_val_per_line<T>();
    constexpr static size_t val_width = get_val_width<T>();

public:
    // 1) Создаётся массив, заполняется случайными значениями и приводится в указанное состояние
    test_array(enum array_initial_state _ais = AIS_UNSORTED)
    :ais(_ais)
    {
        for (size_t i = 0; i < n; ++i)
            a[i] = random_value<T>();

        switch (ais)
        {
            case AIS_SORTED_ASC:
                ais_str = "sorted asc";
                std::sort(a, a + n, [](T a, T b) { return a < b; });
                break;

            case AIS_UNSORTED:
                ais_str = "unsorted";
                break;

            case AIS_SORTED_DESC:
                ais_str = "sorted desc";
                std::sort(a, a + n, [](T a, T b) { return a > b; });
                break;

            default:
                ais_str = "unknown";
                break;
        }
    }

    // 2) Берётся массив, созданный предыдущим конструктором, копируется и далее сортируется
    // (чтобы проверять разные сортировки на идентичном массиве)
    test_array(test_array<T, n>& t)
    :ais (t.get_ais()), ais_str(t.get_ais_str())
    {
        for (size_t i = 0; i < n; ++i)
            a[i] = t[i];
    }

    T&
    operator[](size_t i) { return a[i]; }

    const T&
    operator[](size_t i) const { return a[i]; }

    enum array_initial_state
    get_ais() const { return (ais); }

    const char *
    get_ais_str() const { return (ais_str); }

    constexpr size_t
    get_len() const { return (n); }

    constexpr size_t
    get_size() const { return (sizeof (T)); }

    void
    print() const
    {
        size_t line = n / val_per_line;
        size_t rem  = n % val_per_line;

        size_t i = 0;

        while (line)
        {
            for (size_t j = 0; j < val_per_line; ++j)
                std::cout << std::setw(val_width) << a[i++] << ", ";
            std::cout << '\n';
            --line;
        }
        while (rem > 1)
        {
            std::cout << std::setw(val_width) << a[i++] << ", ";
            --rem;
        }
        if (rem)
            std::cout << std::setw(val_width) << a[i++] << '\n';
    }

    bool
    is_sorted() const
    {
        for (size_t i = 0; i < n - 1; ++i)
            if (a[i] > a[i + 1])
                return (false);
        return (true);
    }
};

// --------------------------------------------------------------------------------
extern "C"
{
    // функция-компаратор, идентичная тому, что принимает qsort из stdlib
    typedef int (*f_cmp) (const void *, const void*);

    // Функция сортировки, реализованная под конкретный тип (тип будет конкретизирован далее)
    // Аргументы:
    //  1) указатель на сортируемый массив
    //  2) кол-во элементов
    typedef void (*f_sort_strict)(void *, size_t);

    // Функция сортировки, способная сортировать любой массив любых данных
    //  1) указатель на сортируемый массив
    //  2) кол-во элементов
    //  3) размер одного элемента в байтах
    //  4) функция-компаратор
    typedef void (*f_sort_generic)(void *, size_t, size_t, f_cmp c);

}

#endif // SORT_TEST_HPP
