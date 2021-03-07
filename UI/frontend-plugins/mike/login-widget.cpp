#include "login-widget.hpp"

#include <obs-module.h>
#include <curl/curl.h>

#include <QAction>
#include <QDockWidget>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QPushButton>
#include <QMessageBox>

int curl_string_callback(void *data, int size, int cnt, void *user)
{
    auto str = (std::string *)user;
    str->insert(str->end(), (const char *)data, (const char *)data + (size * cnt));
    return cnt;
}

std::string Login(std::string un, std::string pw)
{
    CURL *h = curl_easy_init();
    if (!h)
        return "";
    auto e_un = curl_easy_escape(h, un.c_str(), un.size());
    auto e_pw = curl_easy_escape(h, pw.c_str(), pw.size());
    std::string url =
            std::string("https://mdca.co.com/api/obs_login?user=") + e_un + "&pass=" + e_pw;
    curl_free(e_un);
    curl_free(e_pw);

    std::string result;
    curl_easy_setopt(h, CURLOPT_TIMEOUT, 10L);
    curl_easy_setopt(h, CURLOPT_URL, url.c_str());
    curl_easy_setopt(h, CURLOPT_WRITEFUNCTION, &curl_string_callback);
    curl_easy_setopt(h, CURLOPT_WRITEDATA, &result);
    curl_easy_setopt(h, CURLOPT_SSL_OPTIONS, CURLSSLOPT_NO_REVOKE);
    auto ret = curl_easy_perform(h);
    curl_easy_cleanup(h);

    if (ret == CURLE_OK)
        return result;
    else
        return "";
}

LoginWidget::LoginWidget(QWidget *parent, LoginCallback _cb) : QWidget(parent), cb(_cb)
{
    setWindowTitle("Login");

    resize(397, 303);

    gridLayout = new QGridLayout(this);
    gridLayout->setColumnStretch(0, 1);
    gridLayout->setColumnStretch(1, 1);
    gridLayout->setColumnStretch(2, 1);

    login_button = new QPushButton("Login");
    password = new QLineEdit();
    username = new QLineEdit();
    username_label = new QLabel("Username:");
    password_label = new QLabel("Password:");

    password->setEchoMode(QLineEdit::EchoMode::Password);

    gridLayout->addWidget(login_button, 4, 1, 1, 1);
    gridLayout->addWidget(password, 3, 0, 1, 3);
    gridLayout->addWidget(username, 1, 0, 1, 3);
    gridLayout->addWidget(username_label, 0, 0, 1, 3);
    gridLayout->addWidget(password_label, 2, 0, 1, 3);

    QWidget::setTabOrder(username, password);

    QObject::connect(login_button, &QPushButton::clicked, [&]() {
        auto un = username->text();
        auto pw = password->text();

        std::string err;
        std::string res = Login(un.toUtf8().constData(), pw.toUtf8().constData());
        Json parsed = Json::parse(res, err);
        if (parsed["status"] != "good" || !err.empty()) {
            QMessageBox messageBox;
            messageBox.critical(0, "Error", "Login is incorrect!");
            messageBox.setFixedSize(500, 200);
            return;
        }

        cb(parsed);
    });

    // dockWidgetContents->setMaximumHeight(150);
    setMaximumHeight(150);

    // setWidget(dockWidgetContents);

    QMetaObject::connectSlotsByName(this);
}
