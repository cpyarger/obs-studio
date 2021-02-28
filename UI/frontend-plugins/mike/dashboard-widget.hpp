#pragma once

#include <json11.hpp>

#include <QWidget>
#include <QGridLayout>
#include <QVector>

using namespace json11;

class DashboardWidget : public QWidget {
    private:
    QGridLayout *gridLayout;
    QVector<QWidget *> servers;

    public:
    DashboardWidget(QWidget *parent, Json parsed);
};
