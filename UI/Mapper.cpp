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
QString ControlMapper::BroadcastControlEvent(QString input, obs_data_t *inputAction,
					     QString output,
					     obs_data_t *outputAction)
{
	emit(ControlMapper::EventCall(input,inputAction,
				      output,outputAction));
	return QString("return");
}
bool isJSon(QString val) {
	return (val.startsWith(QChar('[')) || val.startsWith(QChar('{')));
}
int ControlMapper::MappingExists(obs_data_t* mapping)
{
	
	bool exit = false;
	int count = obs_data_array_count(MapArray);
	obs_data_set_string(mapping, "value", "");
	
	
	for (int i = 0; i < count; i++) {
		obs_data_t *item = obs_data_array_item(MapArray, i);
		obs_data_t *listitem = obs_data_create_from_json(
			obs_data_get_string(item, "triggerstring"));
			obs_data_set_string(listitem, "value", "");
			
		
		if (obs_data_get_json(mapping) ==
			    obs_data_get_json(listitem)) {
			exit = true;
			return i;
		} 
		obs_data_release(listitem);
		obs_data_release(item);
	}
	return -1;
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
	obs_data_set_string(data, "triggerstring", obs_data_get_json(CurrentTriggerString));
	obs_data_set_string(data, "actiontype", CurrentActionType.toStdString().c_str());

	obs_data_set_string(data, "actionstring",   obs_data_get_json(CurrentActionString));
	//	obs_data_array_push_back(MapArray, data);
	int size = obs_data_array_count(MapArray);
	obs_data_array_insert(MapArray, size, data);
	obs_data_t *newdata = obs_data_create();
	obs_data_set_array(newdata, "mapper", MapArray);

	
	config_set_string(MapConfig, SECTION_NAME, PARAM_DEVICES,obs_data_get_json(newdata));
	config_save(MapConfig);
	obs_data_release(data);
	obs_data_release(newdata);
	//LoadMapping();
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
			obs_data_create_from_json(obs_data_get_string(data, "triggerstring")),
			QString(obs_data_get_string(data, "actiontype")),
			obs_data_create_from_json(obs_data_get_string(data, "actionstring"))));
		blog(1, "mapping load");
	}
	
	return true;
}
config_t* ControlMapper::GetMappingStore()
{
	return obs_frontend_get_global_config();
}

void ControlMapper::UpdateTrigger(QString type,obs_data_t *triggerstring) {
	PreviousTriggerString = CurrentTriggerString;
	PreviousTriggerType = CurrentTriggerType;
	CurrentTriggerString = triggerstring;
	CurrentTriggerType = type;
	triggerEvent(type, triggerstring);
}
void ControlMapper::deleteEntry(int entry) {
	
	MapConfig = GetMappingStore();
	obs_data_array_erase(MapArray, entry);
	obs_data_t *newdata = obs_data_create();
	obs_data_set_array(newdata, "mapper", MapArray);
	config_set_string(MapConfig, SECTION_NAME, PARAM_DEVICES, obs_data_get_json(newdata));
	config_save(MapConfig);
	obs_data_release(newdata);
}
void ControlMapper::triggerEvent(QString type, obs_data_t *triggerstring )
{
	if (MappingExists(triggerstring)!=-1) {
		blog(1, "mapping exists  ");
		obs_data_set_string(CurrentActionString, "value",
				    obs_data_get_string(triggerstring,
							"value"));
	} else {
		blog(1, "mapping doesnt exist  ");
	}
	
	
}
void ControlMapper::UpdateAction(QString type, obs_data_t *outputstring)
{
	PreviousActionString = CurrentActionString;
	PreviousActionType = CurrentActionType;
	CurrentActionString = outputstring;
	CurrentActionType = type;
}
