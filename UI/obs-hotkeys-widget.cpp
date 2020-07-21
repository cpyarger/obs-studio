#include <util/dstr.hpp>
#include <QPointer>
#include <QStyle>

#include "obs-app.hpp"
#include "qt-wrappers.hpp"
#include "window-basic-main.hpp"
#include "obs-hotkeys-widget.hpp"

static inline bool operator!=(const obs_key_combination_t &c1,
			      const obs_key_combination_t &c2)
{
	return c1.modifiers != c2.modifiers || c1.key != c2.key;
}

static inline bool operator==(const obs_key_combination_t &c1,
			      const obs_key_combination_t &c2)
{
	return !(c1 != c2);
}

OBSHotkeysWidget::OBSHotkeysWidget() : ui(new Ui::OBSHotkeysWidget)
{
	ui->setupUi(this);

	OBSBasic *main = (OBSBasic *)obs_frontend_get_main_window();
	ControlMapper *map = main->mapper;

	connect(map, SIGNAL(EditTrigger(QString, obs_data_t*)), this,
		SLOT(EditTrigger(QString, obs_data_t*)));

	connect(ui->keySequenceEdit, SIGNAL(keySequenceChanged(QKeySequence)), this,
		SLOT(dothing(QKeySequence)));
	connect(this, SIGNAL(updated(QString, obs_data_t*)), map,
		SLOT(UpdateTrigger(QString, obs_data_t*)));
}
OBSHotkeysWidget::~OBSHotkeysWidget() {
	delete ui;
}



void OBSHotkeysWidget::dothing(QKeySequence edit)
{
	 obs_data_t *keys = obs_data_create();

	 obs_data_set_string(keys, "Hotkey",
			     edit.toString().toStdString().c_str());
	emit(updated("Hotkeys", keys));
}
void OBSHotkeysWidget::EditTrigger(QString type,obs_data_t* trigger) {
	
	if (type == "Hotkeys") {
		
		ui->keySequenceEdit->setKeySequence(QKeySequence(obs_data_get_string(trigger,"Hotkey")));
	}

	emit(updated("Hotkeys", trigger));
}

