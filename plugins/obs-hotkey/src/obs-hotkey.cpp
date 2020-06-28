#include "hotkey-edit.hpp"
#include "obs-hotkey.h""
auto ReloadHotkeys = [](void *data, calldata_t *) {	
		auto settings = static_cast<OBSHotkey *>(data);	
		//QMetaObject::invokeMethod(settings, "ReloadHotkeys");	
	};	
	//hotkeyRegistered.Connect(obs_get_signal_handler(), "hotkey_register",		 ReloadHotkeys, this);	

	auto ReloadHotkeysIgnore = [](void *data, calldata_t *param) {	
		auto settings = static_cast<OBSHotkey *>(data);	
		auto key =	
			static_cast<obs_hotkey_t *>(calldata_ptr(param, "key"));	
		//QMetaObject::invokeMethod(settings, "ReloadHotkeys",						  Q_ARG(obs_hotkey_id,	obs_hotkey_get_id(key)));	
	};	
	//hotkeyUnregistered.Connect(obs_get_signal_handler(),		   "hotkey_unregister", ReloadHotkeysIgnore,	this);

void OBSHotkey::LoadHotkeySettings(obs_hotkey_id ignoreKey)	
{	
	hotkeys.clear();	
	//ui->hotkeyPage->takeWidget()->deleteLater();	

	using keys_t = map<obs_hotkey_id, vector<obs_key_combination_t>>;	
	keys_t keys;	
	obs_enum_hotkey_bindings(	
		[](void *data, size_t, obs_hotkey_binding_t *binding) {	
			auto &keys = *static_cast<keys_t *>(data);	

			keys[obs_hotkey_binding_get_hotkey_id(binding)]	
				.emplace_back(	
					obs_hotkey_binding_get_key_combination(	
						binding));	

			return true;	
		},	
		&keys);	

	auto layout = new QFormLayout();	
	layout->setVerticalSpacing(0);	
	layout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);	
	layout->setLabelAlignment(Qt::AlignRight | Qt::AlignTrailing |	
				  Qt::AlignVCenter);	

	auto widget = new QWidget();	
	widget->setLayout(layout);	
	//ui->hotkeyPage->setWidget(widget);	

	auto filterLayout = new QGridLayout();	
	auto filterWidget = new QWidget();	
	filterWidget->setLayout(filterLayout);	

	//auto filterLabel = new QLabel(QTStr("Basic.Settings.Hotkeys.Filter"));	
	auto filter = new QLineEdit();	

	auto setRowVisible = [=](int row, bool visible, QLayoutItem *label) {	
		label->widget()->setVisible(visible);	

		auto field = layout->itemAt(row, QFormLayout::FieldRole);	
		if (field)	
			field->widget()->setVisible(visible);	
	};	

	auto searchFunction = [=](const QString &text) {	
		for (int i = 0; i < layout->rowCount(); i++) {	
			auto label = layout->itemAt(i, QFormLayout::LabelRole);	
			if (label) {	
				OBSHotkeyLabel *item =	
					qobject_cast<OBSHotkeyLabel *>(	
						label->widget());	
				if (item) {	
					if (item->text().toLower().contains(	
						    text.toLower()))	
						setRowVisible(i, true, label);	
					else	
						setRowVisible(i, false, label);	
				}	
			}	
		}	
	};	

	connect(filter, &QLineEdit::textChanged, this, searchFunction);	

	//filterLayout->addWidget(filterLabel, 0, 0);	
	filterLayout->addWidget(filter, 0, 1);	

	layout->addRow(filterWidget);	



	using std::move;	

	auto HandleEncoder = [&](void *registerer, OBSHotkeyLabel *label,	
				 OBSHotkeyWidget *hw) {	
		auto weak_encoder =	
			static_cast<obs_weak_encoder_t *>(registerer);	
		auto encoder = OBSGetStrongRef(weak_encoder);	

		if (!encoder)	
			return true;	

		encoders.emplace_back(move(encoder), label, hw);	
		return false;	
	};	

	auto HandleOutput = [&](void *registerer, OBSHotkeyLabel *label,	
				OBSHotkeyWidget *hw) {	
		auto weak_output = static_cast<obs_weak_output_t *>(registerer);	
		auto output = OBSGetStrongRef(weak_output);	

		if (!output)	
			return true;	

		outputs.emplace_back(move(output), label, hw);	
		return false;	
	};	

	auto HandleService = [&](void *registerer, OBSHotkeyLabel *label,	
				 OBSHotkeyWidget *hw) {	
		auto weak_service =	
			static_cast<obs_weak_service_t *>(registerer);	
		auto service = OBSGetStrongRef(weak_service);	

		if (!service)	
			return true;	

		services.emplace_back(move(service), label, hw);	
		return false;	
	};	

	auto HandleSource = [&](void *registerer, OBSHotkeyLabel *label,	
				OBSHotkeyWidget *hw) {	
		auto weak_source = static_cast<obs_weak_source_t *>(registerer);	
		auto source = OBSGetStrongRef(weak_source);	

		if (!source)	
			return true;	

		if (obs_scene_from_source(source))	
			scenes.emplace_back(source, label, hw);	
		else	
			sources.emplace_back(source, label, hw);	

		return false;	
	};	

	auto RegisterHotkey = [&](obs_hotkey_t *key, OBSHotkeyLabel *label,	
				  OBSHotkeyWidget *hw) {	
		auto registerer_type = obs_hotkey_get_registerer_type(key);	
		void *registerer = obs_hotkey_get_registerer(key);	

		obs_hotkey_id partner = obs_hotkey_get_pair_partner_id(key);	
		if (partner != OBS_INVALID_HOTKEY_ID) {	
			pairLabels.emplace(obs_hotkey_get_id(key),	
					   make_pair(partner, label));	
			pairIds.push_back(obs_hotkey_get_id(key));	
		}	

		using std::move;	

		switch (registerer_type) {	
		case OBS_HOTKEY_REGISTERER_FRONTEND:	
			layout->addRow(label, hw);	
			break;	

		case OBS_HOTKEY_REGISTERER_ENCODER:	
			if (HandleEncoder(registerer, label, hw))	
				return;	
			break;	

		case OBS_HOTKEY_REGISTERER_OUTPUT:	
			if (HandleOutput(registerer, label, hw))	
				return;	
			break;	

		case OBS_HOTKEY_REGISTERER_SERVICE:	
			if (HandleService(registerer, label, hw))	
				return;	
			break;	

		case OBS_HOTKEY_REGISTERER_SOURCE:	
			if (HandleSource(registerer, label, hw))	
				return;	
			break;	
		}	

		hotkeys.emplace_back(	
			registerer_type == OBS_HOTKEY_REGISTERER_FRONTEND, hw);	
		//connect(hw, &OBSHotkeyWidget::KeyChanged, this,			&OBSHotkey::HotkeysChanged);	
	};	

	auto data = make_tuple(RegisterHotkey, std::move(keys), ignoreKey);	
	using data_t = decltype(data);	
	obs_enum_hotkeys(	
		[](void *data, obs_hotkey_id id, obs_hotkey_t *key) {	
			data_t &d = *static_cast<data_t *>(data);	
			if (id != get<2>(d))	
				//LayoutHotkey(id, key, get<0>(d), get<1>(d));	
			return true;	
		},	
		&data);	

	for (auto keyId : pairIds) {	
		auto data1 = pairLabels.find(keyId);	
		if (data1 == end(pairLabels))	
			continue;	

		auto &label1 = data1->second.second;	
		if (label1->pairPartner)	
			continue;	

		auto data2 = pairLabels.find(data1->second.first);	
		if (data2 == end(pairLabels))	
			continue;	

		auto &label2 = data2->second.second;	
		if (label2->pairPartner)	
			continue;	

		//QString tt = QTStr("Basic.Settings.Hotkeys.Pair");	
		auto name1 = label1->text();	
		auto name2 = label2->text();	

		auto Update = [&](OBSHotkeyLabel *label, const QString &name,	
				  OBSHotkeyLabel *other,	
				  const QString &otherName) {	
			QString string =	
				other->property("fullName").value<QString>();	

			if (string.isEmpty() || string.isNull())	
				string = otherName;	

			//label->setToolTip(tt.arg(string));	
			label->setText(name + " *");	
			label->pairPartner = other;	
		};	
		Update(label1, name1, label2, name2);	
		Update(label2, name2, label1, name1);	
	}	

	//AddHotkeys(*layout, obs_output_get_name, outputs);	
	//AddHotkeys(*layout, obs_source_get_name, scenes);	
	//AddHotkeys(*layout, obs_source_get_name, sources);	
	//AddHotkeys(*layout, obs_encoder_get_name, encoders);	
	//AddHotkeys(*layout, obs_service_get_name, services);	
}

void OBSHotkey::SaveHotkeySettings()	
{	
	//const auto &config = main->Config();	

	using namespace std;	

	std::vector<obs_key_combination> combinations;	
	for (auto &hotkey : hotkeys) {	
		auto &hw = *hotkey.second;	
		if (!hw.Changed())	
			continue;	

		hw.Save(combinations);	

		if (!hotkey.first)	
			continue;	

		obs_data_array_t *array = obs_hotkey_save(hw.id);	
		obs_data_t *data = obs_data_create();	
		obs_data_set_array(data, "bindings", array);	
		const char *json = obs_data_get_json(data);	
		//config_set_string(config, "Hotkeys", hw.name.c_str(), json);	
		obs_data_release(data);	
		obs_data_array_release(array);	
	}	

	//if (!main->outputHandler || !main->outputHandler->replayBuffer)	
		return;	

	//const char *id = obs_obj_get_id(main->outputHandler->replayBuffer);	
	//if (strcmp(id, "replay_buffer") == 0) {	
	//	obs_data_t *hotkeys = obs_hotkeys_save_output(	
	//		main->outputHandler->replayBuffer);	
	//	config_set_string(config, "Hotkeys", "ReplayBuffer",  obs_data_get_json(hotkeys));	
	//	obs_data_release(hotkeys);	
	//}	
}	
void OBSHotkey::HotkeysChanged()	
{	
	using namespace std;	
	if (loading)	
		return;	

	hotkeysChanged =	
		any_of(begin(hotkeys), end(hotkeys),	
		       [](const pair<bool, QPointer<OBSHotkeyWidget>> &hotkey) {	
			       const auto &hw = *hotkey.second;	
			       return hw.Changed();	
		       });	


	//if (hotkeysChanged)	
		//EnableApplyButton(true);	
}	

void OBSHotkey::ReloadHotkeys(obs_hotkey_id ignoreKey)	
{	
	LoadHotkeySettings(ignoreKey);	
}

