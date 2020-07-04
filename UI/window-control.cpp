#include <window-control.hpp>


OBSControlWidget::OBSControlWidget(QDialog *parent ){}
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
