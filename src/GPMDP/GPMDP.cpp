#include <QLabel>

#include "GPMDP.h"
#include "ui_GPMDP.h"

GPMDP::GPMDP(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GPMDP)
{
    ui->setupUi(this);

    ui->verticalLayout_2->addWidget(new QLabel("Dango dango daikazoku"));
}

GPMDP::~GPMDP(){
    delete ui;
}