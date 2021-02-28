#pragma once
#include "pch.h"
#include <functional>

class LoginWidget : virtual public QWidget {
    public:
    virtual ~LoginWidget() {}
    virtual void SetLoginEnabled(bool enabled) = 0;
};

using OnLoginCallback = std::function<void(const char *username, const char *password)>;

LoginWidget *createLoginWidget(OnLoginCallback cb, QWidget *parent = 0);
