#pragma once
#include <obs.hpp>
#include <memory>
#include <QObject>

#include <QHotkey/qhotkey.h>
#include <util/dstr.h>

void setup_ui();
void register_gui();
void reloadEvents();
#define OBS_MIDI_VERSION "0.1"

#define blog(level, msg, ...) blog(level, "[obs-hotkey] " msg, ##__VA_ARGS__)

/************************Hotkeys bits*************************/
class HKC : public QObject {
	Q_OBJECT

public:
	struct KPair {
		QKeySequence Sequence;
		bool pressed;
		QHotkey *hotkey;
	};
	QMap<QKeySequence, KPair *> pressmap;
	HKC();
	~HKC();
	void AddHK(QKeySequence hk);
	void remaphk(obs_data_t *map);
signals:
	void EmitHotkey(QString actiontype, QString action);
	void Trigger(QString actiontype, obs_data_t *action);
public slots:

	void loadmap(obs_data_t *map);
	void DoQHK();
};
