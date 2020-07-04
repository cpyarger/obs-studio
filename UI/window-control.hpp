#pragma once
#if __has_include(<obs-frontend-api.h>)
#include <obs-frontend-api.h>
#else
#include <obs-frontend-api/obs-frontend-api.h>
#endif
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
