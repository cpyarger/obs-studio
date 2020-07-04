#include <window-control.hpp>
#include <obs-app.hpp>

OBSControlWidget::OBSControlWidget(QDialog *parent ): QDialog(parent){
}
OBSControlWidget::~OBSControlWidget() {}


void OBSControlWidget::AddIcon(QIcon *Icon)
{
	icon = (QIcon)*Icon;
};
void OBSControlWidget::AddPage(QWidget *Page)
{
	page = Page;
};
void OBSControlWidget::AddName(QString *Name)
{
	name = (QString)*Name;
};
