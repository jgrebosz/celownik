#include "topcje_dlg.h"
#include "ui_topcje_dlg.h"
#include <iostream>
#include <iomanip>
#include <sstream>

#include <QMessageBox>
#include <QTime>
#include <QTimeZone>
#include <QTableWidgetItem>
#include <QProcess>

#include "mainwindow.h"
#include "tpreferred_hands.h"


using namespace  std;

extern bool flag_na_wierzchu;
extern bool flag_ograniczenie_pozycji;
extern bool flag_blokuj_na_ekranie;
extern int przezroczystosc;
extern int przezroczystosc_dla_kursora;

extern bool flag_uruchom_z_loginem;
extern bool flag_uruchom_przy_logowaniu;
extern bool flag__kilka_kopii;
extern int nr_jezyka;
extern double gskala;
extern bool flag_gskala_active;

extern bool flag_show_AM_PM;
extern bool flag_show_dzien_tygodnia;
extern bool flag_show_date;
extern bool flag_show_sekundnik;

extern QTimeZone time_zone;
extern string nazwa_timezone;



vector <Tpreferred_hands>  vec_pref_hands;
//******************************************************************************************************
Topcje_dlg::Topcje_dlg(MainWindow * parent) :
    QDialog(parent),
    ui(new Ui::Topcje_dlg)
{

    hide();
    setModal(false);  // wolno zrobic tylko gdy jest hide
    show();

    zegar = parent;
    ui->setupUi(this);

    ui->groupBox_preferences->setVisible(false);
    ui->checkBox_na_wierzchu->setChecked( flag_na_wierzchu);
    ui->checkBox_blokuj_pozycje_na_ekranie->setChecked(flag_blokuj_na_ekranie);
    //    ui->checkBox_ogranicz_pozycje->setChecked(flag_ograniczenie_pozycji);

    ui->label_transparency->setNum(przezroczystosc);
    ui->horizontalSlider_przezroczystosc->setValue(przezroczystosc);


    //ui->checkBox_uruchom_z_linuxem->setChecked(flag_uruchom_z_linuxem);
    ui->checkBox_uruchom_przy_logowaniu->setChecked(flag_uruchom_przy_logowaniu);
    ui->checkBox_kilka_kopii->setChecked(flag__kilka_kopii);

    ui->horizontalSlider_zoom->setValue(gskala * 100);

       ui->comboBox_language->clear();
    QStringList list_jezykow = { "English"};
    ui->comboBox_language->insertItems(0, list_jezykow);
    ui->comboBox_language->setCurrentIndex(nr_jezyka);

    ui->checkBox_zoom->setChecked(flag_gskala_active);

    ui->checkBox_dispaly_sekundnik->setChecked( flag_show_sekundnik);

    ui->checkBox_dzien_tygodnia->setChecked(flag_show_dzien_tygodnia);
    ui->checkBox_display_date->setChecked(flag_show_date);

    ui->checkBox_AM_PM->setChecked(flag_show_AM_PM);

    laduj_tablice_cyferblatow();

    //---------------------------------------
    QDateTime localTime = QDateTime::currentDateTime();
    QDateTime zoneDateTime = localTime;
    string nazwa_strefy =  nazwa_timezone ;
    if(time_zone.isValid())
    {
        ////        zoneDateTime = localTime.toTimeZone(time_zone) ;
        ////        nazwa_strefy =  time_zone.displayName( zoneDateTime) .toStdString();
        //        cout << "biezacy time_zone ma nazwe = "
        //               << nazwa_strefy
        //               << endl;//    QDateTime::currentDateTimeUtc();
    }
    else
    {
        // jeśli invalid, to sami obliczamy róznice
        auto roznica = QDateTime::currentDateTimeUtc().time().hour() -
                QDateTime::currentDateTime().time().hour();

        //  cout << "biezacy time_zone   jest    inValid , roznica = " << roznica << endl;
        ostringstream s;
        s << "UTC"
          << ((roznica >= 0) ?  '+' : '-' )
          << setfill('0') << setw(2)
          << abs(roznica)  ;
        //        cout << "Zbudowana nazwa = [" << s.str() << "]" << endl;
        nazwa_strefy = s.str();

        // szukanie tego na liście


        //zoneDateTime = localTime;
    }


    // zbudowanie listy nazw stref ----------------------------------
    // z ustawieniem wybranej linijki na bieżąco obowiązującej strefie czasowej
    int indeks = 0;
    QString n;
    QList<QByteArray> lista = QTimeZone::availableTimeZoneIds();
    for(int i = 0 ; i < lista.size() ; ++i)
    {
        QString nazwa = lista[i].toStdString().c_str();
        ui->comboBox_timezone->addItem(nazwa);
        if(nazwa.toStdString() .find(nazwa_strefy) != string::npos )
        {
            indeks = i;
            n = nazwa;
        }
    }
    ui->comboBox_timezone->setCurrentIndex(indeks);
    time_zone = QTimeZone( n.toLatin1());

    // =======tablice wskazówek =====================

    // godzinowa
    {
        uint ile =  parent->vect_godzinowych.size();
        ui->tableWidget_wsk_godzinowe->setRowCount(ile);
        ui->tableWidget_wsk_godzinowe->setColumnCount(1);


        /*       QStringList labels;
                     labels << "nazwa wskazowk; minutowej";
                     ui->tableWidget->  setHorizontalHeaderLabels(labels) ;        */
        ui->tableWidget_wsk_godzinowe->setIconSize(QSize(150, 100));

        for(uint i = 0 ; i < ile ; ++i)
        {
            QString nazwa_ikony = parent->vect_godzinowych[i].bitmapa.c_str();
            //   QString nazwa_ikony = parent->cyferblat [i].bitmapa_tarcza.c_str();
            QPixmap robocza(nazwa_ikony);
            QTransform t ;
            t= t.rotate(90);

            QPixmap  robocza_pozioma = robocza.transformed( t );
            QIcon ikona (robocza_pozioma);

            string naz = parent->vect_godzinowych[i].nazwa;

            if(i == 0) ikona = QIcon();
            auto  *newItem = new  QTableWidgetItem( ikona, naz.c_str());


            ui->tableWidget_wsk_godzinowe->setItem(i, 0, newItem );
        }
        ui->tableWidget_wsk_godzinowe->resizeColumnsToContents ();

    }




    // -------minutowa tablica===========================================
    {
        uint ile =  parent->vect_minutowych.size();
        ui->tableWidget_wsk_minutowe->setRowCount(ile);
        ui->tableWidget_wsk_minutowe->setColumnCount(1);


        /*       QStringList labels;
           labels << "nazwa wskazowk; minutowej";
           ui->tableWidget->  setHorizontalHeaderLabels(labels) ;        */
        ui->tableWidget_wsk_minutowe->setIconSize(QSize(150, 100));

        for(uint i = 0 ; i < ile ; ++i)
        {
            QString nazwa_ikony = parent->vect_minutowych[i].bitmapa.c_str();
            //   QString nazwa_ikony = parent->cyferblat [i].bitmapa_tarcza.c_str();
            QPixmap robocza(nazwa_ikony);
            QTransform t ;
            t= t.rotate(90);

            QPixmap  robocza_pozioma = robocza.transformed( t );
            QIcon ikona (robocza_pozioma);

            string naz = parent->vect_minutowych[i].nazwa;

            if(i == 0) ikona = QIcon();
            auto  *newItem = new  QTableWidgetItem( ikona, naz.c_str());


            ui->tableWidget_wsk_minutowe->setItem(i, 0, newItem );
        }
        ui->tableWidget_wsk_minutowe->resizeColumnsToContents ();
    } // kon lok


    // -------sekundowa tablica-----------------------------------------
    {
        uint ile =  parent->vect_sekundnikow.size();
        ui->tableWidget_wsk_sekundowe->setRowCount(ile);
        ui->tableWidget_wsk_sekundowe->setColumnCount(2);


        /*       QStringList labels;
           labels << "nazwa wskazowk; minutowej";
           ui->tableWidget->  setHorizontalHeaderLabels(labels) ;        */
        ui->tableWidget_wsk_sekundowe->setIconSize(QSize(120, 200));


        for(uint i = 0 ; i < ile ; ++i)
        {
            QString nazwa_ikony = parent->vect_sekundnikow[i].bitmapa.c_str();
            //   QString nazwa_ikony = parent->cyferblat [i].bitmapa_tarcza.c_str();
            QPixmap robocza(nazwa_ikony);
            QTransform t ;
            t= t.rotate(90);

            QPixmap  robocza_pozioma = robocza.transformed( t );
            QIcon ikona (robocza_pozioma);

            string naz = parent->vect_sekundnikow[i].nazwa;

            if(i == 0) ikona = QIcon();
            auto  *newItem = new  QTableWidgetItem( ikona, naz.c_str());


            ui->tableWidget_wsk_sekundowe->setItem(i, 0, newItem );




        }
        ui->tableWidget_wsk_sekundowe->resizeColumnsToContents ();


    } // kon lok


    //-------------



    ui->radioButton_hours_hand_bitmap_type->
            setChecked(parent->flag_wsk_godz_bitmapowa );
    ui->radioButton_hours_hand_vector_type->
            setChecked(!parent->flag_wsk_godz_bitmapowa );

    ui->tableWidget_wsk_godzinowe->setEnabled(parent->flag_wsk_godz_bitmapowa);

    ui->radioButton_minutes_hand_bitmap_type->
            setChecked(parent->flag_wsk_min_bitmapowa );
    ui->radioButton_minutes_hand_vector_type->
            setChecked(!parent->flag_wsk_min_bitmapowa );

    ui->tableWidget_wsk_minutowe->setEnabled(parent->flag_wsk_min_bitmapowa);

    // sek
    ui->radioButton_sec_bitmap_type->
            setChecked(parent->flag_wsk_sek_bitmapowa );
    ui->radioButton_sec_vector_type ->
            setChecked(!parent->flag_wsk_sek_bitmapowa );

    ui->tableWidget_wsk_sekundowe->setEnabled(parent->flag_wsk_sek_bitmapowa);


    aktualizuj_taby_wskazowkowe();




    zapamientanie_stanu();
}
//******************************************************************************************************
Topcje_dlg::~Topcje_dlg()
{

    // może yu trzeba skasować treść tablic z bitmapami wskazowek?
    for(uint i = 0 ; i < zegar->vect_minutowych.size() ; ++i)
    {
        delete  ui->tableWidget_wsk_minutowe->itemAt(0, i);
    }
    for(uint i = 0 ; i < zegar->vect_godzinowych.size() ; ++i)
    {
        delete  ui->tableWidget_wsk_godzinowe->itemAt(0, i);
    }
    for(uint i = 0 ; i < zegar->vect_sekundnikow.size() ; ++i)
    {
        delete  ui->tableWidget_wsk_sekundowe->itemAt(0, i);
    }

    delete ui;
}
//******************************************************************************************************
void Topcje_dlg::on_checkBox_na_wierzchu_clicked(bool /*checked*/ )
{
    // To się załatwia po zrobieniu OK

    // cout << "obecnie stan checkboxa to " <<checked << endl;

}
////******************************************************************************************************
//void Topcje_dlg::on_checkBox_ogranicz_pozycje_clicked(bool checked)
//{
//    cout << "obecnie stan checkboxaogranicz_pozycje " <<checked << endl;
//}
//******************************************************************************************************
void Topcje_dlg::on_checkBox_blokuj_pozycje_na_ekranie_clicked(bool /*checked*/ )
{
    // cout << "obecnie stan checkboxaogranicz_pozycje " <<checked << endl;
}
//******************************************************************************************************
void Topcje_dlg::on_horizontalSlider_przezroczystosc_valueChanged(int value)
{
    //    cout << "obecnie wartosc slidera  " << value << endl;
    ui->label_transparency->setNum(value);
    zegar-> zmien_przezroczystosc(value);
}
//******************************************************************************************************
//******************************************************************************************************
void Topcje_dlg::on_buttonBox_accepted()
{
    przezroczystosc =  ui->horizontalSlider_przezroczystosc->value();

    flag_na_wierzchu = ui->checkBox_na_wierzchu->isChecked();
    flag_blokuj_na_ekranie = ui->checkBox_blokuj_pozycje_na_ekranie->isChecked();
    //flag_ograniczenie_pozycji = ui->checkBox_ogranicz_pozycje->isChecked();

    //flag_uruchom_z_linuxem = ui->checkBox_uruchom_z_linuxem->isChecked();
    flag_uruchom_przy_logowaniu = ui->checkBox_uruchom_przy_logowaniu->isChecked();
    flag__kilka_kopii = ui->checkBox_kilka_kopii->isChecked();

    nr_jezyka = ui->comboBox_language->currentIndex();

    flag_show_AM_PM  = ui->checkBox_AM_PM->isChecked();
    flag_show_dzien_tygodnia =  ui->checkBox_dzien_tygodnia->isChecked();
    flag_show_date = ui->checkBox_display_date->isChecked();
    nazwa_timezone = ui->comboBox_timezone->currentText().toStdString();

    //    zegar->flag_wsk_godz_bitmapowa = ui->radioButton_hours_hand_bitmap_type->isChecked();
    //    zegar->flag_wsk_min_bitmapowa = ui->radioButton_minutes_hand_bitmap_type->isChecked();
    //    zegar->flag_wsk_sek_bitmapowa = ui->radioButton_sec_bitmap_type->isChecked();
}
//******************************************************************************************************
void Topcje_dlg::on_horizontalSlider_zoom_valueChanged(int value)
{
    gskala = value/100.0;
    //   ui->label_zoom->setNum(gskala);

    ui->label_zoom->setNum(value);
    auto tmp = ui->label_zoom->text();
    ui->label_zoom->setText(tmp + " %");

    zegar->resize_okna_z_tarcza();
}
//******************************************************************************************************
void Topcje_dlg::on_checkBox_zoom_clicked(bool checked)
{
    ui->horizontalSlider_zoom->setEnabled(checked);
    ui->label_zoom->setEnabled(checked);
    if(!checked) gskala = 1.0;
}
//******************************************************************************************************
//void Topcje_dlg::on_checkBox_am_pm_clicked(bool checked)
//{
//    flag_show_dzien_tygodnia = checked;
//}
//******************************************************************************************************
void Topcje_dlg::on_checkBox_display_date_clicked(bool checked)
{
    flag_show_date = checked;
    // cout << "obecnie on_checkBox_display_date_clicked  " << checked << endl;
}
//******************************************************************************************************
void Topcje_dlg::on_checkBox_dispaly_sekundnik_clicked(bool checked)
{
    flag_show_sekundnik = checked;
}
//******************************************************************************************************
//******************************************************************************************************
void Topcje_dlg::on_comboBox_timezone_currentIndexChanged(int /* index */ )
{
    //    cout << "Combo Indeks strefy czasowej, indeks " << index
    //         << " " <<ui->comboBox_timezone->currentText().toStdString()
    //         << endl;



    //    QDateTime now = QDateTime::currentDateTime();
    //    cout << "Nazwa strefy " << now.timeZone().displayName(now).toStdString()
    //         << endl;
    //    //    QTime time = QTime::currentTime();
    //    //    QDate date = QDate::currentDate();

    //    QDateTime UTC = QDateTime::currentDateTimeUtc();

    QString strefa = ui->comboBox_timezone->currentText();
    time_zone = QTimeZone( strefa.toLatin1()  );

    //    QDateTime local(UTC.toTimeZone(time_zone) );

    //    //dateTime.setTimeZone(zone );

    //    QTime time = local.time();
    //    int godzina = time.hour();

    //    // wydruk biezacego czasu w danej strefie
    //    cout << "godzina w tej strefie = "
    //         << local.toString().toStdString()
    //         << " godzina = "
    //         << godzina
    //         << endl;

    //     QList<QByteArray> lista = QTimeZone::availableTimeZoneIds();

    //    for(int i = 0 ; i < lista.size() ; ++i)
    //    {
    //        cout <<  lista[i].toStdString().c_str() << endl;

    //    }


}
//*****************************************************************************
void Topcje_dlg::on_tableWidget_wsk_minutowe_cellClicked(int row, int /*column*/ )
{
    zegar->nr_wybranej_bitmapy_wsk_minutowej = row;
    zegar->zmiana_wygladu_cyferblatu(zegar->nr_tarczy, tryb_wskazowek::zwykle);
}
//*********************************************************************************
void Topcje_dlg::on_radioButton_minutes_hand_bitmap_type_toggled(bool checked)
{
    zegar->flag_wsk_min_bitmapowa = checked;
    zegar->zmiana_wygladu_cyferblatu(zegar->nr_tarczy,   tryb_wskazowek::zwykle);

    ui->tableWidget_wsk_minutowe->setEnabled(checked);
    ui->spinBox_nr_wsk_wektorowej_min->setEnabled(! checked);
}
//*********************************************************************************
void Topcje_dlg::on_radioButton_hours_hand_bitmap_type_toggled(bool checked)
{
    zegar->flag_wsk_godz_bitmapowa = checked;
    zegar->zmiana_wygladu_cyferblatu(zegar->nr_tarczy,  tryb_wskazowek::zwykle);

    ui->tableWidget_wsk_godzinowe->setEnabled(checked);
    ui->spinBox_nr_wsk_wektorowej_godz->setEnabled(! checked);
}
//*********************************************************************************
void Topcje_dlg::on_tableWidget_wsk_godzinowe_cellClicked(int row, int /*column*/)
{
    zegar->nr_wybranej_bitmapy_wsk_godzinowej = row;
    zegar->zmiana_wygladu_cyferblatu(zegar->nr_tarczy,  tryb_wskazowek::zwykle);
}
//*********************************************************************************
void Topcje_dlg::on_radioButton_sec_bitmap_type_toggled(bool checked)
{
    //    cout << " flag_wsk_sek_bitmapowa " << checked << endl;
    zegar->flag_wsk_sek_bitmapowa= checked;
    zegar->zmiana_wygladu_cyferblatu(zegar->nr_tarczy,   tryb_wskazowek::zwykle);

    ui->tableWidget_wsk_sekundowe->setEnabled(checked);
    ui->spinBox_nr_wsk_wektorowej_sek->setEnabled(! checked);
    // aktualizuj_taby_wskazowkowe(); blad?
}
//*********************************************************************************
void Topcje_dlg::on_tableWidget_wsk_sekundowe_cellClicked(int row, int /*column*/)
{
    zegar->nr_wybranej_bitmapy_wsk_sekundowej= row;
    zegar->zmiana_wygladu_cyferblatu(zegar->nr_tarczy,  tryb_wskazowek::zwykle);
}
//**********************************************************************************************************
void Topcje_dlg::on_spinBox_nr_wsk_wektorowej_godz_valueChanged(int arg1)
{
    zegar->nr_wsk_wektorowej_godz = arg1;
    zegar->zmiana_wygladu_cyferblatu(zegar->nr_tarczy,  tryb_wskazowek::zwykle);
}
//**********************************************************************************************************
void Topcje_dlg::on_spinBox_nr_wsk_wektorowej_min_valueChanged(int arg1)
{
    zegar->nr_wsk_wektorowej_min = arg1;
    zegar->zmiana_wygladu_cyferblatu(zegar->nr_tarczy,  tryb_wskazowek::zwykle);
}
//**********************************************************************************************************
void Topcje_dlg::on_spinBox_nr_wsk_wektorowej_sek_valueChanged(int arg1)
{
    zegar->nr_wsk_wektorowej_sek = arg1;
    zegar->zmiana_wygladu_cyferblatu(zegar->nr_tarczy,  tryb_wskazowek::zwykle);
}
//**********************************************************************************************************
void Topcje_dlg::on_checkBox_AM_PM_clicked(bool checked)
{
    flag_show_AM_PM = checked;
}
//**********************************************************************************************************
void Topcje_dlg::zapamientanie_stanu()
{
    pam_flag_na_wierzchu  = flag_na_wierzchu;
    pam_flag_blokuj_na_ekranie = flag_blokuj_na_ekranie;
    pam_przezroczystosc = przezroczystosc;
    pam_flag_uruchom_przy_logowaniu = flag_uruchom_przy_logowaniu;
    pam_flag__kilka_kopii = flag__kilka_kopii;
    pam_gskala = gskala;
    pam_nr_jezyka = nr_jezyka;
    pam_flag_gskala_active = flag_gskala_active;
    pam_flag_show_sekundnik = flag_show_sekundnik;
    pam_flag_show_dzien_tygodnia = flag_show_dzien_tygodnia;
    pam_flag_show_date = flag_show_date;
    pam_flag_show_AM_PM = flag_show_AM_PM;
    pam_nr_tarczy = zegar->nr_tarczy;
    pam_nazwa_timezone = nazwa_timezone;  // nazwa timezone

    // nr wskazowek godz, min, sek

    pam_flag_wsk_godz_bitmapowa =     zegar->flag_wsk_godz_bitmapowa;  // bo jak nie to wektorowa
    pam_flag_wsk_min_bitmapowa =  zegar->flag_wsk_min_bitmapowa;  // bo jak nie to wektorowa
    pam_flag_wsk_sek_bitmapowa =  zegar->flag_wsk_sek_bitmapowa;


    pam_nr_wybranej_bitmapy_wsk_godzinowej =  zegar-> nr_wybranej_bitmapy_wsk_godzinowej ;
    pam_nr_wybranej_bitmapy_wsk_minutowej =  zegar-> nr_wybranej_bitmapy_wsk_minutowej;
    pam_nr_wybranej_bitmapy_wsk_sekundowej = zegar-> nr_wybranej_bitmapy_wsk_sekundowej;

    pam_nr_wsk_wektorowej_godz = zegar->nr_wsk_wektorowej_godz;
    pam_nr_wsk_wektorowej_min = zegar->nr_wsk_wektorowej_min;
    pam_nr_wsk_wektorowej_sek = zegar->nr_wsk_wektorowej_sek;
}
//**********************************************************************************************************
void Topcje_dlg::odpamientanie_stanu()
{
    flag_na_wierzchu  =pam_flag_na_wierzchu;
    flag_blokuj_na_ekranie = pam_flag_blokuj_na_ekranie;
    przezroczystosc = pam_przezroczystosc;
    flag_uruchom_przy_logowaniu =pam_flag_uruchom_przy_logowaniu;
    flag__kilka_kopii =pam_flag__kilka_kopii;
    gskala = pam_gskala;
    nr_jezyka = pam_nr_jezyka;
    flag_gskala_active =pam_flag_gskala_active;
    flag_show_sekundnik =pam_flag_show_sekundnik;
    flag_show_dzien_tygodnia =pam_flag_show_dzien_tygodnia;
    flag_show_date =pam_flag_show_date;
    flag_show_AM_PM =pam_flag_show_AM_PM;
    zegar->nr_tarczy = pam_nr_tarczy;
    nazwa_timezone = pam_nazwa_timezone;  // nazwa timezone

    // nr wskazowek godz, min, sek

    zegar-> flag_wsk_godz_bitmapowa =     pam_flag_wsk_godz_bitmapowa;  // bo jak nie to wektorowa
    zegar->flag_wsk_min_bitmapowa = pam_flag_wsk_min_bitmapowa;  // bo jak nie to wektorowa
    zegar->flag_wsk_sek_bitmapowa =  pam_flag_wsk_sek_bitmapowa;

    zegar-> nr_wybranej_bitmapy_wsk_godzinowej  = pam_nr_wybranej_bitmapy_wsk_godzinowej ;
    zegar-> nr_wybranej_bitmapy_wsk_minutowej = pam_nr_wybranej_bitmapy_wsk_minutowej;
    zegar-> nr_wybranej_bitmapy_wsk_sekundowej = pam_nr_wybranej_bitmapy_wsk_sekundowej;

    zegar->nr_wsk_wektorowej_godz = pam_nr_wsk_wektorowej_godz;
    zegar->nr_wsk_wektorowej_min = pam_nr_wsk_wektorowej_min;
    zegar->nr_wsk_wektorowej_sek = pam_nr_wsk_wektorowej_sek;


}
//**********************************************************************************************************
int Topcje_dlg::indeks_favourites_dla_tarczy(string nazwa_tarczy)
{
    for(uint nr = 0 ; nr < vec_pref_hands.size() ; ++nr)
    {
        if(vec_pref_hands[nr].clock_face_name == nazwa_tarczy) return  nr;
    }
    return  -1;
}
//**********************************************************************************************************
void Topcje_dlg::info_o_faworytach(string /*nazwa_cyf*/ )
{

    auto w = zegar;
    // tekst na okoliczność użycia innych wskazowek niż fabryczne

    string info;
#if 0
    info += "The current clock face is called: \n        ";
    info += zegar->cyferblat[zegar-> nr_tarczy].nazwa;

    info +=  "\nThis clock face is currently displayed with the following clock hands:\n";
    info +=  "hours hand is: \n       ";
    if(w->flag_wsk_godz_bitmapowa)
    {
        info += w->vect_godzinowych[w->nr_wybranej_bitmapy_wsk_godzinowej].nazwa ;
    }
    else{
        info += "vector-type hours hand nr " + to_string( w->nr_wsk_wektorowej_godz) ;

    }


    info +=  "\nminutes hand is: \n       ";
    if(w->flag_wsk_min_bitmapowa)
    {
        info += w->vect_minutowych[w->nr_wybranej_bitmapy_wsk_minutowej].nazwa ;
    }
    else{
        info += "vector-type minutes hand nr " + to_string( w->nr_wsk_wektorowej_min) ;

    }


    info +=  "\nsecondes  hand is: \n       ";

    if(w->flag_wsk_sek_bitmapowa)
    {
        info += w->vect_sekundnikow[w->nr_wybranej_bitmapy_wsk_sekundowej].nazwa ;
    }
    else{
        info += "vector-type seconds hand nr " + to_string( w->nr_wsk_wektorowej_sek) ;

    }

#endif

    int nrf = indeks_favourites_dla_tarczy((zegar->cyferblat[zegar-> nr_tarczy].nazwa.c_str()));


    bool  flag_zaproponuj_assignment = false;
    bool flag_delete_assignment_posisble = true;
    bool flag_return_to_preferred = false;
    //  bool flag_return_to_factory = true;
    if( nrf == -1)
    {
        //        tzn. nie ma faworytow
        // można pokazać button "assing"
        string txt = info +  "\n\nThere are NO PREFERENCES YET\nabout hands to be used with this clock face\n";

        ui->label_fav_info->setText(txt.c_str());

        flag_delete_assignment_posisble = false;;
        flag_zaproponuj_assignment = true ;
        flag_return_to_preferred = false;

        //        flag_favourtes_sa_obecnie = false;
    }
    else
    {
        string txt = info + "\n\nYou already decided about a set of hands,\nfor this clock-face";

        // jest faworyt po nr nrf

        // spr godzinowej ====================================
        if(sprawdz_identycznosc_wskazowki_z_faworytami(
                    vec_pref_hands[nrf].hours_hand,

                    w->flag_wsk_godz_bitmapowa,
                    w->vect_godzinowych[w->nr_wybranej_bitmapy_wsk_godzinowej].nazwa,
                    w->nr_wsk_wektorowej_godz)
                )
        {
            //           txt += "\nWskazowka godzinowa indentyczna jak favoutite ";
        }else
        {
            //            txt += "\nobecna wsk godzinowa jest INNA niż favoirte ";
            flag_zaproponuj_assignment = true ;
            //            flag_favourtes_sa_obecnie = false;
            flag_return_to_preferred = true;
        }


        // spr minutowej ====================================
        if(sprawdz_identycznosc_wskazowki_z_faworytami(
                    vec_pref_hands[nrf].minutes_hand,

                    w->flag_wsk_min_bitmapowa,
                    w->vect_minutowych[w->nr_wybranej_bitmapy_wsk_minutowej].nazwa,
                    w->nr_wsk_wektorowej_min)
                )
        {
            //            txt += "\nWskazowka minutowa indentyczna jak favoutite ";
        }else
        {
            //            txt += "\nobecna wsk minutowa jest INNA niż favoirte ";
            flag_zaproponuj_assignment = true ;
            //            flag_favourtes_sa_obecnie = false;
            flag_return_to_preferred = true;
        }



        // spr sekundowej ====================================
        if(sprawdz_identycznosc_wskazowki_z_faworytami(
                    vec_pref_hands[nrf].seconds_hand,
                    w->flag_wsk_sek_bitmapowa,
                    w->vect_sekundnikow[w->nr_wybranej_bitmapy_wsk_sekundowej].nazwa,
                    w->nr_wsk_wektorowej_sek)
                )
        {
            //            txt += "\nWskazowka sekudowa indentyczna jak favoutite ";
        }else
        {
            //             txt += "\nobecna wsk sekudowa jest INNA niż favoirte ";
            flag_zaproponuj_assignment = true ;
            //            flag_favourtes_sa_obecnie = false;
            flag_return_to_preferred = true;
        }
        //========================

        if(flag_zaproponuj_assignment)
        {
            txt += "\nbut currently some hands are DIFFERENT than the preferred ones ";
        }
        else{
            txt += "\nand this is EXACTLY the current set of hands";
        }
        ui->label_fav_info->setText(txt.c_str());
    }
    ui->pushButton_assign_preferred_hands->setEnabled(flag_zaproponuj_assignment);
    ui->pushButton_return_favourite->setEnabled(flag_return_to_preferred);
    ui->pushButton_delete_preference->setEnabled(flag_delete_assignment_posisble);




    bool fabr = zegar->czy_biezace_wskazowki_sa_fabryczne();
    ui->pushButton_reset_to_factory ->setEnabled(fabr != true);


    ui->label_inf_czy_fabryczne->setText( fabr ? "This is a FACTORY set of hands"
                                               : "This is not a factory set of hands");
    ui->label_inf_czy_fabryczne->setStyleSheet("color: rgb(0, 0, 127);");


}
//******************************************************************************************************
bool Topcje_dlg::sprawdz_identycznosc_wskazowki_z_faworytami(
        string nazwa_ulubionej_wskazowki,
        bool flag_czy_bitmapa,
        std::string nazwa_bitmapy,
        int nr_wektora)
{

    if(flag_czy_bitmapa)
    {
        //        cout << "Porównanie bitmap: "
        //                " wskazowka obecna  = " << nazwa_bitmapy
        //             << " a ulubiona jest :" << nazwa_ulubionej_wskazowki << endl;

        if(nazwa_ulubionej_wskazowki == nazwa_bitmapy) return true;
        else return false;
    }
    else {
        //        cout << "Porównanie wsk wektorowo  "
        //                " wskazowka obecna  nr = " << nr_wektora
        //             << " a ulubiona jest :" << nazwa_ulubionej_wskazowki << endl;

        if(nazwa_ulubionej_wskazowki == to_string(nr_wektora)) return true;
        else return false;
    }
}


//*********************************************************************************************************************
void Topcje_dlg::on_buttonBox_rejected()
{
    odpamientanie_stanu();
    zegar->zmiana_wygladu_cyferblatu(zegar->nr_tarczy,  tryb_wskazowek::zwykle);
}
//*********************************************************************************************************************
void Topcje_dlg::on_checkBox_dzien_tygodnia_clicked(bool checked)
{
    flag_show_dzien_tygodnia  = checked;
}

//*********************************************************************************************************************
//*********************************************************************************************************************

void Topcje_dlg::on_pushButton_assign_preferred_hands_clicked()
{
    //    stworzenie chiwlowego obiektu z danymi

    string naz_wsk_godz;

    if(zegar-> flag_wsk_godz_bitmapowa)
    {
        int nr =  zegar->nr_wybranej_bitmapy_wsk_godzinowej;
        naz_wsk_godz = zegar->vect_godzinowych[nr].nazwa;
        //        cout << "bitmapa wsk godzi nazywa sie " <<  naz_wsk_godz << endl;
    }else    // bo jak nie to wektorowa
    {
        naz_wsk_godz = // "wektorowa_nr_" +
                to_string(zegar->nr_wsk_wektorowej_godz) ;

        //        cout << "bitmapa wsk godzi nazywa sie " <<  naz_wsk_godz << endl;
    }
    // ----------- minutowa ------------
    string naz_wsk_min;
    if(zegar-> flag_wsk_min_bitmapowa)
    {
        int nr = zegar-> nr_wybranej_bitmapy_wsk_minutowej;
        naz_wsk_min = zegar->vect_minutowych[nr].nazwa;
        //        cout << "bitmapa wsk minutowa nazywa sie " <<  naz_wsk_min << endl;
    }else    // bo jak nie to wektorowa
    {
        naz_wsk_min = // "wektorowa_nr_" +
                to_string(zegar->nr_wsk_wektorowej_min) ;

        //        cout << "bitmapa wsk minutowa nazywa sie " <<  naz_wsk_min << endl;
    }

    // ----------- sekundnik ------------
    string naz_wsk_sek;
    if(zegar-> flag_wsk_sek_bitmapowa)
    {
        int nr = zegar-> nr_wybranej_bitmapy_wsk_sekundowej;
        naz_wsk_sek = zegar->vect_sekundnikow[nr].nazwa;
        //        cout << "bitmapa wsk sekundowa nazywa sie " <<  naz_wsk_sek << endl;
    }else    // bo jak nie to wektorowa
    {
        naz_wsk_sek = // "wektorowa_nr_" +
                to_string(zegar->nr_wsk_wektorowej_sek) ;

        //        cout << "bitmapa wsk sekundowa nazywa sie " <<  naz_wsk_sek << endl;
    }


    string nazwa_cyferbatu = zegar->cyferblat[ zegar->nr_tarczy].nazwa;
    //    cout << "Biezaca nazwa cyferblatu to " << nazwa_cyferbatu << endl;

    Tpreferred_hands  robocze_pref(
                nazwa_cyferbatu,
                naz_wsk_godz,
                naz_wsk_min,
                naz_wsk_sek);

    // biezaco na ekranie jest cyfreblat o nazwie



    // szukanie czy jest info n/t teakiego cyferblatu
    bool znalezione = false;
    // sprawdzenie czy jest entry dla biezącego cyferblatu
    for(auto & elem : vec_pref_hands )
    {
        if(elem.clock_face_name == nazwa_cyferbatu)
        {
            // jeśli tak, to  podmieniamy entry
            elem = robocze_pref;
            znalezione = true;
            //            cout << "Podmienione preferencje dla cyferblatu to " << nazwa_cyferbatu << endl;
        }

    }
    if(!znalezione){
        // jeśli nie, to nowe entry dodajeym do końca vektora
        vec_pref_hands.push_back(robocze_pref);
        //        cout << "Dodane nowe preferencje dla cyferblatu to " << nazwa_cyferbatu << endl;

    }

    zegar->zapisz_ulubione_wskazowki_na_dysku();
    info_o_faworytach(nazwa_cyferbatu);
    laduj_tablice_cyferblatow();

}
//***********************************************************************************************************************************
void Topcje_dlg::on_pushButton_delete_preference_clicked()

{
    vector<Tpreferred_hands> tmp ;

    string nazwa_cyferbatu = zegar->cyferblat[ zegar->nr_tarczy].nazwa;
    // szukanie czy jest info n/t teakiego cyferblatu

    // sprawdzenie czy jest entry dla biezącego cyferblatu
    for(uint i = 0 ; i < vec_pref_hands.size() ; ++i )
    {
        if(vec_pref_hands[i].clock_face_name != nazwa_cyferbatu)
        {
            tmp.push_back(vec_pref_hands[i]);
        }

    }
    vec_pref_hands = tmp;

    zegar->zapisz_ulubione_wskazowki_na_dysku();

    //    // powrót to bitmapowych wskazowek nr 0 bo te są "fabryczne". NIE zawsze


    //    zegar->flag_wsk_godz_bitmapowa =
    //             zegar->cyferblat[ zegar->nr_tarczy].flag_fabryczna_wsk_godz_bitmapowa ?
    //                true : false ;

    //    zegar->flag_wsk_min_bitmapowa =
    //            zegar->cyferblat[ zegar->nr_tarczy].flag_fabryczna_wsk_min_bitmapowa ?
    //                true : false ;
    //    zegar->flag_wsk_sek_bitmapowa =
    //            zegar->cyferblat[ zegar->nr_tarczy].flag_fabryczna_wsk_sek_bitmapowa ?
    //                true : false ;


    //    zegar->nr_wybranej_bitmapy_wsk_godzinowej = 0;
    //    zegar->nr_wybranej_bitmapy_wsk_minutowej = 0;
    //    zegar->nr_wybranej_bitmapy_wsk_sekundowej = 0;

    //    zegar->zmiana_wygladu_cyferblatu(zegar->nr_tarczy, tryb_wskazowek::fabryczne);

    info_o_faworytach(nazwa_cyferbatu);
    laduj_tablice_cyferblatow();
}
//**************************************************************************************************************
void Topcje_dlg::on_tabWidget_currentChanged(int /*index*/)
{

    string nazwa_cyferbatu = zegar->cyferblat[ zegar->nr_tarczy].nazwa;
    info_o_faworytach(nazwa_cyferbatu);
    aktualizuj_taby_wskazowkowe();

}
//**************************************************************************************************************
void Topcje_dlg::aktualizuj_taby_wskazowkowe()
{

    // dowiadujemy się o typ biezace wsk  (bitmapa/wektor)
    // ustawiamy radio buttton

    // godz ----------------------------------------------------------



    ui->radioButton_hours_hand_bitmap_type->
            setChecked(zegar->flag_wsk_godz_bitmapowa );
    ui->radioButton_hours_hand_vector_type->
            setChecked(!zegar->flag_wsk_godz_bitmapowa );


    ui->spinBox_nr_wsk_wektorowej_godz->setEnabled(! zegar->flag_wsk_godz_bitmapowa);
    ui->spinBox_nr_wsk_wektorowej_godz->setValue(zegar->nr_wsk_wektorowej_godz);

    if(zegar->flag_wsk_godz_bitmapowa)
    {
        ui->tableWidget_wsk_godzinowe->selectRow(zegar->nr_wybranej_bitmapy_wsk_godzinowej);
    }else{
        ui->tableWidget_wsk_godzinowe->clearSelection();
    }
    ui->tableWidget_wsk_godzinowe->setEnabled( zegar->flag_wsk_godz_bitmapowa);


    // min----------------------------------------------------------
    ui->radioButton_minutes_hand_bitmap_type->
            setChecked(zegar->flag_wsk_min_bitmapowa );
    ui->radioButton_minutes_hand_vector_type->
            setChecked(!zegar->flag_wsk_min_bitmapowa );


    ui->spinBox_nr_wsk_wektorowej_min->setEnabled(! zegar->flag_wsk_min_bitmapowa);
    ui->spinBox_nr_wsk_wektorowej_min->setValue(zegar->nr_wsk_wektorowej_min);

    if(zegar->flag_wsk_min_bitmapowa)
    {
        ui->tableWidget_wsk_minutowe->selectRow(zegar->nr_wybranej_bitmapy_wsk_minutowej);
    }else{
        ui->tableWidget_wsk_minutowe->clearSelection();
    }

    ui->tableWidget_wsk_minutowe->setEnabled(zegar->flag_wsk_min_bitmapowa);

    // sek ----------------------------------------------------------
    ui->radioButton_sec_bitmap_type->
            setChecked(zegar->flag_wsk_sek_bitmapowa );
    ui->radioButton_sec_vector_type ->
            setChecked(!zegar->flag_wsk_sek_bitmapowa );


    ui->spinBox_nr_wsk_wektorowej_sek->setEnabled(! zegar->flag_wsk_sek_bitmapowa);
    ui->spinBox_nr_wsk_wektorowej_sek->setValue(zegar->nr_wsk_wektorowej_sek);

    if(zegar->flag_wsk_sek_bitmapowa)
    {
        ui->tableWidget_wsk_sekundowe->selectRow(zegar->nr_wybranej_bitmapy_wsk_sekundowej);
    }else{
        ui->tableWidget_wsk_sekundowe->clearSelection();
    }

    ui->tableWidget_wsk_sekundowe->setEnabled(zegar->flag_wsk_sek_bitmapowa);


}
//**************************************************************************************************************
void Topcje_dlg::laduj_tablice_cyferblatow()
{

    ui->tableWidget_cyferblaty->setRowCount(zegar->cyferblat.size());
    ui->tableWidget_cyferblaty->setColumnCount(2);
    ui->tableWidget_cyferblaty->setIconSize(QSize(50, 30));

    // Uwaga, naglowki mozna ustalac dopiero gdy ustawi sie rozmiary tablicy
    QStringList naglowki;
    naglowki += "Clock-face";
    naglowki += "preferences?";

    ui->tableWidget_cyferblaty->setHorizontalHeaderLabels(naglowki);
    //    ui->tableWidget_cyferblaty-> setHorizontalHeaderItem(1, new QTableWidgetItem("naza") );


    //       QPixmap gwiazdka(":/new/prefix1/content/star.png");
    QPixmap gwiazdka(":/new/prefix1/content/pencil.png");
    //gwiazdka = gwiazdka.scaled(15, 15);
    QPixmap nic;

    for(uint i = 0 ; i < zegar->cyferblat.size(); ++i)
    {


        QString nazwa_ikony = zegar->cyferblat[i].bitmapa_tarcza.c_str();

        QPixmap robocza(nazwa_ikony);

        QIcon ikona (robocza);
        //----------
        auto  *newItem = new  QTableWidgetItem(robocza, zegar->cyferblat[i].nazwa.c_str());
        ui->tableWidget_cyferblaty->setItem(i, 0, newItem );



        bool flag_pref = indeks_favourites_dla_tarczy(zegar->cyferblat[i].nazwa) < 0 ? false : true;
        string pref_txt = flag_pref ? "yes" : " ";
        //        cout << "dla tarczy " << zegar->cyferblat[i].nazwa << " pref = " << flag_pref << endl;
        QIcon jest_pref ( flag_pref ? gwiazdka : nic);

        ui->tableWidget_cyferblaty->setItem(i, 1, new QTableWidgetItem( jest_pref, pref_txt.c_str() ) );
    }

    ui->tableWidget_cyferblaty->resizeColumnsToContents ();
    int nr = zegar-> nr_tarczy;
    //  ui->tableWidget_cyferblaty->setCurrentCell(nr, 0) ;
    //    ui->tableWidget_cyferblaty->selectRow(nr);
    ui->tableWidget_cyferblaty->scrollToItem(
                ui->tableWidget_cyferblaty->item(nr, 0),
                QAbstractItemView::PositionAtCenter );
    ui->tableWidget_cyferblaty->selectRow(nr);
    info_o_faworytach(zegar->cyferblat[nr].nazwa.c_str());

}


//**************************************************************************************************************
void Topcje_dlg::on_pushButton_return_favourite_clicked()
{
    //   auto nazwa = current->text().toStdString();

    zegar->zmiana_wygladu_cyferblatu(zegar->nr_tarczy,  tryb_wskazowek::ulubione ) ; //  force_favorites!
    string nazwa_cyferbatu = zegar->cyferblat[ zegar->nr_tarczy].nazwa;
    info_o_faworytach(nazwa_cyferbatu);

}
//**************************************************************************************************************
void Topcje_dlg::on_checkBox_favourites_clicked(bool checked)
{
    ui->groupBox_preferences->setVisible(checked);
}
//**************************************************************************************************************

void Topcje_dlg::on_tableWidget_cyferblaty_itemClicked(QTableWidgetItem *item)
{
    auto nazwa = item->text().toStdString();
    zegar->wybor_cyferblatu(nazwa);
    info_o_faworytach(nazwa);

}
//**************************************************************************************************************
void Topcje_dlg::on_tableWidget_cyferblaty_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *  /*previous*/ )
{
    auto nazwa = current->text().toStdString();
    zegar->wybor_cyferblatu(nazwa);
    info_o_faworytach(nazwa);
    zmiana_ikony_przy_edycji();
}
//**************************************************************************************************************


//void Topcje_dlg::on_tableWidget_cyferblaty_itemSelectionChanged()
//{

//    cout << "Nie uzywane AAA ???" << endl;
////    ui->tableWidget_cyferblaty->selectRow( clearContents();
//}
//**************************************************************************************************************

//**************************************************************************************************************
void Topcje_dlg::on_tableWidget_cyferblaty_cellClicked(int row, int /* column */ )
{
    // cout << "Kliknieta komorka [" << row << "] [" << column <<"]"  << endl;
    // zaznaczamy cały rząd, mimo że tylko np. drugą kolumnę kliknięto
    ui->tableWidget_cyferblaty->selectRow(row);
}
//**************************************************************************************************************
void Topcje_dlg::zmiana_ikony_przy_edycji()
{

    int nr = zegar->nr_tarczy;
    // QString nazwa_ikony =
    QPixmap ikona_tarczy (zegar->cyferblat[nr].bitmapa_tarcza.c_str());
    ui->checkBox_favourites->setIcon(ikona_tarczy);
    ui->checkBox_favourites->setIconSize(QSize(40,40));
}
//**************************************************************************************************************

void Topcje_dlg::on_tableWidget_cyferblaty_cellDoubleClicked(int row, int column)
{

    on_tableWidget_cyferblaty_cellClicked(row, column);

}
//**************************************************************************************************************
void Topcje_dlg::on_pushButton_reset_to_factory_clicked()
{
    // powrót to bitmapowych wskazowek nr 0 bo te są "fabryczne". NIE zawsze

    zegar->flag_wsk_godz_bitmapowa =
            zegar->cyferblat[ zegar->nr_tarczy].flag_fabryczna_wsk_godz_bitmapowa ?
                true : false ;

    zegar->flag_wsk_min_bitmapowa =
            zegar->cyferblat[ zegar->nr_tarczy].flag_fabryczna_wsk_min_bitmapowa ?
                true : false ;
    zegar->flag_wsk_sek_bitmapowa =
            zegar->cyferblat[ zegar->nr_tarczy].flag_fabryczna_wsk_sek_bitmapowa ?
                true : false ;


    zegar->nr_wybranej_bitmapy_wsk_godzinowej = 0;
    zegar->nr_wybranej_bitmapy_wsk_minutowej = 0;
    zegar->nr_wybranej_bitmapy_wsk_sekundowej = 0;

    zegar->zmiana_wygladu_cyferblatu(zegar->nr_tarczy, tryb_wskazowek::fabryczne);

    string nazwa_cyferbatu = zegar->cyferblat[ zegar->nr_tarczy].nazwa;

    info_o_faworytach(nazwa_cyferbatu);
    laduj_tablice_cyferblatow();
}
//***********************************************************************************************************
void Topcje_dlg::on_pushButton_shutdown_clicked()
{
    zegar->make_shutdown();
}
//***********************************************************************************************************
void Topcje_dlg::on_pushButton_sciezka_clicked()
{
    zegar->id_linux_czy_windows_version();
}
//***********************************************************************************************************

void Topcje_dlg::on_checkBox_uruchom_przy_logowaniu_stateChanged(int arg1 )
{
#ifdef Q_OS_WINDOWS

    int wersja = zegar->id_linux_czy_windows_version();
    cout << "System to  Windows " << wersja

            << ". Uruchamianie z loginem na razie nie dziala, nie wiem jak to zrobic" << endl;

    if(arg1 == Qt::Unchecked)
    {
        flag_uruchom_z_loginem = false;
    }
    else if ( arg1 == Qt::Checked){
        flag_uruchom_z_loginem = true;
    }


#elif defined      Q_OS_LINUX
    if(arg1 == Qt::Unchecked)
    {
        flag_uruchom_z_loginem = false;
    }
    else if ( arg1 == Qt::Checked){
        flag_uruchom_z_loginem = true;

        /*-------------
         * Zrobić plik ~/.config/zegar.desktop a w nim:
        [Desktop Entry]
        Exec=zro/zegar/zegar

    -------------*/
    }
    }

    // wlasciwe wykonanie tej komendy w funkcji Accept

#endif
}
