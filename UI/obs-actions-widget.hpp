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
#include <mapper.hpp>
#include <obs.hpp>
#include "ui_OBSActionsWidget.h"
#include <QtWidgets/qwidget.h>
namespace Ui {
class OBSActionsWidget;
}


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


	QStringList GetScenes();
	QStringList *items;
	QString sceneName;
	bool switching = false;
	bool DoMap(obs_data_t* map);
	QStringList FrontendActions = {"Start Streaming",
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

	QStringList filterActions = {"Enable Source Filter",
				     "Disable Source Filter",
				     "Toggle Source Filter"};
	
				     

	QStringList sceneActions = {"Reset Scene Item",
				    "Set Current Scene",
				    "Set Scene Transition Override",
				    "Set Current Transition"};

	QStringList sourceActions = {"Toggle Mute",
				     "Take Source Screenshot"};



	QStringList mediaActions = {"Play/Pause Media",
				    "Restart Media",
				    "Stop Media",
				    "Next Media",
				    "Previous Media"};


	QStringList AllActions = {"Disable Preview",
				  "Disable Source Filter",
				  "Enable Preview",
				  "Enable Source Filter",
				  "Next Media",
				  "Pause Recording",
				  "Play/Pause Media",
				  "Previous Media",
				  "Reset Scene Item",
				  "Reset Stats",
				  "Restart Media",
				  "Set Audio Monitor Type",
				  "Set Current Scene",
				  "Set Current Transition",
				  "Set Gain Filter",
				  "Set Media Time",
				  "Set Mute",
				  "Set Scene Item Crop",
				  "Set Scene Item Position",
				  "Set Scene Item Render",
				  "Set Scene Item Transform",
				  "Set Scene Transition Override",
				  "Set Source Filter Visibility",
				  "Set Source Name",
				  "Set Source Settings",
				  "Set Sync Offset",
				  "Set Volume",
				  "Start Recording",
				  "Start Replay Buffer",
				  "Start Streaming",
				  "Stop Media",
				  "Stop Recording",
				  "Stop Replay Buffer",
				  "Stop Streaming",
				  "Studio Mode",
				  "Take Source Screenshot",
				  "Toggle Mute",
				  "Toggle Source Filter",
				  "Toggle Start/Stop Streaming",
				  "Transition",
				  "Unpause Recording"};
	QStringList AdvancedSourceActions = {
					     "Set Mute",
					     "Set Source Name",
					     "Set Sync Offset",
					     "Set Source Settings",
					     "Set Source Filter Visibility",
					     "Set Audio Monitor Type"};
	QStringList AdvancedMediaActions = {
					    "Scrub Media"};

	QStringList AdvancedFilterActions = {"Set Gain Filter"};
	QStringList AdvancedSceneActions = {"Set Scene Item Render",
					    "Set Scene Item Position",
					    "Set Scene Item Transform",
					    "Set Scene Item Crop"};
	
	QStringList intActions = {
		"Set Volume",
		"Set Media Time",
	};
	void ShowIntActions();
	void ShowStringActions();
	void ShowBoolActions();
	void ShowOnly(QStringList shows);
	void ShowEntry(QString Entry);
	void HideEntry(QString Entry);
	void ShowAllActions();
	void HideAdvancedActions();
	void HideEntries(QStringList entrys);
	void ShowEntries(QStringList entrys);
	QString FirstVisible();

	void ShowPair(QString pair);
	void HidePair(QString pair);

public slots:
	bool MapCall(QString plugin, obs_data_t* map);
private slots:
	QStringList GetSources(QString scene);
	void obs_actions_filter_select(int);
	QStringList GetFilters(QString Source);
	QStringList GetTransitions();
	void obs_actions_select(QString action);
	void check_advanced_switch(bool state);
	
	void EditAction(QString ActionType, obs_data_t* TriggerType);
	void ResetToDefaults();
	void onChange();
	void on_source_change(QString source);
	void on_scene_change(QString source);
signals:
	void changed(QString type, obs_data_t *change);


private:
	Ui::OBSActionsWidget *ui;
	QListView *listview;
	
	
	
	};
