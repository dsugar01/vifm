/* vifm
 * Copyright (C) 2020 xaizek.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include "vlua.h"

#include <assert.h> /* assert() */
#include <stddef.h> /* NULL size_t */
#include <stdlib.h> /* free() */

#include "../cfg/config.h"
#include "../compat/dtype.h"
#include "../compat/fs_limits.h"
#include "../engine/options.h"
#include "../modes/dialogs/msg_dialog.h"
#include "../ui/statusbar.h"
#include "../ui/ui.h"
#include "../utils/fs.h"
#include "../utils/path.h"
#include "../utils/str.h"
#include "../utils/string_array.h"
#include "../utils/utils.h"
#include "../cmd_core.h"
#include "../filelist.h"
#include "../filename_modifiers.h"
#include "../macros.h"
#include "../plugins.h"
#include "../status.h"
#include "lua/lauxlib.h"
#include "lua/lua.h"
#include "common.h"
#include "vifm_cmds.h"
#include "vifm_handlers.h"
#include "vifm_viewcolumns.h"
#include "vifmjob.h"
#include "vifmview.h"
#include "vlua_state.h"

static void patch_env(lua_State *lua);
static void load_api(lua_State *lua);
static int print(lua_State *lua);
static int opts_global_index(lua_State *lua);
static int opts_global_newindex(lua_State *lua);
static int vifm_errordialog(lua_State *lua);
static int vifm_fnamemodify(lua_State *lua);
static int vifm_exists(lua_State *lua);
static int vifm_makepath(lua_State *lua);
static int vifm_expand(lua_State *lua);
static int sb_info(lua_State *lua);
static int sb_error(lua_State *lua);
static int sb_quick(lua_State *lua);
static int load_plugin(lua_State *lua, const char name[], plug_t *plug);
static void setup_plugin_env(lua_State *lua, plug_t *plug);

/* Functions of `vifm` global table. */
static const struct luaL_Reg vifm_methods[] = {
	{ "errordialog",   &vifm_errordialog   },
	{ "fnamemodify",   &vifm_fnamemodify   },
	{ "exists",        &vifm_exists        },
	{ "makepath",      &vifm_makepath      },
	{ "startjob",      &vifmjob_new        },
	{ "expand",        &vifm_expand        },
	{ "currview",      &vifmview_currview  },
	{ "addcolumntype", &vifm_addcolumntype },
	{ "addhandler",    &vifm_addhandler    },
	{ NULL,            NULL                }
};

/* Functions of `vifm.sb` table. */
static const struct luaL_Reg sb_methods[] = {
	{ "info",   &sb_info  },
	{ "error",  &sb_error },
	{ "quick",  &sb_quick },
	{ NULL,     NULL      }
};

vlua_t *
vlua_init(void)
{
	vlua_t *vlua = vlua_state_alloc();
	if(vlua != NULL)
	{
		patch_env(vlua->lua);
		load_api(vlua->lua);

		vifm_viewcolumns_init(vlua);
		vifm_handlers_init(vlua);
	}
	return vlua;
}

void
vlua_finish(vlua_t *vlua)
{
	vlua_state_free(vlua);
}

/* Adjusts standard libraries. */
static void
patch_env(lua_State *lua)
{
	lua_pushcfunction(lua, &print);
	lua_setglobal(lua, "print");

	lua_getglobal(lua, "os");
	lua_newtable(lua);
	lua_getfield(lua, -2, "clock");
	lua_setfield(lua, -2, "clock");
	lua_getfield(lua, -2, "date");
	lua_setfield(lua, -2, "date");
	lua_getfield(lua, -2, "difftime");
	lua_setfield(lua, -2, "difftime");
	lua_getfield(lua, -2, "time");
	lua_setfield(lua, -2, "time");
	lua_setglobal(lua, "os");
	lua_pop(lua, 1);
}

/* Fills Lua state with application-specific API. */
static void
load_api(lua_State *lua)
{
	vifmjob_init(lua);
	vifmview_init(lua);

	luaL_newmetatable(lua, "VifmPluginEnv");
	lua_pushglobaltable(lua);
	lua_setfield(lua, -2, "__index");
	lua_pop(lua, 1);

	luaL_newlib(lua, vifm_methods);

	lua_pushvalue(lua, -1);
	lua_setglobal(lua, "vifm");

	/* Setup vifm.cmds. */
	vifm_cmds_init(lua);
	lua_setfield(lua, -2, "cmds");

	/* Setup vifm.opts. */
	lua_newtable(lua);
	lua_pushvalue(lua, -1);
	lua_setfield(lua, -3, "opts");
	lua_newtable(lua);
	lua_newtable(lua);
	lua_pushcfunction(lua, &opts_global_index);
	lua_setfield(lua, -2, "__index");
	lua_pushcfunction(lua, &opts_global_newindex);
	lua_setfield(lua, -2, "__newindex");
	lua_setmetatable(lua, -2);
	lua_setfield(lua, -2, "global");
	lua_pop(lua, 1);

	/* Setup vifm.plugins. */
	lua_newtable(lua);
	lua_newtable(lua);
	lua_setfield(lua, -2, "all");
	lua_setfield(lua, -2, "plugins");

	/* Setup vifm.sb. */
	luaL_newlib(lua, sb_methods);
	lua_setfield(lua, -2, "sb");

	/* vifm. */
	lua_pop(lua, 1);
}

/* Replacement of standard global `print` function.  Outputs to statusbar.
 * Doesn't return anything. */
static int
print(lua_State *lua)
{
	char *msg = NULL;
	size_t msg_len = 0U;

	int nargs = lua_gettop(lua);
	int i;
	for(i = 0; i < nargs; ++i)
	{
		const char *piece = luaL_tolstring(lua, i + 1, NULL);
		if(i > 0)
		{
			(void)strappendch(&msg, &msg_len, '\t');
		}
		(void)strappend(&msg, &msg_len, piece);
		lua_pop(lua, 1);
	}

	plug_t *plug = lua_touserdata(lua, lua_upvalueindex(1));
	if(plug != NULL)
	{
		plug_log(plug, msg);
	}
	else
	{
		ui_sb_msg(msg);
		curr_stats.save_msg = 1;
	}

	free(msg);
	return 0;
}

/* Provides read access to global options by their name as
 * `vifm.opts.global[name]`. */
static int
opts_global_index(lua_State *lua)
{
	const char *opt_name = luaL_checkstring(lua, 2);

	opt_t *opt = vle_opts_find(opt_name, OPT_ANY);
	if(opt == NULL || opt->scope == OPT_LOCAL)
	{
		return 0;
	}

	return get_opt(lua, opt);
}

/* Provides write access to global options by their name as
 * `vifm.opts.global[name] = value`. */
static int
opts_global_newindex(lua_State *lua)
{
	const char *opt_name = luaL_checkstring(lua, 2);

	opt_t *opt = vle_opts_find(opt_name, OPT_ANY);
	if(opt == NULL || opt->scope == OPT_LOCAL)
	{
		return 0;
	}

	return set_opt(lua, opt);
}

/* Member of `vifm` that displays an error dialog.  Doesn't return anything. */
static int
vifm_errordialog(lua_State *lua)
{
	const char *title = luaL_checkstring(lua, 1);
	const char *msg = luaL_checkstring(lua, 2);
	show_error_msg(title, msg);
	return 0;
}

/* Member of `vifm` that modifies path according to specifiers.  Returns
 * modified path. */
static int
vifm_fnamemodify(lua_State *lua)
{
	const char *path = luaL_checkstring(lua, 1);
	const char *modifiers = luaL_checkstring(lua, 2);
	const char *base = luaL_optstring(lua, 3, flist_get_dir(curr_view));
	lua_pushstring(lua, mods_apply(path, base, modifiers, 0));
	return 1;
}

/* Member of `vifm` that checks whether specified path exists without resolving
 * symbolic links.  Returns a boolean, which is true when path does exist. */
static int
vifm_exists(lua_State *lua)
{
	const char *path = luaL_checkstring(lua, 1);
	lua_pushboolean(lua, path_exists(path, NODEREF));
	return 1;
}

/* Member of `vifm` that creates a directory and all of its missing parent
 * directories.  Returns a boolean, which is true on success. */
static int
vifm_makepath(lua_State *lua)
{
	const char *path = luaL_checkstring(lua, 1);
	lua_pushboolean(lua, make_path(path, 0755) == 0);
	return 1;
}

/* Member of `vifm` that expands macros and environment variables.  Returns the
 * expanded string. */
static int
vifm_expand(lua_State *lua)
{
	const char *str = luaL_checkstring(lua, 1);

	char *env_expanded = expand_envvars(str, 0);
	char *full_expanded = ma_expand(env_expanded, NULL, NULL, 0);
	lua_pushstring(lua, full_expanded);
	free(env_expanded);
	free(full_expanded);

	return 1;
}

/* Member of `vifm.sb` that prints a normal message on the statusbar.  Doesn't
 * return anything. */
static int
sb_info(lua_State *lua)
{
	const char *msg = luaL_checkstring(lua, 1);
	ui_sb_msg(msg);
	curr_stats.save_msg = 1;
	return 0;
}

/* Member of `vifm.sb` that prints an error message on the statusbar.  Doesn't
 * return anything. */
static int
sb_error(lua_State *lua)
{
	const char *msg = luaL_checkstring(lua, 1);
	ui_sb_err(msg);
	curr_stats.save_msg = 1;
	return 0;
}

/* Member of `vifm.sb` that prints statusbar message that's not stored in
 * history.  Doesn't return anything. */
static int
sb_quick(lua_State *lua)
{
	const char *msg = luaL_checkstring(lua, 1);
	ui_sb_quick_msgf("%s", msg);
	return 0;
}

int
vlua_load_plugin(vlua_t *vlua, const char plugin[], plug_t *plug)
{
	if(load_plugin(vlua->lua, plugin, plug) == 0)
	{
		lua_getglobal(vlua->lua, "vifm");
		lua_getfield(vlua->lua, -1, "plugins");
		lua_getfield(vlua->lua, -1, "all");
		lua_pushvalue(vlua->lua, -4);
		lua_setfield(vlua->lua, -2, plugin);
		lua_pop(vlua->lua, 4);
		return 0;
	}
	return 1;
}

/* Loads a single plugin as a module.  Returns zero on success and places value
 * that corresponds to the module onto the stack, otherwise non-zero is
 * returned. */
static int
load_plugin(lua_State *lua, const char name[], plug_t *plug)
{
	char full_path[PATH_MAX + 32];
	snprintf(full_path, sizeof(full_path), "%s/plugins/%s/init.lua",
			cfg.config_dir, name);

	if(luaL_loadfile(lua, full_path))
	{
		const char *error = lua_tostring(lua, -1);
		plug_log(plug, error);
		ui_sb_errf("Failed to load '%s' plugin: %s", name, error);
		lua_pop(lua, 1);
		return 1;
	}

	setup_plugin_env(lua, plug);
	if(lua_pcall(lua, 0, 1, 0))
	{
		const char *error = lua_tostring(lua, -1);
		plug_log(plug, error);
		ui_sb_errf("Failed to start '%s' plugin: %s", name, error);
		lua_pop(lua, 1);
		return 1;
	}

	if(lua_gettop(lua) == 0 || !lua_istable(lua, -1))
	{
		ui_sb_errf("Failed to load '%s' plugin: %s", name,
				"it didn't return a table");
		if(lua_gettop(lua) > 0)
		{
			lua_pop(lua, 1);
		}
		return 1;
	}

	return 0;
}

/* Sets upvalue #1 to a plugin-specific version of environment. */
static void
setup_plugin_env(lua_State *lua, plug_t *plug)
{
	/* Global environment table. */
	lua_newtable(lua);
	luaL_getmetatable(lua, "VifmPluginEnv");
	lua_setmetatable(lua, -2);

	/* Plugin-specific `vifm` table. */
	lua_newtable(lua);
	/* Meta-table for it. */
	lua_newtable(lua);
	lua_getglobal(lua, "vifm");
	lua_setfield(lua, -2, "__index");
	lua_setmetatable(lua, -2);

	/* Plugin-specific `vifm.plugin` table. */
	lua_newtable(lua);
	lua_pushstring(lua, plug->name);
	lua_setfield(lua, -2, "name");
	lua_pushstring(lua, plug->path);
	lua_setfield(lua, -2, "path");
	lua_setfield(lua, -2, "plugin");

	/* Plugin-specific `vifm.addhandler()`. */
	lua_pushlightuserdata(lua, plug);
	lua_pushcclosure(lua, &vifm_addhandler, 1);
	lua_setfield(lua, -2, "addhandler");

	/* Assign `vifm` as a plugin-specific global. */
	lua_setfield(lua, -2, "vifm");

	/* Plugin-specific `print()`. */
	lua_pushlightuserdata(lua, plug);
	lua_pushcclosure(lua, &print, 1);
	lua_setfield(lua, -2, "print");

	if(lua_setupvalue(lua, -2, 1) == NULL)
	{
		lua_pop(lua, 1);
	}
}

int
vlua_run_string(vlua_t *vlua, const char str[])
{
	int old_top = lua_gettop(vlua->lua);

	int errored = luaL_dostring(vlua->lua, str);
	if(errored)
	{
		ui_sb_err(lua_tostring(vlua->lua, -1));
	}

	lua_settop(vlua->lua, old_top);
	return errored;
}

int
vlua_complete_cmd(vlua_t *vlua, const struct cmd_info_t *cmd_info, int arg_pos)
{
	return vifm_cmds_complete(vlua->lua, cmd_info, arg_pos);
}

int
vlua_viewcolumn_map(vlua_t *vlua, const char name[])
{
	return vifm_viewcolumns_map(vlua, name);
}

int
vlua_viewcolumn_is_primary(vlua_t *vlua, int column_id)
{
	return vifm_viewcolumns_is_primary(vlua, column_id);
}

int
vlua_handler_cmd(vlua_t *vlua, const char cmd[])
{
	return vifm_handlers_check(vlua, cmd);
}

int
vlua_handler_present(vlua_t *vlua, const char cmd[])
{
	return vifm_handlers_present(vlua, cmd);
}

strlist_t
vlua_view_file(vlua_t *vlua, const char viewer[], const char path[],
		const struct preview_area_t *parea)
{
	return vifm_handlers_view(vlua, viewer, path, parea);
}

/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 noexpandtab cinoptions-=(0 : */
/* vim: set cinoptions+=t0 : */
