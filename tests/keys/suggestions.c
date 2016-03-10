#include <stic.h>

#include <stddef.h> /* NULL wchar_t */
#include <wchar.h> /* wcscmp() */

#include "../../src/engine/keys.h"
#include "../../src/modes/modes.h"

static void process_suggestion(const wchar_t lhs[], const wchar_t rhs[],
		const char descr[]);

static int nsuggestions;
static const wchar_t *rhs;
static const char *descr;

SETUP()
{
	vle_keys_user_add(L"hi", L"j", NORMAL_MODE, 0);
	vle_keys_user_add(L"hi2", L"hi", NORMAL_MODE, 0);

	vle_keys_user_add(L"ho", L"j", NORMAL_MODE, 0);
	vle_keys_user_add(L"ha2", L"ho", NORMAL_MODE, 0);

	nsuggestions = 0;
	descr = NULL;
}

TEST(all_keys_are_listed_no_selectors)
{
	vle_keys_suggest(L"", &process_suggestion);
	assert_int_equal(22, nsuggestions);
}

TEST(user_keys_with_prefix_are_listed)
{
	vle_keys_suggest(L"h", &process_suggestion);
	assert_int_equal(4, nsuggestions);
}

TEST(builtin_keys_with_prefix_are_listed)
{
	vle_keys_suggest(L"g", &process_suggestion);
	assert_int_equal(3, nsuggestions);
}

TEST(selectors_are_completed_from_beginning)
{
	vle_keys_suggest(L"d", &process_suggestion);
	assert_int_equal(7, nsuggestions);
}

TEST(selectors_are_completed_with_prefix)
{
	vle_keys_suggest(L"dg", &process_suggestion);
	assert_int_equal(1, nsuggestions);
}

TEST(descr_of_user_defined_keys_is_rhs)
{
	vle_keys_suggest(L"ha", &process_suggestion);
	assert_int_equal(1, nsuggestions);
	assert_int_equal(0, wcscmp(rhs, L"ho"));
	assert_string_equal("", descr);
}

static void
process_suggestion(const wchar_t lhs[], const wchar_t r[], const char d[])
{
	++nsuggestions;
	rhs = r;
	descr = d;
}

/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 noexpandtab cinoptions-=(0: */
/* vim: set cinoptions+=t0 filetype=c : */
