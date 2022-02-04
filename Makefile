.PHONY: all clean

# компилятор
COMPILER = g++

# стандартный набор флагов санитайзера
SAN_FLAGS = -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

# дедовский набор флагов для компиляции под GCC (убран -fstack-check и -Wlarger-than)
GCC_FLAGS = -D _DEBUG -ggdb3 -std=c++2a -O0 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations								\
-Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported						\
-Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness			\
-Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith					\
-Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2				\
-Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default					\
-Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix				\
-Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new					\
-fsized-deallocation -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer							\
-fPIE $(SAN_FLAGS) -pie -Wstack-usage=8192

all: diftor

diftor: diftor_main.o diftor.o bin_tree.o common.o
	$(COMPILER) $(SAN_FLAGS) -o run_diftor diftor_main.o diftor.o bin_tree.o common.o

diftor_main.o: diftor_main.cpp
	$(COMPILER) $(GCC_FLAGS) -c diftor_main.cpp

diftor.o: diftor.cpp
	$(COMPILER) $(GCC_FLAGS) -c diftor.cpp

bin_tree.o: bin_tree.cpp
	$(COMPILER) $(GCC_FLAGS) -c bin_tree.cpp

common.o: common.cpp
	$(COMPILER) $(GCC_FLAGS) -c common.cpp

clean:
	rm -rf *.o diftor
