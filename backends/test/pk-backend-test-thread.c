/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*-
 *
 * Copyright (C) 2007-2008 Richard Hughes <richard@hughsie.com>
 *
 * Licensed under the GNU General Public License Version 2
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <gmodule.h>
#include <glib.h>
#include <string.h>
#include <pk-backend.h>
#include <pk-backend-thread.h>

static PkBackendThread *thread;

/**
 * backend_initalize:
 * This should only be run once per backend load, i.e. not every transaction
 */
static void
backend_initalize (PkBackend *backend)
{
	g_return_if_fail (backend != NULL);
	pk_debug ("FILTER: initalize");

	/* we use the thread helper */
	thread = pk_backend_thread_new ();
}

/**
 * backend_destroy:
 * This should only be run once per backend load, i.e. not every transaction
 */
static void
backend_destroy (PkBackend *backend)
{
	g_return_if_fail (backend != NULL);
	pk_debug ("FILTER: destroy");
	g_object_unref (thread);
}

/**
 * backend_search_group_thread:
 */
static gboolean
backend_search_group_thread (PkBackendThread *thread, gpointer data)
{
	PkBackend *backend;

	/* get current backend */
	backend = pk_backend_thread_get_backend (thread);
	pk_backend_set_status (backend, PK_STATUS_ENUM_QUERY);

	/* emit */
	pk_backend_package (backend, PK_INFO_ENUM_INSTALLED,
			    "glib2;2.14.0;i386;fedora", "The GLib library");
	pk_backend_package (backend, PK_INFO_ENUM_INSTALLED,
			    "gtk2;gtk2-2.11.6-6.fc8;i386;fedora", "GTK+ Libraries for GIMP");
	pk_backend_finished (backend);
	return TRUE;
}

/**
 * backend_search_group:
 */
static void
backend_search_group (PkBackend *backend, const gchar *filter, const gchar *search)
{
	g_return_if_fail (backend != NULL);
	pk_backend_thread_create (thread, backend_search_group_thread, NULL);
}

/**
 * backend_search_name_thread:
 */
static gboolean
backend_search_name_thread (PkBackendThread *thread, gpointer data)
{
	GTimer *timer;
	gdouble elapsed;
	guint percentage;
	PkBackend *backend;

	/* get current backend */
	backend = pk_backend_thread_get_backend (thread);

	pk_debug ("started task (%p,%p)", backend, data);
	pk_backend_set_status (backend, PK_STATUS_ENUM_QUERY);
	timer = g_timer_new ();
	percentage = 0;
	do {
		pk_backend_set_percentage (backend, percentage);
		percentage += 1;
		g_usleep (1000*100);
		elapsed = g_timer_elapsed (timer, NULL);
		pk_debug ("elapsed task (%p,%p) = %f", backend, data, elapsed);
	} while (elapsed < 10.0);
	g_timer_destroy (timer);
	pk_backend_set_percentage (backend, 100);
	pk_debug ("exited task (%p,%p)", backend, data);

	pk_backend_package (backend, PK_INFO_ENUM_INSTALLED,
			    "glib2;2.14.0;i386;fedora", "The GLib library");
	pk_backend_package (backend, PK_INFO_ENUM_INSTALLED,
			    "gtk2;gtk2-2.11.6-6.fc8;i386;fedora", "GTK+ Libraries for GIMP");
	pk_backend_finished (backend);
	return TRUE;
}

/**
 * backend_search_name:
 */
static void
backend_search_name (PkBackend *backend, const gchar *filter, const gchar *search)
{
	g_return_if_fail (backend != NULL);
	pk_backend_thread_create (thread, backend_search_name_thread, NULL);
}

PK_BACKEND_OPTIONS (
	"Test Thread",				/* description */
	"Richard Hughes <richard@hughsie.com>",	/* author */
	backend_initalize,			/* initalize */
	backend_destroy,			/* destroy */
	NULL,					/* get_groups */
	NULL,					/* get_filters */
	NULL,					/* cancel */
	NULL,					/* get_depends */
	NULL,					/* get_description */
	NULL,					/* get_files */
	NULL,					/* get_requires */
	NULL,					/* get_update_detail */
	NULL,					/* get_updates */
	NULL,					/* install_package */
	NULL,					/* install_file */
	NULL,					/* refresh_cache */
	NULL,					/* remove_package */
	NULL,					/* resolve */
	NULL,					/* rollback */
	NULL,					/* search_details */
	NULL,					/* search_file */
	backend_search_group,			/* search_group */
	backend_search_name,			/* search_name */
	NULL,					/* update_package */
	NULL,					/* update_system */
	NULL,					/* get_repo_list */
	NULL,					/* repo_enable */
	NULL					/* repo_set_data */
);

