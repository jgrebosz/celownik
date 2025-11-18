#include "tedytor_alarmow_dlg.h"
#include "ui_tedytor_alarmow_dlg.h"

#include <QFileDialog>

#include <iostream>
#include <bitset>
using namespace std;

#include "tjeden_alarm.h"

#include "teksty.h"

// QStringList list_czestotliwosc = { "Raz", "Codziennie", "Co tydzień", "Co miesiąc", "Co rok", "Minuty"};
//**********************************************************************************************************************************
Tedytor_alarmow_dlg::Tedytor_alarmow_dlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Tedytor_alarmow_dlg)
{
    ui->setupUi(this);
    ui->comboBox_powtarzanie->insertItems(0, list_czestotliwosc);
}
//*******************************************************************************************
Tedytor_alarmow_dlg::~Tedytor_alarmow_dlg()
{
    delete ui;
}
//*******************************************************************************************
void Tedytor_alarmow_dlg::rozlozenie_po_kontrolkach()
{
    ui->lineEdit_nazwa->setText(al.nazwa.c_str());
    //    ui->comboBox_powtarzanie->setCurrentIndex(al.czestotliwosc);
    //             cout << "SPR czestotliwosci  = " << ui->comboBox_powtarzanie->currentIndex()
    //                  << ", zrobiona z "
    //                  << al.czestotliwosc
    //                  << endl;

    // gdy jest nowy, to takie dane wpisujemy
    if(al.nr_entry == -1) {
        cout << "nowy alarm, wiec biezaca data i czas " << endl;

        QDateTime dateTime;
        dateTime = QDateTime::currentDateTime();
        ui->dateEdit_daty->setDate(dateTime.date());
        ui->timeEdit_godzina->setTime(dateTime.time());
        ui->comboBox_powtarzanie->setCurrentIndex(0);

        ui->spinBox_interwal->setValue(15);


        // ui->frame_dni_tygodnia->hide();// pojawi się na życzenie
        ui->checkBox_pokaz_okno->setCheckState(Qt::Checked);
        ui->plainTextEdit_dodatkowy_opis->setPlainText(detailed_description_txt);
        ui->checkBox_odtworz_dzwiek->setCheckState(Qt::Unchecked);
        ui->lineEdit_plik_muz->setText("");
        ui->spinBox__sekund_dzwieku->setValue(0);
        ui->checkBox_loop_muzyki->setCheckState(Qt::Unchecked);
        ui->checkBox_flag_uruchom_program->setCheckState(Qt::Unchecked);
        ui->lineEdit_nazwa_programu->setText("");
        ui->checkBox_wylacz_komputer->setCheckState(Qt::Unchecked);
    }
    else   // czyli to jest alarm już wczesniej zdefiniowany
    {
        ui->dateEdit_daty->setDate(QDate::fromString( al.data.c_str(), "yyyy-MM-dd"));

       QTime czas2 = QTime::fromString(al.godzina.c_str(), "hh:mm");
       ui->timeEdit_godzina->setTime(czas2);

        ui->comboBox_powtarzanie->setCurrentIndex(al.czestotliwosc);

        // ui->frame_dni_tygodnia->hide();// pojawi się na życzenie
        if(al.dni_tygodnia & 1)  ui->checkBox_pn->setCheckState(Qt::Checked);
        if(al.dni_tygodnia & 2)  ui->checkBox_wt->setCheckState(Qt::Checked);
        if(al.dni_tygodnia & 4)  ui->checkBox_sr->setCheckState(Qt::Checked);
        if(al.dni_tygodnia & 8)  ui->checkBox_cz->setCheckState(Qt::Checked);
        if(al.dni_tygodnia & 16)  ui->checkBox_pt->setCheckState(Qt::Checked);
        if(al.dni_tygodnia & 32)  ui->checkBox_so->setCheckState(Qt::Checked);
        if(al.dni_tygodnia & 64)  ui->checkBox_nd->setCheckState(Qt::Checked);


        ui->checkBox_pokaz_okno->setCheckState(al.flag_okno == true ? Qt::Checked : Qt::Unchecked );
        ui->plainTextEdit_dodatkowy_opis->setPlainText(al.dodatkowy_opis.c_str() );
        ui->checkBox_odtworz_dzwiek->setCheckState(al.flag_dzwiek == true ? Qt::Checked : Qt::Unchecked );
        ui->lineEdit_plik_muz->setText(al.muzyka.c_str());
        ui->spinBox__sekund_dzwieku->setValue(al.sekund_dzwieku);
        ui->checkBox_loop_muzyki->setCheckState(al.loop== true ? Qt::Checked : Qt::Unchecked );
        ui->checkBox_flag_uruchom_program->setCheckState(al.flag_program== true ? Qt::Checked : Qt::Unchecked );
        ui->lineEdit_nazwa_programu->setText(al.nazwa_programu.c_str());
        ui->checkBox_wylacz_komputer->setCheckState(al.flag_wylacz_komputer== true ? Qt::Checked : Qt::Unchecked );
    }
}
//*******************************************************************************************
void Tedytor_alarmow_dlg::ustaw_alarm_do_edycji(const Tjeden_alarm &alarm)
{
    al = alarm;
//    cout << "ma byc edytowany alar z czestotliwoscia = " << al.czestotliwosc
//         << " o godzinie " << al.godzina
//         << endl;
    rozlozenie_po_kontrolkach();
}
//*******************************************************************************************
void Tedytor_alarmow_dlg::mod_raz_miesiac_rok()
{
    mod_zgas_wszystkie();
    // widoczna data i tool for calendar
    ui->label_data->setText(date_txt);
    ui->label_data->setVisible(true);
    ui->dateEdit_daty->setVisible(true);
    ui->toolButton_kalendarz->setVisible(true);
    adjustSize();
}
//*******************************************************************************************
void Tedytor_alarmow_dlg::mod_codziennie()
{
    // niewidoczne wszystko
    mod_zgas_wszystkie();
    adjustSize();
}
//*******************************************************************************************
void Tedytor_alarmow_dlg::mod_co_tydzien()
{
    mod_zgas_wszystkie();
    // widoczny tylko frame z dniami tygodnia
    ui->frame_dni_tygodnia->setVisible(true);
    ui->frame_dni_tygodnia->adjustSize();
    adjustSize();
}
//********************************************************************************************
void  Tedytor_alarmow_dlg::mod_minuty()
{
    mod_zgas_wszystkie();
    // widoczna data, tool kalendarza
    // oraz interwał w minutach
    ui->label_data->setText(start_on_txt);
    ui->label_data->setVisible(true);
    ui->dateEdit_daty->setVisible(true);
    ui->toolButton_kalendarz->setVisible(true);
    ui->label_interwal->setVisible(true);
    ui->spinBox_interwal->setVisible(true);
    adjustSize();
}
//********************************************************************************************
void Tedytor_alarmow_dlg::mod_zgas_wszystkie()
{
    ui->label_data->setText(date_txt);
    ui->label_data->setVisible(false);
    ui->dateEdit_daty->setVisible(false);

    ui->toolButton_kalendarz->setVisible(false);
    ui->frame_dni_tygodnia->setVisible(false);
    ui->label_interwal->setVisible(false);
    ui->spinBox_interwal->setVisible(false);
    ui->calendarWidget_kalendarz->setVisible(false);
    ui->pushButton_today->hide();
}
//******************************************************************************************************************
void Tedytor_alarmow_dlg::on_comboBox_powtarzanie_currentIndexChanged(int index)
{
    switch(index)
    {
    case 0: mod_raz_miesiac_rok(); break;
    case 1: mod_codziennie(); break;
    case 2: mod_co_tydzien(); break;
    case 3: case 4:
        mod_raz_miesiac_rok(); break;
    case 5:
        mod_minuty(); break;
    }
}
//******************************************************************************************************************
void Tedytor_alarmow_dlg::on_toolButton_kalendarz_triggered(QAction *  /*arg1*/)
{
    cout << __func__ << endl;
    //  ui->calendarWidget_kalendarz->setVisible(true);
}
//******************************************************************************************************************
void Tedytor_alarmow_dlg::on_toolButton_kalendarz_clicked()
{
    cout << __func__ << endl;
    ui->calendarWidget_kalendarz->setVisible(true);
    ui->pushButton_today->setVisible(true);
}
//******************************************************************************************************************
void Tedytor_alarmow_dlg::on_calendarWidget_kalendarz_clicked(const QDate &wybr_date)
{
    cout << __func__ << endl;
    ui->dateEdit_daty->setDate(wybr_date );
    ui->pushButton_today->setVisible(true);
}
//******************************************************************************************************************
void Tedytor_alarmow_dlg::on_pushButton_today_clicked()
{
    cout << __func__ << endl;
    QDateTime dateTime;
    dateTime = QDateTime::currentDateTime();
    ui->dateEdit_daty->setDate(dateTime.date());
    ui->calendarWidget_kalendarz->setSelectedDate(dateTime.date());
}
//******************************************************************************************************************
void Tedytor_alarmow_dlg::on_buttonBox_accepted()
{
    // zapisanie biezącego stanu okna do tablicy
    // inaczej dla sytuacji new a inaczej dla edit
    vector<QCheckBox *> dni_ptr
    { ui->checkBox_pn,  ui->checkBox_wt,  ui->checkBox_sr,  ui->checkBox_cz,
                ui->checkBox_pt, ui->checkBox_so, ui->checkBox_nd };

    // obliczenie wartosci dni tygodnia ------------------------------------------
    int8_t dni = 0;
    for(uint i = 0 ; i < dni_ptr.size() ; i++)
    {
        int yes = (dni_ptr[i]->checkState() == Qt::Checked) ? 1 : 0;
        // cout << "bit nr " << i << " ma watosc " << yes << endl;
        dni +=  (yes << i);
    }
    //    bitset<8> bbb (dni);
    //    cout << "wartosc dni = " << bbb << endl;

    al.nazwa = ui->lineEdit_nazwa->text().toStdString();
    al.czestotliwosc = ui->comboBox_powtarzanie->currentIndex();
    al.data = ui->dateEdit_daty->text().toStdString();
    al.dni_tygodnia = dni;
    al.godzina = ui->timeEdit_godzina->text().toStdString();
//    cout << "zapisana data  = " << al.data<< endl;

    al.flag_okno = ui->checkBox_pokaz_okno->checkState() == Qt::Checked? true : false;
    al.interwal = ui->spinBox_interwal->value();


    al.flag_okno = ui->checkBox_pokaz_okno-> checkState() ==  Qt::Checked? true : false;
    al.dodatkowy_opis =  ui->plainTextEdit_dodatkowy_opis->toPlainText().toStdString();
    al.flag_dzwiek = ui->checkBox_odtworz_dzwiek->checkState() ==  Qt::Checked? true : false;
    al.muzyka = ui->lineEdit_plik_muz->text().toStdString();
    al.sekund_dzwieku = ui->spinBox__sekund_dzwieku->value();
    al.loop = ui->checkBox_loop_muzyki->checkState() ==  Qt::Checked? true : false;
    al.flag_program = ui->checkBox_flag_uruchom_program->isChecked();
    al.nazwa_programu = ui->lineEdit_nazwa_programu->text().toStdString();
    al.flag_wylacz_komputer = ui->checkBox_wylacz_komputer->checkState() ==  Qt::Checked? true : false;

    // nie zapisujemy, bo inaczej dla sytuacji new a inaczej dla edit
}
//******************************************************************************************************************
void Tedytor_alarmow_dlg::on_toolButton_wybor_pliku_muz_clicked()
{
    //     cout << "Clicked wywolanie wyboru pliku" << endl;
    auto fname =   QFileDialog::getOpenFileName(this,
                                               choose_music_txt ,
                                                "./",                                // const QString &dir = QString(),  directory where to start
                                                "sounds *.mp3;;sounds *.wav"                 // filter
                                                );
    //   cout << "wywolanie wyboru pliku " <<fname.toStdString() << endl;
    ui->lineEdit_plik_muz->setText(fname);
}
//******************************************************************************************************************
void Tedytor_alarmow_dlg::on_toolButton_nazwy_programu_clicked()
{
    auto fname =   QFileDialog::getOpenFileName(this,
                                               choose_program_txt,
                                                "./",                                                  // directory where to start
                                                "all files *;;*.exe;;*.com;;*.bat"                       // filter
                                                );

    //   cout << "wywolanie wyboru pliku " <<fname.toStdString() << endl;
    ui->lineEdit_nazwa_programu->setText(fname);
}
//******************************************************************************************************************

