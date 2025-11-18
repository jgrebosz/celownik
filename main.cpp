#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>

bool flag_ma_byc_restart;

extern bool flag_na_wierzchu;
extern bool wstepne_flag_na_wierzchu;
extern int nr_jezyka;
extern int wstepne_nr_jezyka;
extern int nr_obecnie_zainstalowanego_jezyka;
//************************************************************************************
int main(int argc, char *argv[])
{

    nr_jezyka = 1;
    while(1) {
        QApplication a(argc, argv);

        QTranslator t;

        switch(nr_jezyka)
        {
        case 0:
            qDebug() << "Język nr 0 (domyślny, angielski)";
            break;

        default:
        case 1:  // polish
        {
            QString nazwa_jezyka = ":/polish.qm";
            qDebug() << "Wybrany język: " << nazwa_jezyka;

            bool rezultat = t.load(nazwa_jezyka);
            qDebug() << "Wynik wczytania: " << rezultat;

            if (rezultat)
                a.installTranslator(&t);
            break;
        }
        }
        nr_obecnie_zainstalowanego_jezyka = nr_jezyka;


        MainWindow w;
        w.show();
        int answer = a.exec();


        if(flag_ma_byc_restart )
        {
//                    cout << "111 Powtorne pokazanie okna, z opcją flag_na_wierzchu = "
//                         << flag_na_wierzchu << " i jezykiem nr " << nr_jezyka << endl;

            w.setWindowFlag(Qt::WindowStaysOnTopHint, flag_na_wierzchu );
            w.setWindowFlag(Qt::X11BypassWindowManagerHint, flag_na_wierzchu);
            flag_ma_byc_restart = false;
            // kontynuuj nieskonczona petle?
        }
        else
        {
            return answer;
        }
    }
}
