//
// Created by drogonmar on 11/19/19.
//
#include "MainOverlay.h"
#include "ui_MainOverlay.h"

MainOverlay::MainOverlay(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainOverlay)
{
    ui->setupUi(this);
}

MainOverlay::~MainOverlay() {
    delete ui;
}