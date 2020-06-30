/******************************************************************************
    Copyright (C) 2013-2014 by Hugh Bailey <obs.jim@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#include <inttypes.h>
#include <math.h>
#include "obs.h"
#include "obs-internal.h"



static const char *control_signals[] = {
	"void destroy(ptr control)",
	"void remove(ptr control)",
	"void save(ptr control)",
	"void load(ptr control)",
	"void enable(ptr control, bool enabled)",
	"void rename(ptr control, string new_name, string prev_name)",
	"void update_properties(ptr control)",
	NULL,
};
static inline bool data_valid(const struct obs_control *control, const char *f)
{
	return obs_control_valid(control, f) && control->context.data;
}


struct obs_control_info *get_control_info(const char *id)
{
	for (size_t i = 0; i < obs->control_types.num; i++) {
		struct obs_control_info *info = &obs->control_types.array[i];
		if (strcmp(info->id, id) == 0)
			return info;
	}

	return NULL;
}

struct obs_control_info *get_control_info2(const char *unversioned_id,
					 uint32_t ver)
{
	for (size_t i = 0; i < obs->control_types.num; i++) {
		struct obs_control_info *info = &obs->control_types.array[i];
		if (strcmp(info->unversioned_id, unversioned_id) == 0 &&
		    info->version == ver)
			return info;
	}

	return NULL;
}


bool obs_control_init_context(struct obs_control *control, obs_data_t *settings,
			     const char *name, 
			     bool private)
{
	if (!obs_context_data_init(&control->context, OBS_OBJ_TYPE_CONTROL,
				   settings, name, NULL, private))
		return false;

	return signal_handler_add_array(control->context.signals,
					control_signals);
}

const char *obs_control_get_display_name(const char *id)
{
	const struct obs_control_info *info = get_control_info(id);
	return (info != NULL) ? info->get_name(info->type_data) : NULL;
}

extern char *find_libobs_data_file(const char *file);

/* internal initialization */
static bool obs_control_init(struct obs_control *control)
{
	pthread_mutexattr_t attr;

	
	return true;
}

static void obs_control_init_finalize(struct obs_control *control)
{
	
	obs_context_data_insert(&control->context, &obs->data.controls_mutex,
				&obs->data.first_control);
}


static obs_control_t *
obs_control_create_internal(const char *id, const char *name,
			   obs_data_t *settings,
			   bool private, uint32_t last_obs_ver)
{
	struct obs_control *control = bzalloc(sizeof(struct obs_control));

	const struct obs_control_info *info = get_control_info(id);
	if (!info) {
		blog(LOG_ERROR, "Control ID '%s' not found", id);

		control->info.id = bstrdup(id);
		control->owns_info_id = true;
		control->info.unversioned_id = bstrdup(control->info.id);
	} else {
		control->info = *info;

		/* Always mark filters as private so they aren't found by
		 * control enum/search functions.
		 *
		 * XXX: Fix design flaws with filters */
		
	}

	control->last_obs_ver = last_obs_ver;

	if (!obs_control_init_context(control, settings, name,
				     private))
		goto fail;

	if (info) {
		if (info->get_defaults) {
			info->get_defaults(control->context.settings);
		}
		if (info->get_defaults2) {
			info->get_defaults2(info->type_data,
					    control->context.settings);
		}
	}

	if (!obs_control_init(control))
		goto fail;



	/* allow the control to be created even if creation fails so that the
	 * user's data doesn't become lost */
	if (info && info->create)
		control->context.data =
			info->create(control->context.settings, control);
	if ((!info || info->create) && !control->context.data)
		blog(LOG_ERROR, "Failed to create control '%s'!", name);

	blog(LOG_DEBUG, "%scontrol '%s' (%s) created", private ? "private " : "",
	     name, id);

	control->enabled = true;

	if (!private) {
		obs_control_dosignal(control, "control_create", NULL);
	}

	obs_control_init_finalize(control);
	return control;

fail:
	blog(LOG_ERROR, "obs_control_create failed");
	obs_control_destroy(control);
	return NULL;
}

obs_control_t *obs_control_create(const char *id, const char *name,
				obs_data_t *settings)
{
	return obs_control_create_internal(id, name, settings,
					  false, LIBOBS_API_VER);
}

obs_control_t *obs_control_create_private(const char *id, const char *name,
					obs_data_t *settings)
{
	return obs_control_create_internal(id, name, settings,  true,
					  LIBOBS_API_VER);
}

obs_control_t *obs_control_create_set_last_ver(const char *id, const char *name,
					     obs_data_t *settings,
					     uint32_t last_obs_ver)
{
	return obs_control_create_internal(id, name, settings,
					  false, last_obs_ver);
}


void obs_control_destroy(struct obs_control *controlv)
{
	size_t i;

	if (!obs_control_valid(controlv, "obs_control_destroy"))
		return;

	

	obs_context_data_remove(&controlv->context);

	blog(LOG_DEBUG, "%scontrol '%s' destroyed",
	     controlv->context.private ? "private " : "", controlv->context.name);

	obs_control_dosignal(controlv, "control_destroy", "destroy");

	if (controlv->context.data) {
		controlv->info.destroy(controlv->context.data);
		controlv->context.data = NULL;
	}


	
	obs_data_release(controlv->private_settings);
	obs_context_data_free(&controlv->context);

	if (controlv->owns_info_id) {
		bfree((void *)controlv->info.id);
		bfree((void *)controlv->info.unversioned_id);
	}

	bfree(controlv);
}

void obs_control_addref(obs_control_t *control)
{
	if (!control)
		return;

	obs_ref_addref(&control->control->ref);
}

void obs_control_release(obs_control_t *control)
{
	if (!obs) {
		blog(LOG_WARNING, "Tried to release a control when the OBS "
				  "core is shut down!");
		return;
	}

	if (!control)
		return;

	obs_weak_control_t *controlv = control->control;
	if (obs_ref_release(&controlv->ref)) {
		obs_control_destroy(control);
		obs_weak_control_release(control);
	}
}

void obs_weak_control_addref(obs_weak_control_t *weak)
{
	if (!weak)
		return;

	obs_weak_ref_addref(&weak->ref);
}

void obs_weak_control_release(obs_weak_control_t *weak)
{
	if (!weak)
		return;

	if (obs_weak_ref_release(&weak->ref))
		bfree(weak);
}

obs_control_t *obs_control_get_ref(obs_control_t *control)
{
	if (!control)
		return NULL;

	return obs_weak_control_get_control(control->control);
}

obs_weak_control_t *obs_control_get_weak_control(obs_control_t *control)
{
	if (!control)
		return NULL;

	obs_weak_control_t *weak = control->control;
	obs_weak_control_addref(weak);
	return weak;
}

obs_control_t *obs_weak_control_get_control(obs_weak_control_t *weak)
{
	if (!weak)
		return NULL;

	if (obs_weak_ref_get_ref(&weak->ref))
		return weak->control;

	return NULL;
}

bool obs_weak_control_references_control(obs_weak_control_t *weak,
				       obs_control_t *control)
{
	return weak && control && weak->control == control;
}

void obs_control_remove(obs_control_t *control)
{
	if (!obs_control_valid(control, "obs_control_remove"))
		return;

	if (!control->removed) {
		control->removed = true;
		obs_control_dosignal(control, "control_remove", "remove");
	}
}

bool obs_control_removed(const obs_control_t *control)
{
	return obs_control_valid(control, "obs_control_removed") ? control->removed
							      : true;
}

static inline obs_data_t *get_defaults(const struct obs_control_info *info)
{
	obs_data_t *settings = obs_data_create();
	if (info->get_defaults2)
		info->get_defaults2(info->type_data, settings);
	else if (info->get_defaults)
		info->get_defaults(settings);
	return settings;
}

obs_data_t *obs_control_settings(const char *id)
{
	const struct obs_control_info *info = get_control_info(id);
	return (info) ? get_defaults(info) : NULL;
}

obs_data_t *obs_get_control_defaults(const char *id)
{
	const struct obs_control_info *info = get_control_info(id);
	return info ? get_defaults(info) : NULL;
}

obs_properties_t *obs_get_control_properties(const char *id)
{
	const struct obs_control_info *info = get_control_info(id);
	if (info && (info->get_properties || info->get_properties2)) {
		obs_data_t *defaults = get_defaults(info);
		obs_properties_t *props;

		if (info->get_properties2)
			props = info->get_properties2(NULL, info->type_data);
		else
			props = info->get_properties(NULL);

		obs_properties_apply_settings(props, defaults);
		obs_data_release(defaults);
		return props;
	}
	return NULL;
}

bool obs_is_control_configurable(const char *id)
{
	const struct obs_control_info *info = get_control_info(id);
	return info && (info->get_properties || info->get_properties2);
}

bool obs_control_configurable(const obs_control_t *control)
{
	return data_valid(control, "obs_control_configurable") &&
	       (control->info.get_properties || control->info.get_properties2);
}

obs_properties_t *obs_control_properties(const obs_control_t *control)
{
	if (!data_valid(control, "obs_control_properties"))
		return NULL;

	if (control->info.get_properties2) {
		obs_properties_t *props;
		props = control->info.get_properties2(control->context.data,
						     control->info.type_data);
		obs_properties_apply_settings(props, control->context.settings);
		return props;

	} else if (control->info.get_properties) {
		obs_properties_t *props;
		props = control->info.get_properties(control->context.data);
		obs_properties_apply_settings(props, control->context.settings);
		return props;
	}

	return NULL;
}




static void obs_control_deferred_update(obs_control_t *control)
{
	if (control->context.data && control->info.update)
		control->info.update(control->context.data,
				    control->context.settings);

	control->defer_update = false;
}

void obs_control_update(obs_control_t *control, obs_data_t *settings)
{
	if (!obs_control_valid(control, "obs_control_update"))
		return;

	if (settings)
		obs_data_apply(control->context.settings, settings);

	
	control->info.update(control->context.data,control->context.settings);
	
}

void obs_control_update_properties(obs_control_t *control)
{
	if (!obs_control_valid(control, "obs_control_update_properties"))
		return;

	obs_control_dosignal(control, NULL, "update_properties");
}

void obs_control_send_mouse_click(obs_control_t *control,
				 const struct obs_mouse_event *event,
				 int32_t type, bool mouse_up,
				 uint32_t click_count)
{
	if (!obs_control_valid(control, "obs_control_send_mouse_click"))
		return;

	
		if (control->info.mouse_click) {
			control->info.mouse_click(control->context.data, event,
						 type, mouse_up, click_count);
		}
	
}

void obs_control_send_mouse_move(obs_control_t *control,
				const struct obs_mouse_event *event,
				bool mouse_leave)
{
	if (!obs_control_valid(control, "obs_control_send_mouse_move"))
		return;

	
		if (control->info.mouse_move) {
			control->info.mouse_move(control->context.data, event,
						mouse_leave);
		}
	
}








static void activate_tree(obs_control_t *parent, obs_control_t *child,
			  void *param)
{
	os_atomic_inc_long(&child->activate_refs);

	UNUSED_PARAMETER(parent);
	UNUSED_PARAMETER(param);
}

static void deactivate_tree(obs_control_t *parent, obs_control_t *child,
			    void *param)
{
	os_atomic_dec_long(&child->activate_refs);

	UNUSED_PARAMETER(parent);
	UNUSED_PARAMETER(param);
}

static void show_tree(obs_control_t *parent, obs_control_t *child, void *param)
{
	os_atomic_inc_long(&child->show_refs);

	UNUSED_PARAMETER(parent);
	UNUSED_PARAMETER(param);
}

static void hide_tree(obs_control_t *parent, obs_control_t *child, void *param)
{
	os_atomic_dec_long(&child->show_refs);

	UNUSED_PARAMETER(parent);
	UNUSED_PARAMETER(param);
}

void obs_control_activate(obs_control_t *control, enum view_type type)
{


	os_atomic_inc_long(&control->show_refs);
	obs_control_enum_active_tree(control, show_tree, NULL);

	if (type == MAIN_VIEW) {
		os_atomic_inc_long(&control->activate_refs);
		obs_control_enum_active_tree(control, activate_tree, NULL);
	}
}

void obs_control_deactivate(obs_control_t *control, enum view_type type)
{
	if (!obs_control_valid(control, "obs_control_deactivate"))
		return;

	if (os_atomic_load_long(&control->show_refs) > 0) {
		os_atomic_dec_long(&control->show_refs);
		obs_control_enum_active_tree(control, hide_tree, NULL);
	}

	if (type == MAIN_VIEW) {
		if (os_atomic_load_long(&control->activate_refs) > 0) {
			os_atomic_dec_long(&control->activate_refs);
			obs_control_enum_active_tree(control, deactivate_tree,
						    NULL);
		}
	}
}




obs_data_t *obs_control_get_settings(const obs_control_t *control)
{
	if (!obs_control_valid(control, "obs_control_get_settings"))
		return NULL;

	obs_data_addref(control->context.settings);
	return control->context.settings;
}


const char *obs_control_get_name(const obs_control_t *control)
{
	return obs_control_valid(control, "obs_control_get_name")
		       ? control->context.name
		       : NULL;
}

void obs_control_set_name(obs_control_t *control, const char *name)
{
	if (!obs_control_valid(control, "obs_control_set_name"))
		return;

	if (!name || !*name || !control->context.name ||
	    strcmp(name, control->context.name) != 0) {
		struct calldata data;
		char *prev_name = bstrdup(control->context.name);
		obs_context_data_setname(&control->context, name);

		calldata_init(&data);
		calldata_set_ptr(&data, "control", control);
		calldata_set_string(&data, "new_name", control->context.name);
		calldata_set_string(&data, "prev_name", prev_name);
		if (!control->context.private)
			signal_handler_signal(obs->signals, "control_rename",
					      &data);
		signal_handler_signal(control->context.signals, "rename", &data);
		calldata_free(&data);
		bfree(prev_name);
	}
}

enum obs_control_type obs_control_get_type(const obs_control_t *control)
{
	return obs_control_valid(control, "obs_control_get_type")
		       ? control->info.type
		       : OBS_CONTROL_TYPE_INPUT;
}

const char *obs_control_get_id(const obs_control_t *control)
{
	return obs_control_valid(control, "obs_control_get_id") ? control->info.id
							     : NULL;
}

const char *obs_control_get_unversioned_id(const obs_control_t *control)
{
	return obs_control_valid(control, "obs_control_get_unversioned_id")
		       ? control->info.unversioned_id
		       : NULL;
}


signal_handler_t *obs_control_get_signal_handler(const obs_control_t *control)
{
	return obs_control_valid(control, "obs_control_get_signal_handler")
		       ? control->context.signals
		       : NULL;
}

proc_handler_t *obs_control_get_proc_handler(const obs_control_t *control)
{
	return obs_control_valid(control, "obs_control_get_proc_handler")
		       ? control->context.procs
		       : NULL;
}

struct control_enum_data {
	obs_control_enum_proc_t enum_callback;
	void *param;
};

static void enum_control_active_tree_callback(obs_control_t *parent,
					     obs_control_t *child, void *param)
{
	struct control_enum_data *data = param;

	if (child->info.enum_active_controls) {
		if (child->context.data) {
			child->info.enum_active_controls(
				child->context.data,
				enum_control_active_tree_callback, data);
		}
	}

	data->enum_callback(parent, child, data->param);
}

void obs_control_enum_active_controls(obs_control_t *control,
				    obs_control_enum_proc_t enum_callback,
				    void *param)
{
	if (!data_valid(control, "obs_control_enum_active_controls"))
		return;



	obs_control_addref(control);

	if (control->info.enum_active_controls)
		control->info.enum_active_controls(control->context.data,
						 enum_callback, param);

	obs_control_release(control);
}

void obs_control_enum_active_tree(obs_control_t *control,
				 obs_control_enum_proc_t enum_callback,
				 void *param)
{
	struct control_enum_data data = {enum_callback, param};

	if (!data_valid(control, "obs_control_enum_active_tree"))
		return;


	obs_control_addref(control);

	if (control->info.enum_active_controls)
		control->info.enum_active_controls(
			control->context.data, enum_control_active_tree_callback,
			&data);

	obs_control_release(control);
}

static void enum_control_full_tree_callback(obs_control_t *parent,
					   obs_control_t *child, void *param)
{
	struct control_enum_data *data = param;

	if (child->info.enum_all_controls) {
		if (child->context.data) {
			child->info.enum_active_controls(
				child->context.data,
				enum_control_full_tree_callback, data);
		}
	} else if (child->info.enum_active_controls) {
		if (child->context.data) {
			child->info.enum_active_controls(
				child->context.data,
				enum_control_full_tree_callback, data);
		}
	}

	data->enum_callback(parent, child, data->param);
}

static void obs_control_enum_full_tree(obs_control_t *control,
				      obs_control_enum_proc_t enum_callback,
				      void *param)
{
	struct control_enum_data data = {enum_callback, param};

	if (!data_valid(control, "obs_control_enum_active_tree"))
		return;

	obs_control_addref(control);


	if (control->info.enum_all_controls) {
		control->info.enum_all_controls(control->context.data,
					      enum_control_full_tree_callback,
					      &data);

	} else if (control->info.enum_active_controls) {
		control->info.enum_active_controls(control->context.data,
						 enum_control_full_tree_callback,
						 &data);
	}

	obs_control_release(control);
}

struct descendant_info {
	bool exists;
	obs_control_t *target;
};

static void check_descendant(obs_control_t *parent, obs_control_t *child,
			     void *param)
{
	struct descendant_info *info = param;
	if (child == info->target || parent == info->target)
		info->exists = true;
}

bool obs_control_add_active_child(obs_control_t *parent, obs_control_t *child)
{
	struct descendant_info info = {false, parent};

	if (!obs_ptr_valid(parent, "obs_control_add_active_child"))
		return false;
	if (!obs_ptr_valid(child, "obs_control_add_active_child"))
		return false;
	if (parent == child) {
		blog(LOG_WARNING, "obs_control_add_active_child: "
				  "parent == child");
		return false;
	}

	obs_control_enum_full_tree(child, check_descendant, &info);
	if (info.exists)
		return false;

	for (int i = 0; i < parent->show_refs; i++) {
		enum view_type type;
		type = (i < parent->activate_refs) ? MAIN_VIEW : AUX_VIEW;
		obs_control_activate(child, type);
	}

	return true;
}

void obs_control_remove_active_child(obs_control_t *parent, obs_control_t *child)
{
	if (!obs_ptr_valid(parent, "obs_control_remove_active_child"))
		return;
	if (!obs_ptr_valid(child, "obs_control_remove_active_child"))
		return;

	for (int i = 0; i < parent->show_refs; i++) {
		enum view_type type;
		type = (i < parent->activate_refs) ? MAIN_VIEW : AUX_VIEW;
		obs_control_deactivate(child, type);
	}
}

void obs_control_save(obs_control_t *control)
{
	if (!data_valid(control, "obs_control_save"))
		return;

	obs_control_dosignal(control, "control_save", "save");

	if (control->info.save)
		control->info.save(control->context.data,
				  control->context.settings);
}

void obs_control_load(obs_control_t *control)
{
	if (!data_valid(control, "obs_control_load"))
		return;
	if (control->info.load)
		control->info.load(control->context.data,
				  control->context.settings);

	obs_control_dosignal(control, "control_load", "load");
}



bool obs_control_enabled(const obs_control_t *control)
{
	return obs_control_valid(control, "obs_control_enabled") ? control->enabled
							      : false;
}

void obs_control_set_enabled(obs_control_t *control, bool enabled)
{
	struct calldata data;
	uint8_t stack[128];

	if (!obs_control_valid(control, "obs_control_set_enabled"))
		return;

	control->enabled = enabled;

	calldata_init_fixed(&data, stack, sizeof(stack));
	calldata_set_ptr(&data, "control", control);
	calldata_set_bool(&data, "enabled", enabled);

	signal_handler_signal(control->context.signals, "enable", &data);
}

void *obs_control_get_type_data(obs_control_t *control)
{
	return obs_control_valid(control, "obs_control_get_type_data")
		       ? control->info.type_data
		       : NULL;
}


uint32_t obs_control_get_last_obs_version(const obs_control_t *control)
{
	return obs_control_valid(control, "obs_control_get_last_obs_version")
		       ? control->last_obs_ver
		       : 0;
}

