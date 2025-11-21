#ifndef TEDYTOR_ALARMOW_DLG_H
#define TEDYTOR_ALARMOW_DLG_H

#include <QDialog>
#include "tjeden_alarm.h"
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Ui {
class Tedytor_alarmow_dlg;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Tedytor_alarmow_dlg : public QDialog
{
    Q_OBJECT

public:
    explicit Tedytor_alarmow_dlg(QWidget *parent = nullptr);
    ~Tedytor_alarmow_dlg();

    void ustaw_alarm_do_edycji(const Tjeden_alarm & alarm);
   Tjeden_alarm daj_rezultat() {return al;}

    void changeEvent(QEvent *event) override;
    void updateTexts();

private slots:
    void on_comboBox_powtarzanie_currentIndexChanged(int index);

    void on_toolButton_kalendarz_triggered(QAction *arg1);

    void on_toolButton_kalendarz_clicked();

    void on_calendarWidget_kalendarz_clicked(const QDate &date);

    void on_pushButton_today_clicked();

    void on_buttonBox_accepted();

    void on_toolButton_wybor_pliku_muz_clicked();

    void on_toolButton_nazwy_programu_clicked();

private:
    Ui::Tedytor_alarmow_dlg *ui;

    void mod_raz_miesiac_rok();
    void mod_codziennie();
    void mod_co_tydzien();
    void mod_minuty();
    void mod_zgas_wszystkie();
    Tjeden_alarm al;
    void rozlozenie_po_kontrolkach();

};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern QStringList list_czestotliwosc;

#endif // TEDYTOR_ALARMOW_DLG_H
