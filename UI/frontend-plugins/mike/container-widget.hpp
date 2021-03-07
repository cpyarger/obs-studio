#pragma once

#include <QDockWidget>
#include <QWidget>
#include <QPointer>

class ContainerWidget : public QDockWidget {
    private:
    QPointer<QWidget> current_widget;

    public:
    ContainerWidget(QWidget *parent);
};
