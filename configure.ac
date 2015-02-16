#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

m4_include([m4/ax_cxx_compile_stdcxx_11.m4])
m4_include([m4/ax_pthread.m4])
m4_include([m4/ax_boost_base.m4])
m4_include([m4/ax_boost_program_options.m4])
m4_include([m4/ax_boost_unit_test_framework.m4])
m4_include([m4/ax_boost_thread.m4])
m4_include([m4/ax_boost_filesystem.m4])
m4_include([m4/ax_boost_system.m4])
m4_include([m4/ax_boost_date_time.m4])
m4_include([m4/ax_boost_regex.m4])
m4_include([m4/ax_boost_log.m4])
m4_include([m4/ax_boost_log_setup.m4])

AC_PREREQ([2.69])
AC_INIT(SimpleDNSTunnel, master, adam@chyla.org)
AM_INIT_AUTOMAKE
AC_CONFIG_HEADERS([config.h])

AC_ARG_ENABLE(debug,
	[AS_HELP_STRING([--enable-debug], [Enable debug mode])],
	[
	  CFLAGS='-O0 -g'
	  CXXFLAGS='-O0 -g'
	  AC_DEFINE([HAVE_ENABLE_DEBUG], , [define if debug mode is enabled])
	],
	[
	  CFLAGS='-O2'
	  CXXFLAGS='-O2'
	])	     

AC_LANG_CPLUSPLUS

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
AX_CXX_COMPILE_STDCXX_11([noext])

AX_PTHREAD([], [AC_MSG_ERROR([cannot find pthread library])])

AX_BOOST_BASE([1.55], [], [AC_MSG_ERROR([cannot find Boost libraries])])

AX_BOOST_PROGRAM_OPTIONS
if test "$ax_cv_boost_program_options" != "yes"; then
    AC_MSG_ERROR([cannot find the Boost Program Options library])
fi

AX_BOOST_UNIT_TEST_FRAMEWORK
AM_CONDITIONAL(HAVE_BOOST_UNIT_TEST_FRAMEWORK, [test "x$ax_cv_boost_unit_test_framework" = "xyes"]) 

AX_BOOST_THREAD
if test "$ax_cv_boost_thread" != "yes"; then
   AC_MSG_ERROR([cannot find the Boost Thread Library])
fi

AX_BOOST_FILESYSTEM
if test "$ax_cv_boost_filesystem" != "yes"; then
   AC_MSG_ERROR([cannot find the Boost Filesystem Library])
fi

AX_BOOST_SYSTEM
if test "$ax_cv_boost_system" != "yes"; then
   AC_MSG_ERROR([cannot find the Boost System Library])
fi

AX_BOOST_DATE_TIME
if test "$ax_cv_boost_date_time" != "yes"; then
   AC_MSG_ERROR([cannot find the Boost Date_Time Library])
fi

AX_BOOST_REGEX
if test "$ax_cv_boost_regex" != "yes"; then
   AC_MSG_ERROR([cannot find the Boost Regex Library])
fi

AX_BOOST_LOG

AX_BOOST_LOG_SETUP

# Checks for header files.
AC_CHECK_HEADERS(sys/socket.h, [], [AC_MSG_ERROR([cannot find sys/socket.h])])
AC_CHECK_HEADERS(linux/if.h, [], [AC_MSG_ERROR([cannot find linux/if.h])], [#include <sys/socket.h>])
AC_CHECK_HEADERS(linux/if_tun.h, [], [AC_MSG_ERROR([cannot find linux/if_tun.h])])
AC_CHECK_HEADERS(sys/ioctl.h, [], [AC_MSG_ERROR([cannot find sys/ioctl.h])])
AC_CHECK_HEADERS(sys/types.h, [], [AC_MSG_ERROR([cannot find sys/types.h])])
AC_CHECK_HEADERS(sys/stat.h, [], [AC_MSG_ERROR([cannot find sys/stat.h])])
AC_CHECK_HEADERS(fcntl.h, [], [AC_MSG_ERROR([cannot find fcntl.h])])
AC_CHECK_HEADERS(unistd.h, [], [AC_MSG_ERROR([cannot find unistd.h])])
AC_CHECK_HEADERS(sys/types.h, [], [AC_MSG_ERROR([cannot find sys/types.h])])
AC_CHECK_HEADERS(sys/socket.h, [], [AC_MSG_ERROR([cannot find sys/socket.h])])
AC_CHECK_HEADERS(arpa/inet.h, [], [AC_MSG_ERROR([cannot find arpa/inet.h])])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_OUTPUT(Makefile src/Makefile tests/Makefile etc/Makefile)
