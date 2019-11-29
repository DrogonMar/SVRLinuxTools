//
// Created by drogonmar on 11/19/19.
//

#ifndef SVRLINUXTOOLS_MAINOVERLAY_H
#define SVRLINUXTOOLS_MAINOVERLAY_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QGroupBox>

#include "../GPMDP/GPMDP.h"

namespace Ui{
    class MainOverlay;
}

class MainOverlay : public QWidget{
    Q_OBJECT

public:
    explicit MainOverlay(QWidget *parent = nullptr);
    ~MainOverlay() override;

    void SetMainWidget(QWidget* widget);

private slots:
    void on_GPMDPMenuButton_clicked();
    void on_HomeButton_clicked();
private:
    Ui::MainOverlay *ui;

    //Menu layouts
    QLabel *nothingLabel;
    GPMDP *googleMusicWidget;

    //Menu stuff
    QWidget *currentWidget;
    QGroupBox *groupBox;
    QVBoxLayout *layout;
};

#endif //SVRLINUXTOOLS_MAINOVERLAY_H
