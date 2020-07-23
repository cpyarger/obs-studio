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
	void DoAction(obs_data_t*actionString);
	
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
};
