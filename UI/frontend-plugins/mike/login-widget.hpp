#pragma once

#include <json11.hpp>

#include <QDockWidget>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>
#include <qdockwidget.h>

using namespace json11;

typedef std::function<void(Json res)> LoginCallback;

class LoginWidget : public QWidget {
    private:
    // QWidget *dockWidgetContents;
    QGridLayout *gridLayout;
    QPushButton *login_button;
    QLineEdit *password;
    QLineEdit *username;
    QLabel *username_label;
    QLabel *password_label;

    LoginCallback cb;

    public:
    LoginWidget(QWidget *parent, LoginCallback cb);
};
