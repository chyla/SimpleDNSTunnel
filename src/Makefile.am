AUTOMAKE_OPTIONS	= subdir-objects

AM_CPPFLAGS		= $(PTHREAD_CFLAGS) @BOOST_CPPFLAGS@ -DSYSCONFDIR="\"$(sysconfdir)\"" -DLOGDIR="\"$(localstatedir)/log/\""

AM_LDFLAGS		= @BOOST_LDFLAGS@

bin_PROGRAMS		= sdnst
sdnst_SOURCES		= main.cpp \
				Options/ProgramOptions.cpp \
				Interfaces/TunTap.cpp

sdnst_LDADD		= @BOOST_PROGRAM_OPTIONS_LIB@ \
				@BOOST_LOG_LIB@ \
				@BOOST_LOG_SETUP_LIB@ \
				@BOOST_REGEX_LIB@ \
				@BOOST_DATE_TIME_LIB@ \
				@BOOST_FILESYSTEM_LIB@ \
				@BOOST_SYSTEM_LIB@ \
				@BOOST_THREAD_LIB@ \
				@PTHREAD_LIBS@ \
				@PTHREAD_CFLAGS@
