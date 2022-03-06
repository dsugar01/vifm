#include <stic.h>

#include "../../src/engine/keys.h"
#include "../../src/modes/modes.h"
#include "../../src/modes/wk.h"

#include "builtin_keys.h"

static void key_X(key_info_t key_info, keys_info_t *keys_info);
static void selector_X(key_info_t key_info, keys_info_t *keys_info);

static int called;

SETUP()
{
	called = 0;
}

TEST(add_foreign_key)
{
	key_conf_t key = { { &key_X } };
	assert_success(vle_keys_foreign_add(L"X", &key, /*is_selector=*/0,
				NORMAL_MODE));
	assert_true(vle_keys_user_exists(L"X", NORMAL_MODE));

	assert_false(IS_KEYS_RET_CODE(vle_keys_exec(L"X")));
	assert_int_equal(1, called);
}

TEST(replace_user_key_by_foreign_key)
{
	assert_success(vle_keys_user_add(L"X", L"dd", NORMAL_MODE, KEYS_FLAG_NONE));

	key_conf_t key = { { &key_X } };
	assert_success(vle_keys_foreign_add(L"X", &key, /*is_selector=*/0,
				NORMAL_MODE));
	assert_true(vle_keys_user_exists(L"X", NORMAL_MODE));

	assert_false(IS_KEYS_RET_CODE(vle_keys_exec(L"X")));
	assert_int_equal(1, called);
}

TEST(add_foreign_selector)
{
	key_conf_t key = { { &selector_X } };
	assert_success(vle_keys_foreign_add(L"X", &key, /*is_selector=*/1,
				NORMAL_MODE));
	assert_false(vle_keys_user_exists(L"X", NORMAL_MODE));

	last_indexes_count = 0;
	assert_false(IS_KEYS_RET_CODE(vle_keys_exec(L"d10X")));
	assert_int_equal(1, called);
	assert_int_equal(10, last_indexes_count);
}

TEST(foreign_selectors_can_not_clash_with_builtins)
{
	key_conf_t key = { { &selector_X } };
	assert_failure(vle_keys_foreign_add(L"gg", &key, /*is_selector=*/1,
				NORMAL_MODE));
}

TEST(foreign_selectors_can_be_cleared)
{
	key_conf_t key = { { &selector_X } };
	assert_success(vle_keys_foreign_add(L"X", &key, /*is_selector=*/1,
				NORMAL_MODE));

	vle_keys_user_clear();

	assert_true(IS_KEYS_RET_CODE(vle_keys_exec(L"d10X")));
}

static void
key_X(key_info_t key_info, keys_info_t *keys_info)
{
	called = 1;
}

static void
selector_X(key_info_t key_info, keys_info_t *keys_info)
{
	called = 1;
	keys_info->count = key_info.count;
}

/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 noexpandtab cinoptions-=(0: */
/* vim: set cinoptions+=t0 filetype=c : */
