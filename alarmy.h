#ifndef ALARMY_H
#define ALARMY_H

#include <QDialog>
#include <vector>
#include "tjeden_alarm.h"

namespace Ui {
class Alarmy;
}
extern std:: vector <Tjeden_alarm> tablica_alarmow;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Alarmy : public QDialog
{
    Q_OBJECT

public:
    explicit Alarmy(QWidget *parent, string fname);
    ~Alarmy();

private slots:
   // void on_buttonBox_accepted();
	void on_pushButton_usun_rzad_clicked();
	void on_tableWidget_cellClicked(int row, int column);
    void on_pushButton_nowy_clicked();
    void on_pushButton_edytuj_clicked();
    void on_pushButton_OK_clicked();

    void on_tableWidget_cellDoubleClicked(int row, int column);

    void on_pushButton_test_clicked();

private:
    Ui::Alarmy *ui;

	void redisplay_table();
	void table_value_changed();
	void on_checkBox_AND_gates_clicked(bool checked);
    string nazwa_pliku_z_alarmami ; // ustawiane przez konstruktor
//	void wstepne_zaladowanie_tablicy();
    void zapisanie_tablicy_na_dysku();
//    Tjeden_alarm wyjecie_itemow(string one);
//    string znajdz_txt_item(string txt, string keyword);
//    int znajdz_int_item(string txt, string keyword);
};

#endif // ALARMY_H
