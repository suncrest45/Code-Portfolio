#GCC=g++
PRG=gcc0.exe
GCCFLAGS=-O -Werror -Wall -Wextra -Wconversion -std=c++14 -pedantic -Wold-style-cast

OBJECTS0=ObjectAllocator.cpp PRNG.cpp
DRIVER0=driver.cpp

VALGRIND_OPTIONS=-q --leak-check=full
DIFF_OPTIONS=-y --strip-trailing-cr --suppress-common-lines -b

OSTYPE := $(shell uname)
ifeq ($(OSTYPE),Linux)
CYGWIN=
else
CYGWIN=-Wl,--enable-auto-import
endif

gcc0:
	g++ -o $(PRG) $(CYGWIN) $(DRIVER0) $(OBJECTS0) $(GCCFLAGS)
gcc1:
	clang++ -o $(PRG) $(CYGWIN) $(DRIVER0) $(OBJECTS0) $(GCCFLAGS)
gcc2:
	g++ -o $(PRG) $(CYGWIN) $(DRIVER0) $(OBJECTS0) $(GCCFLAGS) -m32
0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21:
	echo "running test$@"
	./$(PRG) $@ > studentout$@.txt
	diff test$@.txt studentout$@.txt $(DIFF_OPTIONS) > difference$@.txt
make%:  $(shell seq 1 21)
		echo "running test$@"
	./$(PRG) $@ > studentout$@.txt
	diff test$@.txt studentout$@.txt $(DIFF_OPTIONS) > difference$@.txt
clean : 
	rm *.exe student* difference*
