#include "rc.h"
#include "ui_rcform.h"

rc::rc(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::rc)
{
    ui->setupUi(this);
}

rc::~rc()
{
    delete ui;
}
