//
// Created by drogonmar on 11/19/19.
//
#include <QDebug>

#include "MainOverlay.h"
#include "ui_MainOverlay.h"

MainOverlay::MainOverlay(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainOverlay)
{
    ui->setupUi(this);

    groupBox = new QGroupBox();
    layout = new QVBoxLayout;
    groupBox->setLayout(layout);

    QSizePolicy pol = groupBox->sizePolicy();
    pol.setHorizontalStretch(2);
    groupBox->setSizePolicy(pol);

    ui->gridLayout->addWidget(groupBox, 0, 1);
    currentWidget = nullptr;

    nothingLabel = new QLabel("Select something.");
    googleMusicWidget = new GPMDP;

    SetMainWidget(nothingLabel);
}

MainOverlay::~MainOverlay() {
    delete ui;
}

void MainOverlay::SetMainWidget(QWidget *widget) {
    if(currentWidget != nullptr) {
        QWidget *oldWidget = layout->takeAt(0)->widget();
        layout->removeWidget(oldWidget);
        oldWidget->setParent(this);
    }

    layout->addWidget(widget);
    currentWidget = widget;
}

void MainOverlay::on_GPMDPMenuButton_clicked() {
    SetMainWidget(googleMusicWidget);
}

void MainOverlay::on_HomeButton_clicked() {
    SetMainWidget(nothingLabel);
}
