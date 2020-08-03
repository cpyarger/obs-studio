
#include <iostream>
#include <obs-module.h>
#if __has_include(<obs-frontend-api.h>)
#include <obs-frontend-api.h>
#else
#include <obs-frontend-api/obs-frontend-api.h>
#endif
#include <obs-data.h>
#include <string>
#include <map>
#include <iostream>
#include <utility>
#include "obs-hotkey.h"
#include <window-control.hpp>
#include "forms/obs-hotkeys-widget.hpp"
#include <QtWidgets/QAction>
#include <QtWidgets/QMainWindow>
#include <QIcon>
#include <util/dstr.h>
#include "qt-wrappers.hpp"
using namespace std;


OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("obs-hotkeys", "en-US")


void register_gui()
{
	QWidget *input = new OBSHotkeysWidget();
	QString *na = new QString("Hotkeys");
	obs_frontend_add_input_control(na, input);
}

bool obs_module_load(void)
{

	register_gui();
	return true;
}
void obs_module_unload()
{

	blog(LOG_INFO, "goodbye!");
}

/**********************Hotkey Controller************ MAKE INTO PLUGIN ********************************/

HKC::HKC()
{
	obs_hotkey_enable_background_press(true);
}

void HKC::AddHK(QKeySequence ks)
{
	if (!pressmap.contains(ks)) {
		auto x = new KPair();
		x->Sequence = ks;
		x->pressed = false;
		x->hotkey = new QHotkey(QKeySequence(ks), true, this);
		pressmap.insert(ks, x);
		qDebug() << "Is Registered: " << x->hotkey->isRegistered();
		QObject::connect(x->hotkey, &QHotkey::activated, this,
				 &HKC::DoQHK);
	}
}

void HKC::DoQHK()
{

	obs_data_t *data = obs_data_create();
	
	QHotkey *x = qobject_cast<QHotkey *>(sender());
	auto y = x->shortcut();
	//if (!pressmap[y]->pressed) {
	blog(1, "Qhotkey Pressed  -- %s", y.toString().toStdString().c_str());
	obs_data_set_string(data, "Hotkey",
			    y.toString().simplified().toStdString().c_str());
	//	pressmap[y]->pressed = true;
	emit(Trigger("Hotkeys", data));

	//	} else {
	// blog(1, "Qhotkey Released  -- %s", y.toString().toStdString().c_str());
	//  pressmap[y]->pressed = false;
	//	}
}


void HKC::loadmap(obs_data_t *map)
{
	if (QString(obs_data_get_string(map, "triggertype")).simplified() ==
	    "Hotkeys") {
		obs_data_t * ts = obs_data_create_from_json(
			obs_data_get_string(map, "triggerstring"));

		AddHK(QKeySequence(obs_data_get_string(ts, "Hotkey")));
	}
}

HKC::~HKC()
{

	QMapIterator<QKeySequence, KPair *> i(pressmap);
	while (i.hasNext()) {
		i.next();
		i.value()->hotkey->~QHotkey();
	}
	delete this;
}
