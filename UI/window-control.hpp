#pragma once

#include <QWidget>

class OBSControlWidget : public QWidget {
	Q_OBJECT

public:
	inline OBScontrol(QWidget* parent = nullptr) : QWidget(parent) {}

	virtual void closeEvent(QCloseEvent* event);
};
