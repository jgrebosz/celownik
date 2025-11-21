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

QStringList labels {QObject::tr("Enable"), QObject::tr("Name"), QObject::tr("Time"),  QObject::tr("Frequency"), QObject::tr("Data"), QObject::tr("id")};

std::vector<QString> tyg {
    QObject::tr("Mon"),
            QObject::tr("Tue"),
            QObject::tr("Wed"),
            QObject::tr("Thu"),
            QObject::tr("Fri"),
            QObject::tr("Sat"),
            QObject::tr("Sun")
};

QString minutes_txt { QObject::tr("%1, ") },
error_opening_txt {  QObject::tr("Error whilie opening file with user-defined alarms  ")};

QString mess_usuwanie { QObject::tr("You are going to remove the alarm =  nr %1\n\n"
                       " called  : %2\n"
                       " hour: %3\n"
                       " date: %4\n"
                       " polygon   : %5\n\n"
                       "Are you sure ?") };

QString are_you_sure { QObject::tr("ARE YOU SURE ? ") } ;

QString yes_txt { QObject::tr("Yes") } ;
QString no_txt { QObject::tr("No") } ;
QString cancel_txt { QObject::tr("Cancel") } ;

QString no_entry_txt {  QObject::tr("No alarm entry selected") } ;
QString af_first_select_txt { QObject::tr( "At first select some alarm by clicking on its name ") } ;


// plik "tjeden_alarm.cpp" -----------------------------------------
//     nic


// plik Tedytor_alarmow.cpp ---------------------------------------

QStringList list_czestotliwosc =
{ QObject::tr("Once"),            // "Raz",
   QObject::tr("Every day"),            //"Codziennie",
   QObject::tr("Every week"),            //"Co tydzień",
   QObject::tr("Every month"),            //"Co miesiąc",
   QObject::tr("Every year"),            //"Co rok",
   QObject::tr("Minutes"),            //"Minuty"
};

QString detailed_description_txt  {  QObject::tr("user text...") } ;

QString  date_txt( QObject::tr("Date:")) ;      // data (kalendarzowa)
QString  start_on_txt( QObject::tr("Start on:")) ;      // "począwszy od "data (kalendarzowa)"

QString  choose_music_txt( QObject::tr("Choose a file containing the sound/music"));
QString  choose_program_txt( QObject::tr( "Choose a program to be exectued during alarm"));



