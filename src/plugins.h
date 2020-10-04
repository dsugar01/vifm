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

#ifndef VIFM__PLUGINS_H__
#define VIFM__PLUGINS_H__

/* Declaration of opaque state of the unit. */
typedef struct plugs_t plugs_t;

struct vlua_t;

/* Creates new instance of the unit.  Returns the instance or NULL. */
plugs_t * plugs_create(struct vlua_t *vlua);

/* Frees resources of the unit. */
void plugs_free(plugs_t *plugs);

/* Loads plugins. */
void plugs_load(plugs_t *plugs, const char base_dir[]);

#endif /* VIFM__PLUGINS_H__ */

/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 noexpandtab cinoptions-=(0 : */
/* vim: set cinoptions+=t0 : */
