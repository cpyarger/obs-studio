#pragma once
#if __has_include(<obs-frontend-api.h>)
#include <obs-frontend-api.h>
#else
#include <obs-frontend-api/obs-frontend-api.h>
#endif
#include <util/config-file.h>
#include <QtCore/QString>
#include <QtCore/QSharedPointer>
#include <obs.hpp>
#include <QWidget>
#include <QPointer>
#include <QDoubleSpinBox>
#include <QStackedWidget>
#include <vector>
#include "obs-controller.hpp"
#include <src/qglobalshortcut.h>
#include <map>
/************************Hotkeys bits*************************/
class HKC : public QObject {
	Q_OBJECT

public:
	struct KPair {
		QKeySequence Sequence;
		QGlobalShortcut *gs;
		bool pressed;
	};
	HKC();
	~HKC();
	void DoHK(obs_key_combination_t event, bool pressed);
	void AddHK(QKeySequence hk);
	void remaphk(obs_data_t *map);
	std::vector<QGlobalShortcut*> qv;
signals:
	void EmitHotkey(QString actiontype, QString action);
	void Trigger(QString actiontype, obs_data_t *action);
public slots:
	void loadmap(obs_data_t *map);
	void DoQHK(QKeySequence hk);

};


class ControlMapper : public QObject {
	Q_OBJECT

public:

	ControlMapper();
	~ControlMapper();

	bool DebugEnabled=true;
	bool AlertsEnabled=false;
	int editRow=-1;
	bool EditMode=false;
	bool SettingsLoaded;
	bool LoadMapping();
	config_t* GetMappingStore();
	
	obs_data_t *CurrentTriggerString=obs_data_create();
	QString CurrentTriggerType="Hotkeys";
	obs_data_t *CurrentActionString=obs_data_create();
	QString CurrentActionType="OBS";

	obs_data_t *PreviousTriggerString;
	QString PreviousTriggerType;
	obs_data_t *PreviousActionString;
	QString PreviousActionType;
	int MappingExists(obs_data_t *triggerstring);
signals:



	void EditTrigger(QString TriggerType,obs_data_t *TriggerString);
	void EditAction(QString ActionType, obs_data_t *Action);
	void AddTableRow(QString TriggerType,obs_data_t *TriggerString,
			   QString ActionType,obs_data_t *actionstring);
	void EditTableRow(int row, QString TriggerType,obs_data_t *TriggerString,
			   QString ActionType,obs_data_t *actionstring);
	void ResetToDefaults();
	void ClearTable();
	void DoAction(obs_data_t*actionString);
	void mapLoadAction(obs_data_t *map);
public slots:
	void UpdateTrigger(QString type,obs_data_t *string);
	void UpdateAction(QString type,obs_data_t *string);
	void SaveMapping();
	void AddorEditMapping();
	void TriggerEvent(QString triggertype,obs_data_t *TriggerString);
	
	void deleteEntry(int row);


private:
	void SetDefaults();
	config_t *MapConfig;
	obs_data_array_t *MapArray;
	Controller *controller;
	
	public:
	HKC *hotkeyController;
	QStringList AllActions = {
		"control.action.Disable_Preview",
		"control.action.Disable_Source_Filter",
		"control.action.Enable_Preview",
		"control.action.Enable_Source_Filter",
		"control.action.Next_Media",
		"control.action.Pause_Recording",
		"control.action.Play_Pause_Media",
		"control.action.Previous_Media",
		"control.action.Reset_Scene_Item",
		"control.action.Reset_Stats",
		"control.action.Restart_Media",
		"control.action.Set_Audio_Monitor_Type",
		"control.action.Set_Current_Scene",
		"control.action.Set_Current_Transition",
		"control.action.Set_Gain_Filter",
		"control.action.Set_Media_Time",
		"control.action.Set_Mute",
		"control.action.Set_Scene_Item_Crop",
		"control.action.Set_Scene_Item_Position",
		"control.action.Set_Scene_Item_Render",
		"control.action.Set_Scene_Item_Transform",
		"control.action.Set_Scene_Transition_Override",
		"control.action.Set_Source_Filter_Visibility",
		"control.action.Set_Source_Name",
		"control.action.Set_Source_Settings",
		"control.action.Set_Sync_Offset",
		"control.action.Set_Volume",
		"control.action.Start_Recording",
		"control.action.Start_Replay_Buffer",
		"control.action.Start_Streaming",
		"control.action.Stop_Media",
		"control.action.Stop_Recording",
		"control.action.Stop_Replay_Buffer",
		"control.action.Stop_Streaming",
		"control.action.Studio_Mode",
		"control.action.Take_Source_Screenshot",
		"control.action.Toggle_Mute",
		"control.action.Toggle_Source_Filter",
		"control.action.Toggle_Start_Stop_Streaming",
		"control.action.Transition",
		"control.action.Unpause_Recording"};
};


