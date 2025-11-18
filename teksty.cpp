#include "teksty.h"

#include <vector>




// plik mainwindow.cpp
QString alarm_clock  = QObject::tr("Alarm Clock") ; // nie tłumaczymy
QString options_txt  { QObject::tr("Options...")};
QString alarms_txt  { QObject::tr("Alarms...")};
QString exit_txt  { QObject::tr("Exit program")};
QString default_hours_hand_txt  { QObject::tr("factory hours hand  (for a particular clock face)")} ;
QString default_minutes_hand_txt { QObject::tr("factory minutes hand  (for a particular clock face)") };
QString default_seconds_hand_txt { QObject::tr("factory seconds hand  (for a particular clock face)") };

// plik alarmy.cpp

#define tr  QObject::tr

QStringList labels {tr("Enable"), tr("Name"), tr("Time"),  tr("Frequency"), tr("Data"), tr("id")};

std::vector<QString> tyg {
    QObject::tr("Mon"),
            QObject::tr("Tue"),
            QObject::tr("Wed"),
            QObject::tr("Thu"),
            QObject::tr("Fri"),
            QObject::tr("Sat"),
            QObject::tr("Sun")
};

QString minutes_txt { tr("%1, ") },
error_opening_txt {  tr("Error whilie opening file with user-defined alarms  ")};

QString mess_usuwanie { tr("You are going to remove the alarm =  nr %1\n\n"
                       " called  : %2\n"
                       " hour: %3\n"
                       " date: %4\n"
                       " polygon   : %5\n\n"
                       "Are you sure ?") };

QString are_you_sure { tr("ARE YOU SURE ? ") } ;

QString yes_txt { tr("Yes") } ;
QString no_txt { tr("No") } ;
QString cancel_txt { tr("Cancel") } ;

QString no_entry_txt {  tr("No alarm entry selected") } ;
QString af_first_select_txt { tr( "At first select some alarm by clicking on its name ") } ;


// plik "tjeden_alarm.cpp" -----------------------------------------
//     nic


// plik Tedytor_alarmow.cpp ---------------------------------------

QStringList list_czestotliwosc =
{ tr("Once"),            // "Raz",
   tr("Every day"),            //"Codziennie",
   tr("Every week"),            //"Co tydzień",
   tr("Every month"),            //"Co miesiąc",
   tr("Every year"),            //"Co rok",
   tr("Minutes"),            //"Minuty"
};

QString detailed_description_txt  {  tr("user text...") } ;

QString  date_txt( tr("Date:")) ;      // data (kalendarzowa)
QString  start_on_txt( tr("Start on:")) ;      // "począwszy od "data (kalendarzowa)"

QString  choose_music_txt( tr("Choose a file containing the sound/music"));
QString  choose_program_txt( tr( "Choose a program to be exectued during alarm"));



