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

	connect(map, SIGNAL(EditTrigger(QString, QString)), this,
		SLOT(EditTrigger(QString, QString)));

	connect(ui->keySequenceEdit, SIGNAL(keySequenceChanged(QKeySequence)), this,
		SLOT(dothing(QKeySequence)));
	connect(this, SIGNAL(updated(QString, QString)), map,
		SLOT(UpdateTrigger(QString, QString)));
}
OBSHotkeysWidget::~OBSHotkeysWidget() {
	delete ui;
}



	void OBSHotkeysWidget::dothing(QKeySequence keys) {
	emit(updated("Hotkeys", keys.toString()));
}
void OBSHotkeysWidget::EditTrigger(QString type, QString trigger) {
	if (type == "Hotkeys") {
		blog(1, "OBS Trigger Edit -- %s -- %s",
		     type.toStdString().c_str(), trigger.toStdString().c_str());
		ui->keySequenceEdit->setKeySequence(QKeySequence(trigger));
	}
	emit(updated("Hotkeys", ui->keySequenceEdit->keySequence().toString()));
}

