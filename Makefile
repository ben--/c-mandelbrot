APPROVAL_TESTS	=  approve_mandelbrot
THE_TESTS	=  check_mandelbrot
THE_LIBRARY	=  mandelbrot.a
THE_PROGRAM	=  main

CFLAGS		+= -g -O0 -Wall -Werror -Wextra -std=c99
CAIRO_CFLAGS	:= $(shell pkg-config --cflags cairo)
CAIRO_LIBS	:= $(shell pkg-config --libs cairo)
GD_CFLAGS	:= $(shell pkg-config --cflags gdlib)
GD_LIBS		:= $(shell pkg-config --libs gdlib)
CHECK_CFLAGS	:= $(shell pkg-config --cflags check)
CHECK_LIBS	:= $(shell pkg-config --libs check)

BUILD_WITH_MPC	?= no

SILENT		=  @

all: check approval

check: ${THE_TESTS}
	${SILENT}./${THE_TESTS}

approval: ${THE_PROGRAM}
	${SILENT}./${THE_PROGRAM} gd 800 500 0.0 0.0 4.0
	${SILENT}./${APPROVAL_TESTS} pngelbrot.png

valgrind: ${THE_TESTS}
	${SILENT}valgrind --leak-check=full --show-leak-kinds=all ./${THE_TESTS}

clean:
	${SILENT}rm -f *.o ${THE_TESTS} ${THE_LIBRARY} ${THE_PROGRAM}
	${SILENT}rm -rf *.dSYM

is-cairo-installed:
ifeq (, ${CAIRO_LIBS})
	${SILENT}echo "Please install Cairo (https://www.cairographics.org)." && false
endif

is-check-installed:
ifeq (, ${CHECK_LIBS})
	${SILENT}echo "Please install Check (https://libcheck.github.io/check/)." && false
endif

is-gd-installed:
ifeq (, ${GD_LIBS})
	${SILENT}echo "Please install GD (http://libgd.github.io)." && false
endif

is-mpc-installed:
ifeq (yes, ${BUILD_WITH_MPC}) # XXX a little too phony
MPC_CFLAGS	=  -DUSE_MPC ${GD_CFLAGS}
MPC_LIBS	=  -lmpc -lmpfr -lgmp
endif

.PHONY: all check approval valgrind clean is-cairo-installed is-check-installed is-gd-installed is-mpc-installed

${THE_TESTS}: is-check-installed ${THE_LIBRARY} check_mandelbrot.c
	${SILENT}${CC} ${CFLAGS} ${CAIRO_CFLAGS} ${GD_CFLAGS} ${CHECK_CFLAGS} -o ${THE_TESTS} check_mandelbrot.c ${CAIRO_LIBS} ${GD_LIBS} ${MPC_LIBS} ${CHECK_LIBS} ${THE_LIBRARY}

${THE_LIBRARY}: is-cairo-installed is-gd-installed is-mpc-installed mandelbrot.h mandelbrot.c
	${SILENT}${CC} ${CFLAGS} ${CAIRO_CFLAGS} ${GD_CFLAGS} ${MPC_CFLAGS} -c mandelbrot.c
	${SILENT}ar rc ${THE_LIBRARY} mandelbrot.o
	${SILENT}ranlib ${THE_LIBRARY}

${THE_PROGRAM}: ${THE_LIBRARY} mandelbrot.h main.c
	${SILENT}${CC} ${CFLAGS} -o ${THE_PROGRAM} main.c ${CAIRO_LIBS} ${GD_LIBS} ${MPC_LIBS} ${THE_LIBRARY}
