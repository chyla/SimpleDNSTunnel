AUTOMAKE_OPTIONS	= subdir-objects
AM_CPPFLAGS		= -DSYSCONFDIR="\"$(sysconfdir)\""

bin_PROGRAMS		= sdnst
sdnst_SOURCES		= main.cpp \
				Options/ProgramOptions.cpp
sdnst_LDADD		= @BOOST_PROGRAM_OPTIONS_LIB@
