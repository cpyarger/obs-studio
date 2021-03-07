#pragma once

#include <json11.hpp>
#include "container-widget.hpp"
#include "switch.hpp"

#include <QWidget>
#include <QGridLayout>
#include <unordered_map>
#include <util/platform.h>

using namespace json11;

struct ServerInformation {
    std::string server;
    std::string key;
    Switch *widget;
};

class DashboardWidget : public QWidget {
    private:
    QTimer *timer;
    os_cpu_usage_info_t *cpu_info;
    QGridLayout *gridLayout;
    std::unordered_map<std::string, ServerInformation> server_information;
    void send_update(std::string url);
        std::string id;
        std::string name;

    public:
    DashboardWidget(QWidget *parent, Json parsed);
        ~DashboardWidget();
};
