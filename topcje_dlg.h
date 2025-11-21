#ifndef TOPCJE_DLG_H
#define TOPCJE_DLG_H

#include <QDialog>
#include <QListWidget>
#include <QTableWidgetItem>

#include <string>

namespace Ui {
class Topcje_dlg;
}
class MainWindow;
//////////////////////////////////////////////////////////////////////////////////////////
class Topcje_dlg : public QDialog
{
    Q_OBJECT

public:
    explicit Topcje_dlg(
            //QWidget *parent,
            MainWindow *ptr);
    ~Topcje_dlg();

    void changeEvent(QEvent *event) override;
    void updateTexts();

private slots:
    void on_checkBox_na_wierzchu_clicked(bool checked);

    // void on_checkBox_ogranicz_pozycje_clicked(bool checked);

    void on_checkBox_blokuj_pozycje_na_ekranie_clicked(bool checked);

    void on_horizontalSlider_przezroczystosc_valueChanged(int value);


    void on_buttonBox_accepted();

    void on_horizontalSlider_zoom_valueChanged(int value);

    void on_checkBox_zoom_clicked(bool checked);



    void on_checkBox_display_date_clicked(bool checked);

    void on_checkBox_dispaly_sekundnik_clicked(bool checked);

//    void on_listWidget_cyferblaty_itemClicked(QListWidgetItem *item);

//    void on_listWidget_cyferblaty_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    //  void on_radioButton_wsk_wektorowe_clicked(bool checked);


    //void on_radioButton_wsk_bitmapowe_clicked(bool checked);

    //    void on_comboBox_bitmapy_wsk_godzinowych_currentIndexChanged(int index);

    //    void on_comboBox_bitmapy_wsk_minutowych_currentIndexChanged(int index);

    //    void on_comboBox_bitmapy_wsk_sekundnikow_currentIndexChanged(int index);

    void on_comboBox_timezone_currentIndexChanged(int index);



    void on_tableWidget_wsk_minutowe_cellClicked(int row, int column);

    void on_radioButton_minutes_hand_bitmap_type_toggled(bool checked);

    void on_radioButton_hours_hand_bitmap_type_toggled(bool checked);

    void on_tableWidget_wsk_godzinowe_cellClicked(int row, int column);

    void on_radioButton_sec_bitmap_type_toggled(bool checked);

    void on_tableWidget_wsk_sekundowe_cellClicked(int row, int column);

    void on_spinBox_nr_wsk_wektorowej_godz_valueChanged(int arg1);

    void on_spinBox_nr_wsk_wektorowej_min_valueChanged(int arg1);

    void on_spinBox_nr_wsk_wektorowej_sek_valueChanged(int arg1);

    // void on_comboBox_language_currentIndexChanged(int index);

    void on_checkBox_AM_PM_clicked(bool checked);

    void on_buttonBox_rejected();

    void on_checkBox_dzien_tygodnia_clicked(bool checked);


    void on_pushButton_assign_preferred_hands_clicked();

    void on_pushButton_reset_to_factory_clicked();

    void on_tabWidget_currentChanged(int);

    void on_pushButton_return_favourite_clicked();

    void on_checkBox_favourites_clicked(bool checked);

    void on_tableWidget_cyferblaty_itemClicked(QTableWidgetItem *item);

    void on_tableWidget_cyferblaty_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous);

//    void on_tableWidget_cyferblaty_itemSelectionChanged();

//    void on_tableWidget_cyferblaty_cellActivated(int row, int column);

    void on_tableWidget_cyferblaty_cellClicked(int row, int column);

    void on_tableWidget_cyferblaty_cellDoubleClicked(int row, int column);

    void on_pushButton_delete_preference_clicked();

    void on_pushButton_shutdown_clicked();

	void on_pushButton_sciezka_clicked();

    void on_checkBox_uruchom_przy_logowaniu_stateChanged(int arg1);

    void on_comboBox_language_currentIndexChanged(int index);

    void on_radioButton_english_toggled(bool checked);

private:
    Ui::Topcje_dlg *ui;

    MainWindow * zegar;

    bool pam_flag_na_wierzchu;
    bool pam_flag_blokuj_na_ekranie;
    int pam_przezroczystosc;
    bool pam_flag_uruchom_przy_logowaniu;
    bool pam_flag__kilka_kopii;
    int pam_gskala;
    int pam_nr_jezyka;
    bool pam_flag_gskala_active;
    bool pam_flag_show_sekundnik;
    bool pam_flag_show_dzien_tygodnia;
    bool pam_flag_show_date;
    bool pam_flag_show_AM_PM;
    int pam_nr_tarczy;
    std::string pam_nazwa_timezone;  // nazwa timezone

    // nr wskazowek godz, min, sek

    bool pam_flag_wsk_godz_bitmapowa;  // bo jak nie to wektorowa
    bool pam_flag_wsk_min_bitmapowa;  // bo jak nie to wektorowa
    bool pam_flag_wsk_sek_bitmapowa;  // bo jak nie to wektorowa

    int pam_nr_wybranej_bitmapy_wsk_godzinowej;
    int pam_nr_wybranej_bitmapy_wsk_minutowej;
    int pam_nr_wybranej_bitmapy_wsk_sekundowej ;


    int pam_nr_wsk_wektorowej_godz;
    int pam_nr_wsk_wektorowej_min;
    int pam_nr_wsk_wektorowej_sek;

    void zapamientanie_stanu();
    void odpamientanie_stanu();

    int  indeks_favourites_dla_tarczy(std::string nazwa_tarczy);

    void info_o_faworytach(std::string nazwa_cyf);

    int get_info_current_hands();
    bool sprawdz_identycznosc_wskazowki_z_faworytami(
            std::string nazwa_ulubionej_wskazowki,
            bool flag_czy_bitmapa,
            std::string nazwa_bitmapy,
            int nr_wektora);

    void aktualizuj_taby_wskazowkowe();
    void laduj_tablice_cyferblatow();
    void zmiana_ikony_przy_edycji();

};

#endif // TOPCJE_DLG_H
