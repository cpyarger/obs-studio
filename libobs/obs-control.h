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

#pragma once

#include "obs.h"

/**
 * @file
 * @brief header for modules implementing controls.
 *
 * Controls are modules that either feed data to libobs or modify it.
 */

#ifdef __cplusplus
extern "C" {
#endif

enum obs_control_type {
	OBS_CONTROL_TYPE_INPUT,
	OBS_CONTROL_TYPE_OUTPUT,
};

enum obs_default_controls_types {
	SOURCE,
	SCENE,
	FILTER,
	OUTPUT,
	CONTROL,
	MEDIA,
	FRONTEND,
};


/** @} */

typedef void (*obs_control_enum_proc_t)(obs_control_t *parent,
				       obs_control_t *child, void *param);


/**
 * Control definition structure
 */
struct obs_control_info {
	/* ----------------------------------------------------------------- */
	/* Required implementation*/

	/** Unique string identifier for the control */
	const char *id;

	/**
	 * Type of control.
	 *
	 * OBS_CONTROL_TYPE_INPUT for input controls,
	 * OBS_CONTROL_TYPE_FILTER for filter controls, and
	 * OBS_CONTROL_TYPE_TRANSITION for transition controls.
	 */
	enum obs_control_type type;

	/** Control output flags */

	/**
	 * Get the translated name of the control type
	 *
	 * @param  type_data  The type_data variable of this structure
	 * @return               The translated name of the control type
	 */
	const char *(*get_name)(void *type_data);

	/**
	 * Creates the control data for the control
	 *
	 * @param  settings  Settings to initialize the control with
	 * @param  control    Control that this data is associated with
	 * @return           The data associated with this control
	 */
	void *(*create)(obs_data_t *settings, obs_control_t *control);

	/**
	 * Destroys the private data for the control
	 *
	 * Async controls must not call obs_control_output_video after returning
	 * from destroy
	 */
	void (*destroy)(void *data);


	/* ----------------------------------------------------------------- */
	/* Optional implementation */

	/**
	 * Gets the default settings for this control
	 *
	 * @param[out]  settings  Data to assign default settings to
	 * @deprecated            Use get_defaults2 if type_data is needed
	 */
	void (*get_defaults)(obs_data_t *settings);

	/**
	 * Gets the property information of this control
	 *
	 * @return         The properties data
	 * @deprecated     Use get_properties2 if type_data is needed
	 */
	obs_properties_t *(*get_properties)(void *data);

	/**
	 * Updates the settings for this control
	 *
	 * @param data      Control data
	 * @param settings  New settings for this control
	 */
	void (*update)(void *data, obs_data_t *settings);

	/** Called when the control is visible */
	void (*show)(void *data);

	/** Called when the control is no longer visible */
	void (*hide)(void *data);

	
	/**
	 * Called to enumerate all active controls being used within this
	 * control.  If the control has children that render audio/video it must
	 * implement this callback.
	 *
	 * @param  data           Filter data
	 * @param  enum_callback  Enumeration callback
	 * @param  param          User data to pass to callback
	 */
	void (*enum_active_controls)(void *data,
				    obs_control_enum_proc_t enum_callback,
				    void *param);

	/**
	 * Called when saving a control.  This is a separate function because
	 * sometimes a control needs to know when it is being saved so it
	 * doesn't always have to update the current settings until a certain
	 * point.
	 *
	 * @param  data      Control data
	 * @param  settings  Settings
	 */
	void (*save)(void *data, obs_data_t *settings);

	/**
	 * Called when loading a control from saved data.  This should be called
	 * after all the loading controls have actually been created because
	 * sometimes there are controls that depend on each other.
	 *
	 * @param  data      Control data
	 * @param  settings  Settings
	 */
	void (*load)(void *data, obs_data_t *settings);

	/**
	 * Private data associated with this entry
	 */
	void *type_data;

	/**
	 * If defined, called to free private data on shutdown
	 */
	void (*free_type_data)(void *type_data);



	/**
	 * Gets the default settings for this control
	 * 
	 * If get_defaults is also defined both will be called, and the first
	 * call will be to get_defaults, then to get_defaults2.
	 *
	 * @param       type_data The type_data variable of this structure
	 * @param[out]  settings  Data to assign default settings to
	 */
	void (*get_defaults2)(void *type_data, obs_data_t *settings);

	/**
	 * Gets the property information of this control
	 *
	 * @param data      Control data
	 * @param type_data The type_data variable of this structure
	 * @return          The properties data
	 */
	obs_properties_t *(*get_properties2)(void *data, void *type_data);


	/** Icon type for the control */
	enum obs_icon_type icon_type;

	
	void (*enum_all_controls)(void *data,
				  obs_control_enum_proc_t enum_callback,
				  void *param);
	/* version-related stuff */
	uint32_t version; /* increment if needed to specify a new version */
	const char *unversioned_id; /* set internally, don't set manually */
};

EXPORT void obs_register_control_s(const struct obs_control_info *info,
				  size_t size);



/**
 * Registers a control definition to the current obs context.  This should be
 * used in obs_module_load.
 *
 * @param  info  Pointer to the control definition structure
 */
#define obs_register_control(info) \
	obs_register_control_s(info, sizeof(struct obs_control_info))

#ifdef __cplusplus
}
#endif
