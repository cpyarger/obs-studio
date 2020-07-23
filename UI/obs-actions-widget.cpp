#if __has_include(<obs-frontend-api.h>)
#include <obs-frontend-api.h>
#else
#include <obs-frontend-api/obs-frontend-api.h>
#endif
#include <obs-data.h>
#include <string>
#include <map>
#include <functional>
#include <iostream>
#include <utility>

#include <inttypes.h>
#include <QtCore/QDir>
#include <QtCore/QUrl>
#include <obs.hpp>
#include <util/platform.h>

#include "obs-actions-widget.hpp"
#include <qdialogbuttonbox.h>
#include <qcheckbox.h>
#include <QWidget>
#include <QMainWindow>
#include "utils.hpp"
#include "window-basic-main.hpp"

OBSActionsWidget::OBSActionsWidget() : ui(new Ui::OBSActionsWidget) {
	ui->setupUi(this);
	/**************connect labels and comboboxes***************/
	ui->label_advanced->hide();
	HidePair("extra_1");
	HidePair("transition");
	HidePair("audio_source");
	HidePair("media_source");
	HidePair("filter");
	HidePair("scene");
	HidePair("source");
	HidePair("item");
	ui->pb_override->hide();
	ui->cb_obs_output_audio_source->addItems(Utils::GetAudioSourceNames());
	ui->cb_obs_output_media_source->addItems(Utils::GetMediaSourceNames());
	ui->cb_obs_output_transition->addItems(Utils::GetTransitionsList());
	OBSBasic *main = (OBSBasic* )obs_frontend_get_main_window();
	ControlMapper *map = main->mapper;
	
	connect(map, SIGNAL(EditAction(QString, obs_data_t*)), this,
		SLOT(EditAction(QString, obs_data_t*)));
	connect(ui->cb_obs_action, SIGNAL(currentIndexChanged(int)), this,
		SLOT(obs_actions_filter_select(int)));
	connect(ui->cb_obs_output_scene, SIGNAL(currentTextChanged(QString)),
		this, SLOT(GetSources(QString)));
	connect(ui->cb_obs_output_action, SIGNAL(currentTextChanged(QString)), this, SLOT(obs_actions_select(QString)));
	connect(ui->cb_obs_output_scene, SIGNAL(currentTextChanged(QString)), this, SLOT(GetSources(QString)));
	connect(ui->cb_obs_output_source, SIGNAL(currentTextChanged(QString)), this, SLOT(GetFilters(QString)));
	connect(ui->check_advanced, SIGNAL(toggled(bool)), this, SLOT(check_advanced_switch(bool)));

	//connect all combos to on change
	connect(ui->cb_obs_output_action, SIGNAL(currentTextChanged(QString)),this, SLOT(onChange()));
	connect(ui->cb_obs_output_source, SIGNAL(currentTextChanged(QString)),this, SLOT(onChange()));
	connect(ui->cb_obs_output_source, SIGNAL(currentTextChanged(QString)),this, SLOT(on_source_change(QString)));
	connect(ui->cb_obs_output_scene, SIGNAL(currentTextChanged(QString)),this, SLOT(on_scene_change(QString)));
	connect(ui->cb_obs_output_scene, SIGNAL(currentTextChanged(QString)),this, SLOT(onChange()));
	connect(ui->cb_obs_output_item, SIGNAL(currentTextChanged(QString)),this, SLOT(onChange()));
	connect(ui->cb_obs_output_filter, SIGNAL(currentTextChanged(QString)),this, SLOT(onChange()));
	connect(ui->cb_obs_output_transition, SIGNAL(currentTextChanged(QString)),this, SLOT(onChange()));
	connect(ui->cb_obs_output_audio_source, SIGNAL(currentTextChanged(QString)),this, SLOT(onChange()));
	connect(ui->cb_obs_output_media_source, SIGNAL(currentTextChanged(QString)),this, SLOT(onChange()));
	connect(map, SIGNAL(ResetToDefaults()), this, SLOT(ResetToDefaults()));
	connect(this, SIGNAL(changed(QString, obs_data_t *)), map,
		SLOT(UpdateAction(QString, obs_data_t *)));
	setStyleSheet(
		"QComboBox { min-width: 60px; }"
		"QComboBox QAbstractItemView { min-height: 100px;}");
	this->listview = new QListView(this->ui->cb_obs_output_action);
	this->ui->cb_obs_output_action->setView(this->listview);
	this->ui->cb_obs_output_action->addItems(AllActions);
	this->listview->setSizeAdjustPolicy(
		QAbstractScrollArea::SizeAdjustPolicy::AdjustToContents);
	ui->cb_obs_output_action->setSizeAdjustPolicy(QComboBox::SizeAdjustPolicy::AdjustToContents);
}

OBSActionsWidget::~OBSActionsWidget()
{
	delete ui;
}
void OBSActionsWidget::ResetToDefaults()
{
	ui->cb_obs_output_action->setCurrentIndex(0);
	ui->cb_obs_action->setCurrentIndex(0);
	ui->cb_obs_output_transition->setCurrentIndex(0);
	ui->cb_obs_output_filter->setCurrentIndex(0);
	ui->cb_obs_output_scene->setCurrentIndex(0);
	ui->cb_obs_output_source->setCurrentIndex(0);
	ui->cb_obs_output_audio_source->setCurrentIndex(0);
	ui->cb_obs_output_media_source->setCurrentIndex(0);

}
QStringList OBSActionsWidget::GetTransitions() {
	return Utils::GetTransitionsList();

}
void OBSActionsWidget::ShowPair(QString Pair)
{
	if (Pair == "scene") {
		ui->label_obs_output_scene->show();
		ui->cb_obs_output_scene->show();
		ui->cb_obs_output_scene->addItems(GetScenes());
	}else if (Pair == "source") {
		ui->label_obs_output_source->show();
		ui->cb_obs_output_source->show();
		ui->cb_obs_output_source->addItems(GetSources(ui->cb_obs_output_scene->currentText()));
	} else if (Pair == "filter") {
		ui->label_obs_output_filter->show();
		ui->cb_obs_output_filter->show();
		ui->cb_obs_output_filter->addItems(
			GetFilters(ui->cb_obs_output_source->currentText()));
	} else if (Pair == "transition") {
		ui->label_obs_output_transition->show();
		ui->cb_obs_output_transition->show();
		
	} else if (Pair == "item") {
		ui->label_obs_output_item->show();
		ui->cb_obs_output_item->show();
	} else if (Pair == "audio_source") {
		ui->label_obs_output_audio_source->show();
		ui->cb_obs_output_audio_source->show();
		
	} else if (Pair == "media_source") {
		ui->label_obs_output_media_source->show();
		ui->cb_obs_output_media_source->show();
	} else if (Pair == "extra_1") {
		ui->label_obs_output_extra_1->show();
		ui->cb_obs_output_extra_1->show();
	}

}
void OBSActionsWidget::HidePair(QString Pair) {
	if (Pair == "scene") {
		ui->label_obs_output_scene->hide();
		ui->cb_obs_output_scene->hide();
	}else if (Pair == "source") {
		ui->label_obs_output_source->hide();
		ui->cb_obs_output_source->hide();
	} else if (Pair == "filter") {
		ui->label_obs_output_filter->hide();
		ui->cb_obs_output_filter->hide();
	} else if (Pair == "transition") {
		ui->label_obs_output_transition->hide();
		ui->cb_obs_output_transition->hide();
	} else if (Pair == "item") {
		ui->label_obs_output_item->hide();
		ui->cb_obs_output_item->hide();
	} else if (Pair == "audio_source") {
		ui->label_obs_output_audio_source->hide();
		ui->cb_obs_output_audio_source->hide();
	} else if (Pair == "media_source") {
		ui->label_obs_output_media_source->hide();
		ui->cb_obs_output_media_source->hide();
	} else if (Pair == "extra_1") {
		ui->label_obs_output_extra_1->hide();
		ui->cb_obs_output_extra_1->hide();
	}

}


	void OBSActionsWidget::ShowIntActions() {}
void OBSActionsWidget::on_source_change(QString source) {
		ui->cb_obs_output_filter->clear();
		ui->cb_obs_output_filter->addItems(GetFilters(source));
	}
void OBSActionsWidget::on_scene_change(QString scene) {
		ui->cb_obs_output_source->clear();
		ui->cb_obs_output_source->addItems(GetSources(scene));
	}
void OBSActionsWidget::ShowStringActions() {}
	void OBSActionsWidget::ShowBoolActions() {}
void OBSActionsWidget::ShowOnly(QStringList shows)
{
	int count = AllActions.count();
	for (int i = 0; i < count; i++) {
		if (shows.contains(AllActions.at(i))) {
			ShowEntry(AllActions.at(i));
		} else {
			HideEntry(AllActions.at(i));
		}
	}
	

}
QString OBSActionsWidget::FirstVisible() {
	int count = AllActions.count();
	for (int i = 0; i < count; i++) {
		if (!listview->isRowHidden(i)) {
			return AllActions.at(i);
		}
	}

}
 void OBSActionsWidget::ShowEntry(QString Entry) {
	 int x = AllActions.indexOf(Entry);
	 if (x == -1) {
		 blog(1, "no entry -- %s", Entry.toStdString().c_str());
	 } else {
		 listview->setRowHidden(x, false);
		 listview->adjustSize();
		 ui->cb_obs_output_action->adjustSize();
	 }
	 
 }
void OBSActionsWidget::HideEntry(QString Entry) {
	int x = AllActions.indexOf(Entry);
	if (x == -1) {
		blog(1, "no entry -- %s", Entry.toStdString().c_str());
	} else {
		listview->setRowHidden(x, true);
		listview->adjustSize();
		ui->cb_obs_output_action->adjustSize();
	}
	
}
void OBSActionsWidget::ShowAllActions() {
	int count = ui->cb_obs_output_action->count();
	for (int i = 0; i < count; i++) {	
	ShowEntry(AllActions.at(i));
	}
}
void OBSActionsWidget::HideEntries(QStringList entrys) {
	int count = ui->cb_obs_output_action->count();

	for (int i = 0; i < count; i++) {
		if (entrys.contains(AllActions.at(i))) {
			HideEntry(AllActions.at(i));
		}
	}
	listview->adjustSize();
}
void OBSActionsWidget::ShowEntries(QStringList entrys)
{
	int count = ui->cb_obs_output_action->count();

	for (int i = 0; i < count; i++) {
		if (entrys.contains(AllActions.at(i))) {
			ShowEntry(AllActions.at(i));
		}
	}
	listview->adjustSize();
}

bool OBSActionsWidget::MapCall(QString plugin, obs_data_t* map)
{
	if (plugin == "OBS") {
		return DoMap(map);
	} else {
		return false;
	}
}
bool OBSActionsWidget::DoMap(obs_data_t* map)
{
		//make map into data array
		//pull action from data array
		//map action based on actionsMap

	return false;
}
void OBSActionsWidget::HideAdvancedActions()
{
	HideEntries(AdvancedFilterActions);
	HideEntries(AdvancedMediaActions);
	HideEntries(AdvancedSceneActions);
	HideEntries(AdvancedSourceActions);
}
QStringList OBSActionsWidget::GetSources(QString scene)
{
	
	SL_sources.clear();
	ui->cb_obs_output_source->clear();
	auto arrayref = Utils::GetSceneArray(scene);
	int size = obs_data_array_count(arrayref);
	for (int i = 0; i < size; i++) {
		obs_data *item = obs_data_array_item(arrayref, i);
		
		SL_sources.append(QString(obs_data_get_string(item, "name")));
		obs_data_release(item);

	}
	SL_sources.sort();
	if (!switching) {
		ui->cb_obs_output_source->addItems(SL_sources);
	
	}
	obs_data_array_release(arrayref);
	return SL_sources;
}
QStringList OBSActionsWidget::GetScenes()
{
	obs_data_array *x = Utils::GetScenes();
	int cnt = obs_data_array_count(x);
	for (int i = 0; i <= cnt; i++) {
		auto it = obs_data_array_item(x, i);
		SL_scenes.append(obs_data_get_string(it, "name"));
		obs_data_release(it);
	}	
	obs_data_array_release(x);
	return SL_scenes;
}
QStringList OBSActionsWidget::GetFilters(QString source) {
	
	ui->cb_obs_output_filter->clear();
	SL_filters.clear();

	auto x = obs_get_source_by_name(source.toStdString().c_str());
	OBSDataArrayAutoRelease y = Utils::GetSourceFiltersList(x, false);
	for (int i = 0; i < obs_data_array_count(y); i++) {
	OBSDataAutoRelease z=	 obs_data_array_item(y, i);
	SL_filters.append(QString(obs_data_get_string(z, "name")));
	}
	if (!switching) {
		ui->cb_obs_output_filter->addItems(SL_filters);
	}
	return SL_filters;
}
void OBSActionsWidget::check_advanced_switch(bool state) {
	obs_actions_filter_select(ui->cb_obs_action->currentIndex());
}
void OBSActionsWidget::obs_actions_filter_select(int selection)
{
	switching = true;

	switch (selection) {
		case 0:
			//All filters
			if (ui->check_advanced->isChecked()) {
				ShowAllActions();
			} else {
				ShowAllActions();
				HideAdvancedActions();
			}
		case 1:
			// Frontend
			ShowOnly(FrontendActions);

			break;
		case 2:
			// Scenes
			ShowOnly(sceneActions);

			if (ui->check_advanced->isChecked()) {
				ShowEntries(AdvancedSceneActions);
			}
			break;
		case 3:
			//Sources
			ShowOnly(sourceActions);
			if (ui->check_advanced->isChecked()) {
				ShowEntries(AdvancedSourceActions);
			}
			break;
		case 4:
			//Filters
			ShowOnly(filterActions);
			if (ui->check_advanced->isChecked()) {
				ShowEntries(AdvancedFilterActions);
			}
			break;
		case 5:
			//Media
			ShowOnly(mediaActions);
			if (ui->check_advanced->isChecked()) {
				ShowEntries(AdvancedMediaActions);
			}
			break;
	};
		ui->cb_obs_output_action->setCurrentText(FirstVisible());
	switching = false;
	
}







void OBSActionsWidget::EditAction(QString type,obs_data_t* actions) {
if (type == "OBS") {
	
		
		ui->cb_obs_output_action->setCurrentText(
		QString(obs_data_get_string(actions, "action")));
	ui->cb_obs_output_scene->setCurrentText(
		QString(obs_data_get_string(actions, "scene")));
		ui->cb_obs_output_audio_source->setCurrentText(
			QString(obs_data_get_string(actions, "audio_source")));
	blog(1, "EditAction-- widget -- %s",
	     obs_data_get_string(actions, "action"));
	
}

}
void OBSActionsWidget::onChange() {
	obs_data_t *data = obs_data_create();
	obs_data_set_string(data, "action",ui->cb_obs_output_action->currentText().toStdString().c_str());
	obs_data_set_string(data, "scene",ui->cb_obs_output_scene->currentText().toStdString().c_str());
	obs_data_set_string(data, "source",ui->cb_obs_output_source->currentText().toStdString().c_str());
	obs_data_set_string(data, "filter",ui->cb_obs_output_filter->currentText().toStdString().c_str());
	obs_data_set_string(data, "transition",ui->cb_obs_output_transition->currentText().toStdString().c_str());
	obs_data_set_string(data, "item",ui->cb_obs_output_item->currentText().toStdString().c_str());
	obs_data_set_string(data, "audio_source",ui->cb_obs_output_audio_source->currentText().toStdString().c_str());
	obs_data_set_string(data, "media_source",ui->cb_obs_output_media_source->currentText().toStdString().c_str());
	emit(changed("OBS", data));
	//obs_data_release(data);

}


void OBSActionsWidget::obs_actions_select(QString action)
{
	HidePair("extra_1");
	HidePair("transition");
	HidePair("audio_source");
	HidePair("media_source");
	HidePair("filter");
	HidePair("scene");
	HidePair("source");
	HidePair("item");
	ui->label_advanced->hide();
	std::map<QString,
		 std::function<void(QString action, Ui::OBSActionsWidget * wid,
				    OBSActionsWidget * here)>>
		funcMap = {{"Set Current Scene",
			    [](QString action, Ui::OBSActionsWidget *wid,
			       OBSActionsWidget *here) {
				    here->ShowPair("scene");
			    }},
			   {"Start Streaming",
			    [](QString action, Ui::OBSActionsWidget *wid,
			       OBSActionsWidget *here) {

			    }},
			   {"Stop Streaming",
			    [](QString action, Ui::OBSActionsWidget *wid,
			       OBSActionsWidget *here) {}},
			   {"Toggle Start/Stop Streaming",
			    [](QString action, Ui::OBSActionsWidget *wid,
			       OBSActionsWidget *here) {}},
			   {"Start Recording",
			    [](QString action, Ui::OBSActionsWidget *wid,
			       OBSActionsWidget *here) {}},
			   {"Stop Recording",
			    [](QString action, Ui::OBSActionsWidget *wid,
			       OBSActionsWidget *here) {}},
			   {"Pause Recording",
			    [](QString action, Ui::OBSActionsWidget *wid,
			       OBSActionsWidget *here) {}},
			   {"Unpause Recording",
			    [](QString action, Ui::OBSActionsWidget *wid,
			       OBSActionsWidget *here) {}},
			   {"Start Replay Buffer",
			    [](QString action, Ui::OBSActionsWidget *wid,
			       OBSActionsWidget *here) {}},
			   {"Stop Replay Buffer",
			    [](QString action, Ui::OBSActionsWidget *wid,
			       OBSActionsWidget *here) {}},
			   {"Enable Preview",
			    [](QString action, Ui::OBSActionsWidget *wid,
			       OBSActionsWidget *here) {}},
			   {"Disable Preview",
			    [](QString action, Ui::OBSActionsWidget *wid,
			       OBSActionsWidget *here) {}},
			   {"Studio Mode",
			    [](QString action, Ui::OBSActionsWidget *wid,
			       OBSActionsWidget *here) {}},
			   {"Transition",
			    [](QString action, Ui::OBSActionsWidget *wid,
			       OBSActionsWidget *here) {}},
			   {"Reset Stats",
			    [](QString action, Ui::OBSActionsWidget *wid,
			       OBSActionsWidget *here) {}},
			   //source
			   {"Enable Source Filter",
			    [](QString action, Ui::OBSActionsWidget *wid,
			       OBSActionsWidget *here) {
				    here->ShowPair("scene");
				    here->ShowPair("source");
				    here->ShowPair("filter");


			    }},
			   {"Disable Source Filter",
			    [](QString action, Ui::OBSActionsWidget *wid,
			       OBSActionsWidget *here) {
				    here->ShowPair("scene");
				    here->ShowPair("source");
				    here->ShowPair("filter");

			    }},
			   {"Set Gain Filter",
			    [](QString action, Ui::OBSActionsWidget *wid,
			       OBSActionsWidget *here) {
				    here->ShowPair("scene");
				    here->ShowPair("source");
				    here->ShowPair("filter");

	
			    }},
			   {"Toggle Source Filter",
			    [](QString action, Ui::OBSActionsWidget *wid,
			       OBSActionsWidget *here) {
				    here->ShowPair("scene");
				    here->ShowPair("source");
				    here->ShowPair("filter");

		
			    }},
			   {"Reset Scene Item",
			    [](QString action, Ui::OBSActionsWidget *wid,
			       OBSActionsWidget *here) {
				    wid->cb_obs_output_scene->addItems(
					    here->GetScenes());
				    here->ShowPair("scene");
				    here->ShowPair("item");
			    }},
			   {"Set Scene Item Render",
			    [](QString action, Ui::OBSActionsWidget *wid,
			       OBSActionsWidget *here) {
				    here->ShowPair("scene");
				    here->ShowPair("item");
				    
			    }},
			   {"Set Scene Item Position",
			    [](QString action, Ui::OBSActionsWidget *wid,
			       OBSActionsWidget *here) {
				    here->ShowPair("scene");
				    here->ShowPair("item");
			
			    }},
			   {"Set Scene Item Transform",
			    [](QString action, Ui::OBSActionsWidget *wid,
			       OBSActionsWidget *here) {
	
				    here->ShowPair("scene");
				    here->ShowPair("item");
			    }},
			   {"Set Scene Item Crop",
			    [](QString action, Ui::OBSActionsWidget *wid,
			       OBSActionsWidget *here) {

				    here->ShowPair("scene");
				    here->ShowPair("item");
			    }},
			   {"Set Current Scene",
			    [](QString action, Ui::OBSActionsWidget *wid,
			       OBSActionsWidget *here) {
				   
				    here->ShowPair("scene");

			    }},
			   {"Set Scene Transition Override",
			    [](QString action, Ui::OBSActionsWidget *wid,
			       OBSActionsWidget *here) {
				    here->ShowPair("scene");
				    here->ShowPair("transition");
			    }},
			   {"Set Current Transition",
			    [](QString action, Ui::OBSActionsWidget *wid,
			       OBSActionsWidget *here) {
				    here->ShowPair("transition");
			    }},
			   {"Set Volume",
			    [](QString action, Ui::OBSActionsWidget *wid,
			       OBSActionsWidget *here) {
				    here->ShowPair("audio_source");
				    wid->label_advanced->show();
			    }},
			   {"Set Mute",
			    [](QString action, Ui::OBSActionsWidget *wid,
			       OBSActionsWidget *here) {
				    here->ShowPair("audio_source");
			    }},
			   {"Toggle Mute",
			    [](QString action, Ui::OBSActionsWidget *wid,
			       OBSActionsWidget *here) {
				    here->ShowPair("audio_source");
			    }},
			   {"Set Source Name",
			    [](QString action, Ui::OBSActionsWidget *wid,
			       OBSActionsWidget *here) {}},
			   {"Set Sync Offset",
			    [](QString action, Ui::OBSActionsWidget *wid,
			       OBSActionsWidget *here) {}},
			   {"Set Source Settings",
			    [](QString action, Ui::OBSActionsWidget *wid,
			       OBSActionsWidget *here) {}},
			   {"Set Source Filter Visibility",
			    [](QString action, Ui::OBSActionsWidget *wid,
			       OBSActionsWidget *here) {
				    here->ShowPair("source");
				    here->ShowPair("filter");

			    }},
			   {"Set Audio Monitor Type",
			    [](QString action, Ui::OBSActionsWidget *wid,
			       OBSActionsWidget *here) {}},
			   {"Take Source Screenshot",
			    [](QString action, Ui::OBSActionsWidget *wid,
			       OBSActionsWidget *here) {
				    here->ShowPair("source");
				    here->ShowPair("scene");
			    }},
			   {"Play/Pause Media",
			    [](QString action, Ui::OBSActionsWidget *wid,
			       OBSActionsWidget *here) {
				    here->ShowPair("media_source");
			    }},
			   {"Restart Media",
			    [](QString action, Ui::OBSActionsWidget *wid,
			       OBSActionsWidget *here) {
				    here->ShowPair("media_source");
			    }},
			   {"Stop Media",
			    [](QString action, Ui::OBSActionsWidget *wid,
			       OBSActionsWidget *here) {
				    here->ShowPair("media_source");
			    }},
			   {"Next Media",
			    [](QString action, Ui::OBSActionsWidget *wid,
			       OBSActionsWidget *here) {
				    here->ShowPair("media_source");
			    }},
			   {"Previous Media",
			    [](QString action, Ui::OBSActionsWidget *wid,
			       OBSActionsWidget *here) {
				    here->ShowPair("media_source");
			    }},
			   {"Set Media Time",
			    [](QString action, Ui::OBSActionsWidget *wid,
			       OBSActionsWidget *here) {
				    here->ShowPair("media_source");
			    }},
			   {"Scrub Media",
			    [](QString action, Ui::OBSActionsWidget *wid,
			       OBSActionsWidget *here) {
				    here->ShowPair("media_source");
			    }}

		};
	try {
		funcMap[action](action, ui, this);
	} catch (std::exception &e) {
		blog(LOG_DEBUG, "error %s", e.what());
	}
}
