#include "helpwindow.h"
#include "ui_helpwindow.h"

helpwindow::helpwindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::helpwindow)
{
	ui->setupUi(this);
}

helpwindow::~helpwindow()
{
	delete ui;
}
