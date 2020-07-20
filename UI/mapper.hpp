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
using std::vector;
EXPORT typedef struct Mapping
{
	QString input;
	QString inputAction;
	QString output;
	QString outputAction;
}Mapping;
class ControlMapper : public QObject {
	Q_OBJECT

public:

	ControlMapper();
	~ControlMapper();

	bool DebugEnabled=true;
	bool AlertsEnabled=false;

	bool SettingsLoaded;
	bool LoadMapping();
	config_t* GetMappingStore();
	
	QString CurrentTriggerString="";
	QString CurrentTriggerType="";
	QString CurrentActionString="";
	QString CurrentActionType="";

	QString PreviousTriggerString;
	QString PreviousTriggerType;
	QString PreviousActionString;
	QString PreviousActionType;
	bool MappingExists(QString triggerstring);
signals:
	QString EventCall(QString input, QString inputAction, QString output,
			  QString outputAction);


	void EditTrigger(QString TriggerType, QString TriggerString);
	void EditAction(QString ActionType, QString TriggerType);
	void AddRowToTable(QString TriggerType, QString TriggerString,
			   QString ActionType, QString ActionString);
public slots
		:
	void UpdateTrigger(QString type,QString string);
	void UpdateAction(QString type,QString string);
	bool SaveMapping();
	bool SaveMappings();
	void triggerEvent(QString triggertype, QString triggerstring);
	QString BroadcastControlEvent(QString input, QString inputAction,
				      QString output, QString outputAction);
	void deleteEntry(int row);

private:
	void SetDefaults();
	config_t *MapConfig;
	obs_data_array_t *MapArray;
};
