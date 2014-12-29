#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

m4_include([m4/ax_cxx_compile_stdcxx_11.m4])
m4_include([m4/ax_boost_base.m4])
m4_include([m4/ax_boost_unit_test_framework.m4])

AC_PREREQ([2.69])
AC_INIT(SimpleDNSTunnel, master, adam@chyla.org)
AM_INIT_AUTOMAKE
AC_CONFIG_HEADERS([config.h])

AC_LANG_CPLUSPLUS
AX_CXX_COMPILE_STDCXX_11([noext])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
AX_BOOST_BASE([1.55]) #, [], [AC_MSG_ERROR([cannot find Boost libraries])])
AX_BOOST_UNIT_TEST_FRAMEWORK

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_OUTPUT(Makefile src/Makefile tests/Makefile)
