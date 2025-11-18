#include "tjeden_alarm.h"

#include <QMediaPlayer>

Tjeden_alarm::Tjeden_alarm()
{
	flag_enable = true;
	nazwa = "Alarm";
	godzina = "00:00:00";
	czestotliwosc = 1;
    data = "12-05-2020";
            interwal = 0;
    flag_okno = true;
	dodatkowy_opis = "...";

}
//**********************************************************************************************************************************
Tjeden_alarm::Tjeden_alarm(
        int nr_entry_,
        bool flag_enable_,
                           std::string nazwa_,
                           std::string godzina_,
                           int czestotliwosc_,
                           std::string data_,
                          int8_t dni_tygodnia_,
                           int interwal_,
                           bool flag_okno_,
                           std::string dodatkowy_opis_,
                           bool flag_dzwiek_,
                           std::string muzyka_,
                           int sekund_dzwieku_,
                           bool loop_,
                             bool flag_program_,
                           std::string nazwa_programu_,
                           bool flag_wylacz_komputer_)
{
    nr_entry = nr_entry_;
    flag_enable = flag_enable_;
    nazwa = nazwa_;
    godzina = godzina_;
    czestotliwosc = czestotliwosc_;
    data = data_;
    dni_tygodnia = dni_tygodnia_;
    interwal = interwal_;
    flag_okno = flag_okno_;
	dodatkowy_opis = dodatkowy_opis_;

    flag_dzwiek = flag_dzwiek_;
    muzyka = muzyka_;
    sekund_dzwieku = sekund_dzwieku_;
    loop = loop_;
    flag_program =   flag_program_;
    nazwa_programu = nazwa_programu_;
    flag_wylacz_komputer = flag_wylacz_komputer_;
    flag_alarm_jest_na_ekranie = false;
}

