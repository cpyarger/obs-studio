#pragma once

#include <QWidget>
#include <QIcon>
#include <QString>
#include <QDialog>
class OBSControlWidget : public QDialog
{

public:
	OBSControlWidget(QDialog *parent = nullptr);
	~OBSControlWidget();
	void AddPage(QWidget *Page = nullptr);
	void AddIcon(QIcon *Icon = nullptr);
	void AddName(QString *Name = nullptr);
	QWidget *page;
	QIcon icon;
	QString name;

};
