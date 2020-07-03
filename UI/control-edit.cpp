/******************************************************************************
    Copyright (C) 2020 by Christopher P Yarger <cpyarger@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/
#include <QtWidgets/QMainWindow>
#include <QWidget>
#if __has_include(<obs-frontend-api.h>)
#include <obs-frontend-api.h>
#else
#include <obs-frontend-api/obs-frontend-api.h>
#endif
#include "control-edit.hpp"
OBSBasicControl::OBSBasicControl()
{
	
	//Add UI 
}

OBSBasicControl::~OBSBasicControl() {}

int OBSBasicControl::AddControl(QIcon icon, QString name, QWidget *page) {
	//OBSS->
	return 0;
}
