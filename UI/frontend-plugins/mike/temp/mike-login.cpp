#include "pch.h"

#include "mike-login.hpp"

#include <qdockwidget.h>
#include <qgridlayout.h>
#include <qscrollarea.h>

#define ConfigSection "obs-multi-rtmp"

class LoginWidgetImpl : virtual public LoginWidget {
    public:
    virtual ~LoginWidgetImpl() {}
    QPushButton *loginBtn;

    LoginWidgetImpl(QWidget *parent, OnLoginCallback cb) : QWidget(parent)
    {
        QLineEdit *username;
        QLineEdit *password;

        auto layout = new QGridLayout(this);
        layout->setColumnStretch(0, 1);
        layout->setColumnStretch(1, 1);
        layout->setColumnStretch(2, 1);
        layout->addWidget(new QLabel("Username", this), 0, 0, 1, 3);
        layout->addWidget(username = new QLineEdit("", this), 1, 0, 1, 3);
        layout->addWidget(new QLabel("Password", this), 2, 0, 1, 3);
        layout->addWidget(password = new QLineEdit("", this), 3, 0, 1, 3);
        layout->addWidget(loginBtn = new QPushButton("Login", this), 4, 1, 1, 1);

        password->setEchoMode(QLineEdit::EchoMode::Password);

        QObject::connect(loginBtn, &QPushButton::clicked, [=]() {
            auto un = username->text();
            auto pw = password->text();
            cb(un.toUtf8().constData(), pw.toUtf8().constData());
        });

        setLayout(layout);
    }

    void SetLoginEnabled(bool enabled) override { loginBtn->setEnabled(enabled); }
};

LoginWidget *createLoginWidget(OnLoginCallback cb, QWidget *parent)
{
    return new LoginWidgetImpl(parent, cb);
}
