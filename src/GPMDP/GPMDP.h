//
// Created by drogonmar on 11/19/19.
//

#ifndef SVRLINUXTOOLS_GPMDP_H
#define SVRLINUXTOOLS_GPMDP_H

#include <QWidget>

namespace Ui{
    class GPMDP;
}

class GPMDP : public QWidget{
    Q_OBJECT

public:
    explicit GPMDP(QWidget *parent = 0);
    ~GPMDP();


private slots:

private:
    Ui::GPMDP *ui;
};

#endif //SVRLINUXTOOLS_GPMDP_H
