#include "dashboard-widget.hpp"

#include <obs-module.h>
#include "obs-frontend-api.h"
#include "switch.hpp"
#include "util/config-file.h"

DashboardWidget::DashboardWidget(QWidget *parent, Json parsed) : QWidget(parent)
{
    setWindowTitle("Test");
    auto parsed_servers =  parsed["servers"].array_items();

    gridLayout = new QGridLayout(this);

    for (const auto &server : parsed_servers) {
        auto name = server["name"].string_value();

        auto tswitch = new Switch(name.c_str());
        tswitch->setLayoutDirection(Qt::RightToLeft);
        tswitch->setChecked(server["status"] == "on");

        gridLayout->addWidget(tswitch);

        servers.push_back(tswitch);
    }

    config_t* profile = obs_frontend_get_profile_config();
    config_set_string(profile, "Output", "Mode", parsed["output"]["mode"].string_value().c_str());
    config_set_string(profile, "AdvOut", "Encoder", "obs_x264");

    // const char *t = config_get_string(profile, "Output", "Mode");


    config_save(profile);

    setMaximumHeight(parsed_servers.size() * 50);
}
