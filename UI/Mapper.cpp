#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <cmath>
#include "mapper.hpp"
#include <qevent.h>
#include <util/dstr.hpp>
#include <QPointer>
#include <QStyle>
#include "obs-app.hpp"
#include "qt-wrappers.hpp"
#include <obs.hpp>
#include <functional>

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
	controller = new Controller();
	connect(this, SIGNAL(DoAction(obs_data_t *)), controller, SLOT(execute(obs_data_t *)));
	hotkeyController = new HKC();
	connect(hotkeyController, SIGNAL(Trigger(QString,obs_data_t*)), this, SLOT(TriggerEvent(QString, obs_data_t*)));
	connect(this, SIGNAL(mapLoadAction(obs_data_t *)), hotkeyController,
		SLOT(loadmap(obs_data_t *)));
	LoadMapping();
}
HKC::HKC() {
	obs_hotkey_enable_background_press(true);
	
}

void HKC::AddHK(QKeySequence ks)
{
	
auto hotkey = new QHotkey(QKeySequence(ks), true, this);
qDebug() << "Is Registered: " << hotkey->isRegistered();
pressmap.insert(ks, false);
QObject::connect(hotkey, &QHotkey::activated, this, &HKC::DoQHK);
}
	

void HKC::DoQHK()
{

	//obs_data_t *data = obs_data_create();
	obs_data_t *data = obs_data_create();
	QHotkey *x = qobject_cast<QHotkey *>(sender());
	auto y = x->shortcut();
	if (!pressmap[y]) {
	
		blog(1, "Qhotkey Pressed  -- %s", y.toString().toStdString().c_str());
		obs_data_set_string(data, "Hotkey", y.toString().simplified().toStdString().c_str());
		emit(Trigger("Hotkeys", data));
		pressmap[y] = true;
	} else {
		blog(1, "Qhotkey Released  -- %s", y.toString().toStdString().c_str());
		pressmap[y] = false;
	}
}
void HKC::DoHK(obs_key_combination_t hotkey, bool pressed)
{
	obs_data_t * data = obs_data_create();
	DStr str;
	obs_key_combination_to_str(hotkey, str);
	auto tstring = QT_UTF8(str);	
	if (pressed) {
		blog(1, "hotkey pressed -- %s",tstring.toStdString().c_str());
		obs_data_set_string(data, "Hotkey",
				    tstring.simplified().toStdString().c_str());
		emit(Trigger("Hotkeys",data));
	}
	obs_data_release(data);
}

void HKC::loadmap(obs_data_t *map) {
	if (QString(obs_data_get_string(map, "triggertype")).simplified() == "Hotkeys") {
		obs_data_t *ts = obs_data_create_from_json(obs_data_get_string(map, "triggerstring"));
		
		AddHK(QKeySequence(obs_data_get_string(ts,"Hotkey")));
	}
}

HKC::~HKC()
{
	delete this;
}

ControlMapper::~ControlMapper()
{
	obs_data_array_release(MapArray);
	delete this;
}

bool isJSon(QString val) {
	return (val.startsWith(QChar('[')) || val.startsWith(QChar('{')));
}
void ControlMapper::SetDefaults()
{
	// OBS Config defaults
	obs_data_set_string(CurrentActionString, "action", "Start Streaming");

	if (MapConfig) {
		config_set_default_bool(MapConfig, SECTION_NAME, PARAM_DEBUG,
					DebugEnabled);
		config_set_default_bool(MapConfig, SECTION_NAME, PARAM_ALERT,
					AlertsEnabled);
		config_set_default_string(MapConfig, SECTION_NAME,
					  PARAM_DEVICES, DEFUALT_MAPPING);
	}
}

void ControlMapper::AddorEditMapping()
{
	
	MapConfig = GetMappingStore();
	
	
	obs_data_t *data = obs_data_create();
	config_set_bool(MapConfig, SECTION_NAME, PARAM_DEBUG, DebugEnabled);
	config_set_bool(MapConfig, SECTION_NAME, PARAM_ALERT, AlertsEnabled);

	obs_data_set_string(data, "triggertype", CurrentTriggerType.toStdString().c_str());
	obs_data_set_string(data, "triggerstring", obs_data_get_json(CurrentTriggerString));
	obs_data_set_string(data, "actiontype", CurrentActionType.toStdString().c_str());

	obs_data_set_string(data, "actionstring",   obs_data_get_json(CurrentActionString));
	//	obs_data_array_push_back(MapArray, data);
	if (!EditMode) {
		emit(AddTableRow(CurrentTriggerType, CurrentTriggerString,
				   CurrentActionType, CurrentActionString));
		int size = obs_data_array_count(MapArray);

		obs_data_array_insert(MapArray, size, data);
	} else {
		emit(EditTableRow(editRow,CurrentTriggerType, CurrentTriggerString,
				   CurrentActionType, CurrentActionString));
		obs_data_array_erase(MapArray, editRow);
		obs_data_array_insert(MapArray, editRow, data);
	}

	SaveMapping();
	LoadMapping();
	EditMode = false;
	editRow = -1;
}
void ControlMapper::SaveMapping() {
	obs_data_t *newdata = obs_data_create();
	obs_data_set_array(newdata, "mapper", MapArray);

	config_set_string(MapConfig, SECTION_NAME, PARAM_DEVICES,
			  obs_data_get_json(newdata));
	config_save(MapConfig);
	obs_data_release(newdata);
	

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
	emit(ClearTable());
	for (int i = 0; i < obs_data_array_count(MapArray); i++) {
		obs_data_t * data = obs_data_array_item(MapArray, i);
		emit(mapLoadAction(data));
		emit(AddTableRow(QString(obs_data_get_string(data, "triggertype")),
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


void ControlMapper::deleteEntry(int entry) {
	
	MapConfig = GetMappingStore();
	obs_data_array_erase(MapArray, entry);
	obs_data_t *newdata = obs_data_create();
	obs_data_set_array(newdata, "mapper", MapArray);
	config_set_string(MapConfig, SECTION_NAME, PARAM_DEVICES, obs_data_get_json(newdata));
	config_save(MapConfig);
	obs_data_release(newdata);
}


int ControlMapper::MappingExists(obs_data_t *mapping)
{

	int count = obs_data_array_count(MapArray);
	const char *X;
	if (isJSon(obs_data_get_string(mapping, "value"))) {
		X= obs_data_get_json(obs_data_create_from_json(
			obs_data_get_string(mapping, "value")));
	} else {
		X = "";
	}
	
	obs_data_set_string(mapping, "value", "");
	
	

	for (int i = 0; i < count; i++) {
		obs_data_t *item = obs_data_array_item(MapArray, i);
		obs_data_t *listitem = obs_data_create_from_json(obs_data_get_string(item, "triggerstring"));
		obs_data_set_string(listitem, "value", "");

		if (QString(obs_data_get_json(mapping)).simplified() == QString(obs_data_get_json(listitem)).simplified()) {
			obs_data_release(listitem);
			obs_data_set_string(mapping, "value", X);
			return i;
		}
		obs_data_release(listitem);

		
	}
	obs_data_set_string(mapping, "value", X);
	return -1;
}
void ControlMapper::TriggerEvent(QString type,obs_data_t *triggerstring )
{
	int x = MappingExists(triggerstring);
	if (x!=-1) {
		obs_data_t *data = obs_data_array_item(MapArray, x);
		obs_data_t *senddata = obs_data_create_from_json(obs_data_get_string(data, "actionstring"));
		obs_data_set_string(senddata, "value",obs_data_get_string(triggerstring, "value"));	   
		//controller->execute(senddata);
		emit(DoAction(senddata));
	} else {
	
	}	
}
void ControlMapper::UpdateTrigger(QString type, obs_data_t *triggerstring)
{
	PreviousTriggerString = CurrentTriggerString;
	PreviousTriggerType = CurrentTriggerType;
	CurrentTriggerString = triggerstring;
	CurrentTriggerType = type;
	//triggerEvent(type, triggerstring);
}
void ControlMapper::UpdateAction(QString type, obs_data_t *outputstring)
{
	PreviousActionString = CurrentActionString;
	PreviousActionType = CurrentActionType;
	CurrentActionString = outputstring;
	CurrentActionType = type;
}
