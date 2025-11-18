#ifndef TOGLOSZENIE_ALARMU_H
#define TOGLOSZENIE_ALARMU_H

#include <QDialog>


namespace Ui {
class Togloszenie_alarmu;
}

class Tjeden_alarm;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Togloszenie_alarmu : public QDialog
{
    Q_OBJECT

public:
    explicit Togloszenie_alarmu(QWidget *parent = nullptr);
    ~Togloszenie_alarmu();
    void wstaw_opis(QString tytul, QString dodatkowy);
    void adres_mojego_alarmu (Tjeden_alarm * wsk_alarmu );

    void usuniecie_playera_z_wektora_grajacych();

private slots:
      void on_pushButton_close_clicked();

private:
    Ui::Togloszenie_alarmu *ui;

    Tjeden_alarm * wsk_alarmu;
};

#endif // TOGLOSZENIE_ALARMU_H
