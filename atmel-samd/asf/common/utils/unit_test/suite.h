/**
 * \file
 *
 * \brief Test suite core declarations
 *
 * Copyright (c) 2011-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#ifndef TEST_SUITE_H_INCLUDED
#define TEST_SUITE_H_INCLUDED

#include <compiler.h>
#include <parts.h>
#include <stdio.h>

#if XMEGA
#  include <progmem.h>
#endif

/**
 * \defgroup test_suite_group Test Suite Framework
 *
 * This module is the test suite framework, which provides a set of standard
 * functions and macros for defining and running test suites.
 *
 * A test \e suite consists of test \e cases. Each test case consists of a set
 * of functions that are called in a specific order by the framework when the
 * test suite is run. The runtime environment of a test is referred to as the
 * test \e fixture. A setup function can be defined for the purpose of
 * fulfilling preconditions that are needed for the test to run. To free up or
 * reset resources which were used in the test case, a cleanup function can be
 * defined.
 *
 * The following three functions can be defined for each test case:
 * - fixture \e setup
 * - test \e run
 * - fixture \e cleanup
 *
 * The setup and/or cleanup functions may be skipped if they are not needed.
 *
 * A function may report an error or failure by use of the macros \ref test_fail
 * , \ref test_assert_true and \ref test_assert_false. Note that only the first
 * macro allows for the result value to be specified, while the two latter will
 * automatically return TEST_FAIL if the specified condition is false.
 *
 * If any of the functions return a failure/error result, execution of that
 * specific function is ended. However, depending on which function did not
 * pass, execution of the test case may continue:
 * - if the setup does not pass, the rest of the test case is skipped
 * - if the test itself does not pass, cleanup will still be executed
 *
 * The result of the first function in the test case which does not pass
 * determines the end result.
 *
 * Example code for definition of a test suite:
 * \code
	 void setup_1(const struct test_case *test);
	 void run_1(const struct test_case *test);
	 void cleanup_1(const struct test_case *test);

	 void run_2(const struct test_case *test);

	 ...
	 DEFINE_TEST_CASE(test_1,
	 		setup_1,
	 		run_1,
	 		cleanup_1,
	 		"First test");
	 DEFINE_TEST_CASE(test_2,
	 		NULL,
	 		run_2,
	 		NULL,
	 		"Second test");

	 DEFINE_TEST_ARRAY(my_tests) = {
	 	&test_1,
	 	&test_2,
	 };

	 DEFINE_TEST_SUITE(my_suite, my_tests, "Two-test suite");

	 test_suite_run(&my_suite);
	 ...
\endcode
 *
 * Example code for definition of a test case:
 * \code
	 void setup_1(const struct test_case *test)
	 {
	 	void *resource = allocate_test_resource();

	 	if (resource == NULL) {
	 		test_fail(test, TEST_ERROR,
	 			"Could not allocate test resource");
	 	}
	 	test_set_data(resource);
	 }

	 void run_1(const struct test_case *test)
	 {
	 	void *resource = test_get_data();
	 	int8_t test_result;

	      ...
		// TEST CODE
	      ...
	 	test_assert_true(test, test_result > 0,
	 			"Test result is not larger than 0: %i", test_result);
	 	test_assert_false(test, test_result > 2,
	 			"Test result is larger than 2: %i", test_result);
	 }

	 void cleanup_1(const struct test_case *test)
	 {
	 	void *resource = test_get_data();

	 	free_test_resource(resource);
	 }
\endcode
 *
 * @{
 */

/**
 * \brief A test case
 *
 * This structure represents a test case which tests one specific
 * feature or behavior.
 */
struct test_case {
	/**
	 * \brief Set up the environment in which \a test is to be run
	 *
	 * This may involve allocating memory, initializing hardware,
	 * etc. If something goes wrong, this function may call
	 * test_fail(), normally with a negative status code.
	 */
	void (*setup)(const struct test_case *test);
	//! Run the test
	void (*run)(const struct test_case *test);
	/**
	 * \brief Clean up the environment after \a test has been run
	 *
	 * This may free up any memory allocated by setup(), gracefully
	 * shutting down hardware, etc. If something goes wrong, this
	 * function may call test_fail(), normally with a negative
	 * status code.
	 */
	void (*cleanup)(const struct test_case *test);
	//! The name of the test case
	const char *name;
};

/**
 * \brief A test suite
 *
 * A test suite may contain one or more test cases which are to be run
 * in sequence.
 */
struct test_suite {
	//! The number of tests in this suite
	unsigned int nr_tests;
	//! Array of pointers to the test cases
	const struct test_case *const *tests;
	//! The name of the test suite
	const char *name;
};

/**
 * \name Wrappers for printing debug information
 *
 * \note The test suite framework feeds its output to printf. It is left up to
 * the test application to set up the stream.
 */
//@{
#if XMEGA && !defined(__ICCAVR__)
#  define dbg(__fmt_)                                                          \
	printf_P(PROGMEM_STRING(__fmt_))
#  define dbg_info(__fmt_, ...)                                                \
	printf_P(PROGMEM_STRING(__fmt_), __VA_ARGS__)
#  define dbg_error(_x, ...)                                                   \
	printf_P(PROGMEM_STRING(_x), __VA_ARGS__)
#  define dbg_putchar(c)                                                       \
	putc(c, stdout)
#  define dbg_vprintf_pgm(...)                                                 \
	vfprintf_P(stdout, __VA_ARGS__)
#else
#  define dbg(__fmt_)                                                          \
	printf(__fmt_)
#  define dbg_info(__fmt_, ...)                                                \
	printf(__fmt_, __VA_ARGS__)
#  define dbg_error(_x, ...)                                                   \
	printf(_x, __VA_ARGS__)
#  define dbg_putchar(c)                                                       \
	putc(c, stdout)
#  define dbg_vprintf_pgm(...)                                                 \
	vfprintf(stdout, __VA_ARGS__)
#endif
//@}

//! \name Test suite definition macros
//@{
/**
 * \brief Create a test case struct.
 *
 * \param _sym Variable name of the resulting struct
 * \param _setup Function which sets up a test case environment. Can be NULL.
 * \param _run Test function
 * \param _cleanup Function which cleans up what was set up. Can be NULL.
 * \param _name String describing the test case.
 */
#define DEFINE_TEST_CASE(_sym, _setup, _run, _cleanup, _name)                  \
	static const char _test_str_##_sym[] = _name;                          \
	static const struct test_case _sym = {                                 \
		.setup   = _setup,                                             \
		.run     = _run,                                               \
		.cleanup = _cleanup,                                           \
		.name    = _test_str_##_sym,                                   \
	}

/**
 * \brief Create an array of test case pointers.
 *
 * \param _sym Variable name of the resulting array
 */
#define DEFINE_TEST_ARRAY(_sym)                                                \
	const struct test_case *const _sym[]

/**
 * \brief Create a test suite.
 *
 * \param _sym Variable name of the resulting struct
 * \param _test_array Array of test cases, created with DEFINE_TEST_ARRAY()
 * \param _name String describing the test suite.
 */
#define DEFINE_TEST_SUITE(_sym, _test_array, _name)                            \
	static const char _test_str_##_sym[] = _name;                          \
	const struct test_suite _sym = {                                       \
		.nr_tests = ARRAY_LEN(_test_array),                            \
		.tests    = _test_array,                                       \
		.name     = _test_str_##_sym,                                  \
	}
//@}

#ifndef ARRAY_LEN
/**
 * \internal
 * \brief Convenience macro for counting elements in arrays
 */
# define ARRAY_LEN(a) (sizeof(a) / sizeof((a)[0]))
#endif

//! \name Test data access
//@{
extern void *test_priv_data;

/**
 * \brief Set private data pointer for the current test.
 *
 * \param data Pointer to arbitrary run-time data for the test currently
 * being run.
 */
static inline void test_set_data(void *data)
{
	test_priv_data = data;
}

/**
 * \brief Get the private data pointer for the current test.
 *
 * \return Pointer to arbitrary run-time data for the test currently
 * being run, previously registered using test_set_data().
 */
static inline void *test_get_data(void)
{
	return test_priv_data;
}
//@}

//! \name Test case pointer access
//@{

/**
 * \brief Pointer to current test case
 */
extern struct test_case *test_case_ptr;

/**
 * \internal
 * \brief Set pointer to current test.
 */
static inline void test_set_case(const struct test_case *test)
{
	test_case_ptr = (struct test_case *)test;
}

/**
 * \internal
 * \brief Get pointer to current test.
 */
static inline struct test_case *test_get_case(void)
{
	return test_case_ptr;
}
//@}

//@{
//! \name Test result reporting
//@{
/**
 * \brief Status codes returned by test cases and fixtures
 *
 * Note that test cases and especially fixtures may return any of the
 * status codes defined by \ref status_code as well.
 */
enum test_status {
	TEST_ERROR = -1, //!< Test error
	TEST_PASS  = 0,  //!< Test success
	TEST_FAIL  = 1,  //!< Test failure
};

/**
 * \def test_fail(test, result, ...)
 * \brief Fail the test
 *
 * Calling this function will cause the test to terminate with a
 * failure. It will return directly to the test suite core, not to the
 * caller.
 *
 * \param test Test case which failed
 * \param result The result of the test (may not be 0)
 * \param ... printf()-style format string and arguments
 */
#if XMEGA && !defined(__ICCAVR__)
#  define test_priv_fail_ps(test, result, format, ...)                         \
	do {                                                                   \
		static PROGMEM_DECLARE(char, _fmtstr[]) = format "%s";         \
		test_case_fail(test, result, __FILE__, __LINE__, _fmtstr,      \
				__VA_ARGS__);                                  \
	} while (0)

#  define test_fail(test, result, ...)                                         \
	test_priv_fail_ps(test, result, __VA_ARGS__, "")
#else
#  define test_fail(test, result, ...)                                         \
	test_case_fail(test, result, __FILE__, __LINE__, __VA_ARGS__)
#endif

/**
 * \brief Verify that \a condition is true
 *
 * If \a condition is false, fail the test with an error message
 * indicating the condition which failed.
 *
 * \param test The test case currently being run
 * \param condition Expression to be validated
 * \param ... Format string and arguments
 */
#define test_assert_true(test, condition, ...)                                 \
	do {                                                                   \
		if (!(condition)) {                                            \
			test_fail(test, TEST_FAIL, __VA_ARGS__);               \
		}                                                              \
	} while (0)

/**
 * \brief Verify that \a condition is false
 *
 * If \a condition is true, fail the test with an error message
 * indicating the condition which failed.
 *
 * \param test The test case currently being run
 * \param condition Expression to be validated
 * \param ... Format string and arguments
 */
#define test_assert_false(test, condition, ...)                                \
	test_assert_true(test, !(condition), __VA_ARGS__)
//@}

//! \name Test suite interaction
//@{
extern int test_suite_run(const struct test_suite *suite);
//@}

#if defined(__GNUC__)
/* GCC should perform additional printf() argument sanity checks on the
 * function call site to try to ensure arguments are printf() compatible */
__attribute__((format(__printf__, 5, 6)))
#endif
extern void test_case_fail(const struct test_case *test, int result,
		const char *file, unsigned int line,
		const char *fmt, ...);

//! @}

/**
 * \brief Assert() macro definition for unit testing
 *
 * The Assert() macro is set up to use test_assert_true(),
 * as this will only halt execution of the current test, allowing
 * the remaining tests a chance to complete.
 */
#if defined(_ASSERT_ENABLE_) && defined(TEST_SUITE_DEFINE_ASSERT_MACRO)
#  define Assert(expr) \
 	{ \
 		if (test_get_case() != NULL) { \
 			test_assert_true(test_get_case(), expr, \
 					"Assertion failed: %s", #expr); \
 		} else { \
 			if (!(expr)) { \
 				dbg_error("Assertion '%s' failed at %s:%d\r\n", \
 						#expr, __FILE__, __LINE__); \
				while (true); \
 			} \
 		} \
 	}
#endif

#endif /* TEST_SUITE_H_INCLUDED */
