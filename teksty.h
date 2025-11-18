#ifndef TEKSTY_H
#define TEKSTY_H

#include <QString>
#include <QObject>
#include <QTranslator>


extern QString alarm_clock;
extern QString
options_txt,
alarms_txt,
exit_txt,
default_hours_hand_txt,
default_minutes_hand_txt,
default_seconds_hand_txt;;


extern QStringList labels;

extern std::vector<QString> tyg;

extern QString minutes_txt, error_opening_txt;
extern QString mess_usuwanie;

extern QString are_you_sure,
yes_txt,
no_txt,
cancel_txt,
no_entry_txt ,
af_first_select_txt;

extern QStringList list_czestotliwosc;

extern QString detailed_description_txt,
date_txt, start_on_txt, choose_music_txt, choose_music_txt, choose_program_txt ;


#endif // TEKSTY_H
