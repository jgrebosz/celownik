#ifndef TJEDEN_ALARM_H
#define TJEDEN_ALARM_H

#include <cstdint>
#include <string>
#include <fstream>
#include <iostream>

#include <QStringList>
#include <QObject>
#include <QMediaPlayer>
#include <QAudioOutput>

using namespace  std;

class Togloszenie_alarmu;
class QMediaPlayer;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Tjeden_alarm
{
    friend class Alarmy;

public:
    int nr_entry;
    bool flag_enable = true;
    std::string nazwa = "Alarm";
    std::string godzina;
    int czestotliwosc ;
    std::string data;
    int8_t dni_tygodnia;
    int interwal = 15;
    bool flag_okno = true;
    std::string dodatkowy_opis;
    bool flag_dzwiek = false;
    std::string muzyka;
    int sekund_dzwieku = 0;
    bool loop = false;
    bool flag_program = false;
    std::string  nazwa_programu;
    bool  flag_wylacz_komputer = false;

    bool flag_alarm_jest_na_ekranie = false;
    Togloszenie_alarmu *dlg = nullptr;
    QMediaPlayer *player = nullptr;
    QAudioOutput *audioOutput;
    bool flag_test_alarm_now = false;


    friend
    std::ofstream & operator << (std::ofstream &plik, Tjeden_alarm & a)
    {
      //  std::string sep {"|||"};
#define xm(n)  "\t<" << #n << ">" << a.n << "</" << #n << ">\n"
        plik
                << "<alarm>\n"
                << xm(flag_enable)
                << xm(nazwa)
                   << xm(godzina)
                   << xm(czestotliwosc)
                   << xm(data)                      
                   <<  "\t<dni_tygodnia>" <<   (int) (a.dni_tygodnia) <<  "</dni_tygodnia>\n"
                   << xm(interwal)
                   << xm(flag_okno)
                   << xm(dodatkowy_opis)
                   << xm(flag_dzwiek)
                   << xm(muzyka)
                   << xm(sekund_dzwieku)
                   << xm(loop)
                   << xm(flag_program)
                   << xm(nazwa_programu)
                   << xm(flag_wylacz_komputer)
               << "</alarm>\n"
                << endl;

        // cout << "Zapisany "   << (int) (a.dni_tygodnia) << endl;
        return  plik;
    }


//    friend
//    std::ifstream & operator >> (std::ifstream &plik, Tjeden_alarm & a)
//    {
//        std::string sep {"|||"};

//        // wczytanie calego jednego alarmu

////        getline(plik, a.flag_enable, sep);

////       plik << a.nazwa << sep
////                   << a.godzina << sep
////                   << a.czestotliwosc << sep
////                   << a.data << sep
////                   << a.dni_tygodnia << sep
////                   << a.interwal << sep
////                   << a.flag_okno << sep
////                   << a.dodatkowy_opis << sep
////                   << a.flag_dzwiek << sep
////                   << a.muzyka << sep
////                   << a.sekund_dzwieku << sep
////                   << a.loop << sep
////                   << a.flag_program << sep
////                   << a.nazwa_programu << sep
////                   << a.flag_wylacz_komputer << sep
////                   << endl;
//        return  plik;
//    }

public:
    Tjeden_alarm();
    //-----------------------------------
    Tjeden_alarm(
            int nr,
            bool flag_enable_,
            std::string nazwa_,
            std::string godzina_,
            int czestotliwosc_,
            std::string data_,
           int8_t dni_tygodnia_,
            int minuty_,
            bool flag_okno_,
            std::string dodatkowy_opis_,
            bool flag_dzwiek_,
            std::string muzyka_,
            int sekund_dzwieku_,
            bool loop_,
            bool flag_program_,
            std::string nazwa_programu_,
            bool flag_wylacz_komputer_);


};

#endif // TJEDEN_ALARM_H
