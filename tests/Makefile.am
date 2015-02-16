AM_CPPFLAGS		= $(PTHREAD_CFLAGS) @BOOST_CPPFLAGS@ -DSYSCONFDIR="\"$(sysconfdir)\"" -DLOGDIR="\"$(localstatedir)/log/\""

AM_LDFLAGS		= @BOOST_LDFLAGS@

check_PROGRAMS	= tests
if HAVE_BOOST_UNIT_TEST_FRAMEWORK
tests_SOURCES	= main.cpp \
			ProgramOptions_ConfigFile.cpp \
			PseudoDNS.cpp \
			Encapsulator.cpp

tests_LDADD	= ../src/Options/ProgramOptions.o \
			../src/Packets/PseudoDNS.o \
			../src/Packets/Encapsulator.o \
			@BOOST_UNIT_TEST_FRAMEWORK_LIB@ \
			@BOOST_PROGRAM_OPTIONS_LIB@ \
			@BOOST_LOG_LIB@ \
			@BOOST_LOG_SETUP_LIB@ \
			@BOOST_REGEX_LIB@ \
			@BOOST_DATE_TIME_LIB@ \
			@BOOST_FILESYSTEM_LIB@ \
			@BOOST_SYSTEM_LIB@ \
			@BOOST_THREAD_LIB@ \
			@PTHREAD_LIBS@ \
			@PTHREAD_CFLAGS@
else
tests_SOURCES	= main.cpp
endif

check-local:
	./tests
