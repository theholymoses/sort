src_sort = sort.nasm
obj_sort = sort.o

src_test = sort_test.cpp
hdr_test = sort_test.hpp
exe_test = sort_test

fcpp = -std=c++20 -pedantic -Wall -Wextra -Werror
clck = -D_POSIX_C_SOURCE=199309L

$(exe_test): $(src_test) $(hdr_test)
	g++ $(clck) $(fcpp) $^ -o $@

$(obj_sort): $(src_sort)
	nasm -f elf64 $^ -o $@

clean:
	rm -f $(obj_sort) $(exe_test)
