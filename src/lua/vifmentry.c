/* vifm
 * Copyright (C) 2021 xaizek.
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

#include "vifmentry.h"

#include <stdlib.h> /* free() */
#include <string.h> /* strdup() */

#include "lua/lauxlib.h"
#include "lua/lua.h"
#include "../ui/ui.h"
#include "../utils/fs.h"
#include "../filelist.h"
#include "../types.h"

/* User data of view entry object. */
typedef struct
{
	char *full_path; /* Full path to the file. */
	FileType type;   /* Type of the file. */
}
vifm_entry_t;

static int vifmentry_gc(lua_State *lua);
static int vifmentry_gettarget(lua_State *lua);

/* Methods of VifmEntry type. */
static const luaL_Reg vifmentry_methods[] = {
	{ "__gc", &vifmentry_gc },
	{ NULL,   NULL          }
};

void
vifmentry_init(lua_State *lua)
{
	luaL_newmetatable(lua, "VifmEntry");
	lua_pushvalue(lua, -1);
	lua_setfield(lua, -2, "__index");
	luaL_setfuncs(lua, vifmentry_methods, 0);
	lua_pop(lua, 1);
}

void
vifmentry_new(lua_State *lua, const dir_entry_t *entry)
{
	lua_newtable(lua);

	lua_pushstring(lua, entry->name);
	lua_setfield(lua, -2, "name");
	lua_pushstring(lua, entry->origin);
	lua_setfield(lua, -2, "location");
	lua_pushinteger(lua, entry->size);
	lua_setfield(lua, -2, "size");
	lua_pushinteger(lua, entry->mtime);
	lua_setfield(lua, -2, "mtime");
	lua_pushinteger(lua, entry->atime);
	lua_setfield(lua, -2, "atime");
	lua_pushinteger(lua, entry->ctime);
	lua_setfield(lua, -2, "ctime");
	lua_pushstring(lua, get_type_str(entry->type));
	lua_setfield(lua, -2, "type");

	int match = (entry->search_match != 0);
	lua_pushboolean(lua, match);
	lua_setfield(lua, -2, "match");
	lua_pushinteger(lua, (match ? entry->match_left + 1 : 0));
	lua_setfield(lua, -2, "matchstart");
	lua_pushinteger(lua, (match ? entry->match_right + 1 : 0));
	lua_setfield(lua, -2, "matchend");

	const char *prefix, *suffix;
	ui_get_decors(entry, &prefix, &suffix);
	lua_newtable(lua);
	lua_pushstring(lua, prefix);
	lua_setfield(lua, -2, "prefix");
	lua_pushstring(lua, suffix);
	lua_setfield(lua, -2, "suffix");
	lua_setfield(lua, -2, "classify");

	vifm_entry_t *vifm_entry = lua_newuserdatauv(lua, sizeof(*vifm_entry), 0);
	luaL_getmetatable(lua, "VifmEntry");
	lua_setmetatable(lua, -2);

	lua_pushcclosure(lua, &vifmentry_gettarget, 1);
	lua_setfield(lua, -2, "gettarget");

	char full_path[PATH_MAX + 1];
	get_full_path_of(entry, sizeof(full_path), full_path);

	vifm_entry->full_path = strdup(full_path);
	vifm_entry->type = entry->type;
}

/* Frees memory allocated to vifm_entry_t fields. */
static int
vifmentry_gc(lua_State *lua)
{
	vifm_entry_t *vifm_entry = luaL_checkudata(lua, 1, "VifmEntry");
	free(vifm_entry->full_path);
	return 0;
}

/* Gets target of a symbolic link. */
static int
vifmentry_gettarget(lua_State *lua)
{
	vifm_entry_t *vifm_entry = lua_touserdata(lua, lua_upvalueindex(1));

	if(vifm_entry->type != FT_LINK)
	{
		return luaL_error(lua, "%s", "Entry is not a symbolic link");
	}

	char link_to[PATH_MAX + 1];
	if(get_link_target(vifm_entry->full_path, link_to, sizeof(link_to)) != 0)
	{
		return luaL_error(lua, "%s", "Failed to resolve symbolic link");
	}

	lua_pushstring(lua, link_to);
	return 1;
}

/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 noexpandtab cinoptions-=(0 : */
/* vim: set cinoptions+=t0 : */