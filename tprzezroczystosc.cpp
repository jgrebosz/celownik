#include "tprzezroczystosc.h"
#include "ui_tprzezroczystosc.h"
#include <iostream>
#include "mainwindow.h"

using namespace std;

//*************************************************************************
Tprzezroczystosc::Tprzezroczystosc(MainWindow *parent) :
    QDialog(parent),
    ui(new Ui::Tprzezroczystosc)
{
    szef =  parent;
    ui->setupUi(this);
    ui->horizontalSlider->setRange(25, 255);
    ui->horizontalSlider->setValue(70);

//            setTickPosition(QSlider::TickPosition position)
}
//*************************************************************************
Tprzezroczystosc::~Tprzezroczystosc()
{
    delete ui;
}
//*************************************************************************
void Tprzezroczystosc::on_horizontalSlider_valueChanged(int value)
{
//    cout << "Biezaca wartosc przezroczystosci= " << value << endl ;
    value_transp = value;
    ui->label_zoom->setNum(value);

    szef->setWindowOpacity(value/255.0);
//    szef->
//            zmien_przezroczystosc(value_transp);

}
//*************************************************************************
void Tprzezroczystosc::set_value(int value)
{
    ui->horizontalSlider->setValue(value);
}
//*************************************************************************
int Tprzezroczystosc::get_value()
{
    return value_transp;
}
//*************************************************************************
void Tprzezroczystosc::on_buttonBox_accepted()
{

}
