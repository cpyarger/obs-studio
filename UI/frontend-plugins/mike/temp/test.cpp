#include "test.hpp"

#include <obs-module.h>
#include "obs-frontend-api.h"
#include "util/config-file.h"

#include <QMainWindow>
#include <qdockwidget.h>
#include <qlabel.h>
#include <qmainwindow.h>
#include <QDockWidget>
#include <QLineEdit>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <qwidget.h>
#include <QAction>

#define ConfigSection "mike-obs-server"

LoginWidget::LoginWidget(QWidget *parent) : QDockWidget(parent)
{
    setWindowTitle("Login");

    resize(397, 303);
    dockWidgetContents = new QWidget();

    gridLayout = new QGridLayout(dockWidgetContents);
    gridLayout->setColumnStretch(0, 1);
    gridLayout->setColumnStretch(1, 1);
    gridLayout->setColumnStretch(2, 1);

    login_button = new QPushButton("Login", dockWidgetContents);
    password = new QLineEdit(dockWidgetContents);
    username = new QLineEdit(dockWidgetContents);
    username_label = new QLabel("Username:", dockWidgetContents);
    password_label = new QLabel("Password:", dockWidgetContents);

    gridLayout->addWidget(login_button, 4, 1, 1, 1);
    gridLayout->addWidget(password, 3, 0, 1, 3);
    gridLayout->addWidget(username, 1, 0, 1, 3);
    gridLayout->addWidget(username_label, 0, 0, 1, 3);
    gridLayout->addWidget(password_label, 2, 0, 1, 3);

    dockWidgetContents->setMaximumHeight(150);

    setWidget(dockWidgetContents);

    QMetaObject::connectSlotsByName(this);
}

OBS_DECLARE_MODULE()
OBS_MODULE_AUTHOR("Ford Smith");
OBS_MODULE_USE_DEFAULT_LOCALE("mike_server", "en-US")

bool obs_module_load()
{
    const auto main_window = static_cast<QMainWindow *>(obs_frontend_get_main_window());
    obs_frontend_push_ui_translation(obs_module_get_string);
    auto dock = new LoginWidget(main_window);
    auto action = (QAction *)obs_frontend_add_dock(dock);

    main_window->removeDockWidget(dock);
    auto docklocation =
            config_get_int(obs_frontend_get_global_config(), ConfigSection, "DockLocation");
    auto visible = config_get_bool(obs_frontend_get_global_config(), ConfigSection, "DockVisible");
    if (!config_has_user_value(obs_frontend_get_global_config(), ConfigSection, "DockLocation")) {
        docklocation = Qt::DockWidgetArea::LeftDockWidgetArea;
    }
    if (!config_has_user_value(obs_frontend_get_global_config(), ConfigSection, "DockVisible")) {
        visible = true;
    }

    main_window->addDockWidget((Qt::DockWidgetArea)docklocation, dock);
    if (visible) {
        dock->setVisible(true);
        action->setChecked(true);
    } else {
        dock->setVisible(false);
        action->setChecked(false);
    }

    return true;
}

void obs_module_unload() {}

MODULE_EXPORT const char *obs_module_description(void)
{
    return obs_module_text("Description");
}

MODULE_EXPORT const char *obs_module_name(void)
{
    return obs_module_text("MediaControls");
}
