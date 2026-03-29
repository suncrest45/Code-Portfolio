PRG=gnu.exe
GCC=g++
GCCFLAGS=-O2 -Wall -Wextra -std=c++11 -pedantic -Weffc++ -Wold-style-cast -Woverloaded-virtual -Wsign-promo  -Wctor-dtor-privacy -Wnon-virtual-dtor -Wreorder

MSC=cl
MSCFLAGS=/EHa /W4 /Za /Zc:forScope /nologo /D_CRT_SECURE_NO_DEPRECATE /D"_SECURE_SCL 0" /O2i /GL

OBJECTS0=
DRIVER0=driver.cpp

VALGRIND_OPTIONS=-q --leak-check=full

DIFF_OPTIONS=--strip-trailing-cr --ignore-trailing-space

OSTYPE := $(shell uname)
ifeq (,$(findstring CYGWIN,$(OSTYPE)))
CYGWIN=
else
CYGWIN=-Wl,--enable-auto-import
endif

gcc0:
	$(GCC) -o $(PRG) $(CYGWIN) $(DRIVER0) $(OBJECTS0) $(GCCFLAGS)
msc0:
	$(MSC) /Fe$@.exe           $(DRIVER0) $(OBJECTS0) $(MSCFLAGS)
0 1 2 3 4 5 6 9 10 11:
	@echo "running test$@"
	@echo "should run in less than 500 ms"
	./$(PRG) $@ >studentout$@
	diff out$@ studentout$@ $(DIFF_OPTIONS) > difference$@ 2>&1
7:
	@echo "running test$@"
	@echo "should run in less than 4000 ms"
	./$(PRG) $@ >studentout$@
	diff out$@ studentout$@ $(DIFF_OPTIONS) > difference$@ 2>&1
8:
	@echo "running test$@"
	@echo "should run in less than 2000 ms"
	./$(PRG) $@ >studentout$@
	diff out$@ studentout$@ $(DIFF_OPTIONS) > difference$@ 2>&1
mem0 mem1 mem2 mem3 mem4 mem5 mem6 mem7 mem8 mem9 mem10 mem11:
	@echo "running memory test $@"
	@echo "should run in less than 5000 ms"
	valgrind $(VALGRIND_OPTIONS) ./$(PRG) $(subst mem,,$@) 1>/dev/null 2>difference$@
	@echo "lines after this are memory errors"; cat difference$@
clean:
	rm -f *.exe *.tds *.o *.obj *manifest* studentout* diff*
