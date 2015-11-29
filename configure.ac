#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([mousemode], [0.1.0])
AM_INIT_AUTOMAKE([foreign subdir-objects])

# Checks for programs.
AC_PROG_CC

AM_SILENT_RULES([yes])

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lX11':
AC_CHECK_LIB([X11], [main])
# FIXME: Replace `main' with a function in `-lXext':
AC_CHECK_LIB([Xext], [main])
# FIXME: Replace `main' with a function in `-lXtst':
AC_CHECK_LIB([Xtst], [main])
# FIXME: Replace `main' with a function in `-lm':
AC_CHECK_LIB([m], [main])
# FIXME: Replace `main' with a function in `-lrt':
AC_CHECK_LIB([rt], [main])
# FIXME: Replace `main' with a function in `-lyaml':
AC_CHECK_LIB([yaml], [main])

# Checks for header files.
AC_PATH_X
AC_CHECK_HEADERS([stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_CHECK_FUNCS([clock_gettime select])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
