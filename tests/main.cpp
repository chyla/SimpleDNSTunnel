/*
 * Copyright 2014-2015 Adam Chyła, adam@chyla.org
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#include "../config.h"

#ifdef HAVE_BOOST_UNIT_TEST_FRAMEWORK

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN

#include <boost/test/unit_test.hpp>

#else

#error Cannot make tests. Boost Unit Test Framework not found.

#endif // HAVE_BOOST_UNIT_TEST_FRAMEWORK

