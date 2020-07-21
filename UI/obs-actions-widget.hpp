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
	QStringList AdvancedFilterActions = {"* Set Gain Filter"};
				     

	QStringList sceneActions = {"Reset Scene Item",
				    "Set Current Scene",
				    "Set Scene Transition Override",
				    "Set Current Transition"};
	QStringList AdvancedSceneActions = {
				    "* Set Scene Item Render",
				    "* Set Scene Item Position",
				    "* Set Scene Item Transform",
				    "* Set Scene Item Crop"};
	

	QStringList sourceActions = {"Toggle Mute",
				     "Take Source Screenshot"};

	QStringList AdvancedSourceActions = {"* Set Volume",
					     "* Set Mute",
					     "* Set Source Name",
					     "* Set Sync Offset",
					     "* Set Source Settings",
					     "* Set Source Filter Visibility",
					     "* Set Audio Monitor Type"};

	QStringList mediaActions = {"Play/Pause Media",
				    "Restart Media",
				    "Stop Media",
				    "Next Media",
				    "Previous Media"};

	QStringList AdvancedMediaActions = {"* Set Media Time",
					    "* Scrub Media"};

	void ShowPair(QString pair);
	void HidePair(QString pair);

public slots:
	bool MapCall(QString plugin, obs_data_t* map);
private slots:
	QStringList GetSources(QString scene);
	void obs_actions_filter_select(int);
	QStringList GetFilters(QString Source);
	void obs_actions_select(QString action);
	void check_advanced_switch(bool state);
	
	void EditAction(QString ActionType, obs_data_t* TriggerType);
	void onChange();
	signals:
	void changed(QString type, obs_data_t *change);

private:
	Ui::OBSActionsWidget *ui;

	
	
	};
