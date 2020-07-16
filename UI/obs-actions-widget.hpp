#pragma once
#include <util/util.hpp>
#include <QPointer>
#include <memory>
#include <string>


#include <vector>
#include <QtCore/QString>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLayout>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QSystemTrayIcon>

#include <obs.hpp>
#include "ui_OBSActionsWidget.h"
#include <QtWidgets/qwidget.h>
void ___source_dummy_addref(obs_source_t *);
void ___sceneitem_dummy_addref(obs_sceneitem_t *);
void ___data_dummy_addref(obs_data_t *);
void ___data_array_dummy_addref(obs_data_array_t *);
void ___output_dummy_addref(obs_output_t *);

using OBSSourceAutoRelease =
	OBSRef<obs_source_t *, ___source_dummy_addref, obs_source_release>;
using OBSSceneItemAutoRelease =
	OBSRef<obs_sceneitem_t *, ___sceneitem_dummy_addref,
	       obs_sceneitem_release>;
using OBSDataAutoRelease =
	OBSRef<obs_data_t *, ___data_dummy_addref, obs_data_release>;
using OBSDataArrayAutoRelease =
	OBSRef<obs_data_array_t *, ___data_array_dummy_addref,
	       obs_data_array_release>;
using OBSOutputAutoRelease =
	OBSRef<obs_output_t *, ___output_dummy_addref, obs_output_release>;


void ___data_item_dummy_addref(obs_data_item_t *);
void ___data_item_release(obs_data_item_t *);
using OBSDataItemAutoRelease =
	OBSRef<obs_data_item_t *, ___data_item_dummy_addref,
	       ___data_item_release>;
namespace Ui {
class OBSActionsWidget;
}
typedef void (*PauseRecordingFunction)(bool);
typedef bool (*RecordingPausedFunction)();

namespace Utils {
class OBSActionsWidget;
float mapper(int x);
int mapper2(double x);
bool is_number(const std::string &s);
std::string getMidiMessageType(int in);
QStringList GetMediaSourceNames();
QStringList GetAudioSourceNames();

QString nsToTimestamp(uint64_t ns);
obs_data_array_t *StringListToArray(char **strings, const char *key);
obs_data_array_t *GetSceneItems(obs_source_t *source);
obs_data_t *GetSceneItemData(obs_sceneitem_t *item);
OBSDataArrayAutoRelease GetSourceArray();
OBSDataArrayAutoRelease GetSceneArray(QString name=NULL);
	// These functions support nested lookup into groups
obs_sceneitem_t *GetSceneItemFromName(obs_scene_t *scene, QString name);
obs_sceneitem_t *GetSceneItemFromId(obs_scene_t *scene, int64_t id);
obs_sceneitem_t *GetSceneItemFromItem(obs_scene_t *scene, obs_data_t *item);
obs_sceneitem_t *GetSceneItemFromRequestField(obs_scene_t *scene,
					      obs_data_item_t *dataItem);

obs_scene_t *GetSceneFromNameOrCurrent(QString sceneName);
obs_data_t *GetSceneItemPropertiesData(obs_sceneitem_t *item);

obs_data_t *GetSourceFilterInfo(obs_source_t *filter, bool includeSettings);
obs_data_array_t *GetSourceFiltersList(obs_source_t *source,
				       bool includeSettings);

bool IsValidAlignment(const uint32_t alignment);

obs_data_array_t *GetScenes();
obs_data_t *GetSceneData(obs_source_t *source);

	// TODO contribute a proper frontend API method for this to OBS and remove this hack
int GetTransitionDuration(obs_source_t *transition);
obs_source_t *GetTransitionFromName(QString transitionName);
bool SetTransitionByName(QString transitionName);
obs_data_t *GetTransitionData(obs_source_t *transition);

QString OBSVersionString();



const char *GetRecordingFolder();
bool SetRecordingFolder(const char *path);

QString ParseDataToQueryString(obs_data_t *data);
obs_hotkey_t *FindHotkeyByName(QString name);

bool ReplayBufferEnabled();
void StartReplayBuffer();
bool IsRPHotkeySet();
const char *GetFilenameFormatting();
bool SetFilenameFormatting(const char *filenameFormatting);
bool inrange(int low, int high, int x);
};
	class OBSActionsWidget : public QWidget {
	Q_OBJECT
public:
	OBSActionsWidget();
	~OBSActionsWidget();

private:

	QGridLayout *layout;
	QStringList SL_filters = {};
	QStringList encoders = {};
	QStringList outputs = {};
	QStringList services = {};
	QStringList SL_scenes = {};
	QStringList SL_sources = {};
	QStringList FrontendActions = {	"Start Streaming",
					"Stop Streaming",
					"Toggle Start/Stop Streaming",
					"Start Recording",
					"Stop Recording",
					"Pause Recording",
					"Unpause Recording",
					"Start Replay Buffer",
					"Stop Replay Buffer",
					"Enable Preview",
					"Disable Preview",
					"Studio Mode",
					"Transition",
					"Reset Stats"};

	QStringList filterActions = {	"Enable Source Filter",
					"Disable Source Filter",
					"Set Gain Filter",
					"Toggle Source Filter"};

	QStringList sceneActions = {	"Reset Scene Item",
					"Set Scene Item Render",
					"Set Scene Item Position",
					"Set Scene Item Transform",
					"Set Scene Item Crop",
					"Set Current Scene",
					"Set Scene Transition Override",
					"Set Current Transition"};

	QStringList sourceActions = {	"Set Volume",
					"Set Mute",
					"Toggle Mute",
					"Set Source Name",
					"Set Sync Offset",
					"Set Source Settings",
					"Set Source Filter Visibility",
					"Set Audio Monitor Type",
					"Take Source Screenshot"};

	QStringList mediaActions = {	"Play/Pause Media",
					"Restart Media",
					"Stop Media",
					"Next Media",
					"Previous Media",
					"Set Media Time",
					"Scrub Media"};

	QStringList GetScenes();
	QStringList *items;
	QString sceneName;
	bool switching = false;
	bool DoMap(QString map);
public slots:
	bool MapCall(QString plugin, QString map);
private slots:
	QStringList GetSources(QString scene);
	void obs_actions_filter_select(int);
	QStringList GetFilters(QString Source);
	void obs_actions_select(QString action);

private:
	Ui::OBSActionsWidget *ui;

	
	
	};
