/*
obs-midi

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program. If not, see <https://www.gnu.org/licenses/>
*/

#include <iostream>
#include "obs-frontend-api.h"
#include "obs-controller.hpp"
#include <QtCore/QSharedPointer>
#include <obs.hpp>
#include <QWidget>
#include <QPointer>
#include <QDoubleSpinBox>
#include <QStackedWidget>
#include <vector>
#include <map>
#include <functional>
using namespace std;

////////////////////
// BUTTON ACTIONS //
////////////////////

/**
 * Sets the currently active scene
 */
void OBSController::SetCurrentScene(const char *sceneName)
{
	OBSSourceAutoRelease source = obs_get_source_by_name(sceneName);

	if (source) {
		obs_frontend_set_current_scene(source);
	} else {
		throw("requested scene does not exist");
	}
}

/**
 * Sets the scene in preview. Must be in Studio mode or will throw error
 */
void OBSController::SetPreviewScene(const char *sceneName)
{
	if (!obs_frontend_preview_program_mode_active()) {
		throw("studio mode not enabled");
	}
	OBSScene scene = Utils::GetSceneFromNameOrCurrent(sceneName);
	if (!scene) {
		throw("specified scene doesn't exist");
	}

	obs_frontend_set_current_preview_scene(obs_scene_get_source(scene));
}

/**
 * Change the active scene collection.
 */
void OBSController::SetCurrentSceneCollection(const char * sceneCollection)
{
	if (sceneCollection =="") {
		throw("Scene Collection name is empty");
	}

	// TODO : Check if specified profile exists and if changing is allowed
	obs_frontend_set_current_scene_collection(sceneCollection);
}

/**
* Reset a scene item.
*/
void OBSController::ResetSceneItem(const char *sceneName, const char *itemName)
{
	OBSScene scene = Utils::GetSceneFromNameOrCurrent(sceneName);
	if (!scene) {
		throw("requested scene doesn't exist");
	}

	OBSDataAutoRelease params = obs_data_create();
	obs_data_set_string(params, "scene-name", sceneName);
	OBSDataItemAutoRelease itemField = obs_data_item_byname(params, "item");

	OBSSceneItemAutoRelease sceneItem =
		Utils::GetSceneItemFromRequestField(scene, itemField);
	if (!sceneItem) {
		throw("specified scene item doesn't exist");
	}

	OBSSource sceneItemSource = obs_sceneitem_get_source(sceneItem);

	OBSDataAutoRelease settings = obs_source_get_settings(sceneItemSource);
	obs_source_update(sceneItemSource, settings);
}

/**
 * Transitions the currently previewed scene to the main output.
 */
void OBSController::TransitionToProgram()
{
	obs_frontend_preview_program_trigger_transition();
}

/**
 * Transitions the currently previewed scene to the main output using specified transition.
 * transitionDuration is optional. (milliseconds)
 */
void OBSController::TransitionToProgram(const char * transitionName,
					int transitionDuration)
{
	if (!obs_frontend_preview_program_mode_active()) {
		throw("studio mode not enabled");
	}

	if (transitionName=="") {
		throw("transition name can not be empty");
	}
	bool success = Utils::SetTransitionByName(transitionName);
	if (!success) {
		throw("specified transition doesn't exist");
	}
	obs_frontend_set_transition_duration(transitionDuration);

	obs_frontend_preview_program_trigger_transition();
}

/**
 * Set the active transition.
 */
void OBSController::SetCurrentTransition(const char * name)
{
	bool success = Utils::SetTransitionByName(name);
	if (!success) {
		throw("requested transition does not exist");
	}
}

/**
 * Set the duration of the currently active transition
 */
void OBSController::SetTransitionDuration(int duration)
{
	obs_frontend_set_transition_duration(duration);
}

void OBSController::SetSourceVisibility() {} //DOESNT EXIST

void OBSController::ToggleSourceVisibility() {} //DOESNT EXIST

/**
* Inverts the mute status of a specified source.
*/
void OBSController::ToggleMute(const char * sourceName)
{
	if (sourceName == "") {
		throw("sourceName is empty");
	}

	OBSSourceAutoRelease source =
		obs_get_source_by_name(sourceName);
	if (!source) {
		throw("sourceName not found");
	}

	obs_source_set_muted(source, !obs_source_muted(source));
}

/**
 * Sets the mute status of a specified source.
 */
void OBSController::SetMute(const char * sourceName, bool mute)
{
	if (sourceName == "") {
		throw("sourceName is empty");
	}

	OBSSourceAutoRelease source =
		obs_get_source_by_name(sourceName);
	if (!source) {
		throw("specified source doesn't exist");
	}

	obs_source_set_muted(source, mute);
}

/**
 * Toggle streaming on or off.
 */
void OBSController::StartStopStreaming()
{
	if (obs_frontend_streaming_active())
		StopStreaming();
	else
		StartStreaming();
}

/**
 * Start streaming.
 */
void OBSController::StartStreaming()
{
	if (obs_frontend_streaming_active() == false) {
		obs_frontend_streaming_start();
	}
}

/**
 * Stop streaming.
 */
void OBSController::StopStreaming()
{
	if (obs_frontend_streaming_active() == true) {
		obs_frontend_streaming_stop();
	}
}

/**
 * Toggle recording on or off.
 */
void OBSController::StartStopRecording()
{
	(obs_frontend_recording_active() ? obs_frontend_recording_stop()
					 : obs_frontend_recording_start());
}

/**
 * Start recording.
 */
void OBSController::StartRecording()
{
	if (!obs_frontend_recording_active()) {
		obs_frontend_recording_start();
	}
}

/**
 * Stop recording.
 */
void OBSController::StopRecording()
{
	if (obs_frontend_recording_active()) {
		obs_frontend_recording_stop();
	}
}

/**
* Pause the current recording.
*/
void OBSController::PauseRecording()
{
	if (obs_frontend_recording_active()) {
		obs_frontend_recording_pause(true);
	}
}

/**
* Resume/unpause the current recording (if paused).
*/
void OBSController::ResumeRecording()
{
	if (obs_frontend_recording_active()) {
		obs_frontend_recording_pause(false);
	}
}

/**
* Toggle the Replay Buffer on/off.
*/
void OBSController::StartStopReplayBuffer()
{
	if (obs_frontend_replay_buffer_active()) {
		obs_frontend_replay_buffer_stop();
	} else {
		Utils::StartReplayBuffer();
	}
}

/**
* Start recording into the Replay Buffer.
* Will throw an error if "Save Replay Buffer" hotkey is not set in OBS' settings.
* Setting this hotkey is mandatory, even when triggering saves only
* through obs-midi.
*/
void OBSController::StartReplayBuffer()
{
	if (!Utils::ReplayBufferEnabled()) {
		throw("replay buffer disabled in settings");
	}

	if (obs_frontend_replay_buffer_active() == false) {
		Utils::StartReplayBuffer();
	}
}

/**
* Stop recording into the Replay Buffer.
*/
void OBSController::StopReplayBuffer()
{
	if (obs_frontend_replay_buffer_active() == true) {
		obs_frontend_replay_buffer_stop();
	}
}

/**
* Flush and save the contents of the Replay Buffer to disk. This is
* basically the same as triggering the "Save Replay Buffer" hotkey.
* Will return an `error` if the Replay Buffer is not active.
*/
void OBSController::SaveReplayBuffer()
{
	if (!obs_frontend_replay_buffer_active()) {
		throw("replay buffer not active");
	}

	OBSOutputAutoRelease replayOutput =
		obs_frontend_get_replay_buffer_output();

	calldata_t cd = {0};
	proc_handler_t *ph = obs_output_get_proc_handler(replayOutput);
	proc_handler_call(ph, "save", &cd);
	calldata_free(&cd);
}

void OBSController::SetCurrentProfile(const char * profileName)
{
	if (profileName == "") {
		throw("profile name is empty");
	}

	// TODO : check if profile exists
	obs_frontend_set_current_profile(profileName);
}

void OBSController::SetTextGDIPlusText() {}

void OBSController::SetBrowserSourceURL() {}

void OBSController::ReloadBrowserSource() {}

void OBSController::TakeSourceScreenshot(const char * source) {}

void OBSController::EnableSourceFilter() {}

void OBSController::DisableSourceFilter() {}

void OBSController::ToggleSourceFilter() {}

////////////////
// CC ACTIONS //
////////////////

void OBSController::SetVolume(const char * source, float volume)
{
	OBSSourceAutoRelease obsSource =
		obs_get_source_by_name(source);
	if (!obsSource) {
		return; // source does not exist
	}

	obs_source_set_volume(obsSource, volume);
}

/**
 * Set the audio sync offset of a specified source.
 */
void OBSController::SetSyncOffset(const char * sourceName, int64_t sourceSyncOffset)
{
	if (sourceName == "") {
		throw("source name is empty");
	}

	OBSSourceAutoRelease source =
		obs_get_source_by_name(sourceName);
	if (!source) {
		throw("specified source doesn't exist");
	}

	obs_source_set_sync_offset(source, sourceSyncOffset);
}

void OBSController::SetSourcePosition() {}

void OBSController::SetSourceRotation() {}

void OBSController::SetSourceScale() {}

void OBSController::SetGainFilter() {}

void OBSController::SetOpacity() {}

Controller::Controller() {}
Controller::~Controller() {}
// BUTTON ACTIONS
std::map<QString, function<void(obs_data_t *)>> funcMap = {
	{"control.action.Set_Current_Scene",
	 [](obs_data_t *data) {
		 OBSController::SetCurrentScene(
			 obs_data_get_string(data, "scene"));
	 }},
	{"control.action.Set_Preview_Scene",
	 [](obs_data_t *data) {
		 OBSController::SetPreviewScene(
			 obs_data_get_string(data, "scene"));
	 }},
	{"control.action.Set_Current_Scene_Collection",
	 [](obs_data_t *data) {
		 OBSController::SetCurrentSceneCollection(obs_data_get_string(data, "scene_collection"));
	 }},
	{"control.action.Reset_Scene_Item",
	 [](obs_data_t *data) {
		 OBSController::ResetSceneItem(
			 obs_data_get_string(data, "scene"),
			 obs_data_get_string(data, "item"));
	 }},
	{"control.action.Transition_To_Program",
	 [](obs_data_t *data) {
		
			  OBSController::TransitionToProgram();
		
	 }},
	{"control.action.Set_Current_Transition",
	 [](obs_data_t *data) {
		 OBSController::SetCurrentTransition(obs_data_get_string(data, "transition"));
	 }},
	{"control.action.Set_Transition_Duration",
	 [](obs_data_t *data) {
		OBSController::SetTransitionDuration(obs_data_get_int(data, "value"));
		
	 }
	 },
	{"control.action.Toggle_Mute",
	  [](obs_data_t *data) {
		  OBSController::ToggleMute(obs_data_get_string(data, "audio_source"));
	  }},
	{"control.action.Set_Mute",
	  [](obs_data_t *data) {
		  OBSController::SetMute(obs_data_get_string(data, "audio_source"), obs_data_get_int(data, "value"));
	  }},
	{"control.action.Start_Stop_Streaming",
	  [](obs_data_t *data) { OBSController::StartStopStreaming(); }},
	{"control.action.Start_Streaming",
	  [](obs_data_t *data) { OBSController::StartStreaming(); }},
	{"control.action.Stop_Streaming",
	 [](obs_data_t *data) { OBSController::StopStreaming(); }},
	{"control.action.Start_Stop_Recording",
	  [](obs_data_t *data) { OBSController::StartStopRecording(); }},
	{"control.action.Start_Recording",
	  [](obs_data_t *data) { OBSController::StartRecording(); }},
	{"control.action.Stop_Recording",
	  [](obs_data_t *data) { OBSController::StopRecording(); }},
	{"control.action.Pause_Recording",
	  [](obs_data_t *data) { OBSController::PauseRecording(); }},
	 {"control.action.Resume_Recording",
	  [](obs_data_t *data) { OBSController::ResumeRecording(); }},
	{"control.action.Start_Stop_Replay_Buffer",
	  [](obs_data_t *data) { OBSController::StartStopReplayBuffer(); }},
	 {"control.action.Start Replay Buffer",
	  [](obs_data_t *data) { OBSController::StartReplayBuffer(); }},
	{"control.action.Stop_Replay_Buffer",
	  [](obs_data_t *data) { OBSController::StopReplayBuffer(); }},
	 {"control.action.Save_Replay_Buffer",
	  [](obs_data_t *data) { OBSController::SaveReplayBuffer(); }},
	 {"control.action.Set_Current_Profile",
	  [](obs_data_t *data) {
		  OBSController::SetCurrentProfile(obs_data_get_string(data, "profile"));
	  }},
	 {"control.action.Set_Text_GDIPlus_Text",
	  [](obs_data_t *data) { OBSController::SetTextGDIPlusText(); }},
	 {"control.action.Set_Browser_Source_URL",
	  [](obs_data_t *data) { OBSController::SetBrowserSourceURL(); }},
	 {"control.action.Reload_Browser_Source",
	  [](obs_data_t *data) { OBSController::ReloadBrowserSource(); }},
	 {"control.action.Take_Source_Screenshot",
	  [](obs_data_t *data) {
		  OBSController::TakeSourceScreenshot(
			  obs_data_get_string(data, "source"));
	  }},
	 {"control.action.Enable_Source_Filter",
	  [](obs_data_t *data) { OBSController::EnableSourceFilter(); }},
	 {"control.action.Disable_Source_Filter",
	  [](obs_data_t *data) { OBSController::DisableSourceFilter(); }},
	 {"control.action.Toggle_Source_Filter",
	  [](obs_data_t *data) { OBSController::ToggleSourceFilter(); }},
	 // CC ACTIONS
	 {"control.action.Set_Volume",
	  [](obs_data_t *data) {
		 OBSDataAutoRelease val = obs_data_create_from_json(
			 obs_data_get_string(data, "value"));
		  OBSController::SetVolume(obs_data_get_string(data, "audio_source"),
			  pow(Utils::mapper(obs_data_get_int(val, "int")),3.0));
		 obs_data_release(val);
		 obs_data_release(data);
	  }},
	 {"control.action.Set_Sync_Offset",
	  [](obs_data_t *data) {
		  OBSController::SetSyncOffset(obs_data_get_string(data, "scene"),(int64_t)obs_data_get_int(data, "value"));
	  }},
	 {"control.action.Set_Source_Position",
	  [](obs_data_t *data) { OBSController::SetSourcePosition(); }},
	 {"control.action.Set_Source_Rotation",
	  [](obs_data_t *data) { OBSController::SetSourceRotation(); }},
	 {"control.action.Set_Source_Scale",
	  [](obs_data_t *data) { OBSController::SetSourceScale(); }},
	 {"control.action.Set_Gain_Filter",
	  [](obs_data_t *data) { OBSController::SetGainFilter(); }},
	 {"control.action.Set_Opacity",
	  [](obs_data_t *data) { OBSController::SetOpacity(); }}};


QString Controller::TranslateActions(obs_data_t *data)
{
	return tr(obs_data_get_string(data, "action"));
}

void Controller::execute(obs_data_t *data) {
	try {

		funcMap[QString(obs_data_get_string(data, "action"))](data);
	} catch (...) {
		blog(1, "Error Executing");
	}
}
