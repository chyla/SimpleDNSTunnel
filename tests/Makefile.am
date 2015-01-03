check_PROGRAMS	= tests
tests_SOURCES	= main.cpp \
			ProgramOptions_ConfigFile.cpp
tests_LDADD	= ../src/Options/ProgramOptions.o \
			@BOOST_UNIT_TEST_FRAMEWORK_LIB@ \
			@BOOST_PROGRAM_OPTIONS_LIB@

check-local:
	./tests
