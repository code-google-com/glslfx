AC_INIT([glslfx],[0.1], [glslfx@sidvind.com])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([-Wall 1.11 silent-rules color-tests])
AC_CONFIG_MACRO_DIR([m4])

AC_CONFIG_HEADERS([src/config.h])
AH_BOTTOM([#include <glslfx/forward.h>
using namespace glslfx;])

AC_DISABLE_STATIC
AC_PROG_CXX
AC_PROG_CC_C99
AM_PROG_CC_C_O
AC_GNU_SOURCE
CHECK_RAGEL
LT_INIT
AX_CHECK_GL

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
