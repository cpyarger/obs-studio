#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <cmath>
#include "mapper.hpp"
#include <qjsonarray.h>
#include <qjsonobject.h>
#include <qjsondocument.h>
#if __has_include(<obs-frontend-api.h>)

#include <obs-frontend-api.h>
#else
#include <obs-frontend-api/obs-frontend-api.h>
#endif
#define SECTION_NAME "MAPPER"
#define PARAM_DEBUG "DebugEnabled"
#define PARAM_ALERT "AlertsEnabled"
#define PARAM_DEVICES "Mapping"
#define DEFUALT_MAPPING "{\"mapping\": []}"
ControlMapper::ControlMapper()
	: DebugEnabled(false), AlertsEnabled(true), SettingsLoaded(false)
{
	MapConfig = GetMappingStore();
	SetDefaults();
}
ControlMapper::~ControlMapper() {
	obs_data_array_release(MapArray);
}
QString ControlMapper::BroadcastControlEvent(QString input, QString inputAction,
					     QString output,
					     QString outputAction)
{
	emit(ControlMapper::EventCall(input,inputAction,
				      output,outputAction));
	return QString("return");
}
bool isJSon(QString val) {
	return (val.startsWith(QChar('[')) || val.startsWith(QChar('{')));
}
bool ControlMapper::MappingExists(QString mapping)
{
	QString teststring; 
	if (isJSon(mapping)){
		obs_data_t *incoming = obs_data_create_from_json(mapping.toStdString().c_str());
		obs_data_set_string(incoming, "value", "");
		blog(1, "test+%s", obs_data_get_json(incoming));
		teststring = obs_data_get_json(incoming);
		obs_data_release(incoming);
	} else {
		teststring = mapping;
	}
	
	bool exit=false;
	int count = obs_data_array_count(MapArray);
	for (int i = 0; i < count; i++) {
		obs_data_t *item = obs_data_array_item(MapArray, i);
		obs_data_t *listitem = obs_data_create();
		QString TriggerString = obs_data_get_string(item, "triggerstring");
		QString itemstring; 
		if (isJSon(TriggerString))
		{
			listitem = obs_data_create_from_json(TriggerString.toStdString().c_str());
			obs_data_set_string(listitem, "value", "");
			itemstring = obs_data_get_json(listitem);
		} else {
			itemstring = TriggerString;
		}
		if (teststring == itemstring) {
			exit = true;
			return true;
		} 
		obs_data_release(listitem);
		obs_data_release(item);

	}
	return false;
}
void ControlMapper::SetDefaults()
{
	// OBS Config defaults
	
	if (MapConfig) {
		config_set_default_bool(MapConfig, SECTION_NAME, PARAM_DEBUG,
					DebugEnabled);
		config_set_default_bool(MapConfig, SECTION_NAME, PARAM_ALERT,
					AlertsEnabled);
		config_set_default_string(MapConfig, SECTION_NAME,
					  PARAM_DEVICES, DEFUALT_MAPPING);
	}
}
bool ControlMapper::SaveMapping()
{
	emit(AddRowToTable(CurrentTriggerType, CurrentTriggerString,
			   CurrentActionType, CurrentActionString));
	MapConfig = GetMappingStore();
	obs_data_t *data = obs_data_create();
	config_set_bool(MapConfig, SECTION_NAME, PARAM_DEBUG, DebugEnabled);
	config_set_bool(MapConfig, SECTION_NAME, PARAM_ALERT, AlertsEnabled);

	obs_data_set_string(data, "triggertype", CurrentTriggerType.toStdString().c_str());
	obs_data_set_string(data, "triggerstring", CurrentTriggerString.toStdString().c_str());
	obs_data_set_string(data, "actiontype", CurrentActionType.toStdString().c_str());
	obs_data_set_string(data, "actionstring", CurrentActionString.toStdString().c_str());
//	obs_data_array_push_back(MapArray, data);
	int size = obs_data_array_count(MapArray);
	obs_data_array_insert(MapArray, size, data);
	obs_data_t *newdata = obs_data_create();
	obs_data_set_array(newdata, "mapper", MapArray);

	
	config_set_string(MapConfig, SECTION_NAME, PARAM_DEVICES,obs_data_get_json(newdata));
	config_save(MapConfig);
	obs_data_release(data);
	obs_data_release(newdata);
	
	return true;
}
bool ControlMapper::SaveMappings()
{

	MapConfig = GetMappingStore();

	obs_data_t *newdata = obs_data_create();
	obs_data_set_array(newdata, "mapper", MapArray);
	config_set_string(MapConfig, SECTION_NAME, PARAM_DEVICES,obs_data_get_json(newdata));
	config_save(MapConfig);
	obs_data_release(newdata);
	
	return true;
}

bool ControlMapper::LoadMapping()
{
	SetDefaults();
	MapConfig = GetMappingStore();

	DebugEnabled = config_get_bool(MapConfig, SECTION_NAME, PARAM_DEBUG);
	AlertsEnabled = config_get_bool(MapConfig, SECTION_NAME, PARAM_ALERT);

	obs_data_t *Data = obs_data_create_from_json(
		config_get_string(
			MapConfig, SECTION_NAME, PARAM_DEVICES));
	
	MapArray = obs_data_get_array(Data, "mapper");
	for (int i = 0; i < obs_data_array_count(MapArray); i++) {
		auto data = obs_data_array_item(MapArray, i);
		emit(AddRowToTable(QString(obs_data_get_string(data, "triggertype")),
			QString(obs_data_get_string(data, "triggerstring")),
			QString(obs_data_get_string(data, "actiontype")),
			QString(obs_data_get_string(data, "actionstring"))));
		blog(1, "mapping load");
	}
	return true;
}
config_t* ControlMapper::GetMappingStore()
{
	return obs_frontend_get_global_config();
}

void ControlMapper::UpdateTrigger(QString type,QString inputstring) {
	PreviousTriggerString = CurrentTriggerString;
	PreviousTriggerType = CurrentTriggerType;
	CurrentTriggerString = inputstring;
	CurrentTriggerType = type;
	triggerEvent(type, inputstring);
}
void ControlMapper::deleteEntry(int entry) {
	
	MapConfig = GetMappingStore();
	obs_data_array_erase(MapArray, entry);
	obs_data_t *newdata = obs_data_create();
	obs_data_set_array(newdata, "mapper", MapArray);
	config_set_string(MapConfig, SECTION_NAME, PARAM_DEVICES,
			  obs_data_get_json(newdata));
	config_save(MapConfig);
	obs_data_release(newdata);
}
void ControlMapper::triggerEvent(QString type, QString outputstring)
{
	if (MappingExists(outputstring)) {
		blog(1, "mapping exists  ");
	} else {
		blog(1, "mapping doesnt exist  ");
	}
	
	
}
void ControlMapper::UpdateAction(QString type, QString outputstring)
{
	PreviousActionString = CurrentActionString;
	PreviousActionType = CurrentActionType;
	CurrentActionString = outputstring;
	CurrentActionType = type;
}
