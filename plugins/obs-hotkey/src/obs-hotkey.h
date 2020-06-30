/*
Plugin Name
Copyright (C) <Year> <Developer> <Email Address>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program. If not, see <https://www.gnu.org/licenses/>
*/


#include <obs.hpp>
#include <util/util.hpp>
#include <util/lexer.h>
#include <graphics/math-defs.h>
#include <initializer_list>
#include <sstream>
#include <QCompleter>
#include <QGuiApplication>
#include <QLineEdit>
#include <QMessageBox>
#include <QCloseEvent>
#include <QFileDialog>
#include <QDirIterator>
#include <QVariant>
#include <QTreeView>
#include <QScreen>
#include <QStandardItemModel>
#include <QSpacerItem>
 #include <QFormLayout>
#include <QIcon>
#include <QDialog>
#include <QPointer>
#include <memory>
#include <string>


#include <obs.hpp>

class OBSHotkeyWidget;

#ifndef OBSHotkey_H
#define OBSHotkey_H

#define PLUGIN_NAME "OBS-hotkey"
#define PLUGIN_VERSION "0.0.1"

#define blog(level, msg, ...) \
	blog(level, "[" PLUGIN_NAME "] " msg, ##__VA_ARGS__)

#endif // PLUGINNAME_H
#include "hotkey-edit.hpp"
using namespace std;
using encoders_elem_t = tuple<OBSEncoder, QPointer<QLabel>, QPointer<QWidget>>;
using outputs_elem_t = tuple<OBSOutput, QPointer<QLabel>, QPointer<QWidget>>;
using services_elem_t = tuple<OBSService, QPointer<QLabel>, QPointer<QWidget>>;
using sources_elem_t = tuple<OBSSource, QPointer<QLabel>, QPointer<QWidget>>;
vector<encoders_elem_t> encoders;
vector<outputs_elem_t> outputs;
vector<services_elem_t> services;
vector<sources_elem_t> scenes;
vector<sources_elem_t> sources;

vector<obs_hotkey_id> pairIds;
map<obs_hotkey_id, pair<obs_hotkey_id, OBSHotkeyLabel *>> pairLabels;
std::vector<std::pair<bool, QPointer<OBSHotkeyWidget>>> hotkeys;
OBSSignal hotkeyRegistered;
OBSSignal hotkeyUnregistered;
class OBSHotkey:QWidget{
	Q_OBJECT


public:
	
	void SaveHotkeySettings();	
	QIcon hotkeysIcon;
	void HotkeysChanged();
	void LoadHotkeySettings(obs_hotkey_id ignoreKey);
	void ReloadHotkeys(obs_hotkey_id ignoreKey = OBS_INVALID_HOTKEY_ID);
	void SetHotkeysIcon(const QIcon &icon);

private:
	bool loading;
	bool hotkeysChanged;
};
