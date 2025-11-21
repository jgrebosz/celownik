#include "alarmy.h"
#include "ui_alarmy.h"

#include <QMessageBox>
#include <QTranslator>
#include "tjeden_alarm.h"
#include "tedytor_alarmow_dlg.h"

#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <bitset>
using namespace  std;
#include "teksty.h"
//****************************************************************************************
Alarmy::Alarmy(QWidget *parent, string fname) :
    QDialog(parent),
    ui(new Ui::Alarmy),
    nazwa_pliku_z_alarmami(fname)
{
    setModal(true);   // chyba nie działa



    ui->setupUi(this);

    // QStringList labels {tr("Enable"), tr("Name"), tr("Time"),  tr("Frequency"), tr("Data"), tr("id")};

    ui->tableWidget->setColumnCount (labels.size());
    ui->tableWidget->setHorizontalHeaderLabels ( labels );
    ui->tableWidget->resizeColumnsToContents ();
    ui->tableWidget->verticalHeader()->setStyleSheet( "background-color: rgb(255, 230, 230);");
    int   nr_rows = (int) tablica_alarmow.size();
    ui->tableWidget->setRowCount (nr_rows);

    redisplay_table();

    //    cout << "CZy modalny = " << isModal() << endl;
}
//****************************************************************************************
Alarmy::~Alarmy()
{
    delete ui;
}
//****************************************************************************************
//void  Alarmy::on_buttonBox_accepted()
//{
//    cout << "wcisniete OK" << endl;
//}
//****************************************************************************************
//***************************************************************************************************************
void Alarmy::redisplay_table()
{
    auto nr_rows = (int) tablica_alarmow.size();
    ui->tableWidget->setRowCount (nr_rows);

    // creating row widgets ---------------------------------------
    int nr_columns = ui->tableWidget->columnCount ();
    for(long r = 0 ; r < nr_rows ; r++)
        for(long c = 0 ; c < nr_columns ; c++)
        {
            ui->tableWidget->setItem(r,c, new QTableWidgetItem("") );
            ui->tableWidget->item(r, c)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        }

    //    QStringList list  ;
    //    list << "consider this line as: true" << "consider this line as: false" ;

    for (int j = 0; j < nr_rows; ++j )
    {
        ui->tableWidget->setItem(j, 0, new QTableWidgetItem( ""));
        //                                     tablica_alarmow[j].flag_enable?
        //                                                                  "":
        //                                                                  "") );

        ui->tableWidget->item(j, 0)->setCheckState(tablica_alarmow[j].flag_enable?
                                                       Qt::Checked : Qt::Unchecked);


        ui->tableWidget->item(j, 1) -> setText(tablica_alarmow[j].nazwa.c_str() );

        //        cout << "do tablicy alarmow wlozona wartosc godziny  = " <<
        //                tablica_alarmow[j].godzina.c_str() << endl;

        ui->tableWidget->item(j, 2) -> setText(tablica_alarmow[j].godzina.c_str()) ;
        ui->tableWidget->item(j, 3) -> setText(
                    list_czestotliwosc[ tablica_alarmow[j].czestotliwosc]
                ) ;

        // kolumna 4 to data, ale czasem dni tygodnia
        int co = tablica_alarmow[j].czestotliwosc;
        switch(co)
        {   // raz, codziennie, co tydzien, co miesiac, co rok, minuty
        case 0: // Raz
        case 3: // co miesiac
        case 4:  // co rok

            ui->tableWidget->item(j, 4) -> setText(tablica_alarmow[j].data.c_str())  ;
            break;
        case 1:   // codziennie
            break; // nic nie wypelniamy
        case 2: // co tydzien
        {
//                        vector<QString> tyg {
//                            tr("Mon"),
//                                    tr("Tue"),
//                                    tr("Wed"),
//                                    tr("Thu"),
//                                    tr("Fri"),
//                                    tr("Sat"),
//                                    tr("Sun")
//                        };


            QString rezultat;
            bitset<8>  dni = tablica_alarmow[j].dni_tygodnia;
            for(int i = 0 ; i < 7 ;++i){
                if(dni[i]) {
                    rezultat +=   tr("%1, ").arg(tyg[i]);
                }
            }
            ui->tableWidget->item(j, 4) -> setText(rezultat) ;
            //            cout << "wypis dla co tydzien. tabl.dni =  " <<
            //                 tablica_alarmow[j].dni_tygodnia
            //                 << ", a rezultat = "
            //                 << rezultat << endl;
            break;
        }
        case 5: // minuty

            ui->tableWidget->item(j, 4) -> setText(
                                                       minutes_txt.arg
                                                               (    tablica_alarmow[j].interwal)
                        ) ;
                                                   break;
        } // switch

        //-----------
        char buff[250] = {  };
        int liczba = tablica_alarmow[j].nr_entry;
        sprintf(buff, "%d", liczba);
        ui->tableWidget->item(j, 5) -> setText( buff )   ;
    }

    ui->tableWidget->resizeColumnsToContents();
}
//***************************************************************************************************************
//***************************************************************************************************************
void Alarmy::table_value_changed()   // działa po dodaniu czegoś albo modyfikacj.
{
//    cout << " f. table_value_changed() " << endl;
    int  row = ui->tableWidget->currentRow() ;
    if((unsigned long) row >= tablica_alarmow.size() ) return ;

    unsigned col = ui->tableWidget->currentColumn() ;
    if(col == 0 )
    {
        ui->tableWidget->resizeColumnToContents(0) ;
    }
    redisplay_table();
    zapisanie_tablicy_na_dysku();
}
//***************************************************************************************************************
//***************************************************************************************************************


//***************************************************************************************************************
void Alarmy::zapisanie_tablicy_na_dysku()
{
    ofstream plik(nazwa_pliku_z_alarmami);
    if(!plik)
    {
        cerr << error_opening_txt.toStdString() << nazwa_pliku_z_alarmami << endl;
        return;
    }
    for(auto x : tablica_alarmow)
    {
        plik << x;
    }

}
//***************************************************************************************************************


//***************************************************************************************************************
//			QMessageBox::warning ( this,
//								   "No spectrum selected",
//								   QString ( "You can not disable this page, because now its table has already some contents" ),
//								   QMessageBox::Ok,
//								   QMessageBox::NoButton,
//								   QMessageBox::NoButton );

//***************************************************************************************************************
void Alarmy::on_pushButton_usun_rzad_clicked()
{
//    cout << "Operacja usuwania rzedu nr" << endl;


    QList<QTableWidgetSelectionRange> range = ui->tableWidget->selectedRanges() ;

   // cout << "Selection has " << range.count() << " ranges" << endl;

    for(int s = range.count()-1; s >=0 ; s--)    // can be many separated ranges on this list
    {
        cout << "range[s].bottomRow()= " << range[s].bottomRow() << ", range[s].topRow()= " <<  range[s].topRow() << endl;
        for(unsigned int row = range[s].bottomRow() ; row >= (unsigned int) range[s].topRow() ; row--)
        {
            //  unsigned row = ui->tableWidget->selectRow();    ro sel currentRow() ;
            if(row >= tablica_alarmow.size() ) return ;

            QString mess_usuwanie = tr(
                                        "You are going to remove the alarm = nr %1\n\n"
                                        "called  : %2\n"
                                        "hour: %3\n"
                                        "date: %4\n"
                                        "polygon : %5\n\n"
                                        "Are you sure?")
                                        .arg(
                                            QString::number(row + 1),
                                            QString::fromStdString(tablica_alarmow[row].nazwa),
                                            QString::fromStdString(tablica_alarmow[row].godzina),
                                            QString::fromStdString(tablica_alarmow[row].data),
                                            QString::number(tablica_alarmow[row].czestotliwosc)
                                            );


            QMessageBox::StandardButton reply = QMessageBox::warning(
                this,
                tr("ARE YOU SURE?"),
                mess_usuwanie,
                QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
                QMessageBox::No // domyślnie zaznaczony
                );

            switch (reply) {
            case QMessageBox::Yes:
                tablica_alarmow.erase(tablica_alarmow.begin() + row);
                redisplay_table();
                break;

            case QMessageBox::No:
                // użytkownik wybrał "No"
                break;

            case QMessageBox::Cancel:
            default:
                // anulowanie lub zamknięcie okna
                break;
            }
        } // for row
    } // for sel
    zapisanie_tablicy_na_dysku();
}
//***************************************************************************************************************
void Alarmy::on_tableWidget_cellClicked(int row, int column)
{
    //    cout << "------------ F.  on_tableWidget_cellClicked row=" << row
    //         << ", column= " << column << endl;
    if(column == 0)
    {
        bool stan = ui->tableWidget->item(row, column)->checkState();
        // change the text in
        // ui->tableWidget->item(row, column)->setText( "");
        tablica_alarmow[row].flag_enable = stan;
        zapisanie_tablicy_na_dysku();
        ui->tableWidget->resizeColumnsToContents();
    }
    else {
        auto ile = ui->tableWidget->columnCount();
        //        cout << "select inne kolumy - to select till column " << ile  << endl;
        ui->tableWidget->setRangeSelected(
                    QTableWidgetSelectionRange(row, 1, row,
                                               ile-1
                                               ),
                    true );

    }
}
//***************************************************************************************************************

void Alarmy::on_pushButton_nowy_clicked()
{
    Tedytor_alarmow_dlg  * dlg = new Tedytor_alarmow_dlg();

    Tjeden_alarm nowy;
    nowy.nr_entry = -1;
    dlg->ustaw_alarm_do_edycji(nowy);
    dlg->exec();
    auto zmieniony = dlg->daj_rezultat();
    delete dlg;

    zmieniony.nr_entry = (int) tablica_alarmow.size();
    tablica_alarmow.push_back(zmieniony);
    redisplay_table();

    // zapis na dysk?
    zapisanie_tablicy_na_dysku();

}
//***************************************************************************************************************
void Alarmy::on_pushButton_edytuj_clicked()
{
    Tedytor_alarmow_dlg  * dlg = new Tedytor_alarmow_dlg();

    // sprawdzenie ktory wiersz w tabliycy jest selected

    // odczytanie, ktory ma nr_entry, bo nie musi być ten sam co nr wiersza,
    //    jako ze można sortować wiersze tabeli wg roznych kolumn
    // który to alarm (wiersz lub entry)
    auto range_list = ui->tableWidget->selectedRanges();
    //    cout << "range list size = ", range_list.size();
    if(range_list.empty())     // jesli zaden, to error message
    {
        QMessageBox::warning
            ( this,
             tr("No alarm entry selected"),
             tr( "At first select some alarm by clicking on its name "),
             QMessageBox::Ok);
        delete dlg;
        return;
    }
    int wiersz = range_list[0].topRow();
    auto txt = ui->tableWidget->item(wiersz, 5)->text().toStdString();

    //cout << "znaleziony id =" << txt << endl;

    int nr_entry = atoi(txt.c_str());
    dlg->ustaw_alarm_do_edycji(tablica_alarmow[nr_entry]);
    dlg->exec();
    auto zmodyfikowany = dlg->daj_rezultat();
    delete dlg;

    //    entry.nr_entry = tablica_alarmow.size();
    tablica_alarmow[nr_entry] = zmodyfikowany;
    redisplay_table();
    // zapis na dysk?
    zapisanie_tablicy_na_dysku();
}
//***************************************************************************************************************
void Alarmy::on_pushButton_OK_clicked()
{
    // cout << "Alarmy:: on_pushButton_OK_clicked()"  << endl;
    close();
}
//***************************************************************************************************************
void Alarmy::on_tableWidget_cellDoubleClicked(int row, int column)
{
    on_tableWidget_cellClicked(row, column);
    on_pushButton_edytuj_clicked();

}
//***************************************************************************************************************

void Alarmy::on_pushButton_test_clicked()
{
   //  cout << "Test alarmu " << endl;
    QList<QTableWidgetSelectionRange> range = ui->tableWidget->selectedRanges() ;
    if( range.count() ) {
        int nr =  range[0].bottomRow();

        // wywołanie funkcji alarmowej
        //     int nr = range.topRow();

        tablica_alarmow[nr].flag_test_alarm_now = true;
    }
}
//***************************************************************************************************************
//**********************************************************************************************************
void Alarmy::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);  // odświeża wszystkie teksty z .ui

        updateTexts();            // jeśli masz teksty ustawiane ręcznie
    }

    QDialog::changeEvent(event);  // WAŻNE!
}
//**********************************************************************************************************
void Alarmy::updateTexts()
{
    // ui->labelStatus->setText(tr("Status"));
    // ui->pushButtonStart->setText(tr("Start"));
    // ui->comboBox_powtarzanie->clear();
    //  ui->comboBox_powtarzanie->addItems(Tjeden_alarm::list_czestotliwosc());
}
//**********************************************************************************************************
