#pragma once

#include <QDockWidget>
#include <QWidget>

class ContainerWidget : public QDockWidget {
    private:
    QWidget *current_widget;

    public:
    ContainerWidget(QWidget *parent);
};
