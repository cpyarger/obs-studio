#include <util/dstr.hpp>
#include <QPointer>
#include <QStyle>

#include "obs-app.hpp"
#include "qt-wrappers.hpp"

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
	InitSignalHandler();
}
OBSHotkeysWidget::~OBSHotkeysWidget() {
	delete ui;
}
void OBSHotkeysWidget::InitSignalHandler()
{
	layoutChanged = {
		obs_get_signal_handler(), "hotkey_layout_change",
		[](void *this_, calldata_t *) {
			auto edit = static_cast<OBSHotkeysWidget *>(this_);
			QMetaObject::invokeMethod(edit, "ReloadKeyLayout");
		},
		this};
}


void OBSHotkeysWidget::keyPressEvent(QKeyEvent *event)
{
	if (event->isAutoRepeat())
		return;

	obs_key_combination_t new_key;

	switch (event->key()) {
	case Qt::Key_Shift:
	case Qt::Key_Control:
	case Qt::Key_Alt:
	case Qt::Key_Meta:
		new_key.key = OBS_KEY_NONE;
		break;

#ifdef __APPLE__
	case Qt::Key_CapsLock:
		// kVK_CapsLock == 57
		new_key.key = obs_key_from_virtual_key(57);
		break;
#endif

	default:
		new_key.key =
			obs_key_from_virtual_key(event->nativeVirtualKey());
	}

	new_key.modifiers =
		TranslateQtKeyboardEventModifiers(event->modifiers());

	HandleNewKey(new_key);
}

#ifdef __APPLE__
void OBSHotkeyEdit::keyReleaseEvent(QKeyEvent *event)
{
	if (event->isAutoRepeat())
		return;

	if (event->key() != Qt::Key_CapsLock)
		return;

	obs_key_combination_t new_key;

	// kVK_CapsLock == 57
	new_key.key = obs_key_from_virtual_key(57);
	new_key.modifiers =
		TranslateQtKeyboardEventModifiers(event->modifiers());

	HandleNewKey(new_key);
}
#endif
void OBSHotkeysWidget::mousePressEvent(QMouseEvent *event)
{
	obs_key_combination_t new_key;

	switch (event->button()) {
	case Qt::NoButton:
	case Qt::LeftButton:
	case Qt::RightButton:
	case Qt::AllButtons:
	case Qt::MouseButtonMask:
		return;

	case Qt::MidButton:
		new_key.key = OBS_KEY_MOUSE3;
		break;

#define MAP_BUTTON(i, j)                        \
	case Qt::ExtraButton##i:                \
		new_key.key = OBS_KEY_MOUSE##j; \
		break;
		MAP_BUTTON(1, 4)
		MAP_BUTTON(2, 5)
		MAP_BUTTON(3, 6)
		MAP_BUTTON(4, 7)
		MAP_BUTTON(5, 8)
		MAP_BUTTON(6, 9)
		MAP_BUTTON(7, 10)
		MAP_BUTTON(8, 11)
		MAP_BUTTON(9, 12)
		MAP_BUTTON(10, 13)
		MAP_BUTTON(11, 14)
		MAP_BUTTON(12, 15)
		MAP_BUTTON(13, 16)
		MAP_BUTTON(14, 17)
		MAP_BUTTON(15, 18)
		MAP_BUTTON(16, 19)
		MAP_BUTTON(17, 20)
		MAP_BUTTON(18, 21)
		MAP_BUTTON(19, 22)
		MAP_BUTTON(20, 23)
		MAP_BUTTON(21, 24)
		MAP_BUTTON(22, 25)
		MAP_BUTTON(23, 26)
		MAP_BUTTON(24, 27)
#undef MAP_BUTTON
	}

	new_key.modifiers =
		TranslateQtKeyboardEventModifiers(event->modifiers());

	HandleNewKey(new_key);
}
void OBSHotkeysWidget::ReloadKeyLayout()
{
	RenderKey();
}
void OBSHotkeysWidget::HandleNewKey(obs_key_combination_t new_key)
{
	if (new_key == key || obs_key_combination_is_empty(new_key))
		return;

	key = new_key;

	changed = true;
	emit KeyChanged(key);

	RenderKey();
}

void OBSHotkeysWidget::RenderKey()
{
	DStr str;
	obs_key_combination_to_str(key, str);

	ui->lineEdit->setText(QT_UTF8(str));
}

void OBSHotkeysWidget::ResetKey()
{
	key = original;

	changed = false;
	emit KeyChanged(key);

	RenderKey();
}

void OBSHotkeysWidget::ClearKey()
{
	key = {0, OBS_KEY_NONE};

	changed = true;
	emit KeyChanged(key);

	RenderKey();
}
