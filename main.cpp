#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QDir>

bool flag_ma_byc_restart;

extern bool flag_na_wierzchu;
extern bool wstepne_flag_na_wierzchu;
extern int nr_jezyka;
string sciezka;

QTranslator translator;
//************************************************************************************
void main(int argc, char *argv[])
{

    QDir dir;
    sciezka = QDir::currentPath().toStdString() + "/";

    // if(czy_wolno_uruchomic(argv[0]) == false) return 0;

    // At first let`s select the language
    pathed_nazwa_pliku_z_opcjami = sciezka + nazwa_pliku_z_opcjami;
    ifstream file(pathed_nazwa_pliku_z_opcjami);
    if(!file)
    {
        qDebug() << "Error while opening file " << pathed_nazwa_pliku_z_opcjami ;
    }
    file >> nr_jezyka ;
    file.close();
    // cout << "wczytany z pliku "
    //         << pathed_nazwa_pliku_z_opcjami
    //         << " nr jezyka = " << nr_jezyka << endl;


    // while(1)
    {
        QApplication a(argc, argv);

        switch(nr_jezyka)
        {
        case 0:
            qDebug() << "Język nr 0 (domyślny, angielski)";
            qApp->removeTranslator(&translator);
            break;

        default:
        case 1:  // polish
        {
            QString nazwa_jezyka = ":/polski2.qm";
             // QString nazwa_jezyka = "./polski2.qm";
            qDebug() << "Wybrany język: " << nazwa_jezyka;

            bool rezultat = translator.load(nazwa_jezyka);
            // qDebug() << "Wynik wczytania: " << rezultat;
            // qDebug() << "Working direcotry =  " ;
            // system("dir *.qm");

            if (rezultat)
                a.installTranslator(&translator);
            break;
        }
        }
        // nr_obecnie_zainstalowanego_jezyka = nr_jezyka;


        MainWindow w;
        w.show();
        // int answer =
            a.exec();


//         if(flag_ma_byc_restart )
//         {
// //                    cout << "111 Powtorne pokazanie okna, z opcją flag_na_wierzchu = "
// //                         << flag_na_wierzchu << " i jezykiem nr " << nr_jezyka << endl;

//             w.setWindowFlag(Qt::WindowStaysOnTopHint, flag_na_wierzchu );
//             w.setWindowFlag(Qt::X11BypassWindowManagerHint, flag_na_wierzchu);
//             flag_ma_byc_restart = false;
//             // kontynuuj nieskonczona petle?
//         }
//         else
//         {
//             return answer;
//         }
    }
}
//***************************************************************************************************************
