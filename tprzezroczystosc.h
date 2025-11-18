#ifndef TPRZEZROCZYSTOSC_H
#define TPRZEZROCZYSTOSC_H

#include <QDialog>

namespace Ui {
class Tprzezroczystosc;
}

class MainWindow;
////////////////////////////////////////////////////////////
class Tprzezroczystosc : public QDialog
{
    Q_OBJECT


public:
    explicit Tprzezroczystosc(MainWindow *parent );
    ~Tprzezroczystosc();
    void set_value(int value  );
    int get_value();

    int value_transp;
private slots:
    void on_horizontalSlider_valueChanged(int value);

    void on_buttonBox_accepted();

private:
    Ui::Tprzezroczystosc *ui;
    MainWindow *szef;
};

#endif // TPRZEZROCZYSTOSC_H
