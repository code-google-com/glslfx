ACLOCAL_AMFLAGS=-I m4

lib_LTLIBRARIES = libglslfx.la
bin_PROGRAMS = glslfx-validator
check_PROGRAMS = tests-foo

TESTS = $(check_PROGRAMS)
warning_flags = -Wall -Wextra

libglslfx_la_CXXFLAGS = ${warning_flags} -I${top_srcdir}/include -I${top_srcdir}/src
libglslfx_la_LDFLAGS=-lboost_regex -lGLEW
libglslfx_la_SOURCES = \
	src/effect.cpp \
	src/libglslfx.cpp \
	src/log.cpp \
	src/parser_fx.rl \
	src/pass.cpp \
	src/technique.cpp

glslfx_validator_CXXFLAGS = ${warning_flags} -I${top_srcdir}/include
glslfx_validator_SOURCES = src/validator.cpp
glslfx_validator_LDADD = libglslfx.la

tests_foo_CXXFLAGS = ${warning_flags} -D_GNU_SOURCE -I${top_srcdir}/include
tests_foo_SOURCES = tests/foo.cpp
tests_foo_LDADD = libglslfx.la -lSDL

SUFFIXES = .rl

.rl.cpp:
	$(RAGEL) $(RAGELFLAGS) -C $< -o $@
