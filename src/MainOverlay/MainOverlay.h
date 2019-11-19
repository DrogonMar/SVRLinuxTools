//
// Created by drogonmar on 11/19/19.
//

#ifndef SVRLINUXTOOLS_MAINOVERLAY_H
#define SVRLINUXTOOLS_MAINOVERLAY_H

#include <QWidget>

namespace Ui{
    class MainOverlay;
}

class MainOverlay : public QWidget{
    Q_OBJECT

public:
    explicit MainOverlay(QWidget *parent = 0);
    ~MainOverlay();

private slots:

private:
    Ui::MainOverlay *ui;
};

#endif //SVRLINUXTOOLS_MAINOVERLAY_H
