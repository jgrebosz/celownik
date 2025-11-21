#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "alarmy.h"
#include "topcje_dlg.h"

#include "togloszenie_alarmu.h"
#include "tprzezroczystosc.h"

#include <QMenu>
#include <QTimer>
#include <QTime>
#include <QTimeZone>
#include <QPainter>
#include <QPixmap>
#include <QBitmap>
#include <QGraphicsPixmapItem>
#include <QtMath>
//#include <QSound>
#include <QMediaPlayer>
#include <QPair>
#include <QDir>
#include <QOperatingSystemVersion>
#include <QProcess>
#include <QMessageBox>
#include <QInputDialog>

#include <QWindow>
#include <QCursor>

#include <iostream>
#include <vector>
#include <string>
// #include <bitset>
#include <QProcess>
#include <QTranslator>
#include "teksty.h"
#include "tpreferred_hands.h"


#define  KRZYZE_PROWADZACE     false //true //

#define COTO  ; // cout << "Linia = " << __LINE__ << endl;


bool flag_pokaz_to = false;

using namespace  std;

extern QStringList list_czestotliwosc;

extern bool flag_ma_byc_restart;
bool wstepne_flag_na_wierzchu;

 std::string nazwa_pliku_z_opcjami { "celownik_options.dat"};
//std::string nazwa_pliku_z_opcjami { "zegar.dat"};
std::string pathed_nazwa_pliku_z_opcjami ;

string nazwa_timezone;
QTimeZone time_zone = QTimeZone( QString("localTime").toLatin1());              // ( QTimeZone::systemTimeZone() );

extern vector <Tpreferred_hands>  vec_pref_hands;
//******************************************************************************************************************
std::vector <Tjeden_alarm> tablica_alarmow;
//std::vector <int>   vec2_playerow_grajaych_teraz;

struct Trojka {
    QMediaPlayer * wsk_playera;
    int sekund_utworu;
    bool flag_loop;
};
std::vector < Trojka >   vec_playerow_grajaych_teraz;
//-------------------
int przezroczystosc = 250;
int przezroczystosc_dla_kursora = 255;

bool flag_na_wierzchu;
bool flag_ograniczenie_pozycji;
bool flag_blokuj_na_ekranie = false;

bool flag_uruchom_z_loginem = false;
bool flag_uruchom_przy_logowaniu = true;
bool flag__kilka_kopii = false;
int nr_jezyka = 0;

double gskala = 1.0;
bool flag_gskala_active = true;

bool flag_show_AM_PM = true;
bool flag_show_dzien_tygodnia = true;
bool flag_show_date= true;
bool flag_show_sekundnik= true;
int poz_zegara_x, poz_zegara_y;

//***************************************************************************************
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent,
                Qt::WindowSystemMenuHint
                |  Qt::FramelessWindowHint

                ),
    ui(new Ui::MainWindow)
{   
    setAttribute(Qt::WA_TranslucentBackground);

    ui->setupUi(this);

    // cout << "Konstruktor Mainwindow "
    //         //         <<   tr( "example of language English ").toStdString()
    //      << endl;

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(1000);

    QTimer *timer_sprawdzania_alarmow = new QTimer(this);
    connect(timer_sprawdzania_alarmow, SIGNAL(timeout()),
            this, SLOT(sprawdzenie_alarmow()));
    timer_sprawdzania_alarmow->start(1000);

    setWindowTitle(alarm_clock);

    QDir dir;
    sciezka = QDir::currentPath().toStdString() + "/";
    // cout << "Current directory = " << sciezka  << endl;
    pathed_nazwa_pliku_z_opcjami = sciezka + nazwa_pliku_z_opcjami;
    // cout << __PRETTY_FUNCTION__ << " pathed_nazwa_pliku_z_opcjami = " << pathed_nazwa_pliku_z_opcjami << endl;
    pathed_nazwa_pliku_z_alarmami = sciezka + nazwa_pliku_z_alarmami ;
    pathed_nazwa_pliku_z_faworytami = sciezka + nazwa_pliku_z_faworytami;

    wstepne_wpisanie_cyferblatow();
    wstepne_wpisanie_wskazowek_godzinowych();
    wstepne_wpisanie_wskazowek_minutowych();

    for(int i = 1 ; i < 8 ; ++i){
        kolory_poligonow.push_back(QColor(250 * (i % 2),
                                          200 * (i/2 % 2),
                                          250 * (i/4 % 2) ));
    }


    wczytanie_opcji_z_dysku();
    wczytaj_poligony();

    odczytaj_ulubione_wskazowki_z_dysku();

    wstepne_flag_na_wierzchu = flag_na_wierzchu;
    time_zone = QTimeZone( QString("localTime").toLatin1());
    time_zone = QTimeZone( QString(nazwa_timezone.c_str() ).toLatin1());

    nr_tarczy = 0;

    zmiana_wygladu_cyferblatu(nr_tarczy,  tryb_wskazowek::ulubione );  // jesli sie nie udalo, to false

    wstepne_zaladowanie_tablicy_alarmow();


    //    string komenda = R"(C:\Program Files (x86)\FreeTime\FormatFactory\FormatFactory.exe)";
    //    //                system(al.nazwa_programu.c_str());
    //    komenda = kosmetyka_nazwy_programu(komenda);

    //    cout << "po kosmetyce = [" << komenda << "]" << endl;


    //    setMouseTracking(true);

    //        poligon.push_back(QPointF(10, 50));
    //        poligon.push_back(QPointF(100, 100));
    //        poligon.push_back(QPointF(10, 100));

}
//***************************************************************************************
MainWindow::~MainWindow()
{

    //    cout << "Destruktor Mainwindow " << endl;
    delete ui;
    delete rubberBand;

    zapis_opcji_na_dysku();
}

//***************************************************************************************
bool MainWindow::wybor_cyferblatu(string nazwa)
{
    for(uint i = 0 ; i < cyferblat.size() ; ++i)
    {
        if(cyferblat[i].nazwa == nazwa)
        {
            zmiana_wygladu_cyferblatu(i,  tryb_wskazowek::ulubione);
            return true;
        }
    }
    return false;
}
//***************************************************************************************
void MainWindow::przelicz_wskazowke(int nr_wybranej_wsk_any,
                                    vector <Tdane_wskazowki> & vect_any,
                                    QPixmap & anyowa,
                                    bool flag_czy_bitmapowo,

                                    // dotychczasowe dane o tej wskazowce
                                    double & cyf_os_x_any,
                                    double & cyf_os_y_any,
                                    double & cyf_dlugosc_any,
                                    double & cyf_szer_any,
                                    QColor & cyf_kolor_any,
                                    // rezultaty
                                    double  & any_mimosrod_x,
                                    double  & any_mimosrod_y,
                                    double & any_length_poly,
                                    double & any_szer_poly,
                                    double & any_wsp_korekcji_dlugosci,
                                    double & any_wsp_korekcji_szerokosci,

                                    // dla bitmapowej  wsk godzinowej
                                    double & any_skalowanie_x,
                                    double & any_skalowanie_y

                                    )
{


    double dlug_bitmapy ;
    double szer_bitmapy;
    double zadana_dlugosc;
    double zadana_szerokosc;

    //    cout << "Wskazowka bitmapowa ? " << boolalpha << flag_czy_bitmapowo << endl;

    if(flag_czy_bitmapowo)
    {
        bool flag_wolno_kolorowac = false;   // raczej false, bo czasm tarcze maja swoje defaultowe kolorowe bitmapy
        //            czy może zmienona została bitmapa wskazowki any
        if( nr_wybranej_wsk_any != 0)
        {
            // auto & c = cyf;
            auto & v = vect_any[nr_wybranej_wsk_any];
            anyowa.load(v.bitmapa.c_str());

            cyf_os_x_any = v.os_x;
            cyf_os_y_any = v.os_y;
            cyf_szer_any = v.skala_x;
            //  cyf_dlugosc_any = v.skala_y;

            flag_wolno_kolorowac = v.flag_bitmapa_moze_byc_kolorowana;
            if(flag_wolno_kolorowac == false){
                cyf_kolor_any = v.kolor;   // oryginalny kolor wskazowki
            }
            //            cout << "Dane z bazy wskazowek " << v.nazwa << ", os obrotu  y = " << cyf_os_y_any << endl;
        }else{
            //            cout << "Dane z cyferblatu,  os obrotu  y = " << cyf_os_y_any << endl;
        }



        dlug_bitmapy = anyowa.height();
        szer_bitmapy = anyowa.height();
        zadana_dlugosc = cyf_dlugosc_any;
        zadana_szerokosc = cyf_szer_any;

        //any_wsp_korekcji_dlugosci = 1.0 /((dlug_bitmapy / promien) / zadana_dlugosc);

        //        any_wsp_korekcji_dlugosci = (promien * zadana_dlugosc) /
        //                (dlug_bitmapy * (1-cyf_os_y_any));

        any_wsp_korekcji_dlugosci =
                (promien *  zadana_dlugosc* ( 1 + cyf_os_y_any) ) /  dlug_bitmapy ;


        any_wsp_korekcji_szerokosci = 1.0 /((szer_bitmapy / promien) / zadana_szerokosc);
        any_mimosrod_x =  cyf_os_x_any - anyowa.width()/2.0;




        //        if(cyf_os_y_any > 1){
        //            sek_inna_os_y = int(cyf_os_y_any);
        //            cyf_os_y_any = cyf_os_y_any -  sek_inna_os_y;
        //            sek_inna_os_y *= any_wsp_korekcji_dlugosci;
        //        } else  sek_inna_os_y = 0;



        // any_mimosrod_y = (cyf_os_y_any *C)   - dlug_bitmapy ; // + anyowa.height()/2;
        any_mimosrod_y = (cyf_os_y_any *dlug_bitmapy)
                -             (dlug_bitmapy);
        //*any_wsp_korekcji_dlugosci) ; // + anyowa.height()/2 ;


        // poszerzenie skali dla nabrania szerokosci wskazowki bitmapowej?
        any_skalowanie_x = 1.0 / (any_wsp_korekcji_szerokosci) * cyf_szer_any;
        any_skalowanie_y = any_wsp_korekcji_dlugosci ;

        // a może nie pozwalać na zmiane proporcji?
        any_skalowanie_x = any_skalowanie_y;




        // podkolorowanie bitmapy dostosowane życzenia  tarczy
        if(cyf_kolor_any != QColor(0,0,0,0) && flag_wolno_kolorowac)
        {
            QPixmap n = anyowa;
            n.fill(cyf_kolor_any);
            auto maska = anyowa.createMaskFromColor(Qt::transparent);
            n.setMask( maska ) ;
            anyowa = n;
            // cout << "kolorowanie wskazowki " << endl;

        }
    }
    else {    // wektorowa wskazowka ===========================================


        //        cout << "wektorowa  os obrotu  y = " << cyf_os_y_any << endl;

        dlug_bitmapy = any_length_poly;   // RRR  - to jest ok 2000
        szer_bitmapy = any_szer_poly;   // RRR



        zadana_dlugosc = cyf_dlugosc_any;
        zadana_szerokosc  = cyf_szer_any;

        //        any_wsp_korekcji_dlugosci = 1.0 /((dlug_bitmapy / promien) / zadana_dlugosc);

        any_wsp_korekcji_dlugosci =
                (promien *  zadana_dlugosc* ( 1 + cyf_os_y_any) ) /  dlug_bitmapy ;


        any_wsp_korekcji_szerokosci = 1.0 /((szer_bitmapy / promien) / zadana_szerokosc);

        //any_mimosrod_x =  (cyf_os_x_any  - (any_szer_poly/2.0));
        any_mimosrod_x =  (cyf_os_x_any * promien); //  - (any_szer_poly/2.0));


        // dawna, rysowana od lewego górnego rogu
        //        any_mimosrod_y = (cyf_os_y_any *dlug_bitmapy)   - any_length_poly  + any_szer_poly/2;

        //  any_mimosrod_y =  (cyf_os_y_any *dlug_bitmapy)   ;// + any_szer_poly/2;


        any_mimosrod_y = (cyf_os_y_any *dlug_bitmapy)
                // -     (dlug_bitmapy)
                ;

        any_skalowanie_x = any_wsp_korekcji_szerokosci;
        any_skalowanie_y = any_wsp_korekcji_dlugosci ;



    }

    if(flag_pokaz_to){
        cout  << " flag_czy_bitmapowo = " << flag_czy_bitmapowo
              << " promien_tarczy = " << promien
              << " dlug_bitmapy = "  << dlug_bitmapy
              << ", zadana dlugosc w skali promienia  "
              << zadana_dlugosc
                 //          << ", konieczne mnozenie przez any_skalowanie_x ="           << (any_skalowanie_x)
              << " any_skalowanie_y = " << any_skalowanie_y
              << ", any_mimosrod_y = " << any_mimosrod_y

              << endl;
    }

}
//***************************************************************************************
int MainWindow::znajdz_nr_wskazowki_o_bitmapie(string naz, vector<Tdane_wskazowki> &v)
{

       // cout << "Szukam wskazowki o takiej nazwie bitmapy [" << naz << "]" << endl;

    for(uint nr = 0 ; nr < v.size() ; ++nr)
    {
        //        cout << "Porownuje z " << v[nr].bitmapa << endl;;


        if( v[nr].bitmapa == naz)
        {
            //            cout << " - znaleziona pod indeksem " << nr << endl;
            return nr;
        }
    }
    cout << " - wskazowka " << naz << " NIE znaleziona w wektorze wskazowek "
         << " blad programisty"
         << endl;
    // exit(3);

    return 0;
    // return -1;

}
//***************************************************************************************
int MainWindow::znajdz_nr_wskazowki_o_nazwie(string naz, vector<Tdane_wskazowki> &v)
{

       // cout << "Szukam wskazowki o nazwie literackiej [" << naz << "]" <<  endl;
    for(uint nr = 0 ; nr < v.size() ; ++nr)
    {
        //        cout << "Porownuje z nazwa " << v[nr].nazwa << endl;

        if( v[nr].nazwa == naz)
        {
            //            cout << " - znaleziona pod indeksem " << nr << endl;
            return nr;
        }
    }
       // cout << __PRETTY_FUNCTION__ << " - NIE znaleziona  po nazwie "
       //      <<  " szukamy wiec po nazwach bitmapy " << endl;

    return     znajdz_nr_wskazowki_o_bitmapie (naz, v);

}
//************************************************************************************************************************
void MainWindow::wgraj_wskazowke(string nazwa,
                                 vector<Tdane_wskazowki> &vecwsk,
                                 bool * flag_bitvec, int *nr_wekt, int * nr_bitm   // tu beda odpowiedzi
                                 , QPixmap *bitmapa)
{


    // jeśli zadamy bitmapowej
    if(*flag_bitvec){
        // czy to naprawdę nazwa bitmapowej
        if(czy_nazwa_naprawde_bitmapowa(nazwa) )
        {

            *flag_bitvec = true;
            // Jeśli bitmapowa, to przepisujemy stamtąd nazwe ulubionej bitmapy
            *nr_bitm = znajdz_nr_wskazowki_o_nazwie(nazwa,  vecwsk);
            bitmapa->load( nazwa.c_str());
        }
        else
        {

            // jeśli wektorowa, to zmien flagę i rozkoduj numer ulubionej wsk wektorowej
            *flag_bitvec = false;
            *nr_wekt = atoi (nazwa.c_str() );
        }
    }
    else // jeśli żadamy wektorowej
    {
        // jeśli wektorowa, to zaznacz flagę i rozkoduj numer ulubionej wsk wektorowej
        // *flag_bitvec = false;
        // *nr_wekt = atoi (nazwa.c_str() );
    }
}

//************************************************************************************************************************
bool MainWindow::czy_biezace_wskazowki_sa_fabryczne()
{

    bool rezultat = true;

    // sprawdzenie godzinowej ----------------------------------
    auto nazwa_obecnej_godzinowej = flag_wsk_godz_bitmapowa ?
                vect_godzinowych[nr_wybranej_bitmapy_wsk_godzinowej].bitmapa   :
                to_string(nr_wsk_wektorowej_godz);

    auto nazwa_fab_godzinowej = cyferblat[nr_tarczy].bitmapa_godzinowa;

    if(nazwa_obecnej_godzinowej == nazwa_fab_godzinowej)
    {
        //        cout << "To jest godzinowa wskazowka fabryczyna" << endl;

    }
    else
    {
        //        cout << "To NIE jest godzinowa wskazowka fabryczyna" << endl;
        rezultat =  false;
    }

    // min ---------------------------------
    auto nazwa_obecnej_min = flag_wsk_min_bitmapowa ?
                vect_minutowych[nr_wybranej_bitmapy_wsk_minutowej].bitmapa   :
                to_string(nr_wsk_wektorowej_min);

    auto nazwa_fab_min = cyferblat[nr_tarczy].bitmapa_minutowa;

    if(nazwa_obecnej_min == nazwa_fab_min)
    {
        //        cout << "To jest mininowa wskazowka fabryczyna" << endl;
    }
    else
    {
        //        cout << "To NIE jest mininowa wskazowka fabryczyna" << endl;
        rezultat =  false;
    }

    // sek  ---------------------------------
    auto nazwa_obecnej_sek = flag_wsk_sek_bitmapowa ?
                vect_sekundnikow[nr_wybranej_bitmapy_wsk_sekundowej].bitmapa   :
                to_string(nr_wsk_wektorowej_sek);

    auto nazwa_fab_sek = cyferblat[nr_tarczy].bitmapa_sekundnik;

    if(nazwa_obecnej_sek == nazwa_fab_sek)
    {
        //        cout << "To jest sekundowa wskazowka fabryczyna" << endl;
    }
    else
    {
        //        cout << "To NIE jest sekundowa wskazowka fabryczyna" << endl;
        rezultat =  false;
    }

    return rezultat;

#if 0
    // czy to naprawdę nazwa bitmapowej
    if(czy_nazwa_naprawde_bitmapowa(nazwa_fab_godzinowej) )
    {
        *flag_bitvec = true;
        // Jeśli bitmapowa, to przepisujemy stamtąd nazwe ulubionej bitmapy
        *nr_bitm = znajdz_nr_wskazowki_o_nazwie(nazwa,  vecwsk);
        bitmapa->load( nazwa.c_str());
    }
    else
    {
        // jeśli wektorowa, to zmien flagę i rozkoduj numer ulubionej wsk wektorowej
        *flag_bitvec = false;
        *nr_wekt = atoi (nazwa.c_str() );
    }
#endif

}
//************************************************************************************************************************
void MainWindow::wgraj_wskazowke_fabryczna(string nazwa,
                                           vector<Tdane_wskazowki> &vecwsk,
                                           bool * flag_bitvec, int *nr_wekt, int * nr_bitm   // tu beda odpowiedzi
                                           , QPixmap *bitmapa)
{
    // czy to naprawdę nazwa bitmapowej
    if(czy_nazwa_naprawde_bitmapowa(nazwa) )
    {
        *flag_bitvec = true;
        // Jeśli bitmapowa, to przepisujemy stamtąd nazwe ulubionej bitmapy
        *nr_bitm = znajdz_nr_wskazowki_o_nazwie(nazwa,  vecwsk);
        bitmapa->load( nazwa.c_str());
    }
    else
    {
        // jeśli wektorowa, to zmien flagę i rozkoduj numer ulubionej wsk wektorowej
        *flag_bitvec = false;
        *nr_wekt = atoi (nazwa.c_str() );
    }
}
//************************************************************************************************************************
bool MainWindow::czy_nazwa_naprawde_bitmapowa(string nazwa)
{
    if( nazwa.size() == 1 && isdigit(nazwa[0] ) )
    {
        return false;
    }
    else
    {
        return true;
    }
}
//************************************************************************************************************************
bool MainWindow::zmiana_wygladu_cyferblatu(int nr,  tryb_wskazowek  tryb)
{
    // cout << __func__  << " (z argumentem force favorites = "
    //         << int (tryb) << ")" << " nr = " << nr << endl;

    if(nr >= (int) cyferblat.size())
    {
        nr = 0;
        return 0;
    }

    nr_tarczy= nr;

    cyf = cyferblat[nr];

#if 1
    if(tryb == tryb_wskazowek::ulubione)
    {
        //

        //        cout << "przejrzenie tablicy favourites " << endl;
        bool flag_znalezione = false;
        for(auto &entry : vec_pref_hands)
        {

            if(entry.clock_face_name == cyf.nazwa)    // czy jest zapis ulubionych wskazowek dla tego cyferbaltu?
            {
                //                cout << "znalezione entry favourites dla tarczy " << cyf.nazwa << endl;
                flag_znalezione = true;

                // sprawdzenie czy ulubiona wsk godzinowa jest cyfrą (czytli wektorowa) -----------------

                wgraj_wskazowke(entry.hours_hand, vect_godzinowych,
                                & flag_wsk_godz_bitmapowa,
                                & nr_wsk_wektorowej_godz,
                                & nr_wybranej_bitmapy_wsk_godzinowej,
                                & godzinowa
                                );

                wgraj_wskazowke(entry.minutes_hand , vect_minutowych,
                                & flag_wsk_min_bitmapowa,
                                & nr_wsk_wektorowej_min,
                                & nr_wybranej_bitmapy_wsk_minutowej,
                                & minutowa
                                );

                wgraj_wskazowke(entry.seconds_hand , vect_sekundnikow,
                                & flag_wsk_sek_bitmapowa,
                                & nr_wsk_wektorowej_sek,
                                & nr_wybranej_bitmapy_wsk_sekundowej,
                                & sekundnik
                                );

                break;

                //                // sprawdzenie czy ulubiona wsk sek jest cyfrą (czytli wektorowa) -------------------
                //                if( entry.seconds_hand.size() == 1 && isdigit(entry.seconds_hand[0] ) )
                //                {
                //                    flag_wsk_sek_bitmapowa = false;
                //                    nr_wsk_wektorowej_sek = atoi (entry.seconds_hand.c_str() );

                //                }
                //                else
                //                {
                //                    flag_wsk_sek_bitmapowa = true;
                //                    // Jeśli bitmapowa, to zapamiętujemy nazwe bitmapy
                //                    nr_wybranej_bitmapy_wsk_sekundowej = znajdz_nr_wskazowki_o_nazwie(
                //                                entry.seconds_hand,
                //                                vect_sekundnikow );
                //                }

            }
        }

        if(flag_znalezione == false)
        {
            //            cout << " Info o ulubionych wskazowkach nie  znalezione, wiec dajemy ostatnie uzywane" << endl;
            tryb = tryb_wskazowek::zwykle;
        }
    }
#endif

    // jesli sie nie uda ladowanie to rezultatem load jest false ???


    tarcza.load( cyf.bitmapa_tarcza.c_str());
    central.load( cyf.bitmapa_central.c_str());

    if(tryb == tryb_wskazowek::fabryczne)
    {
        //        cout << "Wskazowki  fabryczne " << endl;

        wgraj_wskazowke_fabryczna(cyf.bitmapa_godzinowa, vect_godzinowych,
                                  & flag_wsk_godz_bitmapowa,
                                  & nr_wsk_wektorowej_godz,
                                  & nr_wybranej_bitmapy_wsk_godzinowej,
                                  & godzinowa
                                  );

        wgraj_wskazowke_fabryczna(cyf.bitmapa_minutowa , vect_minutowych,
                                  & flag_wsk_min_bitmapowa,
                                  & nr_wsk_wektorowej_min,
                                  & nr_wybranej_bitmapy_wsk_minutowej,
                                  & minutowa
                                  );

        wgraj_wskazowke_fabryczna(cyf.bitmapa_sekundnik , vect_sekundnikow,
                                  & flag_wsk_sek_bitmapowa,
                                  & nr_wsk_wektorowej_sek,
                                  & nr_wybranej_bitmapy_wsk_sekundowej,
                                  & sekundnik
                                  );


    }

    if(tryb == tryb_wskazowek::zwykle)
    {
        //        cout << "Nie ulubione, nie fabryczne, po prostu ostatnio obowiazujace " << endl;


        wgraj_wskazowke( vect_godzinowych[nr_wybranej_bitmapy_wsk_godzinowej].bitmapa,
                         vect_godzinowych,
                         & flag_wsk_godz_bitmapowa,
                         & nr_wsk_wektorowej_godz,
                         & nr_wybranej_bitmapy_wsk_godzinowej,
                         & godzinowa
                         );

        //- min ---------------------
        wgraj_wskazowke(vect_minutowych[nr_wybranej_bitmapy_wsk_minutowej].bitmapa,
                        vect_minutowych,
                        & flag_wsk_min_bitmapowa,
                        & nr_wsk_wektorowej_min,
                        & nr_wybranej_bitmapy_wsk_minutowej,
                        & minutowa
                        );

        //        cout << "ma to byc minutnik bitmapowy?  "
        //             << boolalpha << flag_wsk_min_bitmapowa
        //             << " o numerze " << nr_wybranej_bitmapy_wsk_minutowej
        //             << ", a nr wekt = " << nr_wsk_wektorowej_min
        //             << endl;




        // sek -----------------


        //        cout << "ma to byc sektundnki bitmapowy?  "
        //             << boolalpha << flag_wsk_sek_bitmapowa
        //             << endl;

        if(nr_wybranej_bitmapy_wsk_sekundowej >= (int) vect_sekundnikow.size())
            nr_wybranej_bitmapy_wsk_sekundowej = 0;

        wgraj_wskazowke(vect_sekundnikow[nr_wybranej_bitmapy_wsk_sekundowej].bitmapa,
                        vect_sekundnikow,
                        & flag_wsk_sek_bitmapowa,
                        & nr_wsk_wektorowej_sek,
                        & nr_wybranej_bitmapy_wsk_sekundowej,
                        & sekundnik
                        );

        //        cout << "Po funkcji wgraj . Sektundnki bitmapowy?  "
        //             << boolalpha << flag_wsk_sek_bitmapowa
        //             << " o numerze " << nr_wybranej_bitmapy_wsk_sekundowej
        //             << ", a nr wekt = " << nr_wsk_wektorowej_sek
        //             << endl;

    } // koniec if zwykle



    rozm_tarczy_x =  gskala*tarcza.width();
    rozm_tarczy_y =  gskala*tarcza.height();
    resize(rozm_tarczy_x, rozm_tarczy_y);

    promien = gskala* cyf.promien_tarczy;
    //          cout << "promien_ samej  = " << promien << endl;
    cyf.uskok_cienia = cyf.uskok_cienia * promien / 100.0;

    // wskazowka godzinowa ===================================

#if 1

    flag_pokaz_to = false;
    przelicz_wskazowke(nr_wybranej_bitmapy_wsk_godzinowej ,
                       vect_godzinowych,
                       godzinowa,
                       flag_wsk_godz_bitmapowa,

                       // dotychczasowe dane o tej wskazowce
                       cyf.os_x_godzinowej,
                       cyf.os_y_godzinowej,
                       cyf.dlugosc_godzinowej,
                       cyf.szer_x_godzinowej,
                       cyf.kolor_godzinowej,
                       // rezultaty
                       godz_mimosrod_x,
                       godz_mimosrod_y,
                       godz_length_poly,
                       godz_szer_poly,
                       godz_wsp_korekcji_dlugosci,
                       godz_wsp_korekcji_szerokosci,

                       // dla bitmapowej  wsk godzinowej
                       godz_skalowanie_x,
                       godz_skalowanie_y
                       );


    //        cout <<  "godz_mimosrod_x =" <<   godz_mimosrod_x
    //              << ", godz_mimosrod_y = " <<   godz_mimosrod_y
    //              << ", godz_szer_poly = " <<  godz_szer_poly
    //              << ", godz_length_poly = "<< godz_length_poly
    //              << endl;

    flag_pokaz_to = false;


#endif

    // minutowa -------------------------------------------------------------
    // wskazowka minutowa ===================================

#if 1
    przelicz_wskazowke(nr_wybranej_bitmapy_wsk_minutowej,
                       vect_minutowych,
                       minutowa,
                       flag_wsk_min_bitmapowa,

                       // dotychczasowe dane o tej wskazowce
                       cyf.os_x_minutowej,
                       cyf.os_y_minutowej,
                       cyf.dlugosc_minutowej,
                       cyf.szer_x_minutowej,
                       cyf.kolor_minutowej,
                       // rezultaty
                       min_mimosrod_x,
                       min_mimosrod_y,
                       min_length_poly,
                       min_szer_poly,
                       min_wsp_korekcji_dlugosci,
                       min_wsp_korekcji_szerokosci,

                       // dla bitmapowej  wsk godzinowej
                       min_skalowanie_x,
                       min_skalowanie_y
                       );

#else

#endif


    // sekundnik ----------------------------------------------------------

#if 1

    przelicz_wskazowke(nr_wybranej_bitmapy_wsk_sekundowej,
                       vect_sekundnikow,
                       sekundnik,
                       flag_wsk_sek_bitmapowa,

                       // dotychczasowe dane o tej wskazowce
                       cyf.os_x_sekundnika,
                       cyf.os_y_sekundnika,
                       cyf.skala_y_sekundnika,
                       cyf.skala_x_sekundnika,
                       cyf.kolor_sekundnika,
                       // rezultaty
                       sek_mimosrod_x,
                       sek_mimosrod_y,
                       sek_length_poly,
                       sek_szer_poly,
                       sek_wsp_korekcji_dlugosci,
                       sek_wsp_korekcji_szerokosci,

                       // dla bitmapowej  wsk
                       sek_skalowanie_x,
                       sek_skalowanie_y
                       );

#else


#endif
    update();
    return true;

}
//***************************************************************************************
void MainWindow::mousePressEvent(QMouseEvent *e)
{
    //    cout << "F. mousePressEven - button" << endl ;


    if(e->button() & Qt::RightButton)
    {
        //        cout << "F. spectrum_1D::mousePressEven - RIGHT button" << endl ;
        QMenu *context_Menu = new QMenu;
        enum tryb { zwykly_sa_poligony, zwykly_bez_poligonu, rysowanie, edycja };

        tryb biezacy_tryb = tryb::zwykly_sa_poligony;

        if(vpoligon.empty() )
            biezacy_tryb =zwykly_bez_poligonu;

        if(flag_rysowanie_poligonu) biezacy_tryb = rysowanie;
        if(flag_edycja_poligonu) biezacy_tryb = edycja;


        if(biezacy_tryb == zwykly_sa_poligony)
        {
            context_Menu->addAction(tr("Draw a new polygon"), this, SLOT(wejdz_w_rys_poligonu() ) );
            context_Menu->addAction(tr("Edit polygon"), this, SLOT(wejdz_w_edycje_poligonu() ) );
            //context_Menu->addAction(tr("Remove polygon"), this, SLOT(remove_poligon() ) );
        }
        if(biezacy_tryb == zwykly_bez_poligonu)
            context_Menu->addAction(tr("Draw a new polygon"), this, SLOT(wejdz_w_rys_poligonu() ) );

        if(biezacy_tryb == rysowanie)
            context_Menu->addAction(tr("Finish drawing a polygon"), this,
                                    SLOT(skoncz_rys_poligonu() ) );
        if(biezacy_tryb == edycja){
            context_Menu->addAction(tr("Finish editing polygon"), this,
                                    SLOT(skoncz_edycje_poligonu() ) );
            context_Menu->addAction(tr("Add new vertex of polygon (after a selected vertex)"), this,
                                    SLOT(dodaj_vertex() ) );
            context_Menu->addAction(tr("Remove selected vertex"), this,
                                    SLOT(remove_selected_vertex() ) );
            context_Menu->addAction(tr("Remove selected polygon"), this, SLOT(remove_poligon() ) );
            // context_Menu->addAction("More transparency of background (in steps)", this, SLOT(set_transparency() ) );
            context_Menu->addAction(tr("Transparency of a background (a SLIDER) "), this, SLOT(transparency_slider() ) );
        }

        context_Menu->addSeparator();

        if(biezacy_tryb == zwykly_sa_poligony || biezacy_tryb == zwykly_bez_poligonu){

            context_Menu->addAction(tr("Options..."), this, SLOT(wywolanie_okna_opcji() ));
            context_Menu->addAction(tr("Alarms..."), this, SLOT(wywolanie_okna_alarmow() ) );

            context_Menu->addAction(tr("Scale the ruler"), this, SLOT(skalowanie_podzialki() ) );

        }


        context_Menu->addSeparator();


        context_Menu->addSeparator();
        string info =  tr("You clicked at screen position ").toStdString();
        info += to_string(e->globalPosition().x()) + ", " + to_string(e->globalPosition().y());

        context_Menu->addAction(info.c_str(), this, SLOT(empty_function() ));

        context_Menu->addAction(exit_txt, this, SLOT(koniec_programu() ));

        // uruchomienie menu przygotowanego powyżej -----------------------
        context_Menu->exec(mapToGlobal(QPoint(e->position().x() ,e->position().y()   )));
        delete context_Menu;
    }


    //================================================================================
    else if(flag_rysowanie_poligonu){        // tryb rysowania bramki poligonowej
        //        cout << "F. mousePressEvent - w trybie rysowania bramki" << endl ;

        double translate_x = cyf.os_x_center * gskala;
        double translate_y =  cyf.os_y_center * gskala;

        auto x = ( e->position().x() - translate_x  );
        auto y = (e->position().y() - translate_y) ;

        x /= gskala;
        y /= gskala;

        auto & poligon = vpoligon[biezacy_poligon];
        poligon.push_back(QPointF(x, y));
        cout << "Klik okna x=" << e->position().x()
             << " y=" <<  e->position().y() << endl;

        cout << "Klik poligonu x=" << x
             << " y=" <<  y  << "bo transl x = "
             << translate_x
             << ", " << translate_y << endl;
        zapisz_poligony();
    }
    //===============================================================================
    else
        if(flag_edycja_poligonu) {        // tryb EDYCJI bramki poligonowej
            //            cout << "F. mousePressEvent - w trybie EDYCJI bramki" << endl ;


            QPoint p_klikniety;
            przelicz_wsp_eventowe_na_cyferblatowe(e->pos(), &p_klikniety);
            //                double translate_x = cyf.os_x_center * gskala;
            //                double translate_y =  cyf.os_y_center * gskala;

            //                auto x = ( e->position().x() - translate_x  );
            //                auto y = (e->position().y() - translate_y) ;

            auto x = p_klikniety.x();
            auto y = p_klikniety.y();
            //                cout << "Klik w p-cie okna [" << e->position().x()
            //                     << " , " <<  e->position().y()
            //                     << "] , cyferblatu [" << x
            //                     << " , " <<  y << "]"
            //                     << "bo transl x = "
            //                     << translate_x
            //                     << ", " << translate_y
            //                     << endl;



            // cout << "Klik w p-cie okna " ;
            drukuj_pt (p_klikniety);
            cout << endl;

            // odszukanie sąsiada

            nr_przesuwanego_vertexu = -1;

            for(uint p = 0 ; p < vpoligon.size() ; ++p)
            {
                auto & poligon = vpoligon[p];

                for(uint i = 0 ; i < poligon.size(); ++i)
                {

                    //                    cout << "Sprawdzenie sasiada vertexu nr "
                    //                         << i
                    //                         << " poligon[i]. [" << poligon[i].x()*gskala
                    //                         << ", " << poligon[i].y()*gskala
                    //                         << "], tymczasem kliknales x = " << x
                    //                         << ", y  = " << y
                    //                         << endl;

                    if(fabs(poligon[i].x()*gskala - x) < 11
                            &&
                            fabs(poligon[i].y()*gskala - y) < 11
                            )
                    {

                        nr_przesuwanego_vertexu = i;
                        // cout << "JEST Klik jest w sasiedztwie vertexu nr "
                        //      << i
                        //      << " poligon[i].x() =  " << poligon[i].x()*gskala
                        //      << ", x klik = " << x
                        //      << " poligon[i].y() =  " << poligon[i].y()*gskala
                        //      << ", y klik = " << y
                        //      << endl;

                        // zapamietanie najblizszego punktu
                        p_previous_vtx = QPoint(poligon[i].x()*gskala,  poligon[i].y()*gskala);
                        // cout << "zapamietanie p_previous punktu vertexu ";
                        drukuj_pt (p_previous_vtx); cout << endl;
                        biezacy_poligon = p;

                    }
                }
            }
            // this can be DRAGGING the nearest vertex of the polygon gate

            // for moving
            if ( nr_przesuwanego_vertexu > -1 ) // if true    (jeśli ZNALEZIONY)
            {

                auto & poligon = vpoligon[biezacy_poligon];
                // no controll modifier -------------------------

                // just normal selection of vertex
                setCursor ( QCursor ( Qt::PointingHandCursor ) ) ;
                flag_move_one_vertex_of_polygon = true;

                // for rubberband purposes

                starting_pt = p_klikniety;
                current_end_pt = p_klikniety;

                // karuzela laczaca koniec poligonu z poczatkiem
                int poprzedni = (nr_przesuwanego_vertexu == 0 )? (poligon.size()) - 1:
                                                                 nr_przesuwanego_vertexu -1;
                int nastepny = (nr_przesuwanego_vertexu+1 >= (int) poligon.size() )?  0:
                                                                                      nr_przesuwanego_vertexu +1;


                //                    cout << " nr_przesuwanego_vertexu" << nr_przesuwanego_vertexu
                //                         << " nastepny " << nastepny
                //                         << " bo poligon.size() = " << poligon.size() << endl;
                p_previous_vtx =
                        QPoint(poligon[poprzedni].x() , // *gskala,
                               poligon[poprzedni].y() // *gskala
                               );

                p_next_vtx = QPoint(poligon[nastepny].x(),  // *gskala,
                                    poligon[nastepny].y()   // *gskala
                                    );



                //         rubberband_on = true;
                //         setMouseTracking( true );

                // cout << "Znaleziony vertex " << endl ;


                // nowe
                //                    starting_pt = e->pos();
                //                    current_end_pt = starting_pt;
                //                    previous_end_pt = starting_pt;
                //                            flag_marking_selected_vertices_region_mode = true;

                //                            if(e->modifiers ()  & Qt::ShiftModifier)
                //                                flag_deselect_all_vertices_outside = false;
                //                            else {
                //                                flag_deselect_all_vertices_outside = true;
                //                            }

                if (!rubberBand)
                {
                    // cout << " need to make new rubberband for marking vertices" << endl;
                    rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
                }
                QPalette pal;
                pal.setBrush(QPalette::Highlight, QBrush(Qt::red));
                rubberBand->setPalette(pal);

                rubberBand->setGeometry(QRect(starting_pt, QSize()));
                rubberBand->show();
                flag_mouse_just_pressed = true;


                // dalej przejmuje funkcja MOVE

            } // end of found nearest vertex
            else // NIE ZNALEZIONY, mouse pressed in a neutral place ==============================
            {
                cout << "NIE Znaleziony VTX " << endl ;

                flag_mouse_just_pressed = false;
                //	cout << "pressed in a neutral place " << endl;
            }
            // nowe
            //                starting_pt = e->pos();
            //                current_end_pt = starting_pt;
            //                previous_end_pt = starting_pt;
            //                            flag_marking_selected_vertices_region_mode = true;

            //                            if(e->modifiers ()  & Qt::ShiftModifier)
            //                                flag_deselect_all_vertices_outside = false;
            //                            else {
            //                                flag_deselect_all_vertices_outside = true;
            //                            }

            //                if (!rubberBand)
            //                {
            //                    // cout << " need to make new rubberband for marking vertices" << endl;
            //                    rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
            //                }
            //                QPalette pal;
            //                pal.setBrush(QPalette::Highlight, QBrush(Qt::red));
            //                rubberBand->setPalette(pal);

            //                rubberBand->setGeometry(QRect(starting_pt, QSize()));
            //                rubberBand->show();


            // show_moving_flags("mousePress event: ");
            update();



            // $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
        }   // koniec  if(flag_edycja_poligonu)

        else{   // zwykle klikniecie .. lewym klawiszem
            // do suwania tarczy po ekranie
            lastMouseX = e->position().x();
            lastMouseY = e->position().y();

            // cout << "Na calym ekranie [" << e->globalPosition().x() << ", " << e->globalPosition().y()  << "]" << endl; ;
        }

}
//***************************************************************************************
void MainWindow::mouseMoveEvent(QMouseEvent *event)
{

    auto okno_x = this->x();
    auto okno_y = this->y();

    okno_x += (event->position().x()  - lastMouseX);
    okno_y += (event->position().y()  - lastMouseY);

    if(flag_blokuj_na_ekranie == false && flag_rysowanie_poligonu == false){
        this->move(okno_x, okno_y);
        poz_zegara_x = okno_x;
        poz_zegara_y = okno_y;

    }

    if ( flag_rysowanie_poligonu)
    {
        event->accept();
        return;
    }


    if ( flag_mouse_just_pressed )
    {
        // cout << " flag_mouse_just_pressed -> this is just the  first move after pressing " << endl;
        flag_mouse_just_pressed = false;
        flag_rubberband_on = true;
        setMouseTracking ( true );
    }
    if(flag_edycja_poligonu){

        moving_polygon_vertex_handler ( event );

    }

    //cout << "box_of_matrix:: end of mouse move" << endl;
    event->accept();
    update();

}
//************************************************************************
void MainWindow::moving_polygon_vertex_handler ( QMouseEvent* e )
{
    //    static int i = 0 ;
    //    cout << "normal situation  of moving the vertex "
    //            //            // << i++
    //         << " rubberband is " << flag_rubberband_on << endl;

    // ----------------- perhaps we want to move the vertex which is there ----

    //show_moving_flags("moving handler: ");


    if ( flag_rubberband_on )   // rubberband for moving
    {

        //        cout << "Move to pt okna x="
        //             << e->position().x()
        //             << " y=" <<  e->position().y()
        //             << ", cyferblatu x="
        //             << e->position().x() - cyf.os_x_center*gskala
        //             << " y="
        //             <<  e->position().y() - cyf.os_y_center*gskala
        //              << endl;


        //current_end_pt = e->pos();
        przelicz_wsp_eventowe_na_cyferblatowe(e->pos(), &current_end_pt );

        //                cout << "normal situation  of moving the vertex " // << i++
        //                     << " rubberband is " << flag_rubberband_on << endl;
        //        QPainter p ( this );
        QPoint pd ( x_distance_to_vertex, y_distance_to_vertex );

        current_end_pt /= gskala;

        //         p.setRasterOp ( Qt::XorROP );
        //        p.setCompositionMode(QPainter::CompositionMode_Xor);

        if ( flag_screen_just_repainted )
        {
            previous_end_pt = current_end_pt;
            // and no need to erasing previous
        }



        if ( flag_move_one_vertex_of_polygon )
        {
            //            painter.drawLine ( p_previous, current_end_pt - pd );
            //            painter.drawLine ( current_end_pt - pd, p_next );

            //            cout << "Rubber Shape  p_previous= ";drukuj_pt(p_previous_vtx);
            //            cout << " current_end_pt= "; drukuj_pt(current_end_pt);
            //            cout << " p_next= " ; drukuj_pt(p_next_vtx);
            //            cout << endl;

            QPainterPath tmp;
            tmp.moveTo(p_previous_vtx);   // move to line from previous vertex to here
            tmp.lineTo( current_end_pt);  // line from previous vertex to current
            tmp.lineTo( p_next_vtx );    // line from current to next vertex

            //            tmp.moveTo(current_end_pt);   // move to line from previous vertex to here
            //            tmp.lineTo( 20, 0); // current_end_pt);
            //            tmp.lineTo( 0, 20);  // current_end_pt);
            shape_moving_vertex = tmp;

            //            zapisz_poligony();

        }

        previous_end_pt = current_end_pt;
        //         p.setRasterOp ( Qt::CopyROP );
        //p.setCompositionMode(QPainter::CompositionMode_SourceOver);

    }

}

using typ_x = double;
//************************************************************************
void  MainWindow::mouseReleaseEvent ( QMouseEvent* )
{
    // ;
    //    rubberBand->hide();
    // determine selection, for example using QRect::intersects()
    // and QRect::contains().


    //    cout << "Mouse release event dla widgeta MainWindow" << endl ;
    flag_mouse_just_pressed = false;

    if ( flag_rysowanie_poligonu )
    {
        // nic nie robimy
    }



    if ( flag_edycja_poligonu)
    {
        // so perhaps it was moving the vertex ----------------


        if ( flag_rubberband_on && (flag_move_one_vertex_of_polygon ) )
        {
            // cout << "This is a rubberband " << endl;
            //            QPainter p ( this );
            //            p.eraseRect ( rect() );

            // if ( flag_crosshair_cursor == false )
            flag_rubberband_on = false;
            //setMouseTracking ( false );

            // here make a reaction for zooming or for moving a vertex of the polygon

            // now we can return the coordinates of all the points
            //            typ_x   x1 = typ_x ( ( starting_pt.x() ) );
            //            typ_x   y1 = typ_x ( ( starting_pt.y() ) ) ;

            // p1 and p3 have the coordinates of the magnifying region

            // if somebody just clicked there, so that the region would be too small
            // so we make general magnification aroung this point
            //                  cout << "starting point was x1,2 = "
            //                    << x1 << " " << y1
            //                    << " new is  = " <<  x2  <<  ", "
            //                    <<  y2 << endl;


            auto & poligon = vpoligon[biezacy_poligon];

            if ( nr_przesuwanego_vertexu >= (int) poligon.size() )
            {
                cout << "Illegar nr of polygon, in "
                     << __FILE__ << " line = " << __LINE__ << endl;
                return;
            }

            //------------------------------------------------

            if ( flag_move_one_vertex_of_polygon ) //  normal, just one vertex is moved ---------------
            {
                typ_x   x2 = typ_x (  ( previous_end_pt.x()  - x_distance_to_vertex ) );
                typ_x   y2 = typ_x (  ( previous_end_pt.y()  - y_distance_to_vertex ) ) ;

                //                double translate_x = cyf.os_x_center * gskala;
                //                double translate_y =  cyf.os_y_center * gskala;
                //                auto xc = ( x2 - translate_x  ) / gskala;
                //                auto yc = (y2  - translate_y) / gskala;
                //                cout << " Release at " << x2 << ", " << y2
                //                     << " , cyferblat " << xc << ", " << yc << endl;



                // if it was creating the new gate, so  the nothing sensible is in it_of_moving_vertex
                //if ( flag_it_of_moving_vertex_legal ) // non NULL
                {
                    // wpisanie nowej pozycji do polygonu
                    poligon[nr_przesuwanego_vertexu].setX(x2);
                    poligon[nr_przesuwanego_vertexu].setY(y2);

                }
                flag_move_one_vertex_of_polygon = false;
            }

            // banana[nr_of_moved_polygon].save_to_disk() ;
            zapisz_poligony();
            flag_move_one_vertex_of_polygon = false;
        } // end of rubberball


        //unsetCursor ();
        //		current_cursor = is_usual ;
        //        update() ;

    }
    setMouseTracking ( true );
    update();
    //;
}
//**************************************************************************************************************************
void MainWindow::paintEvent(QPaintEvent *)
{

    //    static int licznik= 1;
    //    cout << "licznik = " << ++licznik  << endl;

    flag_screen_just_repainted = true;


    QColor & hourColor = cyf.kolor_godzinowej;
    QColor & minuteColor =  cyf.kolor_minutowej;
    QColor & secondsColor =  cyf.kolor_sekundnika;
    QColor kolor_cienia = cyf.kolor_cienia;

    double uskok_cienia =  cyf.uskok_cienia ;     // promien * 0.03;
    //====================================================

    //    QTime time = QTime::currentTime();

    //    QDate date = QDate::currentDate();

    //Qt::LocalTime


    //  QDateTime UTC = QDateTime::currentDateTimeUtc();

    QDateTime localTime = QDateTime::currentDateTime();

    QDateTime zoneDateTime = localTime;
    if(time_zone.isValid())
    {
        zoneDateTime = localTime.toTimeZone(time_zone) ;
    } else{

        zoneDateTime = localTime;
    }

    //QDateTime zoneDateTime(localTime.toTimeZone(time_zone) );

    //dateTime.setTimeZone(zone );

    QTime time = zoneDateTime.time();
    QDate today = zoneDateTime.date();


    // int godzina = time.hour();
    //    //    // wydruk biezacego czasu w danej strefie
    //    cout << "godzina w tej strefie = "
    //         << zoneDateTime.toString().toStdString()
    //            //         << " godzina = "
    //            //         << godzina
    //         << "      nazwa biezace; strefy "
    //         << time_zone.abbreviation(zoneDateTime ).toStdString()
    //         << endl;


    //      time.setHMS(12, 00, 0);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    //    painter.translate(width() / 2, height() / 2);

    double translate_x = cyf.os_x_center;
    double translate_y =  cyf.os_y_center;

    painter.translate(gskala* translate_x,  gskala * translate_y);  // ustawienie pt-u 0,0 na srodku tarczy.

    painter.save();    // zapamietujemy obecny stan
    //----------------------------------------------------------------
    painter.scale( gskala, gskala); // skaluje wielkosc tarczy
    painter.drawPixmap( -translate_x, -translate_y,  tarcza);  // transl, bo to pozycja lewego gornego rogu.

    rysowanie_podzialki(painter);
    for(uint i = 0 ; i < vpoligon.size() ; ++i)
    {
        // można by zmieniać kolory

        rysuj_poligon(painter,
                      i,  // nr poligonu
                      2
                      );
    }


    // RUBBER -------------

    QPen pen = painter.pen();

    pen.setColor(QColor(255, 0, 0)) ;
    pen.setStyle(Qt::DotLine);
    painter.setPen(pen );
    // painter.setPen(Qt::DotLine);

    if(flag_move_one_vertex_of_polygon)
    {
        // draw new shape rubberband
        painter.drawPath(shape_moving_vertex);
    }


    painter.restore();
    painter.save();



    // ------------------ napisy  dzien tygodnia  oraz datownik ----------------------------
    if(flag_show_dzien_tygodnia  || flag_show_date)
    {
        painter.setPen(Qt::SolidLine);
        //painter.setBrush(QColor(0, 210, 210));
        painter.setPen(cyf.kolor_dnia_tygodnia);

        QFont font = painter.font();
        int font_w_pikselach = cyf.wys_daty * promien/ 100;
        font.setPixelSize(font_w_pikselach);
        painter.setFont(font);



        QDate date = today; //QDate::currentDate();


        if(flag_show_AM_PM){   // -------------------------------

            QPoint p = cyf.pozycja_AM_PM;
            // przeliczenie wzgl procentu długości promienia
            p.setX(p.x() *  promien/100.0);
            p.setY(p.y() *  promien/100.0);

            //cout << " poz AM w pikselach " << p.x() << ", y " << p.y() << endl;

            int szer = 0.3 * promien;
            //const
            QRect rectangle = QRect(
                        p.x(),
                        p.y(),
                        szer,  // width
                        font_w_pikselach  // height
                        );

            QString text_AM_PM = (time.hour() < 12 ? " AM " : " PM ");
            //  painter.drawText(rectangle, Qt::AlignCenter, text_AM_PM.c_str() );

            //++++++++++++++++++++++++++++


            QRect boundingRect;


            rectangle.setWidth(0);   // bylo 0
            painter.drawText(rectangle, 0,text_AM_PM, &boundingRect);

            //            boundingRect.setLeft(-boundingRect.width()/2.0); // zeby było symetrycznie
            //            boundingRect.setRight()   Left(-boundingRect.width()/2.0); // zeby było symetrycznie
            // p.setX( - boundingRect.width()/2.0);
            //p +=  QPoint( boundingRect.width()/2 , 0);
            boundingRect.moveCenter(p);
            // painter.drawRect(boundingRect.adjusted(0, 0, -pen.width(), -pen.width()));
            // wlasciwy wypis dnia tygodnia

            painter.setPen(Qt::SolidLine);
            painter.setPen(cyf.kolor_ramki_daty);
            painter.setBrush(cyf.kolor_tla_daty);
            painter.drawRect(boundingRect);

            // painter.setPen(cyf.kolor_AM_PM);
            painter.setPen(cyf.kolor_AM_PM);
            painter.drawText(boundingRect, 0,text_AM_PM, &boundingRect);
        }


        if(flag_show_dzien_tygodnia){   // ------------------ napis  dzien tygodnia -------------
            QPoint p = cyf.pozycja_dzien_tygodnia;
            // przeliczenie wzgl procentu długości promienia
            p.setX(p.x() *  promien/100.0);
            p.setY(p.y() *  promien/100.0);

            //cout << " poz daty w pikselach " << p.x() << ", y " << p.y() << endl;

            int szer = 0.3 * promien;
            //const
            QRect rectangle = QRect(
                        p.x(),
                        p.y(),
                        szer,  // width
                        font_w_pikselach  // height
                        );

            auto dzien_tygodnia =  date.toString(" dddd ");
            //dzien_tygodnia = "Poniedziałek wielkanocny";


            //++++++++++++++++++++++++++++


            QRect boundingRect;

            //            QPen pen = painter.pen();
            //            pen.setStyle(Qt::NoPen);

            rectangle.setWidth(0);   // bylo 0
            painter.drawText(rectangle, 0,dzien_tygodnia, &boundingRect);

            //            boundingRect.setLeft(-boundingRect.width()/2.0); // zeby było symetrycznie
            //            boundingRect.setRight()   Left(-boundingRect.width()/2.0); // zeby było symetrycznie
            // p.setX( - boundingRect.width()/2.0);
            //p +=  QPoint( boundingRect.width()/2 , 0);
            boundingRect.moveCenter(p);
            // painter.drawRect(boundingRect.adjusted(0, 0, -pen.width(), -pen.width()));
            // wlasciwy wypis dnia tygodnia

            painter.setPen(Qt::SolidLine);
            painter.setPen(cyf.kolor_ramki_daty);
            painter.setBrush(cyf.kolor_tla_daty);
            painter.drawRect(boundingRect);

            painter.setPen(cyf.kolor_dnia_tygodnia);
            painter.drawText(boundingRect, 0,dzien_tygodnia, &boundingRect);
        }


        if(flag_show_date) {               // data ----------------------------------

            painter.setPen(Qt::NoPen);
            //     auto date.toString("ddd d").toStdString()

            // painter.scale( godz_skalowanie_x, godz_skalowanie_y); // Scales the coordinate system by (sx, sy).

            //    painter.scale( 1,1);
            QPoint p = cyf.pozycja_daty;
            int left = p.x() * promien/100.0;
            int top = -(font_w_pikselach / 2) + (p.y()* promien/100.0) ;
            int szer  = promien * cyf.szer_daty / 100;
            //    cout << "pozycja datownika " << left << ", top = " << top
            //         << ", skal x = " << godz_skalowanie_x << ", skal y " <<godz_skalowanie_y
            //         << endl;
            const QRect rectangle_daty = QRect(
                        left,
                        top,
                        szer,  // witdth
                        font_w_pikselach    // -(2*top) // promien/5// height;
                        );
            painter.setPen(Qt::SolidLine);
            painter.setPen(cyf.kolor_ramki_daty);
            painter.setBrush(cyf.kolor_tla_daty);

            painter.drawRoundedRect(rectangle_daty, 2.0, 2.0);    // narysowanie tła



            painter.setPen(cyf.kolor_daty);
            painter.drawText(
                        rectangle_daty,
                        Qt::AlignCenter,
                        date.toString("d MM")
                        );
        }
    }


    painter.restore();    // odpamietujemy showany stan, translacja tak, skala nie
    painter.save();    // zapamietujemy obecny stan, translacja tak, skala nie
    //    painter.setPen(Qt::NoPen);


    //###################################################################
    //###################################################################
    //###################################################################
    //###################################################################

    painter.setPen(Qt::SolidLine);
    painter.setPen(Qt::RoundJoin | Qt::RoundCap);





#if  1
    // ====================== wskazowka - godzinowa ==============------
    painter.setBrush(hourColor);

    double  kat_obrotu = 30.0 * ((time.hour() + time.minute() / 60.0));
    //            +
    //   kat_obrotu +=  flag_wsk_godz_bitmapowa? 0 :  180;
    //

    //  painter.translate(gskala* translate_x,  gskala * translate_y);
    painter.rotate(kat_obrotu);
    painter.scale( godz_skalowanie_x, godz_skalowanie_y); // Scales the coordinate system by (sx, sy).
    //                 cout << "Skalownie przed drawPixmap = [" << godz_skalowanie_x
    //                      << ", " << godz_skalowanie_y << "]" << endl;

    if(flag_wsk_godz_bitmapowa){

        double  kat_obrotu_rad = qDegreesToRadians(kat_obrotu +45 );
        double odl_cienia_x = sin(kat_obrotu_rad) * uskok_cienia  / godz_skalowanie_x;
        double odl_cienia_y = cos(kat_obrotu_rad) * uskok_cienia /  godz_skalowanie_y;

        //        cout << "uskok_cienia wsk godz bitmapowej = " << odl_cienia_x << ", " << odl_cienia_y  << endl;

        // +++++++++++++++++ najpierw cień  +++++++++++++++++++++
        // cień wskazówki godzinowej

        QPixmap cien_pix = godzinowa;
        cien_pix.fill(cyf.kolor_cienia);
        auto maska = godzinowa.createMaskFromColor(Qt::transparent);
        cien_pix.setMask( maska ) ;



        painter.drawPixmap(
                    godz_mimosrod_x+ odl_cienia_x,
                    godz_mimosrod_y+ odl_cienia_y,
                    cien_pix);

        // wlasciwa wskazowka

        //        cout << "godz wsk ma mimosrod x = "
        //             << godz_mimosrod_x << ", y = "
        //            << godz_mimosrod_y << endl;

        painter.drawPixmap( godz_mimosrod_x, godz_mimosrod_y, godzinowa);



    }else{
        //                       cout << "wsk godzinowa wektorowa" << endl;
        //        painter.scale( cyf.skala_x_godzinowej, godz_wsp_korekcji_y);
        painter.setPen(Qt::RoundJoin );
        QPen pen2;
        //        pen2.setWidth(2* godz_szer_poly);

        pen2.setWidth(godz_szer_poly/1.5);
        painter.setPen(pen2) ;


        // ------------- cien ----------------------------
        painter.setPen( kolor_cienia); // kolor cienia
        painter.setBrush(kolor_cienia);


        double  kat_obrotu_rad = qDegreesToRadians(kat_obrotu +45 );
        double odl_cienia_x = sin(kat_obrotu_rad) * uskok_cienia  / godz_skalowanie_x;
        double odl_cienia_y = cos(kat_obrotu_rad) * uskok_cienia /  godz_skalowanie_y;


        //        cout << "Skalownie przed drawRoundedRect = [" << godz_skalowanie_x
        //             << ", " << godz_skalowanie_y << "]" << endl;
        ////        cout << "uskok_cienia wsk godz wektorowej = " << uskok_cienia << endl;

        //        cout << "uskok_cienia wsk godz wektorowej = " << odl_cienia_x << ", " << odl_cienia_y  << endl;


        //        cout << "godz wsk wektorowa ma mimosrod x = "
        //             << godz_mimosrod_x << ", y = "
        //            << godz_mimosrod_y << endl;

        switch(nr_wsk_wektorowej_godz)
        {
        case 1 :
            // zaokraglony prostokat


            // wskazowka -------------------------------------
            rysuj_wskaz_wektorowa(painter,nr_wsk_wektorowej_godz,
                                  godz_mimosrod_x ,
                                  godz_mimosrod_y - godz_length_poly,
                                  godz_szer_poly,
                                  godz_length_poly,
                                  hourColor,
                                  odl_cienia_x, odl_cienia_y, kolor_cienia,

                                  godz_szer_poly  / godz_skalowanie_x,
                                  godz_szer_poly / godz_skalowanie_y
                                  );

            break;


        default:
        case 0:    // ostry prostokat
            // cien ---------------
            //            rysuj_wskaz_wektorowa(painter,nr_wsk_wektorowej_godz,
            //                                  godz_mimosrod_x + odl_cienia_x,
            //                                  godz_mimosrod_y + odl_cienia_y  - godz_length_poly,
            //                                  godz_szer_poly,
            //                                  godz_length_poly,
            //                                  hourColor,
            //                                  odl_cienia_x, odl_cienia_y, kolor_cienia,
            //                                  0,
            //                                  0
            //                                  );

            // sama wskazowka -------------------------------------
            rysuj_wskaz_wektorowa(painter,nr_wsk_wektorowej_godz,
                                  godz_mimosrod_x ,
                                  godz_mimosrod_y - godz_length_poly,
                                  godz_szer_poly,
                                  godz_length_poly,
                                  hourColor,
                                  odl_cienia_x, odl_cienia_y, kolor_cienia,
                                  0,
                                  0
                                  );
            break;
        case 2: // wektorowa ozdobna typu strzalki  -------------------------------------
            rysuj_wskaz_wektorowa(painter,nr_wsk_wektorowej_godz,
                                  godz_mimosrod_x,
                                  godz_mimosrod_y,
                                  godz_szer_poly*2,
                                  godz_length_poly,
                                  hourColor,
                                  odl_cienia_x, odl_cienia_y, kolor_cienia
                                  );

            //            rysuj_wskaz_wektorowa(painter,nr_wsk_wektorowej_godz,
            //                                  godz_mimosrod_x,
            //                                  godz_mimosrod_y,
            //                                  godz_szer_poly*2,
            //                                  godz_length_poly,
            //                                  hourColor
            //                                  );


            // cout << "godz_mimosrod_x = " << godz_mimosrod_x << endl;
            break;

        } // end switch



    } // endif

    painter.restore();   // wracamy do zapamietanego stanu paintera

#endif // 0


#if 1
    // wskazowka - minutowa --------------------------------------------
    {
        painter.setPen(Qt::NoPen);
        painter.setBrush(minuteColor);

        painter.save();

        double kat_obrotu = 6.0 * (time.minute() + time.second() / 60.0);
        painter.rotate(kat_obrotu);
        painter.scale(min_skalowanie_x,min_skalowanie_y); // Scales the coordinate system by (sx, sy).
        //         cout << "Skalownie przed drawPixmap = [" <<min_skalowanie_x
        //              << ", " <<min_skalowanie_y << "]" << endl;

        if(flag_wsk_min_bitmapowa){


            // +++++++++++++++++ proba cienia +++++++++++++++++++++


            double  kat_obrotu_rad = qDegreesToRadians(kat_obrotu +45 );
            double odl_cienia_x = sin(kat_obrotu_rad) * uskok_cienia /  min_skalowanie_x;
            double odl_cienia_y = cos(kat_obrotu_rad) * uskok_cienia /  min_skalowanie_y;

            // cień wskazówki minutowej


            QPixmap cien_pix = minutowa;
            cien_pix.fill(cyf.kolor_cienia);
            auto maska = minutowa.createMaskFromColor(Qt::transparent);
            cien_pix.setMask( maska ) ;

            painter.drawPixmap( min_mimosrod_x+ odl_cienia_x,
                                min_mimosrod_y+ odl_cienia_y,
                                cien_pix);

            // wlasciwa wskazowka

            painter.drawPixmap(min_mimosrod_x,min_mimosrod_y, minutowa);
        }else{


            painter.setPen(Qt::RoundJoin );
            QPen pen2;
            pen2.setWidth(2* min_szer_poly);
            painter.setPen(pen2) ;

            // -------------cien  wektorowy ----------------
            painter.setPen( kolor_cienia); // kolor cienia
            painter.setBrush(kolor_cienia);

            double  kat_obrotu_rad = qDegreesToRadians(kat_obrotu +45 );
            double odl_cienia_x = sin(kat_obrotu_rad) * uskok_cienia /  min_skalowanie_x;
            double odl_cienia_y = cos(kat_obrotu_rad) * uskok_cienia /  min_skalowanie_y;

            switch(nr_wsk_wektorowej_min){
            case 1 :
                // zaokraglony prostokat
                //            // cien ---------------
                rysuj_wskaz_wektorowa(painter,nr_wsk_wektorowej_min,
                                      min_mimosrod_x ,
                                      min_mimosrod_y  - min_length_poly,
                                      min_szer_poly,
                                      min_length_poly,

                                      minuteColor,
                                      odl_cienia_x, odl_cienia_y, kolor_cienia,
                                      min_szer_poly  / min_skalowanie_x,
                                      min_szer_poly / min_skalowanie_y
                                      );
                break;


            case 0:   default:  // ostry prostokat
                // cien --------------------------------------------------------------
                rysuj_wskaz_wektorowa(painter,nr_wsk_wektorowej_min,
                                      min_mimosrod_x ,
                                      min_mimosrod_y  - min_length_poly,
                                      min_szer_poly,
                                      min_length_poly,
                                      minuteColor,
                                      odl_cienia_x, odl_cienia_y, kolor_cienia,
                                      0,
                                      0
                                      );

                break;

            case 2: // wektorowa ozdobna typu strzalki  -------------------------------------

                rysuj_wskaz_wektorowa(painter,nr_wsk_wektorowej_min,
                                      min_mimosrod_x,
                                      min_mimosrod_y,
                                      min_szer_poly*2,
                                      min_length_poly,

                                      minuteColor,
                                      odl_cienia_x, odl_cienia_y, kolor_cienia
                                      );
                break;

            } // end switch




        } // endif 0
        painter.restore();
    }
#endif // 0







#if 1
    // wskazowka - sekundowa, nowy styl --------------------------------------------


    if(flag_show_sekundnik){
        painter.setPen(Qt::NoPen);
        painter.setBrush(secondsColor);

        painter.save();


        painter.translate(cyf.odrebna_os_x * gskala, cyf.odrebna_os_y * gskala);

#if KRZYZE_PROWADZACE == true
        painter.setPen( kolor_cienia); // kolor cienia
        painter.drawLine(0,-promien, 0, +promien);
        painter.drawLine(-promien, 0, promien, 0);
#endif



        double  kat_obrotu = 6.0 * (time.second() );
        painter.rotate(kat_obrotu);
        painter.scale(sek_skalowanie_x,sek_skalowanie_y);               // Scales the coordinate system by (sx, sy).

        //         cout << "Skalownie przed drawPixmap = [" <<sek_skalowanie_x
        //              << ", " <<sek_skalowanie_y << "]" << endl;



        if(flag_wsk_sek_bitmapowa){
            double  kat_obrotu_rad = qDegreesToRadians(kat_obrotu +45 );
            double odl_cienia_x = sin(kat_obrotu_rad) * uskok_cienia /  sek_skalowanie_x;
            double odl_cienia_y = cos(kat_obrotu_rad) * uskok_cienia /  sek_skalowanie_y;

            // cień wskazówki ------------------

            QPixmap cien_pix = sekundnik;
            cien_pix.fill(cyf.kolor_cienia);
            auto maska = sekundnik.createMaskFromColor(Qt::transparent);
            cien_pix.setMask( maska ) ;

            painter.drawPixmap( sek_mimosrod_x+ odl_cienia_x,
                                sek_mimosrod_y+ odl_cienia_y,
                                cien_pix);

            // wlasciwa wskazowka --------------

            painter.drawPixmap(sek_mimosrod_x,   sek_mimosrod_y, sekundnik);
        }
        else{
            painter.setPen(Qt::RoundJoin );
            QPen pen2;
            pen2.setWidth(2* sek_szer_poly);
            painter.setPen(pen2) ;

            // -------------cien  wektorowy ----------------
            painter.setPen( kolor_cienia); // kolor cienia
            painter.setBrush(kolor_cienia);

            double  kat_obrotu_rad = qDegreesToRadians(kat_obrotu +45 );
            double odl_cienia_x = sin(kat_obrotu_rad) * uskok_cienia /  sek_skalowanie_x;
            double odl_cienia_y = cos(kat_obrotu_rad) * uskok_cienia /  sek_skalowanie_y;


            switch(nr_wsk_wektorowej_sek )
            {
            case 1 :
                // zaokraglony prostokat
                //            // cien ---------------
                rysuj_wskaz_wektorowa(painter,nr_wsk_wektorowej_sek,
                                      sek_mimosrod_x ,
                                      sek_mimosrod_y  - sek_length_poly,
                                      sek_szer_poly,
                                      sek_length_poly,

                                      secondsColor,
                                      odl_cienia_x, odl_cienia_y, kolor_cienia,
                                      sek_szer_poly  / sek_skalowanie_x,
                                      sek_szer_poly / sek_skalowanie_y
                                      );
                break;

            case 0:  default:  // ostry prostokat
                // cien --------------------------------------------------------------
                rysuj_wskaz_wektorowa(painter,nr_wsk_wektorowej_sek,
                                      sek_mimosrod_x ,
                                      sek_mimosrod_y  - sek_length_poly,
                                      sek_szer_poly,
                                      sek_length_poly,
                                      secondsColor,
                                      odl_cienia_x, odl_cienia_y, kolor_cienia,
                                      0,
                                      0
                                      );
                break;

            case 2: // wektorowa ozdobna typu strzalki  -------------------------------------

                rysuj_wskaz_wektorowa(painter,nr_wsk_wektorowej_sek,
                                      sek_mimosrod_x,
                                      sek_mimosrod_y,
                                      sek_szer_poly*2,
                                      sek_length_poly,
                                      secondsColor,
                                      odl_cienia_x, odl_cienia_y, kolor_cienia
                                      );
                break;
            } // end switch
        } // endif 0
        painter.restore();
    }

#endif // 0

    //-centralna zasłonka na oś wskazowek ---------------------------------

#if KRZYZE_PROWADZACE
    painter.setPen( kolor_cienia); // kolor cienia
    painter.drawLine(0,-promien, 0, +promien);
    painter.drawLine(-promien, 0, promien, 0);
#endif

    // to trzeba zmienić, bo nie skaluje się ze zmiana zoomu
    painter.scale( cyf.skala_x_center,  cyf.skala_y_center);
    //   painter.scale(sek_skalowanie_x, sek_skalowanie_y);
    painter.drawPixmap(-central.width()/2.0, -central.height()/2.0, central);



    flag_screen_just_repainted = false;

}
//*********************************************************************************************************
void MainWindow::rysuj_wskaz_wektorowa(QPainter & painter, int nr, double x, double y,
                                       double szer,
                                       double wys,
                                       QColor kolor,
                                       double cien_x,
                                       double cien_y, QColor kolor_cienia,
                                       double owal_x, double owal_y)
{

    switch (nr)
    {
    default:
    case 0:
    case 1:
    {
        QPen pen(QColor(0,0,0,50),
                 szer /3.0,   // szerokosc
                 Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        painter.setPen(pen);
        painter.setBrush(kolor_cienia);
        painter.setPen(Qt::NoPen);

        //\ sam cien wskazowki
        painter.drawRoundedRect(
                    x-(szer/2) + cien_x,
                    y + cien_y,
                    szer, wys, owal_x, owal_y );

        // wlasciwa wskazowka
        painter.setPen(pen);
        painter.setBrush(kolor);
        painter.drawRoundedRect(  x-(szer/2),  y, szer, wys, owal_x, owal_y );

        break;
    }


    case 2:
    {

        y = -y ;                              // bo dodatni Y ma oznaczac obnizenie mimosrodu
        szer = szer / 2.0;
        wys = -wys * 1 ; // 0.8;                       //  lubimy, tutaj, zeby oś y biegła "w górę"
        double w09 =   0.9 * wys -y;
        double w07 =   0.7 * wys - y;
        double w65 =   0.65 * wys - y;
        double w55 =   0.55 * wys - y;
        double przeciwwaga = y;

        const QPointF strzalka[] = {
            QPointF( x, 0 /* -przeciwwaga */ ),           //  srodek dół // to robimy z całym wyświetlaniem poniżej
            QPointF(x ,  w65 ),
            QPointF(x -szer,  w07 ),
            QPointF(x,  w09 ),
            QPointF(x, wys ),
            QPointF(x, w09 ),
            QPointF(x+szer,  w07 ),
            QPointF(x ,  w65  ),
            QPointF(x ,  w55  ),
            QPointF(x - szer / 2,  w55  ),
            QPointF(x + szer / 2,   w55  ),
        };



        QPen pen_cienia(kolor_cienia,
                        szer,   // szerokosc
                        Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        painter.setPen(pen_cienia);

#if  1  // najpierw narysowanie cienia wskazowki
        painter.translate(cien_x, cien_y -przeciwwaga);
        painter.drawPolyline (strzalka, 11);
        painter.translate(-cien_x, -cien_y);
#endif

        // rysowanie wlaściwej wskazowki
#if 1
        QPen pen_wsk(kolor, szer,   // szerokosc
                     Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        painter.setPen(pen_wsk);

        painter.drawPolyline (strzalka, 11);

        // jesli chcemy ewentualne wypelnienie, to to co poniżej
        // painter.setBrush(QColor(250, 250,0) );
        //painter.drawPolygon (hourHand, 11);

#endif
    }
        break;


    }
}
//*********************************************************************************************************
void MainWindow::rysuj_poligon(QPainter & painter, int nr,
                               double szer


                               )
{

    auto polig = vpoligon[nr];  // uwaga, tu nie ma byc referecji bo chcemy lokalną kopię
    // której (chwilowo dopisujemy jeden końcowy vertex)

    //szer = szer / 2.0;
    if(polig.empty()) return;

    if(flag_rysowanie_poligonu == false)
    {
        // zamykam bramke poligonową po zakonczeniu rysowania
        polig.push_back(polig[0]);
    }

    auto vp = new QPointF[polig.size()];
    for(unsigned int i = 0 ; i < polig.size() ; ++i)
    {
        vp[i] = polig[i];
    }

    // rysowanie poligonu

    QPen pen_wsk(
                kolory_poligonow[nr% kolory_poligonow.size()],
            szer,   // szerokosc kreski
            Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    //painter.setBrush(QBrush(QColor(255, 255, 0)));
    painter.setPen(pen_wsk);
    painter.drawPolyline (vp,
                          polig.size()   // ile węzłów
                          );

    int offset = 10;
    painter.drawText(vp[0].x()-(offset), vp[0].y()-(offset), ("Nr_" + to_string(nr)).c_str());
    // narysowanie kółek - na węzłach poligonu
    for(int i = 0 ; i < (int)polig.size() ; ++i)
    {
        QRectF rectangle(vp[i].x()-(offset/2), vp[i].y()-(offset/2), offset, offset);


        painter.drawEllipse(rectangle);
        // wybrany vertex oznaczony wypełninym kwadratem
        if( (i == nr_przesuwanego_vertexu)  && (nr == biezacy_poligon) )
        {
            painter.setBrush(QBrush(QColor(255, 255, 0)));
            painter.drawRect(rectangle);
            painter.setBrush( QBrush());

        }
    }

    delete [] vp;
}
//*********************************************************************************************************
void MainWindow::rysowanie_podzialki(QPainter &painter)
{
    //ruler_mm = 65;
    if(ruler_mm == 0) {
        painter.drawText(-promien/2, 30, tr("No scale yet"));
        return;
    }

    QPen pen_wsk(QColor(250, 150, 0), 4,   // szerokosc kreski
                 Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

    painter.setPen(pen_wsk);

    QFont font = painter.font();
    int font_w_pikselach = 15; // *gskala; // cyf.wys_daty * promien/ 100;
    font.setPixelSize(font_w_pikselach);
    painter.setFont(font);

    auto ile_pixeli = tarcza.width() ;// * gskala;
    int kresek_skali = 13;
    int odstep = ile_pixeli / kresek_skali;

    for(int i = 0 ; i < kresek_skali/2 + 1; ++i)
    {
        int wynik = (i * odstep) * ruler_mm / ile_pixeli;
        string txt = to_string(wynik);
        // w prawo i w lewo

        painter.drawText(i * odstep, 30, txt.c_str()) ;
        painter.drawLine(i * odstep, -10, i * odstep, 10) ;

        painter.drawText(-i * odstep, 30, txt.c_str()) ;
        painter.drawLine(-i * odstep, -10, -i * odstep, 10) ;

        // pionowa skala --------------------------------

        painter.drawText(20, i * odstep, txt.c_str()) ;
        painter.drawLine(-10, i * odstep, 10, i * odstep);

        painter.drawText(20, -i * odstep, txt.c_str()) ;
        painter.drawLine(-10, -i * odstep, 10, -i * odstep) ;

    }
}
//*********************************************************************************************************
void MainWindow::wywolanie_okna_alarmow()
{
    //    cout << "wyolanie okna alarmow " << endl;
    Alarmy *dlg = new Alarmy(this, pathed_nazwa_pliku_z_alarmami);
    dlg->exec();
    delete dlg;

}
//*********************************************************************************************************
void MainWindow::wywolanie_okna_opcji()
{
    // wstepne_nr_jezyka = nr_jezyka;
    // cout << __PRETTY_FUNCTION__ << ", nr_jezyka = " << nr_jezyka
         // << endl;

    auto *dlg = new Topcje_dlg(this);


    if(dlg->exec() == QDialog::Accepted)
    {
        // cout << "Po accepted = "
        //      << __PRETTY_FUNCTION__ << ", nr_jezyka = " << nr_jezyka
        //      // << " nr_obecnie_zainstalowanego_jezyka = " << nr_obecnie_zainstalowanego_jezyka
        //      << endl;

        zapis_opcji_na_dysku();
    }
    delete dlg;

    // cout << "Zakonczenie funkcji "
    //     << __PRETTY_FUNCTION__ << ", nr_jezyka = " << nr_jezyka
    //      // << " nr_obecnie_zainstalowanego_jezyka = " << nr_obecnie_zainstalowanego_jezyka
    //      << endl;


    this->setWindowOpacity(przezroczystosc/255.0);

    //    cout << "Po dialogu opcji jezyk nr  " << nr_jezyka << endl;

    // if(flag_na_wierzchu != wstepne_flag_na_wierzchu
    //         ||
    //         wstepne_nr_jezyka != nr_jezyka
    //         )
    // {
    //     flag_ma_byc_restart = true;
    //     //        cout << "ma byc restart (przed close )" << endl;
    //     close();
    // }
}
//*********************************************************************************************************
void MainWindow::wejdz_w_rys_poligonu()
{
    flag_rysowanie_poligonu = true;
    flag_blokuj_na_ekranie = true;
    podmien_tarcze_na_polprzezroczysta(true);

    setCursor ( QCursor ( Qt::PointingHandCursor ) ) ;

    typ_poligonu nowy;
    vpoligon.push_back(nowy);
    biezacy_poligon = vpoligon.size() - 1;

}
//*********************************************************************************************************
void MainWindow::skoncz_rys_poligonu()
{
    flag_rysowanie_poligonu = false;
    flag_blokuj_na_ekranie = false;
    podmien_tarcze_na_polprzezroczysta(false);
    setCursor ( QCursor (  Qt::ArrowCursor));
    zapisz_poligony();
}
//*********************************************************************************************************
void MainWindow::wejdz_w_edycje_poligonu()
{
    podmien_tarcze_na_polprzezroczysta(true);
    flag_edycja_poligonu = true;
    flag_blokuj_na_ekranie = true;
    setCursor ( QCursor ( Qt::PointingHandCursor ) ) ;
}
//*********************************************************************************************************
void MainWindow::skoncz_edycje_poligonu()
{
    podmien_tarcze_na_polprzezroczysta(false);
    flag_edycja_poligonu = false;
    flag_blokuj_na_ekranie = false;
    setCursor ( QCursor (  Qt::ArrowCursor));
}
//*********************************************************************************************************
void MainWindow::podmien_tarcze_na_polprzezroczysta(bool flaga)
{
    if(flaga){
        zmien_przezroczystosc(transparency_while_editing_poligons);
        zmiana_wygladu_cyferblatu(1,  tryb_wskazowek::ulubione);
    }else{
        zmien_przezroczystosc(255);
        zmiana_wygladu_cyferblatu(0,  tryb_wskazowek::ulubione);
    }
}
//*********************************************************************************************************
void MainWindow::remove_poligon()
{
    if(biezacy_poligon < 0) {
        QMessageBox::information( this, "Note",
                                  "No polygon is currently selected. (click on some one vertex to select it)"
                                  );
        return;  // info, that is not selected
    }


    auto odp =  QMessageBox::warning ( this,
                                       tr("Are you sure?"),
                                       tr( "Removing poligon Nr %1\nAre you sure?").
                                            arg(biezacy_poligon),
                                       QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    if(odp == QMessageBox::Yes)
    {
        vpoligon[biezacy_poligon].clear();
        auto iterator = vpoligon.begin() + biezacy_poligon;
        // usuniecie z zestawu
        vpoligon.erase(iterator);
        //        flag_edycja_poligonu = false;
        //skoncz_edycje_poligonu();
        zapisz_poligony();
    }

}
//*********************************************************************************************************
void MainWindow::set_transparency ()
{
    transparency_while_editing_poligons -= 50;
    if(transparency_while_editing_poligons < 30) transparency_while_editing_poligons = 255;
    podmien_tarcze_na_polprzezroczysta(true);
}
//*********************************************************************************************************
void MainWindow::transparency_slider ()
{

    Tprzezroczystosc dlg(this);
    dlg.set_value(transparency_while_editing_poligons);
    int  odp = dlg.exec();
    if(odp == QDialog::Accepted){
        // cout << "odpowiedz " << odp << endl;

//        transparency_while_editing_poligons = dlg.get_value();
//        //   cout << "Przekazane " << transparency_while_editing_poligons << endl;
//        zmien_przezroczystosc(transparency_while_editing_poligons);
    }
}
//*********************************************************************************************************
void MainWindow::nastepna_tarcza()
{
    if(++nr_tarczy >= cyferblat.size()){
        nr_tarczy = 0;
    }

    zmiana_wygladu_cyferblatu(nr_tarczy, tryb_wskazowek::zwykle);
}
//*********************************************************************************************************
//void MainWindow::wywolanie_okna_wskazowek()
//{
//    // TA FUNKCJA JUZ NIE JEST POTRZEBNA

//    flag_wskazowki_bitmapowe = !flag_wskazowki_bitmapowe;
//    zmiana_wygladu_cyferblatu(nr_tarczy);

//}

//*********************************************************************************************************
void MainWindow::koniec_programu()
{
    // cout << "111  MainWindow::koniec_programu()" << endl;
    zapis_opcji_na_dysku();
    close();

}
//********************************************************************************************************
void MainWindow::player_position_upadate(qint64 position)
{
    cout << "player_position_update , (sprawdzenie długosci grania), obecnie =" << position << " ms" << endl;

    auto & v = vec_playerow_grajaych_teraz;  // bo za dluga nazwa

    cout << "Wektor grajacych ma elementow = " << v.size() << endl;

    // przeszukac alarmy ktory z nich ma adres tego playera
    for(uint i = 0 ; i < v.size() ; ++i)
    {
        // czy to ten palyer ?
        if( v[i].wsk_playera == sender())
        {
            cout << "to ten nadawca " << "ma miec sekund " << v[i].sekund_utworu << endl;

            if(position/1000 > v[i].sekund_utworu)
            {
                cout  << "Mija czas, wiec stop " << endl;
                v[i].wsk_playera->stop();

                if(v[i].flag_loop == false)
                {
                    cout << "nie ma flagi 'loop' wiec usuwamy z tablicy grajacych" << endl;


                    //  v[i].wsk_playera = nullptr;
                    //    v. pop_back();

                    for( auto it =  v.begin() ;                         it != v.end() ; )
                    {
                        if(it->wsk_playera == sender() )
                        {
                            v.erase(it);
                        }
                        else
                            ++it;
                    }
                }
                else
                {
                    cout << "jest flaga 'loop' nie usuwamy z tablicy grajacych " << endl;
                    v[i].wsk_playera->play();
                }
                break;
            }
        }
        else{
            cout << i << ") Ta pozycja w tablicy grajacych to nie TEN " << endl;

        }


        //          if(v.first = )
        //          v.second   .sekund_dzwieku
        //                  al.player
    }
    //cout << " ma byc sekund " << al.sekund_dzwieku << endl;
    //if(al.sekund_dzwieku  )
}

//*********************************************************************************************************
void MainWindow::wstepne_zaladowanie_tablicy_alarmow() {

    tablica_alarmow.clear();
    ifstream plik(pathed_nazwa_pliku_z_alarmami);
    if(!plik)
    {
        cerr << "Blad otwarcia pliku z alarmami " << pathed_nazwa_pliku_z_alarmami << endl;
        return;
    }
    string linijka;
    string tresc;
    while(
          getline(plik, linijka))
    {
        tresc += linijka + '\n';
    }
    // cout << "wczytany caly plik = " << tresc << endl;
    Tjeden_alarm a;

    size_t kursor = 0;
    for(int entry = 0 ; ; entry++)      // petla po alarmach
    {
        // wyjecie jednego alarmu ----------------------
        auto pocz = tresc.find("<alarm>", kursor);
        kursor = pocz;
        auto kon = tresc.find("</alarm>", kursor);
        kursor = kon;
        if(pocz == string::npos || kon == string::npos)
        {
            // cout << "Kniec peteli bo pocz = " << pocz << ", kon = " << kon << endl;
            break;
        }
        string one = tresc.substr(pocz, kon-pocz);

        // cout << "Jeden alarm nr " << entry << " ma tresc " << one << endl;
        // wyjecie poszczegolnych itemow z alarmu -------------------
        a =  wyjecie_itemow(one);
        a.nr_entry = entry;
        tablica_alarmow.push_back(a);
    }
    //    cout << "Po zakoniczeniu petli tablica alarmow  ma rozmiar " << tablica_alarmow.size() << endl;


    // Roboczo, w  przypadku konieczności wstepnego, "ręcznego" zaladowania
    //    tablica_alarmow.push_back(Tjeden_alarm{ true, "alarm A", "12:33", 1, "12-05-2020",
    //                                            0xf,  // czestotliwosc
    //                                            0,   // minuty
    //                                            true, "jakis opis",
    //                              true, "dzwiek.mp3", 0, true,   // dzwięk on, pocz_sekund, loop
    //                              false,
    //                                            "nazwa_programu",
    //                                            false  // czy wylaczyc komputer
    //                              });
    //   tablica_alarmow.push_back(Tjeden_alarm{ false, "alarm B", "17:44", 2,"12-09-2020", true, "inny  opis",
    //                                           true, "dzwiek22.mp3", 0, true,   // dzwięk on, pocz_sekund, loop
    //                                           false,
    //                                                         "gimp2",
    //                                                         false  // czy wylaczyc komputer
    //                                           });

}
//***************************************************************************************************************
Tjeden_alarm MainWindow::wyjecie_itemow(string one)
{
    Tjeden_alarm a;
    // znajdz enabled
    a.flag_enable = znajdz_int_item(one, "flag_enable");
    // znajdz zdarzenie
    a.nazwa = znajdz_txt_item(one, "nazwa");
    a.godzina = znajdz_txt_item(one, "godzina");
    a.czestotliwosc = znajdz_int_item(one, "czestotliwosc");
    a.data = znajdz_txt_item(one, "data");
    a.dni_tygodnia = znajdz_int_item(one, "dni_tygodnia");
    a.interwal = znajdz_int_item(one, "interwal");

    a.flag_okno = znajdz_int_item(one, "flag_okno");
    a.dodatkowy_opis = znajdz_txt_item(one, "dodatkowy_opis");
    a.flag_dzwiek = znajdz_int_item(one, "flag_dzwiek");

    a.muzyka = znajdz_txt_item(one, "muzyka");
    a.sekund_dzwieku = znajdz_int_item(one, "sekund_dzwieku");
    a.loop = znajdz_int_item(one, "loop");
    a.flag_program = znajdz_int_item(one, "flag_program");
    a.nazwa_programu = znajdz_txt_item(one, "nazwa_programu");
    a.flag_wylacz_komputer = znajdz_int_item(one, "flag_wylacz_komputer");

    return a;
}
//***************************************************************************************************************
string MainWindow::znajdz_txt_item(string txt, string keyword)
{
    auto offset = string{"<" + keyword+ ">"}.size();
    auto pocz = txt.find ("<" + keyword+ ">");
    if(pocz == string::npos)
        return "";
    auto kon = txt.find("</" + keyword+ ">");
    if(kon == string::npos)
    {
        exit(2);
    }
    pocz+= offset;
    string wartosc = txt.substr(pocz, kon-pocz);

    // cout << keyword << " ma pocz = " << pocz<< ", kon= " << kon << ",   txt wartosc = [" << wartosc << "]\n\n" << endl;
    return wartosc;
}
//***************************************************************************************************************
int MainWindow::znajdz_int_item(string txt, string keyword)
{
    auto offset = string{"<" + keyword+ ">"}.size();
    auto pocz = txt.find ("<" + keyword+ ">");
    if(pocz == string::npos)
        return -1;
    auto kon = txt.find("</" + keyword+ ">");
    pocz+= offset;
    int wartosc = atoi (txt.substr(pocz, kon-pocz).c_str()) ;
    // cout << keyword << " ma int wartosc = [" << wartosc << "]" << endl;
    return wartosc;
}
//***************************************************************************************************************
// Obecne tu dane o mimośrodach dotyczą sytuacji wskazowek wektorowych. Natomiast
// w przypadku wskazowek bitmapowych dane o mimośrodzie będą one  brane z bazy wskazowek.
//***************************************************************************************************************

void MainWindow::wstepne_wpisanie_cyferblatow()
{
#if 1
    cyferblat.push_back(
                Tdane_cyferblatu  {
                    "CCB Target przezroczysty",
                    ":/new/prefix1/content/celownik_ccb.png",       // ":/new/prefix1/content/AJ-CityHall-500.png",
                    ":/new/prefix1/content/city_hall_hours.png",
                    ":/new/prefix1/content/city_hall_minutes.png",
                    "0", // ":/new/prefix1/content/second.png",
                    ":/new/prefix1/content/center.png",

                    // godzinowa ---
                    0,
                    0.22, // mimosrod godzinowej
                    0.01,   // grubosc w skali promienia tarczy (wazne dla wektorowych)
                    0.1,// wysokosc  w skali promienia tarczy
                    QColor(255, 0, 0, 255),

                    // minutowa ---
                    0.0, 0.1,
                    0.01,   0.15,// os minutowej

                    QColor(0, 0, 50),

                    // sekundnik ---
                    0.0,    0,    // mimosrod
                    0.01, // grubosc wektorowego sekundnika
                    0.4,  // wysokosc  w skali promienia tarczy
                    QColor(200, 0, 0),
                    250,  250,  // srodek tarczy
                    0.05,  0.05,   // center

                    250,   // promien_ tarczy
                    0, 0,  // translacja do odrebnej osi sekundnika (x,y w % promienia)

                    QPoint(0, -30),    // pozycja AM_PM ( % z dlug promienia)
                    QColor(80, 80, 80),   // kolor AM_PM
                    QPoint(0, 60),    // pozycja dnia tyg ( % z dlug promienia)
                    QColor(80, 80, 80),   // kolor dnia tygodnia

                    QPoint(38, -1),    // pozycja datownika % z dlug promienia
                    30,   // szer okienka w % dlugosci promienia
                    10,   // wys okienka i fontu  (w % dlugosci promienia)
                    QColor(50, 0, 0, 255),   // kolor daty
                    QColor(50, 50, 50, 100),   // kolor ramki_daty
                    QColor(100, 100, 100, 100),   // kolor tla_daty
                    QColor(0, 0, 0, 25) ,  // kolor cienia
                    1   // uskok cienia w procentach promienia

                }
                );
    cyferblat.push_back(
                Tdane_cyferblatu  {
                    "CCB Target z tlem",
                    ":/new/prefix1/content/celownik_ccb_tlo.png",       // ":/new/prefix1/content/AJ-CityHall-500.png",
                    ":/new/prefix1/content/city_hall_hours.png",
                    ":/new/prefix1/content/city_hall_minutes.png",
                    "0", // ":/new/prefix1/content/second.png",
                    ":/new/prefix1/content/center.png",

                    // godzinowa ---
                    0,
                    0.22, // mimosrod godzinowej
                    0.01,   // grubosc w skali promienia tarczy (wazne dla wektorowych)
                    0.01,// wysokosc  w skali promienia tarczy
                    QColor(255, 0, 0, 255),

                    // minutowa ---
                    0.0, 0.1,
                    0.01,   0.015,// os minutowej

                    QColor(0, 0, 50),

                    // sekundnik ---
                    0.0,    0,    // mimosrod
                    0.01, // grubosc wektorowego sekundnika
                    0.4,  // wysokosc  w skali promienia tarczy
                    QColor(200, 0, 0),
                    250,  250,  // srodek tarczy
                    0.05,  0.05,   // center

                    250,   // promien_ tarczy
                    0, 0,  // translacja do odrebnej osi sekundnika (x,y w % promienia)

                    QPoint(0, -30),    // pozycja AM_PM ( % z dlug promienia)
                    QColor(80, 80, 80),   // kolor AM_PM
                    QPoint(0, 60),    // pozycja dnia tyg ( % z dlug promienia)
                    QColor(80, 80, 80),   // kolor dnia tygodnia

                    QPoint(38, -1),    // pozycja datownika % z dlug promienia
                    30,   // szer okienka w % dlugosci promienia
                    10,   // wys okienka i fontu  (w % dlugosci promienia)
                    QColor(50, 0, 0, 255),   // kolor daty
                    QColor(50, 50, 50, 100),   // kolor ramki_daty
                    QColor(100, 100, 100, 100),   // kolor tla_daty
                    QColor(0, 0, 0, 25) ,  // kolor cienia
                    0   // uskok cienia w procentach promienia

                }
                );
#endif

#if 0
    cyferblat.push_back(
                Tdane_cyferblatu  {
                    "Target przezroczysty",
                    ":/new/prefix1/content/target_przezroczysty.png",       // ":/new/prefix1/content/AJ-CityHall-500.png",
                    ":/new/prefix1/content/city_hall_hours.png",
                    ":/new/prefix1/content/city_hall_minutes.png",
                    "0", // ":/new/prefix1/content/second.png",
                    ":/new/prefix1/content/center.png",

                    // godzinowa ---
                    0,
                    0.22, // mimosrod godzinowej
                    0.01,   // grubosc w skali promienia tarczy (wazne dla wektorowych)
                    0.1,// wysokosc  w skali promienia tarczy
                    QColor(255, 0, 0, 255),

                    // minutowa ---
                    0.0, 0.1,
                    0.01,   0.15,// os minutowej

                    QColor(0, 0, 50),

                    // sekundnik ---
                    0.0,    0,    // mimosrod
                    0.01, // grubosc wektorowego sekundnika
                    0.4,  // wysokosc  w skali promienia tarczy
                    QColor(200, 0, 0),
                    250,  252,  // srodek tarczy
                    1.5,  1.5,   // center
                    225,   // promien_ tarczy
                    0, 0,  // translacja do odrebnej osi sekundnika (x,y w % promienia)

                    QPoint(0, -30),    // pozycja AM_PM ( % z dlug promienia)
                    QColor(80, 80, 80),   // kolor AM_PM
                    QPoint(0, 60),    // pozycja dnia tyg ( % z dlug promienia)
                    QColor(80, 80, 80),   // kolor dnia tygodnia

                    QPoint(38, -1),    // pozycja datownika % z dlug promienia
                    30,   // szer okienka w % dlugosci promienia
                    10,   // wys okienka i fontu  (w % dlugosci promienia)
                    QColor(50, 0, 0, 255),   // kolor daty
                    QColor(50, 50, 50, 100),   // kolor ramki_daty
                    QColor(100, 100, 100, 100),   // kolor tla_daty
                    QColor(0, 0, 0, 25) ,  // kolor cienia
                    2   // uskok cienia w procentach promienia

                }
                );

    cyferblat.push_back(
                Tdane_cyferblatu  {
                    "Target - do edycji poligonow",
                    ":/new/prefix1/content/target.png",       // ":/new/prefix1/content/AJ-CityHall-500.png",
                    ":/new/prefix1/content/city_hall_hours.png",
                    ":/new/prefix1/content/city_hall_minutes.png",
                    "0", // ":/new/prefix1/content/second.png",
                    ":/new/prefix1/content/center.png",

                    // godzinowa ---
                    0,
                    0.22, // mimosrod godzinowej
                    0.01,   // grubosc w skali promienia tarczy (wazne dla wektorowych)
                    0.1,// wysokosc  w skali promienia tarczy
                    QColor(255, 0, 0, 255),

                    // minutowa ---
                    0.0, 0.1,
                    0.01,   0.15,// os minutowej

                    QColor(0, 0, 50),

                    // sekundnik ---
                    0.0,    0,    // mimosrod
                    0.01, // grubosc wektorowego sekundnika
                    0.4,  // wysokosc  w skali promienia tarczy
                    QColor(200, 0, 0),
                    250,  252,  // srodek tarczy
                    0.05,  0.05,   // center
                    225,   // promien_ tarczy
                    0, 0,  // translacja do odrebnej osi sekundnika (x,y w % promienia)

                    QPoint(0, -30),    // pozycja AM_PM ( % z dlug promienia)
                    QColor(80, 80, 80),   // kolor AM_PM
                    QPoint(0, 60),    // pozycja dnia tyg ( % z dlug promienia)
                    QColor(80, 80, 80),   // kolor dnia tygodnia

                    QPoint(38, -1),    // pozycja datownika % z dlug promienia
                    30,   // szer okienka w % dlugosci promienia
                    10,   // wys okienka i fontu  (w % dlugosci promienia)
                    QColor(50, 0, 0, 255),   // kolor daty
                    QColor(50, 50, 50, 100),   // kolor ramki_daty
                    QColor(100, 100, 100, 100),   // kolor tla_daty
                    QColor(0, 0, 0, 25) ,  // kolor cienia
                    5   // uskok cienia w procentach promienia

                }
                );

#endif

    cyferblat.push_back(
                Tdane_cyferblatu  {
                    "CityHall",
                    ":/new/prefix1/content/city_hall_jg.png",       // ":/new/prefix1/content/AJ-CityHall-500.png",
                    ":/new/prefix1/content/city_hall_hours.png",
                    ":/new/prefix1/content/city_hall_minutes.png",
                    "0", // ":/new/prefix1/content/second.png",
                    ":/new/prefix1/content/center.png",

                    0,
                    0.22, // mimosrod godzinowej
                    0.05,   // grubosc w skali promienia tarczy (wazne dla wektorowych)
                    0.7,// wysokosc  w skali promienia tarczy
                    QColor(255, 0, 0, 255),

                    0.0, 0.1,
                    0.01,   0.85,// os minutowej

                    QColor(0, 0, 50),

                    //                    0.0,    0,   0.005,  0.8,  // os sekundnika
                    0.0,    0,    0.01,
                    1,  // os sekundnika
                    QColor(200, 0, 0),
                    250,  250,  // srodek tarczy
                    0.1, 0.1, //   1.5,  1.5,   // center
                    225,   // promien_ tarczy
                    0, 0,  // translacja do odrebnej osi sekundnika (x,y w % promienia)

                    QPoint(0, -30),    // pozycja AM_PM ( % z dlug promienia)
                    QColor(80, 80, 80),   // kolor AM_PM
                    QPoint(0, 60),    // pozycja dnia tyg ( % z dlug promienia)
                    QColor(80, 80, 80),   // kolor dnia tygodnia

                    QPoint(38, -1),    // pozycja datownika % z dlug promienia
                    30,   // szer okienka w % dlugosci promienia
                    10,   // wys okienka i fontu  (w % dlugosci promienia)
                    QColor(50, 0, 0, 255),   // kolor daty
                    QColor(50, 50, 50, 100),   // kolor ramki_daty
                    QColor(100, 100, 100, 100),   // kolor tla_daty
                    QColor(0, 0, 0, 25) ,  // kolor cienia
                    5   // uskok cienia w procentach promienia

                }
                );
    //---------------------------
#if 0
    cyferblat.push_back(
                Tdane_cyferblatu  {
                    "Rollex Oyster",
                    ":/new/prefix1/content/arne22.png",
                    "1",
                    "0",
                    "0", // ":/new/prefix1/content/second.png",
                    ":/new/prefix1/content/center.png",

                    0,     0.09,  0.05,  0.7, // os godzinowej, skala
                    QColor(30, 30, 30),

                    0.0,    0.09,
                    0.04,  0.9,// os minutowej
                    QColor(30, 30, 50),

                    0,    0.2,   0.005, 1,  // os sekundnika
                    QColor(200, 0, 0),

                    77,   77,  0.25,  0.25,   // center
                    64,  // promien_ tarczy
                    0, 0,  // translacja do odrebnej osi sekundnika (x,y w % promienia)
                    QPoint(0, 45),    // pozycja AM_PM ( % z dlug promienia)
                    QColor(80, 80, 80),   // kolor AM_PM

                    QPoint(0, 30),    // pozycja dnia tygodnia ( % z dlug promienia)
                    QColor(50, 50, 50),   // kolor dnia tygodnia

                    QPoint(30, +1),    // pozycja datownika % z dlug promienia
                    30,   // szer okienka w % dlugosci promienia
                    12,   // wys okienka i fontu  (w % dlugosci promienia)

                    QColor(50, 50, 50, 255),   // kolor daty
                    QColor(30, 30, 30, 50),   // kolor ramki_daty
                    QColor(100, 100, 100, 50),   // kolor tla_daty

                    QColor(0, 0, 0, 50) ,  // kolor cienia
                    5   // uskok cinia w procentach promienia

                }
                );
    //------------------------
    cyferblat.push_back(
                Tdane_cyferblatu  {
                    "Grollex",
                    ":/new/prefix1/content/Grolex.png",
                    "0",
                    "0",
                    "0",
                    ":/new/prefix1/content/center.png",
                    0,     0.09,  0.03,  0.7, // os godzinowej, skala
                    QColor(30, 30, 30),

                    0.0,    0.09,
                    0.02,  0.9,// os minutowej
                    QColor(30, 30, 50),

                    0,    0.2,   0.005, 1,  // os sekundnika
                    QColor(200, 0, 0),

                    175,   175,  0.25,  0.25,   // center
                    150,  // promien_ tarczy
                    0, 0,  // translacja do odrebnej osi sekundnika (x,y w % promienia)
                    QPoint(0, 55),    // pozycja AM_PM ( % z dlug promienia)
                    QColor(80, 80, 80),   // kolor AM_PM

                    QPoint(0, 30),    // pozycja dnia tygodnia ( % z dlug promienia)
                    QColor(100, 0, 0),   // kolor dnia tygodnia

                    QPoint(30, +1),    // pozycja datownika % z dlug promienia
                    30,   // szer okienka w % dlugosci promienia
                    12,   // wys okienka i fontu  (w % dlugosci promienia)

                    QColor(50, 50, 50, 255),   // kolor daty
                    QColor(30, 30, 30, 50),   // kolor ramki_daty
                    QColor(100, 100, 100, 50),   // kolor tla_daty

                    QColor(0, 0, 0, 50) ,  // kolor cienia
                    5   // uskok cinia w procentach promienia

                }
                );


    cyferblat.push_back(
                Tdane_cyferblatu  {
                    "Grollex2",
                    ":/new/prefix1/content/grolex_refleksy_ciemna_korona.png",
                    "0",
                    "0",
                    "0",
                    ":/new/prefix1/content/center.png",
                    0,     0.09,  0.03,  0.7, // os godzinowej, skala
                    QColor(30, 30, 30),

                    0.0,    0.09,
                    0.02,  0.9,// os minutowej
                    QColor(30, 30, 50),

                    0,    0.2,   0.005, 1,  // os sekundnika
                    QColor(200, 0, 0),

                    175,   175,  0.25,  0.25,   // center
                    150,  // promien_ tarczy
                    0, 0,  // translacja do odrebnej osi sekundnika (x,y w % promienia)
                    QPoint(0, 55),    // pozycja AM_PM ( % z dlug promienia)
                    QColor(80, 80, 80),   // kolor AM_PM

                    QPoint(0, 30),    // pozycja dnia tygodnia ( % z dlug promienia)
                    QColor(100, 0, 0),   // kolor dnia tygodnia

                    QPoint(30, +1),    // pozycja datownika % z dlug promienia
                    30,   // szer okienka w % dlugosci promienia
                    12,   // wys okienka i fontu  (w % dlugosci promienia)

                    QColor(50, 50, 50, 255),   // kolor daty
                    QColor(30, 30, 30, 50),   // kolor ramki_daty
                    QColor(100, 100, 100, 50),   // kolor tla_daty

                    QColor(0, 0, 0, 50) ,  // kolor cienia
                    5   // uskok cinia w procentach promienia

                }
                );

    //---------------------------
    cyferblat.push_back(
                Tdane_cyferblatu  {
                    "dragon",
                    ":/new/prefix1/content/dragon.png",
                    ":/new/prefix1/content/roman_hours.png",
                    ":/new/prefix1/content/roman_minut.png",
                    "0", //  ":/new/prefix1/content/second.png",
                    ":/new/prefix1/content/center.png",
                    0 ,    0,  0.04,  0.52, // os godzinowej, skala
                    QColor(30, 30, 30),
                    0.0,    0,   0.03,  0.8,// os minutowej
                    QColor(30, 30, 120),
                    0,    0,   0.005,  1,  // os sekundnika
                    QColor(200, 0, 0),
                    298,   340,  // pozycja srodka tarczy
                    0.75,  0.75,   // skalowanie zaślepki
                    60,  // promien_ tarczy
                    0, 0,  // translacja do odrebnej osi sekundnika (x,y w % promienia)

                    QPoint(0, -30),    // pozycja AM_PM ( % z dlug promienia)
                    QColor(80, 80, 80),   // kolor AM_PM
                    QPoint(0, 30),    // pozycja dnia tygodnia ( % z dlug promienia)
                    QColor(80, 80, 80),   // kolor dnia tygodnia

                    QPoint(30, +1),    // pozycja datownika % z dlug promienia
                    30,   // szer okienka w % dlugosci promienia
                    14,   // wys okienka i fontu  (w % dlugosci promienia)
                    QColor(50, 50, 50, 255),   // kolor daty
                    QColor(30, 30, 30, 50),   // kolor ramki_daty
                    QColor(100, 100, 100, 50),   // kolor tla_daty
                    QColor(0, 0, 0, 50) ,  // kolor cienia
                    5

                }
                );


    cyferblat.push_back(
                Tdane_cyferblatu  {
                    "Patek",
                    ":/new/prefix1/content/PatekPhilippe.png",
                    "0",
                    "0",
                    "0",
                    ":/new/prefix1/content/center.png",
                    0,     0,  0.03    ,  0.570, // os godzinowej, skala
                    QColor(0, 0, 0),
                    0.0,    0,   0.015,  0.80,// os minutowej
                    QColor(0, 0, 0),
                    0,    0,   0.005,  1,  // os sekundnika
                    QColor(0, 0, 200),

                    128,   131,  // pozycja srodka tarczy
                    0.15,  0.15,   // skalowanie zaślepki
                    111,  // promien_ tarczy
                    0, 0,  // translacja do odrebnej osi sekundnika (x,y w % promienia)
                    QPoint(0, -20),    // pozycja AM_PM ( % z dlug promienia)
                    QColor(80, 80, 80),   // kolor AM_PM
                    QPoint(0, 40),    // pozycja dnia tygodnia ( % z dlug promienia)
                    QColor(40, 40, 40),   // kolor dnia tygodnia

                    QPoint(30, -1),    // pozycja datownika % z dlug promienia
                    30,   // szer okienka w % dlugosci promienia
                    10,   // wys okienka i fontu  (w % dlugosci promienia)
                    QColor(50, 50, 50, 255),   // kolor daty
                    QColor(30, 30, 30, 50),   // kolor ramki_daty
                    QColor(100, 100, 100, 50),   // kolor tla_daty
                    QColor(0, 0, 0, 50) ,  // kolor cienia
                    5      // uskok cienia
                }
                );

    cyferblat.push_back(
                Tdane_cyferblatu  {
                    "Azul large",
                    ":/new/prefix1/content/Azul_large.png",
                    "0",
                    "0",
                    "0",
                    ":/new/prefix1/content/center.png",

                    //                    0,     0,  0.6    ,  0.60, // os godzinowej, skala
                    //                    QColor(0, 0, 200),
                    //                    0.0,    0,   1,  0.70,// os minutowej
                    //                    QColor(0, 0, 150),

                    0,     0.1 ,  0.025    ,  0.7, // os godzinowej, skala
                    QColor(0, 0, 200),
                    0.0,    0.07,   0.02,  0.85,// os minutowej
                    QColor(0, 0, 250),

                    0,    0,   0.003,  0.9,  // os sekundnika
                    QColor(200, 200, 0),

                    375,   375,  // pozycja srodka tarczy
                    0.35,  0.35,   // skalowanie zaślepki
                    225,  // promien_ tarczy
                    0, 0,  // translacja do odrebnej osi sekundnika (x,y w % promienia)

                    QPoint(0, -30),    // pozycja AM_PM ( % z dlug promienia)
                    QColor(20, 20, 80),   // kolor AM_PM

                    QPoint(0, 50),    // pozycja dnia tygodnia ( % z dlug promienia)
                    QColor(20, 20, 80),   // kolor dnia tygodnia

                    QPoint(30, -1),    // pozycja datownika % z dlug promienia
                    30,   // szer okienka w % dlugosci promienia
                    10,   // wys okienka i fontu  (w % dlugosci promienia)
                    QColor(0, 0, 150),   // kolor daty
                    QColor(0, 0, 150, 100),   // kolor ramki_daty
                    QColor(100, 100, 100, 50),   // kolor tla_daty
                    QColor(0, 0, 0, 50) ,  // kolor cienia
                    3      // uskok cienia


                }
                );

    cyferblat.push_back(
                Tdane_cyferblatu  {
                    "Citizien",
                    ":/new/prefix1/content/Citizen.png",
                    "0",
                    "0",
                    "0",
                    ":/new/prefix1/content/center.png",

                    0,     0,  0.04 ,  0.60, // os godzinowej, skala
                    QColor(40, 10, 10),   // kolor wskaz godzinowej
                    0.0,    0,   0.02,  0.74,// os minutowej
                    QColor(40, 10, 10),   // kolor wskaz minutowej
                    0,    0,  0.005,  0.8,  // os sekundnika
                    QColor(100, 0, 0),   // kolor wskaz sekundnika

                    100,   100,  // pozycja srodka tarczy
                    0.45,  0.45,   // skalowanie zaślepki
                    80,  // promien_ tarczy
                    0, 0,  // translacja do odrebnej osi sekundnika (x,y w % promienia)

                    QPoint(0, 15),    // pozycja AM_PM ( % z dlug promienia)
                    QColor(80, 80, 80),   // kolor AM_PM
                    QPoint(0, -18),    // pozycja dnia tygodnia ( % z dlug promienia)
                    QColor(80, 80, 80),   // kolor dnia tygodnia

                    QPoint(40, -2),    // pozycja datownika % z dlug promienia
                    38,   // szer okienka w % dlugosci promienia
                    16,   // wys okienka i fontu  (w % dlugosci promienia)
                    QColor(50, 50, 50),   // kolor daty
                    QColor(0, 0, 0, 0),   // kolor ramki_daty
                    QColor(150, 150, 150, 50),   // kolor tla_daty
                    QColor(0, 0, 0, 50) ,  // kolor cienia
                    5      // uskok cienia
                }
                );

    cyferblat.push_back(
                Tdane_cyferblatu  {
                    "Comdex-Omega1",
                    ":/new/prefix1/content/Comdex - Omega1.png",
                    "0",
                    "0",
                    "0",
                    ":/new/prefix1/content/center.png",

                    0,     0.09,  0.04 ,  0.65, // os godzinowej, skala
                    QColor(50, 50, 50, 180),   // kolor wskaz godzinowej
                    0.0,    0.1,   0.03,  0.85,// os minutowej
                    QColor(50, 50, 50),   // kolor wskaz minutowej
                    0,    0,   0.005,  1, //0.85,  // os sekundnika
                    QColor(150, 0, 0),   // kolor wskaz sekundnika

                    71,   157,  // pozycja srodka tarczy
                    0.25,  0.25,   // skalowanie zaślepki
                    50,  // promien_ tarczy
                    0, 0,  // translacja do odrebnej osi sekundnika (x,y w % promienia)

                    QPoint(0, -30),    // pozycja AM_PM ( % z dlug promienia)
                    QColor(80, 50, 50),   // kolor AM_PM
                    QPoint(0, 20),    // pozycja dnia tygodnia ( % z dlug promienia)
                    QColor(80, 50, 50),   // kolor dnia tygodnia

                    QPoint(32, -1),    // pozycja datownika % z dlug promienia
                    45,   // szer okienka w % dlugosci promienia
                    20,   // wys okienka i fontu  (w % dlugosci promienia)

                    QColor(40, 20, 20),   // kolor daty
                    QColor(0, 0, 0, 0),   // kolor ramki_daty
                    QColor(150, 150, 150, 0),   // kolor tla_daty
                    QColor(0, 0, 0, 50) ,  // kolor cienia
                    5      // uskok cienia
                }
                );



    cyferblat.push_back(
                Tdane_cyferblatu  {
                    "Green Marble",
                    ":/new/prefix1/content/greenmarble.png",
                    ":/new/prefix1/content/rombowe_godzinowa_czarna_plaska.png",
                    ":/new/prefix1/content/rombowe_minutowa_czarna_plaska.png",
                    "0",
                    ":/new/prefix1/content/center.png",

                    0,     0.2,  0.06 ,  0.7, // os godzinowej, skala
                    QColor(100, 130, 120),   // kolor wskaz godzinowej
                    0.0,    0.25,   0.04,  1.05,// os minutowej
                    QColor(100, 130, 120),   // kolor wskaz minutowej
                    0,    0,   0.005,  1,  // os sekundnika
                    QColor(100, 130, 120),   // kolor wskaz sekundnika

                    140,   138,  // pozycja srodka tarczy
                    1,  1,   // skalowanie zaślepki
                    110,  // promien_ tarczy
                    0, 0,  // translacja do odrebnej osi sekundnika (x,y w % promienia)
                    QPoint(0, -30),    // pozycja AM_PM ( % z dlug promienia)
                    QColor(80, 250, 80),   // kolor AM_PM
                    QPoint(0, 30),    // pozycja dnia tygodnia ( % z dlug promienia)
                    QColor(100, 130, 120),   // kolor dnia tygodnia

                    QPoint(24, 0),    // pozycja datownika % z dlug promienia
                    45,   // szer okienka w % dlugosci promienia
                    15,   // wys okienka i fontu  (w % dlugosci promienia)

                    QColor(100, 130, 120),   // kolor daty
                    QColor(100, 130, 120, 150),   // kolor ramki_daty
                    QColor(50, 50, 50, 50),   // kolor tla_daty
                    QColor(0, 0, 0, 80) ,  // kolor cienia
                    6      // uskok cienia
                }
                );

    cyferblat.push_back(
                Tdane_cyferblatu  {
                    "Rammstein",
                    ":/new/prefix1/content/Ram_corr.png",
                    "0",
                    "0",
                    "0",
                    ":/new/prefix1/content/center.png",

                    0,     0.0,   // os godzinowej, skala
                    0.04,   // szerokosc wskazowki
                    0.65,  // wzgledna dlugosc
                    QColor(10, 10, 210, 200),   // kolor wskaz godzinowej

                    0,    0,   0.03,  0.80,// os minutowej
                    QColor(10, 10, 10),   // kolor wskaz minutowej

                    0,    0,   0.005,  0.9,  // os sekundnika
                    QColor(140, 10, 10),   // kolor wskaz sekundnika

                    301,   301,  // pozycja srodka tarczy
                    1,  1,   // skalowanie zaślepki
                    290, // promien_ tarczy
                    0, 0,  // translacja do odrebnej osi sekundnika (x,y w % promienia)

                    QPoint(0, -70),    // pozycja AM_PM ( % z dlug promienia)
                    QColor(80, 80, 80),   // kolor AM_PM
                    QPoint(0, 48),    // pozycja dnia tygodnia ( % z dlug promienia)
                    QColor(10, 10, 10, 120),   // kolor dnia tygodnia

                    QPoint(-20, 65),    // pozycja datownika % z dlug promienia
                    40,   // szer okienka w % dlugosci promienia
                    10,   // wys okienka i fontu  (w % dlugosci promienia)

                    QColor(10, 10, 10, 120),   // kolor daty
                    QColor(0, 0, 0, 50),   // kolor ramki_daty
                    QColor(0, 0, 0, 10),   // kolor tla_daty
                    QColor(0, 0, 0, 80) ,  // kolor cienia
                    2     // uskok cienia
                }
                );


    cyferblat.push_back(
                Tdane_cyferblatu  {
                    "Wall Clock.png",
                    ":/new/prefix1/content/Wall Clock medium.png",
                    "0",
                    "0",
                    ":/new/prefix1/content/Wall-sekundnik.png",
                    ":/new/prefix1/content/center.png",

                    0,     0.15,  0.05 ,  0.55, // os godzinowej, skala
                    QColor(10, 10, 10),   // kolor wskaz godzinowej
                    0.0,    0.15,  0.04,  0.85,// os minutowej
                    QColor(10, 10, 10),   // kolor wskaz minutowej
                    0,    0.15,   0.005,  0.85,  // os sekundnika
                    QColor(80, 10, 10),   // kolor wskaz sekundnika

                    79,  79,  // pozycja srodka tarczy
                    0.12,  0.12,   // skalowanie zaślepki
                    75,  // promien_ tarczy
                    0, 0,  // translacja do odrebnej osi sekundnika (x,y w % promienia)

                    QPoint(0, -50),    // pozycja AM_PM ( % z dlug promienia)
                    QColor(80, 80, 80),   // kolor AM_PM
                    QPoint(0, -30),    // pozycja dnia tygodnia ( % z dlug promienia)
                    QColor(10, 10, 10),   // kolor dnia tygodnia

                    QPoint(-20, 40),    // pozycja datownika % z dlug promienia
                    40,   // szer okienka w % dlugosci promienia
                    15,   // wys okienka i fontu  (w % dlugosci promienia)

                    QColor(10, 10, 10),   // kolor daty
                    QColor(0, 0, 0, 50),   // kolor ramki_daty
                    QColor(0, 0, 0, 10) ,
                    QColor(0, 0, 0, 50) ,  // kolor cienia
                    5      // uskok cienia
                }
                );

    //------------------------------
    cyferblat.push_back(
                Tdane_cyferblatu  {
                    "Banner clock",
                    ":/new/prefix1/content/banner-1240822.png",
                    "0",            //                    ":/new/prefix1/content/arnehour_pion.png",
                    "0",            // ":/new/prefix1/content/arneminute_pion.png",

                    "2",     //":/new/prefix1/content/Wall-sekundnik.png",
                    ":/new/prefix1/content/center.png",

                    0,     0.15,  0.06 ,  0.7, // os godzinowej, skala
                    QColor(190, 255, 0),   // kolor wskaz godzinowej
                    0.0,    0.15,   0.04,  0.95,// os minutowej
                    QColor(255, 255, 0),   // kolor wskaz minutowej
                    0,    0.15,   0.01,  1,  // os sekundnika
                    QColor(250, 10, 10),   // kolor wskaz sekundnika

                    468,  163,  // pozycja srodka tarczy
                    0.12,  0.12,   // skalowanie zaślepki
                    120,  // promien_ tarczy
                    0, 0,  // translacja do odrebnej osi sekundnika (x,y w % promienia)

                    QPoint(0, -30),    // pozycja AM_PM ( % z dlug promienia)
                    QColor(100, 255, 100),   // kolor AM_PM
                    QPoint(0, -50),    // pozycja dnia tygodnia ( % z dlug promienia)
                    QColor(100, 255, 100),   // kolor dnia tygodnia

                    QPoint(60, -0),    // pozycja datownika % z dlug promienia
                    30,   // szer okienka w % dlugosci promienia
                    12,   // wys okienka i fontu  (w % dlugosci promienia)

                    QColor(100, 250, 250),   // kolor daty
                    QColor(0, 0, 200, 150),   // kolor ramki_daty
                    QColor(0, 0, 0, 100),   // kolor tla_daty
                    QColor(0, 0, 0, 200) ,  // kolor cienia
                    5      // uskok cienia
                }
                );

    //-------------------------------------------------
    cyferblat.push_back(
                Tdane_cyferblatu  {
                    "Earth luminous",
                    ":/new/prefix1/content/round-luminous-earth-modern.png",
                    ":/new/prefix1/content/neon_wskazowka.png",
                    ":/new/prefix1/content/neon_wskazowka.png",
                    ":/new/prefix1/content/neon_sekundnik.png",
                    ":/new/prefix1/content/center.png",

                    0,     0.05,  0.06 ,  0.7, // os godzinowej, skala
                    QColor(0, 0, 0, 200),   // kolor wskaz godzinowej
                    0.0,    0.05,   0.04,  0.85,// os minutowej
                    QColor(0, 0, 0, 200),   // kolor wskaz minutowej
                    0,    0.15,   0.005,  0.85,  // os sekundnika
                    QColor(250, 10, 10),   // kolor wskaz sekundnika

                    208,  153,  // pozycja srodka tarczy
                    0.12,  0.12,   // skalowanie zaślepki
                    150,  // promien_ tarczy
                    0, 0,  // translacja do odrebnej osi sekundnika (x,y w % promienia)

                    QPoint(0, -25),    // pozycja AM_PM ( % z dlug promienia)
                    QColor(0, 0, 50),   // kolor AM_PM
                    QPoint(0, -55),    // pozycja dnia tygodnia ( % z dlug promienia)
                    QColor(0, 0, 50),   // kolor dnia tygodnia

                    QPoint(-15, 45),    // pozycja datownika % z dlug promienia
                    30,   // szer okienka w % dlugosci promienia
                    10,   // wys okienka i fontu  (w % dlugosci promienia)

                    QColor(0, 0, 50),   // kolor daty
                    QColor(0, 0, 200, 30),   // kolor ramki_daty
                    QColor(10, 100, 255, 30),   // kolor tla_daty
                    QColor(0, 0, 0, 75) ,  // kolor cienia
                    5      // uskok cienia
                }
                );

    //-------------------------------------------------
    cyferblat.push_back(
                Tdane_cyferblatu  {
                    "Spacetime",
                    ":/new/prefix1/content/spacetime.png",
                    "0",
                    "0",
                    "2",
                    ":/new/prefix1/content/center.png",

                    0,
                    0.23, // mimosrod godzinowej
                    0.03,   // grubosc w skali promienia tarczy
                    0.7,// wysokosc  w skali promienia tarczy
                    QColor(255, 0, 0, 255),

                    0.0,
                    0.1,
                    0.03,
                    0.85,// os minutowej

                    QColor(0, 0, 50),

                    //                    0.0,    0,   0.005,  0.8,  // os sekundnika
                    0.0,    0,   0.01,  1,  // os sekundnika
                    QColor(200, 0, 0),
                    355,  200,  // pozycja srodka tarczy
                    0.8,  0.8,   // skalowanie zaślepki
                    70,  // promien_ tarczy
                    0, 0,  // translacja do odrebnej osi sekundnika (x,y w % promienia)

                    QPoint(0, 130),    // pozycja AM_PM ( % z dlug promienia)
                    QColor(250, 250, 250),   // kolor AM_PM
                    QPoint(150, -50),    // pozycja dnia tygodnia ( % z dlug promienia)
                    QColor(250, 250, 255),   // kolor dnia tygodnia

                    QPoint(120, 0),    // pozycja datownika % z dlug promienia
                    40,   // szer okienka w % dlugosci promienia
                    16,   // wys okienka i fontu  (w % dlugosci promienia)

                    QColor(250, 250, 255),   // kolor daty
                    QColor(0, 0, 240, 180),   // kolor ramki_daty
                    QColor(10, 10, 130, 130),   // kolor tla_daty
                    QColor(0, 0, 0, 150) ,  // kolor cienia
                    5      // uskok cienia
                }
                );


    //-------------------------------------------------
    cyferblat.push_back(
                Tdane_cyferblatu  {
                    "Tea-time",
                    ":/new/prefix1/content/tea-time.png",
                    "0",
                    "0",
                    "2",
                    ":/new/prefix1/content/center.png",

                    0,     0.1,  0.03 ,  0.6, // os godzinowej, skala
                    QColor(200, 50, 50, 250),   // kolor wskaz godzinowej
                    0.0,    0.1,   0.03,  0.75,// os minutowej
                    //                    QColor(10, 10, 50, 250),   // kolor wskaz minutowej
                    QColor(230, 230, 255, 250),   // kolor wskaz minutowej
                    0,    0.15,   0.005,  0.95,  // os sekundnika
                    QColor(250, 10, 10),   // kolor wskaz sekundnika

                    118,  118,  // pozycja srodka tarczy
                    0.8,  0.8,   // skalowanie zaślepki
                    118,  // promien_ tarczy
                    0, 0,  // translacja do odrebnej osi sekundnika (x,y w % promienia)

                    QPoint(60, 85),    // pozycja AM_PM ( % z dlug promienia)
                    QColor(80, 80, 80),   // kolor AM_PM
                    QPoint(-50, 85),    // pozycja dnia tygodnia ( % z dlug promienia)
                    QColor(0, 0, 0, 200),   // kolor dnia tygodnia

                    QPoint(60, -70),    // pozycja datownika % z dlug promienia
                    40,   // szer okienka w % dlugosci promienia
                    14,   // wys okienka i fontu  (w % dlugosci promienia)

                    QColor(0, 0, 0, 200),   // kolor daty
                    QColor(0, 0, 0, 150),   // kolor ramki_daty
                    QColor(150, 150, 130, 200),   // kolor tla_daty
                    QColor(0, 0, 0, 150) ,  // kolor cienia
                    5      // uskok cienia
                }
                );

    //-------------------------------------------------
    cyferblat.push_back(
                Tdane_cyferblatu  {
                    "delft-porzelan",
                    ":/new/prefix1/content/delft-porzelan.png",
                    "0",
                    "0",
                    "2",
                    ":/new/prefix1/content/center.png",

                    0,     0.1,  0.06 ,  0.7, // os godzinowej, skala
                    QColor(0, 0, 0),   // kolor wskaz godzinowej

                    0.0,    0.1,   0.05,  0.85,// os minutowej
                    QColor(20, 10, 10, 250),   // kolor wskaz minutowej

                    0,    0.15,   0.02,  1,  // os sekundnika
                    QColor(250, 10, 10),   // kolor wskaz sekundnika

                    180,  181,  // pozycja srodka tarczy
                    0.8,  0.8,   // skalowanie zaślepki
                    145,  // promien_ tarczy
                    0, 0,  // translacja do odrebnej osi sekundnika (x,y w % promienia)

                    QPoint(80, 100),    // pozycja AM_PM ( % z dlug promienia)
                    QColor(80, 80, 80),   // kolor AM_PM
                    QPoint(-80, 100),    // pozycja dnia tygodnia ( % z dlug promienia)
                    QColor(54, 50, 37),   // kolor dnia tygodnia

                    QPoint(70, -100),    // pozycja datownika % z dlug promienia
                    30,   // szer okienka w % dlugosci promienia
                    10,   // wys okienka i fontu  (w % dlugosci promienia)

                    QColor(0, 0, 30, 200),   // kolor daty
                    QColor(0, 0, 0, 150),   // kolor ramki_daty
                    QColor(214, 198, 146, 250),   // kolor tla_daty
                    QColor(0, 0, 0, 100) ,  // kolor cienia
                    //                       QColor(255, 255, 255) ,  // kolor cienia
                    5      // uskok cienia
                }
                );

    //-------------------------------------------------
    cyferblat.push_back(
                Tdane_cyferblatu  {
                    "Blue clock",
                    ":/new/prefix1/content/clock-blue.png",
                    "0",
                    "0",
                    "2",
                    ":/new/prefix1/content/center.png",

                    0,     0.1,  0.03 ,  0.6, // os godzinowej, skala
                    QColor(200, 200, 200),   // kolor wskaz godzinowej

                    0.0,    0.1,   0.04,  0.75,// os minutowej
                    QColor(200, 200, 200, 250),   // kolor wskaz minutowej

                    0,    0.15,   0.01,  0.90,  // os sekundnika
                    QColor(150, 150, 230),   // kolor wskaz sekundnika

                    154, 154,  // pozycja srodka tarczy
                    0.8,  0.8,   // skalowanie zaślepki
                    140,  // promien_ tarczy
                    0, 0,  // translacja do odrebnej osi sekundnika (x,y w % promienia)

                    QPoint(0, -30),    // pozycja AM_PM ( % z dlug promienia)
                    QColor(200, 200, 200),   // kolor AM_PM
                    QPoint(0, 45),    // pozycja dnia tygodnia ( % z dlug promienia)
                    QColor(200, 200, 200, 150),   // kolor dnia tygodnia

                    QPoint(50, -1),    // pozycja datownika % z dlug promienia
                    30,   // szer okienka w % dlugosci promienia
                    10,   // wys okienka i fontu  (w % dlugosci promienia)

                    QColor(200, 200, 200, 200),   // kolor daty
                    QColor(0, 0, 0, 50),   // kolor ramki_daty
                    QColor(0, 0, 0, 10),   // kolor tla_daty
                    QColor(0, 0, 0, 100) ,  // kolor cienia
                    //                       QColor(255, 255, 255) ,  // kolor cienia
                    5      // uskok cienia
                }
                );

    //-------------------------------------------------
    cyferblat.push_back(
                Tdane_cyferblatu  {
                    "Gold classic",
                    ":/new/prefix1/content/gold_KA.png",
                    "0",
                    "0",
                    "2",
                    ":/new/prefix1/content/center.png",

                    0,     0.1,  0.03 ,  0.6, // os godzinowej, skala
                    QColor(0, 0, 0),   // kolor wskaz godzinowej

                    0.0,    0.1,   0.02,  0.75,// os minutowej
                    QColor(20, 10, 10, 250),   // kolor wskaz minutowej

                    0,    0.05,  0.02,  0.9,  // os sekundnika
                    QColor(250, 10, 10),   // kolor wskaz sekundnika

                    125, 123,  // pozycja srodka tarczy
                    0.2, 0.2, //0.8,  0.8,   // skalowanie zaślepki
                    110,  // promien_ tarczy
                    0, 0,  // translacja do odrebnej osi sekundnika (x,y w % promienia)

                    QPoint(0, -30),    // pozycja AM_PM ( % z dlug promienia)
                    QColor(80, 80, 80),   // kolor AM_PM
                    QPoint(0, 32),    // pozycja dnia tygodnia ( % z dlug promienia)
                    QColor(54, 50, 37),   // kolor dnia tygodnia

                    QPoint(-19, 58),    // pozycja datownika % z dlug promienia
                    40,   // szer okienka w % dlugosci promienia
                    12,   // wys okienka i fontu  (w % dlugosci promienia)

                    QColor(0, 0, 30, 200),   // kolor daty
                    QColor(0, 0, 0, 10),   // kolor ramki_daty
                    QColor(214, 198, 146, 180),   // kolor tla_daty
                    QColor(0, 0, 0, 100) ,  // kolor cienia
                    //                       QColor(255, 255, 255) ,  // kolor cienia
                    5      // uskok cienia
                }
                );

    cyferblat.push_back(
                Tdane_cyferblatu  {
                    "Silver classic",
                    ":/new/prefix1/content/silver.png",
                    "0",
                    "0",
                    "2",
                    ":/new/prefix1/content/center.png",

                    0,     0.1,  0.04 ,  0.7, // os godzinowej, skala
                    QColor(0, 0, 0),   // kolor wskaz godzinowej

                    0.0,    0.1,   0.03,  0.88,// os minutowej
                    QColor(20, 10, 10, 250),   // kolor wskaz minutowej

                    0,    0.15,   0.01,  1,  // os sekundnika
                    QColor(250, 10, 10),   // kolor wskaz sekundnika

                    125, 125,  // pozycja srodka tarczy
                    0.8,  0.8,   // skalowanie zaślepki
                    105,  // promien_ tarczy
                    0, 0,  // translacja do odrebnej osi sekundnika (x,y w % promienia)

                    QPoint(0, -30),    // pozycja AM_PM ( % z dlug promienia)
                    QColor(80, 80, 80),   // kolor AM_PM
                    QPoint(0, 40),    // pozycja dnia tygodnia ( % z dlug promienia)
                    QColor(54, 50, 37),   // kolor dnia tygodnia

                    QPoint(-19, 65),    // pozycja datownika % z dlug promienia
                    38,   // szer okienka w % dlugosci promienia
                    13,   // wys okienka i fontu  (w % dlugosci promienia)

                    QColor(0, 0, 30, 200),                     // kolor daty
                    QColor(0, 0, 0, 00),                        // kolor ramki_daty
                    QColor(150, 150, 150, 50),              // kolor tla_daty
                    QColor(0, 0, 0, 100) ,                      // kolor cienia
                    5      // uskok cienia
                }
                );


    cyferblat.push_back(
                Tdane_cyferblatu  {
                    "Roemische_mit_Glas",
                    ":/new/prefix1/content/roemische-Zahlen-mit-Glas-und-Luenette.png",
                    ":/new/prefix1/content/barok1_godzinowa_bevel_zloty_shining.png",
                    ":/new/prefix1/content/barok1_min_bev10.png",
                    "0",
                    ":/new/prefix1/content/center.png",

                    0,     0.1,  0.035 ,  0.6, // os godzinowej, skala
                    QColor(0, 0, 0),   // kolor wskaz godzinowej

                    0.0,    0.1,   0.03,  0.75,// os minutowej
                    QColor(20, 10, 10, 250),   // kolor wskaz minutowej

                    0,    0.15,   0.005,  0.85,  // os sekundnika
                    QColor(250, 10, 10),   // kolor wskaz sekundnika

                    150, 150,  // pozycja srodka tarczy
                    0.8,  0.8,   // skalowanie zaślepki
                    145,  // promien_ tarczy
                    0, 0,  // translacja do odrebnej osi sekundnika (x,y w % promienia)

                    QPoint(0, -30),    // pozycja AM_PM ( % z dlug promienia)
                    QColor(80, 80, 80),   // kolor AM_PM
                    QPoint(-70, 95),    // pozycja dnia tygodnia ( % z dlug promienia)
                    QColor(54, 50, 37),   // kolor dnia tygodnia

                    QPoint(-15, 35),    // pozycja datownika % z dlug promienia
                    30,   // szer okienka w % dlugosci promienia
                    10,   // wys okienka i fontu  (w % dlugosci promienia)

                    QColor(0, 0, 30, 200),   // kolor daty
                    QColor(0, 0, 0, 10),   // kolor ramki_daty
                    QColor(214, 198, 146, 250),   // kolor tla_daty
                    QColor(0, 0, 0, 60) ,  // kolor cienia
                    //                       QColor(255, 255, 255) ,  // kolor cienia
                    5      // uskok cienia
                }
                );

    //-------------------------------------------------
    cyferblat.push_back(
                Tdane_cyferblatu  {
                    "clock-europe-africa",
                    ":/new/prefix1/content/clock-europe-africa.png",
                    "0",
                    "0",
                    "2",
                    ":/new/prefix1/content/center.png",

                    0,     0.1,  0.05 ,  0.7, // os godzinowej, skala
                    QColor(110, 110, 250),   // kolor wskaz godzinowej

                    0.0,    0.1,   0.04,  0.9,// os minutowej
                    QColor(250, 250, 250, 190),   // kolor wskaz minutowej

                    0,    0.15,   0.005,  1,  // os sekundnika
                    QColor(250, 10, 10),   // kolor wskaz sekundnika

                    317, 189,  // pozycja srodka tarczy
                    1,  1,   // skalowanie zaślepki
                    120,  // promien_ tarczy
                    0, 0,  // translacja do odrebnej osi sekundnika (x,y w % promienia)

                    QPoint(0, -30),    // pozycja AM_PM ( % z dlug promienia)
                    QColor(200, 200, 200),   // kolor AM_PM
                    QPoint(0, 50),    // pozycja dnia tygodnia ( % z dlug promienia)
                    QColor(200, 200, 200, 150),   // kolor dnia tygodnia

                    QPoint(-15, 65),    // pozycja datownika % z dlug promienia
                    30,   // szer okienka w % dlugosci promienia
                    12,   // wys okienka i fontu  (w % dlugosci promienia)

                    QColor(200, 200, 200, 200),   // kolor daty
                    QColor(0, 0, 0, 50),   // kolor ramki_daty
                    QColor(0, 0, 0, 10),   // kolor tla_daty
                    QColor(50, 50, 150, 100) ,  // kolor cienia
                    //                       QColor(255, 255, 255) ,  // kolor cienia
                    2      // uskok cienia
                }
                );


    cyferblat.push_back(
                Tdane_cyferblatu  {
                    "Swiss Rapport",
                    ":/new/prefix1/content/Swiss Quartz Pocket Watch.png",
                    ":/new/prefix1/content/roman_hours_grey.png",
                    ":/new/prefix1/content/roman_minut.png",
                    "0",
                    ":/new/prefix1/content/center.png",

                    0,     0.1,  0.04 ,  0.6, // os godzinowej, skala
                    QColor(0, 0, 0),   // kolor wskaz godzinowej

                    0.0,    0.1,   0.04,  0.75,// os minutowej
                    QColor(20, 10, 10, 250),   // kolor wskaz minutowej

                    0,    0.15,   0.005,  1,  // os sekundnika
                    QColor(250, 10, 10),   // kolor wskaz sekundnika

                    150, 240,  // pozycja srodka tarczy
                    1,  1,   // skalowanie zaślepki
                    120,  // promien_ tarczy
                    0, 0,  // translacja do odrebnej osi sekundnika (x,y w % promienia)

                    QPoint(0, 18),    // pozycja AM_PM ( % z dlug promienia)
                    QColor(180, 180, 180),   // kolor AM_PM
                    QPoint(0, 52),    // pozycja dnia tygodnia ( % z dlug promienia)
                    QColor(220, 180, 0, 200),   // kolor dnia tygodnia

                    QPoint(20, 0),    // pozycja datownika % z dlug promienia
                    30,   // szer okienka w % dlugosci promienia
                    10,   // wys okienka i fontu  (w % dlugosci promienia)

                    QColor(220, 180, 0, 200),   // kolor daty
                    QColor(0, 0, 0, 150),   // kolor ramki_daty
                    QColor(50, 0, 0, 250),   // kolor tla_daty
                    QColor(50, 50, 50, 100) ,  // kolor cienia
                    //                       QColor(255, 255, 255) ,  // kolor cienia
                    4     // uskok cienia
                }
                );


    cyferblat.push_back(
                Tdane_cyferblatu  {
                    "Quarz-black",
                    ":/new/prefix1/content/quarz_black.png",
                    ":/new/prefix1/content/roman_hours.png",
                    ":/new/prefix1/content/roman_minut.png",
                    "0",
                    ":/new/prefix1/content/center.png",

                    0,     0.1,  0.05 ,  0.6, // os godzinowej, skala
                    QColor(190, 190, 190),   // kolor wskaz godzinowej

                    0.0,    0.1,   0.04,  0.75,// os minutowej
                    QColor(190, 190, 190),   // kolor wskaz minutowej

                    0,    0.15,   0.005,  1,  // os sekundnika
                    QColor(250, 10, 10),   // kolor wskaz sekundnika

                    150, 150,  // pozycja srodka tarczy
                    1,  1,   // skalowanie zaślepki
                    120,  // promien_ tarczy
                    0, 0,  // translacja do odrebnej osi sekundnika (x,y w % promienia)

                    QPoint(0, 15),    // pozycja AM_PM ( % z dlug promienia)
                    QColor(80, 80, 80),   // kolor AM_PM
                    QPoint(0, 33),    // pozycja dnia tygodnia ( % z dlug promienia)
                    QColor(220, 180, 0, 200),   // kolor dnia tygodnia

                    QPoint(20, 0),    // pozycja datownika % z dlug promienia
                    30,   // szer okienka w % dlugosci promienia
                    10,   // wys okienka i fontu  (w % dlugosci promienia)

                    QColor(220, 180, 0, 200),   // kolor daty
                    QColor(0, 0, 0, 250),   // kolor ramki_daty
                    QColor(50, 0, 0, 250),   // kolor tla_daty
                    QColor(150, 150, 150, 100) ,  // kolor cienia
                    //                       QColor(255, 255, 255) ,  // kolor cienia
                    4     // uskok cienia
                }
                );



    cyferblat.push_back(
                Tdane_cyferblatu  {
                    "Wooden clock",
                    ":/new/prefix1/content/wooden-clock.png",
                    ":/new/prefix1/content/roman_hours_gold.png",
                    ":/new/prefix1/content/roman_min_gold.png",
                    "0",
                    ":/new/prefix1/content/center.png",

                    0,     0.1,  0.06 ,  0.7, // os godzinowej, skala
                    QColor(0, 0, 0),   // kolor wskaz godzinowej

                    0.0,    0.1,   0.04,  0.95,// os minutowej
                    QColor(20, 10, 10, 250),   // kolor wskaz minutowej

                    0,    0.15,   0.01,  1.0,  // os sekundnika
                    QColor(250, 250, 0),   // kolor wskaz sekundnika

                    162, 158,  // pozycja srodka tarczy
                    1,  1,   // skalowanie zaślepki
                    110,  // promien_ tarczy
                    0, 0,  // translacja do odrebnej osi sekundnika (x,y w % promienia)

                    QPoint(0, 25),    // pozycja AM_PM ( % z dlug promienia)
                    QColor(220, 180, 0),   // kolor AM_PM
                    QPoint(0, 55),    // pozycja dnia tygodnia ( % z dlug promienia)
                    QColor(220, 180, 0, 200),   // kolor dnia tygodnia

                    QPoint(30, 0),    // pozycja datownika % z dlug promienia
                    30,   // szer okienka w % dlugosci promienia
                    12,   // wys okienka i fontu  (w % dlugosci promienia)

                    QColor(220, 180, 0, 200),   // kolor daty
                    QColor(0, 0, 0, 250),   // kolor ramki_daty
                    QColor(50, 0, 0, 250),   // kolor tla_daty
                    QColor(0, 0, 0, 120) ,  // kolor cienia
                    //                       QColor(255, 255, 255) ,  // kolor cienia
                    4     // uskok cienia
                }
                );

    cyferblat.push_back(
                Tdane_cyferblatu  {
                    "Rolex-datejust",
                    ":/new/prefix1/content/zifferblatt-uhr-rolex.png",
                    "0",
                    "0",
                    "0",
                    ":/new/prefix1/content/center.png",

                    0,     0.1,  0.06 ,  0.55, // os godzinowej, skala
                    QColor(50, 20, 0),   // kolor wskaz godzinowej

                    0.0,    0.1,   0.05,  0.75,// os minutowej
                    QColor(40, 20, 10, 250),   // kolor wskaz minutowej

                    0,    0.15,   0.01,  0.9,  // os sekundnika
                    QColor(250, 10, 10),   // kolor wskaz sekundnika

                    192, 225,  // pozycja srodka tarczy
                    1,  1,   // skalowanie zaślepki
                    140,  // promien_ tarczy
                    0, 0,  // translacja do odrebnej osi sekundnika (x,y w % promienia)

                    QPoint(0, 25),    // pozycja AM_PM ( % z dlug promienia)
                    QColor(250,220,0,250),   // kolor AM_PM
                    QPoint(0, 50),    // pozycja dnia tygodnia ( % z dlug promienia)
                    QColor (250,220,0,250),   // kolor dnia tygodnia

                    QPoint(50, 0),    // pozycja datownika % z dlug promienia
                    30,   // szer okienka w % dlugosci promienia
                    12,   // wys okienka i fontu  (w % dlugosci promienia)

                    QColor(250,250,0,250),   // kolor daty
                    QColor(0, 0, 0, 50),   // kolor ramki_daty
                    QColor(60, 30, 0, 250),   // kolor tla_daty
                    QColor(0, 0, 0, 150) ,  // kolor cienia

                    4     // uskok cienia
                }
                );


    cyferblat.push_back(
                Tdane_cyferblatu  {
                    "Seiko-grey",
                    ":/new/prefix1/content/seiko_grey.png",
                    "0",
                    "0",
                    "2",
                    ":/new/prefix1/content/center.png",

                    0,     0.23, // mimosrod godzinowej
                    0.03,   // grubosc w skali promienia tarczy
                    0.7,// wysokosc  w skali promienia tarczy
                    QColor(255, 0, 0, 255),

                    0.0,   0.1,   0.03,  0.85,// os minutowej
                    QColor(0, 0, 50),

                    0,    0.15,   0.005,  1,  // os sekundnika
                    QColor(250, 10, 10),   // kolor wskaz sekundnika

                    175, 177,  // pozycja srodka tarczy
                    1,  1,   // skalowanie zaślepki
                    150,  // promien_ tarczy
                    0, 0,  // translacja do odrebnej osi sekundnika (x,y w % promienia)

                    QPoint(0, 35),    // pozycja AM_PM ( % z dlug promienia)
                    QColor(80, 80, 80),   // kolor AM_PM
                    QPoint(0, -45),    // pozycja dnia tygodnia ( % z dlug promienia)
                    QColor(10, 10, 0, 250),   // kolor dnia tygodnia

                    QPoint(-15, -20),    // pozycja datownika % z dlug promienia
                    30,   // szer okienka w % dlugosci promienia
                    10,   // wys okienka i fontu  (w % dlugosci promienia)

                    QColor(10,10,0,250),   // kolor daty
                    QColor(0, 0, 0, 10),   // kolor ramki_daty
                    QColor(50, 0, 0, 20),   // kolor tla_daty
                    QColor(50, 50, 50, 100) ,  // kolor cienia
                    //                       QColor(255, 255, 255) ,  // kolor cienia
                    4     // uskok cienia
                }
                );

    cyferblat.push_back(
                Tdane_cyferblatu  {
                    "Clock_face_India",
                    ":/new/prefix1/content/Clock_face_India.png",
                    "0",
                    "0",
                    "0",
                    ":/new/prefix1/content/center.png",

                    0,     0.23, // mimosrod godzinowej
                    0.03,   // grubosc w skali promienia tarczy
                    0.7,// wysokosc  w skali promienia tarczy
                    QColor(255, 0, 0, 255),

                    0.0,   0.1,   0.03,  0.85,// os minutowej
                    QColor(0, 100, 250),

                    0,    0.15,   0.01,  1,  // os sekundnika
                    QColor(250, 10, 10),   // kolor wskaz sekundnika

                    175, 173,  // pozycja srodka tarczy
                    1,  1,   // skalowanie zaślepki
                    150,  // promien_ tarczy
                    0, 0,  // translacja do odrebnej osi sekundnika (x,y w % promienia)


                    QPoint(0, 25),    // pozycja AM_PM ( % z dlug promienia)
                    QColor(250,250,190),   // kolor AM_PM
                    QPoint(0, 50),    // pozycja dnia tygodnia ( % z dlug promienia)
                    QColor(250,250,190, 250),   // kolor dnia tygodnia

                    QPoint(30, 0),    // pozycja datownika % z dlug promienia
                    30,   // szer okienka w % dlugosci promienia
                    12,   // wys okienka i fontu  (w % dlugosci promienia)

                    QColor(250,250,190, 250),   // kolor daty
                    QColor(0, 0, 0, 50),   // kolor ramki_daty
                    QColor(50, 0, 0, 50),   // kolor tla_daty
                    QColor(50, 50, 50, 100) ,  // kolor cienia
                    //                       QColor(255, 255, 255) ,  // kolor cienia
                    4     // uskok cienia
                }
                );

    cyferblat.push_back(
                Tdane_cyferblatu  {
                    "Clock_face_Decorative",
                    ":/new/prefix1/content/Clock_face_Decorative.png",
                    "0","0",
                    "0",
                    ":/new/prefix1/content/center.png",

                    0,     0.23, // mimosrod godzinowej
                    0.05,   // grubosc w skali promienia tarczy
                    0.7,// wysokosc  w skali promienia tarczy
                    QColor(100, 100, 250),

                    0.0,   0.1,   0.03,  0.85,// os minutowej
                    QColor(200, 200, 250),

                    0,    0.15,   0.015,  0.9,  // os sekundnika
                    QColor(250, 10, 10),   // kolor wskaz sekundnika

                    115, 112,  // pozycja srodka tarczy
                    1,  1,   // skalowanie zaślepki
                    95,  // promien_ tarczy
                    0, 0,  // translacja do odrebnej osi sekundnika (x,y w % promienia)

                    QPoint(0, 25),    // pozycja AM_PM ( % z dlug promienia)
                    QColor(180, 180, 240),   // kolor AM_PM
                    QPoint(0, 50),    // pozycja dnia tygodnia ( % z dlug promienia)
                    QColor(200, 200, 255),   // kolor dnia tygodnia

                    QPoint(37, 1),    // pozycja datownika % z dlug promienia
                    30,   // szer okienka w % dlugosci promienia
                    12,   // wys okienka i fontu  (w % dlugosci promienia)

                    QColor(200,190, 255),   // kolor daty
                    QColor(0, 0, 0, 50),   // kolor ramki_daty
                    QColor(50, 0, 0, 50),   // kolor tla_daty
                    QColor(100, 100, 100, 100) ,  // kolor cienia
                    //                       QColor(255, 255, 255) ,  // kolor cienia
                    4     // uskok cienia
                }
                );


    cyferblat.push_back(
                Tdane_cyferblatu  {
                    "Clock_face_Modern",
                    ":/new/prefix1/content/Clock_face_Modern.png",
                    "0","0",
                    "0",
                    ":/new/prefix1/content/center.png",

                    0,     0.23, // mimosrod godzinowej
                    0.03,   // grubosc w skali promienia tarczy
                    0.7,// wysokosc  w skali promienia tarczy
                    QColor(255, 0, 0, 255),

                    0.0,   0.1,   0.03,  0.85,// os minutowej
                    QColor(0, 0, 50),

                    0,    0.15,   0.005,  1,  // os sekundnika
                    QColor(250, 10, 10),   // kolor wskaz sekundnika

                    112, 112,  // pozycja srodka tarczy
                    1,  1,   // skalowanie zaślepki
                    95,  // promien_ tarczy
                    0, 0,  // translacja do odrebnej osi sekundnika (x,y w % promienia)

                    QPoint(0, 25),    // pozycja AM_PM ( % z dlug promienia)
                    QColor(80, 80, 80),   // kolor AM_PM
                    QPoint(0, -40),    // pozycja dnia tygodnia ( % z dlug promienia)
                    QColor(10, 10, 0, 250),   // kolor dnia tygodnia

                    QPoint(30, 0),    // pozycja datownika % z dlug promienia
                    36,   // szer okienka w % dlugosci promienia
                    12,   // wys okienka i fontu  (w % dlugosci promienia)

                    QColor(10,10,0,250),   // kolor daty
                    QColor(0, 0, 0, 50),   // kolor ramki_daty
                    QColor(50, 0, 0, 50),   // kolor tla_daty
                    QColor(50, 50, 50, 100) ,  // kolor cienia
                    //                       QColor(255, 255, 255) ,  // kolor cienia
                    4     // uskok cienia
                }
                );


    cyferblat.push_back(
                Tdane_cyferblatu  {
                    "Clock_face_Paris2",
                    ":/new/prefix1/content/Clock_face_Paris2.png",
                    "0","0",
                    "0",
                    ":/new/prefix1/content/center.png",

                    0,     0.23, // mimosrod godzinowej
                    0.06,   // grubosc w skali promienia tarczy
                    0.65,// wysokosc  w skali promienia tarczy
                    QColor(255, 0, 0, 255),

                    0.0,   0.1,   0.04,  0.85,// os minutowej
                    QColor(120, 50, 00),

                    0,    0.15,   0.005,  1,  // os sekundnika
                    QColor(250, 10, 10),   // kolor wskaz sekundnika

                    161, 162,  // pozycja srodka tarczy
                    1,  1,   // skalowanie zaślepki
                    156,  // promien_ tarczy
                    0, 0,  // translacja do odrebnej osi sekundnika (x,y w % promienia)

                    QPoint(70, -90),    // pozycja AM_PM ( % z dlug promienia)
                    QColor(80, 80, 80),   // kolor AM_PM
                    QPoint(-70, 90),    // pozycja dnia tygodnia ( % z dlug promienia)
                    QColor(200, 200, 100, 250),   // kolor dnia tygodnia

                    QPoint(70, 75),    // pozycja datownika % z dlug promienia
                    30,   // szer okienka w % dlugosci promienia
                    10,   // wys okienka i fontu  (w % dlugosci promienia)

                    QColor(230,10,0,250),   // kolor daty
                    QColor(0, 0, 0, 50),   // kolor ramki_daty
                    QColor(50, 0, 0, 250),   // kolor tla_daty
                    QColor(50, 50, 50, 100) ,  // kolor cienia
                    //                       QColor(255, 255, 255) ,  // kolor cienia
                    4     // uskok cienia
                }
                );



    cyferblat.push_back(
                Tdane_cyferblatu  {
                    "Neon-modern",
                    ":/new/prefix1/content/neon.png",
                    ":/new/prefix1/content/neon_wskazowka.png",
                    ":/new/prefix1/content/neon_wskazowka.png",
                    ":/new/prefix1/content/neon_sekundnik.png",
                    ":/new/prefix1/content/center.png",

                    0,     0.1, // mimosrod godzinowej
                    0.04,   // grubosc w skali promienia tarczy
                    0.65,// wysokosc  w skali promienia tarczy
                    QColor(0, 220, 255),

                    0.0,   0.1,   0.04,  0.8,// os minutowej
                    QColor(0, 220, 255),

                    0,    0.19,   0.005,  0.9,  // os sekundnika
                    QColor(250, 10, 10),   // kolor wskaz sekundnika

                    170, 174,  // pozycja srodka tarczy
                    0.3,  0.3,   // skalowanie zaślepki
                    140,  // promien_ tarczy
                    0, 0,  // translacja do odrebnej osi sekundnika (x,y w % promienia)

                    QPoint(0, 45),    // pozycja AM_PM ( % z dlug promienia)
                    QColor(80, 80, 80),   // kolor AM_PM
                    QPoint(0, -50),    // pozycja dnia tygodnia ( % z dlug promienia)
                    QColor(0, 200, 250, 250),   // kolor dnia tygodnia

                    QPoint(35, 0),    // pozycja datownika % z dlug promienia
                    30,   // szer okienka w % dlugosci promienia
                    10,   // wys okienka i fontu  (w % dlugosci promienia)

                    QColor(0,200,250,250),   // kolor daty
                    QColor(0, 0, 0, 50),   // kolor ramki_daty
                    QColor(50, 0, 0, 50),   // kolor tla_daty
                    QColor(50, 50, 50, 100) ,  // kolor cienia
                    //                       QColor(255, 255, 255) ,  // kolor cienia
                    4     // uskok cienia
                }
                );

    cyferblat.push_back(
                Tdane_cyferblatu  {
                    "Galaxy",
                    ":/new/prefix1/content/galaxy.png",
                    "0","0",
                    "0",
                    ":/new/prefix1/content/center.png",

                    0,     0.15,  0.04 ,  0.7, // os godzinowej, skala
                    QColor(190, 255, 0),   // kolor wskaz godzinowej
                    0.0,    0.15,   0.03,  0.95,// os minutowej
                    QColor(255, 255, 0),   // kolor wskaz minutowej

                    0,    0.19,   0.005,  1,  // os sekundnika
                    QColor(250, 10, 10),   // kolor wskaz sekundnika

                    175, 175,  // pozycja srodka tarczy
                    0.6,  0.6,   // skalowanie zaślepki
                    150,  // promien_ tarczy
                    0, 0,  // translacja do odrebnej osi sekundnika (x,y w % promienia)

                    QPoint(0, 75),    // pozycja AM_PM ( % z dlug promienia)
                    QColor(80, 80, 80),   // kolor AM_PM
                    QPoint(0, -60),    // pozycja dnia tygodnia ( % z dlug promienia)
                    //                    QColor(0, 200, 250, 250),   // kolor dnia tygodnia
                    QColor(200, 200, 100, 150),   // kolor dnia tygodnia

                    QPoint(35, 0),    // pozycja datownika % z dlug promienia
                    40,   // szer okienka w % dlugosci promienia
                    14,   // wys okienka i fontu  (w % dlugosci promienia)

                    QColor(0,250,250,250),   // kolor daty
                    QColor(0, 0, 0, 50),   // kolor ramki_daty
                    QColor(50, 0, 0, 250),   // kolor tla_daty
                    QColor(50, 50, 50, 100) ,  // kolor cienia
                    //                       QColor(255, 255, 255) ,  // kolor cienia
                    4     // uskok cienia
                }
                );


    cyferblat.push_back(
                Tdane_cyferblatu  {
                    "Ocean_nordic",
                    ":/new/prefix1/content/ocean_nordic.png",
                    "0","0",
                    "0",
                    ":/new/prefix1/content/center.png",

                    0,     0.15,  0.04 ,  0.7, // os godzinowej, skala
                    QColor(190, 255, 0),   // kolor wskaz godzinowej
                    0.0,    0.15,   0.03,  0.95,// os minutowej
                    QColor(255, 255, 0),   // kolor wskaz minutowej

                    0,    0.19,   0.005,  1,  // os sekundnika
                    QColor(250, 10, 10),   // kolor wskaz sekundnika

                    150, 152,  // pozycja srodka tarczy
                    0.3,  0.3,   // skalowanie zaślepki
                    135,  // promien_ tarczy
                    0, 0,  // translacja do odrebnej osi sekundnika (x,y w % promienia)

                    QPoint(0, 45),    // pozycja AM_PM ( % z dlug promienia)
                    QColor(200, 200, 250),   // kolor AM_PM
                    QPoint(0, -50),    // pozycja dnia tygodnia ( % z dlug promienia)
                    QColor(200, 200, 250, 250),   // kolor dnia tygodnia

                    QPoint(35, 0),    // pozycja datownika % z dlug promienia
                    40,   // szer okienka w % dlugosci promienia
                    12,   // wys okienka i fontu  (w % dlugosci promienia)

                    QColor(100,250,100),   // kolor daty
                    QColor(0, 0, 0, 10),   // kolor ramki_daty
                    QColor(0, 40, 30, 150),   // kolor tla_daty
                    QColor(50, 50, 50, 100) ,  // kolor cienia
                    //                       QColor(255, 255, 255) ,  // kolor cienia
                    4     // uskok cienia
                }
                );

    cyferblat.push_back(
                Tdane_cyferblatu  {
                    "mechanic",
                    ":/new/prefix1/content/mechanic.png",
                    "0","0",
                    "0",
                    ":/new/prefix1/content/center.png",

                    0,     0.15,  0.07 ,  0.7, // os godzinowej, skala
                    QColor(190, 255, 0),   // kolor wskaz godzinowej
                    0.0,    0.15,   0.05,  0.95,// os minutowej
                    QColor(255, 255, 0),   // kolor wskaz minutowej

                    0,    0.1,
                    0.045,  // grubosc sekundnika
                    1.3,  // dlugosc sekundnika (w % promienia tarczy)
                    QColor(250, 10, 10),   // kolor wskaz sekundnika

                    177, 108,  // pozycja srodka tarczy
                    0.3,  0.3,   // skalowanie zaślepki
                    60,  // promien_ tarczy

                    0, 113,  // translacja do odrebnej osi sekundnika (x,y w % promienia)

                    QPoint(0, 150),    // pozycja AM_PM ( % z dlug promienia)
                    QColor(80, 80, 80),   // kolor AM_PM
                    QPoint(0, 260),    // pozycja dnia tygodnia ( % z dlug promienia)
                    QColor(250, 100, 100),   // kolor dnia tygodnia

                    QPoint(140, 110),    // pozycja datownika % z dlug promienia
                    80,   // szer okienka w % dlugosci promienia
                    25,   // wys okienka i fontu  (w % dlugosci promienia)

                    QColor(250, 50, 50),   // kolor daty
                    QColor(100, 100, 100),   // kolor ramki_daty
                    QColor(0, 0, 0, 150),   // kolor tla_daty
                    QColor(50, 50, 50, 100) ,  // kolor cienia
                    6     // uskok cienia
                }
                );

    cyferblat.push_back(
                Tdane_cyferblatu  {
                    "Seamaster",
                    ":/new/prefix1/content/Seamaster.png",
                    ":/new/prefix1/content/Omega-hours.png",
                    ":/new/prefix1/content/Omega-minutes.png",
                    ":/new/prefix1/content/Omega-seconds.png",
                    //  ":/new/prefix1/content/neon_sekundnik.png",
                    ":/new/prefix1/content/center.png",

                    0,     0.20,  0.07 ,  0.65, // os godzinowej, skala
                    QColor(190, 255, 0),   // kolor wskaz godzinowej
                    0.0,    0.15,   0.05,  0.9,// os minutowej
                    QColor(255, 255, 0),   // kolor wskaz minutowej

                    0,    0.1,
                    0.025,  // grubosc sekundnika
                    1,  // dlugosc sekundnika (w % promienia tarczy)
                    QColor(250, 10, 10),   // kolor wskaz sekundnika

                    160, 279,  // pozycja srodka tarczy
                    0.3,  0.3,   // skalowanie zaślepki
                    125,  // promien_ tarczy

                    0, 0,  // translacja do odrebnej osi sekundnika (x,y w % promienia)

                    QPoint(0, 68),    // pozycja AM_PM ( % z dlug promienia)
                    QColor(230, 230, 230),   // kolor AM_PM
                    QPoint(0, -120),    // pozycja dnia tygodnia ( % z dlug promienia)
                    QColor(100,250,100, 250),   // kolor dnia tygodnia

                    QPoint(-20, 112),    // pozycja datownika % z dlug promienia
                    40,   // szer okienka w % dlugosci promienia
                    12,   // wys okienka i fontu  (w % dlugosci promienia)

                    QColor(100,250,100),   // kolor daty
                    QColor(0, 0, 0, 50),   // kolor ramki_daty
                    QColor(0, 40, 30, 150),   // kolor tla_daty
                    QColor(40, 40, 40, 150) ,  // kolor cienia
                    //                       QColor(255, 255, 255) ,  // kolor cienia
                    4     // uskok cienia
                }
                );


    cyferblat.push_back(
                Tdane_cyferblatu  {
                    "Morgan",
                    ":/new/prefix1/content/Morgan.png",
                    ":/new/prefix1/content/morgan_hours.png",
                    ":/new/prefix1/content/morgan_minutes.png",
                    ":/new/prefix1/content/Omega-seconds.png",
                    //  ":/new/prefix1/content/neon_sekundnik.png",
                    ":/new/prefix1/content/center.png",

                    0,     0.10,  0.07 ,  0.65, // os godzinowej, skala
                    QColor(190, 255, 0),   // kolor wskaz godzinowej
                    0.0,    0.19,   0.05,  0.95,// os minutowej
                    QColor(255, 255, 0),   // kolor wskaz minutowej

                    0,    0.24,
                    0.02,  // grubosc sekundnika
                    0.25,  // dlugosc sekundnika (w % promienia tarczy)
                    QColor(250, 10, 10),   // kolor wskaz sekundnika

                    162, 276,  // pozycja srodka tarczy
                    0.3,  0.3,   // skalowanie zaślepki
                    140,  // promien_ tarczy

                    0, 68,  // translacja do odrebnej osi sekundnika (x,y w % promienia)

                    QPoint(0, 120),    // pozycja AM_PM ( % z dlug promienia)
                    QColor(200, 200, 50),   // kolor AM_PM
                    QPoint(0, -25),    // pozycja dnia tygodnia ( % z dlug promienia)
                    QColor(200, 200, 50, 250),   // kolor dnia tygodnia

                    QPoint(35, 14),    // pozycja datownika % z dlug promienia
                    40,   // szer okienka w % dlugosci promienia
                    12,   // wys okienka i fontu  (w % dlugosci promienia)

                    QColor(100,200,100),   // kolor daty
                    QColor(110, 110, 110, 50),   // kolor ramki_daty
                    QColor(60, 40, 20, 50),   // kolor tla_daty
                    QColor(0, 0, 0, 180) ,  // kolor cienia
                    //                       QColor(255, 255, 255) ,  // kolor cienia
                    4     // uskok cienia
                }
                );

    cyferblat.push_back(
                Tdane_cyferblatu  {
                    "gear-wall-clocks",
                    ":/new/prefix1/content/gear-wall-clocks.png",
                    "0","0",
                    "0",
                    //  ":/new/prefix1/content/neon_sekundnik.png",
                    ":/new/prefix1/content/center.png",

                    0,     0.15,  0.07 ,  0.7, // os godzinowej, skala
                    QColor(190, 255, 0),   // kolor wskaz godzinowej
                    0.0,    0.15,   0.05,  0.95,// os minutowej
                    QColor(255, 255, 0),   // kolor wskaz minutowej

                    0,    0.15,
                    0.02,  // grubosc sekundnika
                    1,  // dlugosc sekundnika (w % promienia tarczy)
                    QColor(250, 10, 10),   // kolor wskaz sekundnika

                    176, 180,  // pozycja srodka tarczy
                    0.3,  0.3,   // skalowanie zaślepki
                    150,  // promien_ tarczy

                    0, 0,  // translacja do odrebnej osi sekundnika (x,y w % promienia)

                    QPoint(0, 20),    // pozycja AM_PM ( % z dlug promienia)
                    QColor(250, 250, 250),   // kolor AM_PM
                    QPoint(0, 50),    // pozycja dnia tygodnia ( % z dlug promienia)
                    QColor(255, 200, 50),   // kolor dnia tygodnia

                    QPoint(35, 14),    // pozycja datownika % z dlug promienia
                    30,   // szer okienka w % dlugosci promienia
                    11,   // wys okienka i fontu  (w % dlugosci promienia)

                    QColor(100,200,100),   // kolor daty
                    QColor(110, 110, 110, 50),   // kolor ramki_daty
                    QColor(60, 40, 20, 150),   // kolor tla_daty
                    QColor(50, 50, 50, 150) ,  // kolor cienia
                    //                       QColor(255, 255, 255) ,  // kolor cienia
                    4     // uskok cienia
                }
                );


    cyferblat.push_back(
                Tdane_cyferblatu  {
                    "IWC_Schaffhausen",
                    ":/new/prefix1/content/iwc_schaffhausen.png",
                    ":/new/prefix1/content/iwc_hours.png",
                    ":/new/prefix1/content/iwc_minutes.png",
                    "0",                      //  ":/new/prefix1/content/neon_sekundnik.png",
                    ":/new/prefix1/content/center.png",

                    0.0001, 0.115,
                    0.01 ,  0.6, // os godzinowej, skala
                    QColor(66, 21, 2),   // kolor wskaz godzinowej

                    -0.0001,    0.06,
                    0.01,  0.9,// os minutowej
                    QColor(66, 21, 2),   // kolor wskaz minutowej

                    0,    0.0,
                    0.01,  // grubosc sekundnika
                    0.20,  // dlugosc sekundnika (w % promienia tarczy)
                    QColor(250, 10, 10),   // kolor wskaz sekundnika

                    167, 293,  // pozycja srodka tarczy
                    0.005,  0.005,   // skalowanie zaślepki
                    150,  // promien_ tarczy

                    -77, -1,  // translacja do odrebnej osi sekundnika (x,y w % promienia)

                    QPoint(0, 150),    // pozycja AM_PM ( % z dlug promienia)
                    QColor(240, 120, 0),   // kolor AM_PM
                    QPoint(0, 125),    // pozycja dnia tygodnia ( % z dlug promienia)
                    QColor(240, 120, 0),   // kolor dnia tygodnia

                    QPoint(-15, 40),    // pozycja datownika % z dlug promienia
                    30,   // szer okienka w % dlugosci promienia
                    12,   // wys okienka i fontu  (w % dlugosci promienia)

                    QColor(200,100,0),   // kolor daty
                    QColor(110, 110, 0, 150),   // kolor ramki_daty
                    QColor(0, 0, 80, 240),   // kolor tla_daty
                    QColor(0, 0, 0, 100) ,  // kolor cienia
                    3     // uskok cienia
                }
                );



    cyferblat.push_back(
                Tdane_cyferblatu  {
                    "Large-Kitchen-Wall-Clock",
                    ":/new/prefix1/content/Large-Kitchen-Wall-Clock.png",
                    ":/new/prefix1/content/rombowe_godzinowa_ciemna.png",
                    ":/new/prefix1/content/rombowe_minutowa_czarna.png",
                    "0",
                    //  ":/new/prefix1/content/neon_sekundnik.png",
                    ":/new/prefix1/content/center.png",

                    0,     0.12,  0.07 ,  0.6, // os godzinowej, skala
                    QColor(190, 255, 0),   // kolor wskaz godzinowej
                    0.0,    0.075,   0.05,  0.80,// os minutowej
                    QColor(255, 255, 0),   // kolor wskaz minutowej

                    0,    0.15,
                    0.01,  // grubosc sekundnika
                    1,  // dlugosc sekundnika (w % promienia tarczy)
                    QColor(250, 10, 10),   // kolor wskaz sekundnika

                    165, 277,  // pozycja srodka tarczy
                    0.5,  0.5,   // skalowanie zaślepki
                    130,  // promien_ tarczy

                    0, 0,  // translacja do odrebnej osi sekundnika (x,y w % promienia)

                    QPoint(0, -118),    // pozycja AM_PM ( % z dlug promienia)
                    QColor(240, 120, 0),   // kolor AM_PM
                    QPoint(80, -100),    // pozycja dnia tygodnia ( % z dlug promienia)

                    QColor(240, 120, 0),   // kolor dnia tygodnia

                    QPoint(85, -50),    // pozycja datownika % z dlug promienia
                    30,   // szer okienka w % dlugosci promienia
                    12,   // wys okienka i fontu  (w % dlugosci promienia)

                    QColor(240,120,0),   // kolor daty
                    QColor(110, 110, 0, 150),   // kolor ramki_daty
                    QColor(60, 0, 0, 240),   // kolor tla_daty
                    QColor(0, 0, 0, 180) ,  // kolor cienia
                    5     // uskok cienia
                }
                );


#endif

    cyferblat.push_back(
                Tdane_cyferblatu  {
                    "rzymski_chrom",
                    ":/new/prefix1/content/rzymski_chrom.png",
                    ":/new/prefix1/content/rombowe_godzinowa_ciemna.png",
                    ":/new/prefix1/content/rombowe_minutowa_czarna.png",
                    "0",
                    //  ":/new/prefix1/content/neon_sekundnik.png",
                    ":/new/prefix1/content/center.png",

                    0,     0.12,  0.007 ,  0.5, // os godzinowej, skala
                    QColor(190, 255, 0),   // kolor wskaz godzinowej
                    0.0,    0.075,   0.005,  0.750,// os minutowej
                    QColor(255, 255, 0),   // kolor wskaz minutowej

                    0,    0.15,
                    0.01,  // grubosc sekundnika
                    0.9,  // dlugosc sekundnika (w % promienia tarczy)
                    QColor(250, 10, 10),   // kolor wskaz sekundnika

                    175, 175,  // pozycja srodka tarczy
                    0.5,  0.5,   // skalowanie zaślepki
                    155,  // promien_ tarczy

                    0, 0,  // translacja do odrebnej osi sekundnika (x,y w % promienia)

                    QPoint(-40, -105),    // pozycja AM_PM ( % z dlug promienia)
                    QColor(240, 120, 0),   // kolor AM_PM
                    QPoint(60, -105),    // pozycja dnia tygodnia ( % z dlug promienia)

                    QColor(240, 110, 0),   // kolor dnia tygodnia

                    QPoint(75, -70),    // pozycja datownika % z dlug promienia
                    30,   // szer okienka w % dlugosci promienia
                    12,   // wys okienka i fontu  (w % dlugosci promienia)

                    QColor(240,120,0),   // kolor daty
                    QColor(110, 110, 0, 150),   // kolor ramki_daty
                    QColor(60, 0, 0, 240),   // kolor tla_daty
                    QColor(0, 0, 0, 120) ,  // kolor cienia
                    5     // uskok cienia
                }
                );

#if 0
    cyferblat.push_back(
                Tdane_cyferblatu  {
                    "metal_chrom",
                    ":/new/prefix1/content/metal_chrom.png",
                    ":/new/prefix1/content/rombowe_godzinowa_ciemna.png",
                    ":/new/prefix1/content/rombowe_minutowa_czarna.png",
                    "0",
                    //  ":/new/prefix1/content/neon_sekundnik.png",
                    ":/new/prefix1/content/center.png",

                    0,     0.12,  0.07 ,  0.7, // os godzinowej, skala
                    QColor(190, 230, 0),   // kolor wskaz godzinowej
                    0.0,    0.075,   0.05,  0.90,// os minutowej
                    QColor(15, 15, 15),   // kolor wskaz minutowej

                    0.0,    0.15,
                    0.01,  // grubosc sekundnika
                    1,  // dlugosc sekundnika (w % promienia tarczy)
                    QColor(250, 10, 10),   // kolor wskaz sekundnika

                    175, 175,  // pozycja srodka tarczy
                    0.5,  0.5,   // skalowanie zaślepki
                    150,  // promien_ tarczy

                    0, 0,  // translacja do odrebnej osi sekundnika (x,y w % promienia)

                    QPoint(-40, -105),    // pozycja AM_PM ( % z dlug promienia)
                    QColor(240, 120, 0),   // kolor AM_PM
                    QPoint(60, -100),    // pozycja dnia tygodnia ( % z dlug promienia)

                    QColor(240, 120, 0),   // kolor dnia tygodnia

                    QPoint(75, -70),    // pozycja datownika % z dlug promienia
                    30,   // szer okienka w % dlugosci promienia
                    12,   // wys okienka i fontu  (w % dlugosci promienia)

                    QColor(240,120,0),   // kolor daty
                    QColor(110, 110, 0, 150),   // kolor ramki_daty
                    QColor(60, 0, 0, 240),   // kolor tla_daty
                    QColor(0, 0, 0, 70) ,  // kolor cienia
                    5     // uskok cienia
                }
                );


    cyferblat.push_back(
                Tdane_cyferblatu  {
                    "Plum",
                    ":/new/prefix1/content/Clock_face_Plum.png",
                    "0","0",
                    "0",
                    //  ":/new/prefix1/content/neon_sekundnik.png",
                    ":/new/prefix1/content/center.png",

                    0,
                    0.23, // mimosrod godzinowej
                    0.03,   // grubosc w skali promienia tarczy
                    0.7,// wysokosc  w skali promienia tarczy
                    QColor(255, 0, 0, 255),

                    0.0, 0.1,
                    0.03,   0.85,// os minutowej

                    QColor(0, 0, 50),

                    0,    0.1,
                    0.01,  // grubosc sekundnika
                    1,  // dlugosc sekundnika (w % promienia tarczy)
                    QColor(250, 10, 10),   // kolor wskaz sekundnika

                    175, 175,  // pozycja srodka tarczy
                    0.5,  0.5,   // skalowanie zaślepki
                    150,  // promien_ tarczy

                    0, 0,  // translacja do odrebnej osi sekundnika (x,y w % promienia)

                    QPoint(0, 35),    // pozycja AM_PM ( % z dlug promienia)
                    QColor(180, 180, 180),   // kolor AM_PM
                    QPoint(70, -100),    // pozycja dnia tygodnia ( % z dlug promienia)

                    QColor(240, 240, 240),   // kolor dnia tygodnia

                    QPoint(75, -70),    // pozycja datownika % z dlug promienia
                    30,   // szer okienka w % dlugosci promienia
                    12,   // wys okienka i fontu  (w % dlugosci promienia)

                    QColor(240,240,240),   // kolor daty
                    QColor(110, 110, 0, 150),   // kolor ramki_daty
                    QColor(50, 50, 50, 240),   // kolor tla_daty
                    QColor(0, 0, 0, 70) ,  // kolor cienia
                    5     // uskok cienia
                }
                );





    cyferblat.push_back(
                Tdane_cyferblatu  {
                    "roman black empty",
                    ":/new/prefix1/content/rzymskie.png",
                    "0","0",
                    "0",
                    //  ":/new/prefix1/content/neon_sekundnik.png",
                    ":/new/prefix1/content/center.png",

                    0,     0.3,  0.07 ,  0.55, // os godzinowej, skala
                    QColor(0, 0, 0),   // kolor wskaz godzinowej
                    0.0,    0.2,   0.05,  0.90,// os minutowej
                    QColor(0, 0, 0),   // kolor wskaz minutowej

                    0,    0.1,
                    0.01,  // grubosc sekundnika
                    1,  // dlugosc sekundnika (w % promienia tarczy)
                    QColor(250, 10, 10),   // kolor wskaz sekundnika

                    175, 175,  // pozycja srodka tarczy
                    0.5,  0.5,   // skalowanie zaślepki
                    150,  // promien_ tarczy

                    0, 0,  // translacja do odrebnej osi sekundnika (x,y w % promienia)

                    QPoint(0, 35),    // pozycja AM_PM ( % z dlug promienia)
                    QColor(80, 80, 80),   // kolor AM_PM
                    QPoint(70, -100),    // pozycja dnia tygodnia ( % z dlug promienia)

                    QColor(240, 240, 240),   // kolor dnia tygodnia

                    QPoint(75, -70),    // pozycja datownika % z dlug promienia
                    30,   // szer okienka w % dlugosci promienia
                    12,   // wys okienka i fontu  (w % dlugosci promienia)

                    QColor(240,240,240),   // kolor daty
                    QColor(110, 110, 0, 150),   // kolor ramki_daty
                    QColor(50, 50, 50, 240),   // kolor tla_daty
                    QColor(0, 0, 0, 120) ,  // kolor cienia
                    5     // uskok cienia
                }
                );


    cyferblat.push_back(
                Tdane_cyferblatu  {
                    "roman_shadow_empty",
                    ":/new/prefix1/content/rzymskie_cien.png",
                    "0","0",
                    "0",
                    //  ":/new/prefix1/content/neon_sekundnik.png",
                    ":/new/prefix1/content/center.png",

                    0,     0.23,  0.04 ,  0.7, // os godzinowej, skala
                    QColor(0, 0, 0),   // kolor wskaz godzinowej
                    0.0,    0.23,   0.03,  0.90,// os minutowej
                    QColor(0, 0, 0),   // kolor wskaz minutowej

                    0,    0.1,
                    0.01,  // grubosc sekundnika
                    1,  // dlugosc sekundnika (w % promienia tarczy)
                    QColor(250, 10, 10),   // kolor wskaz sekundnika

                    180, 180,  // pozycja srodka tarczy
                    0.5,  0.5,   // skalowanie zaślepki
                    175,  // promien_ tarczy

                    0, 0,  // translacja do odrebnej osi sekundnika (x,y w % promienia)

                    QPoint(0, 35),    // pozycja AM_PM ( % z dlug promienia)
                    QColor(80, 80, 80),   // kolor AM_PM
                    QPoint(70, -100),    // pozycja dnia tygodnia ( % z dlug promienia)

                    QColor(240, 240, 240),   // kolor dnia tygodnia

                    QPoint(75, -70),    // pozycja datownika % z dlug promienia
                    30,   // szer okienka w % dlugosci promienia
                    12,   // wys okienka i fontu  (w % dlugosci promienia)

                    QColor(240,240,240),   // kolor daty
                    QColor(110, 110, 0, 150),   // kolor ramki_daty
                    QColor(50, 50, 50, 240),   // kolor tla_daty
                    QColor(0, 0, 0, 120) ,  // kolor cienia
                    5     // uskok cienia
                }
                );



    cyferblat.push_back(
                Tdane_cyferblatu  {
                    "gold_metalic_roman",
                    ":/new/prefix1/content/radialny_rzymski.png",
                    "0","0",
                    "0",
                    //  ":/new/prefix1/content/neon_sekundnik.png",
                    ":/new/prefix1/content/center.png",

                    0,     0.23,  0.07 ,  0.55, // os godzinowej, skala
                    QColor(0, 0, 0),   // kolor wskaz godzinowej
                    0.0,    0.23,   0.05,  0.90,// os minutowej
                    QColor(0, 0, 0),   // kolor wskaz minutowej

                    0,    0.1,
                    0.01,  // grubosc sekundnika
                    1,  // dlugosc sekundnika (w % promienia tarczy)
                    QColor(250, 10, 10),   // kolor wskaz sekundnika

                    175, 175,  // pozycja srodka tarczy
                    0.5,  0.5,   // skalowanie zaślepki
                    150,  // promien_ tarczy

                    0, 0,  // translacja do odrebnej osi sekundnika (x,y w % promienia)

                    QPoint(0, 35),    // pozycja AM_PM ( % z dlug promienia)
                    QColor(180, 180, 80),   // kolor AM_PM
                    QPoint(70, -100),    // pozycja dnia tygodnia ( % z dlug promienia)

                    QColor(240, 240, 240),   // kolor dnia tygodnia

                    QPoint(75, -70),    // pozycja datownika % z dlug promienia
                    30,   // szer okienka w % dlugosci promienia
                    12,   // wys okienka i fontu  (w % dlugosci promienia)

                    QColor(240,240,240),   // kolor daty
                    QColor(110, 110, 0, 100),   // kolor ramki_daty
                    QColor(50, 50, 50, 240),   // kolor tla_daty
                    QColor(0, 0, 0, 120) ,  // kolor cienia
                    5     // uskok cienia
                }
                );

    cyferblat.push_back(
                Tdane_cyferblatu  {
                    "Old Cathedral",
                    ":/new/prefix1/content/rom_dark_gold.png",
                    ":/new/prefix1/content/cebulasta_godzinowa_zlota.png",
                    ":/new/prefix1/content/cebulasta_minutowa_zlota.png",
                    "0",
                    //  ":/new/prefix1/content/neon_sekundnik.png",
                    ":/new/prefix1/content/center.png",

                    0,     0.33,  0.07 ,  0.55, // os godzinowej, skala
                    QColor(200, 230, 150),   // kolor wskaz godzinowej
                    0.0,    0.25,   0.05,  0.90,// os minutowej
                    QColor(240, 230, 250),   // kolor wskaz minutowej

                    0,    0.1,
                    0.01,  // grubosc sekundnika
                    1,  // dlugosc sekundnika (w % promienia tarczy)
                    QColor(250, 10, 10),   // kolor wskaz sekundnika

                    205, 188,  // pozycja srodka tarczy
                    0.5,  0.5,   // skalowanie zaślepki
                    150,  // promien_ tarczy

                    0, 0,  // translacja do odrebnej osi sekundnika (x,y w % promienia)

                    QPoint(80, 65),    // pozycja AM_PM ( % z dlug promienia)
                    QColor(180, 180, 180),   // kolor AM_PM
                    QPoint(70, -100),    // pozycja dnia tygodnia ( % z dlug promienia)

                    QColor(240, 240, 240),   // kolor dnia tygodnia

                    QPoint(75, -70),    // pozycja datownika % z dlug promienia
                    30,   // szer okienka w % dlugosci promienia
                    12,   // wys okienka i fontu  (w % dlugosci promienia)

                    QColor(240,240,240),   // kolor daty
                    QColor(110, 110, 0, 150),   // kolor ramki_daty
                    QColor(50, 50, 50, 240),   // kolor tla_daty
                    QColor(0, 0, 0, 120) ,  // kolor cienia
                    5     // uskok cienia
                }
                );

    cyferblat.push_back(
                Tdane_cyferblatu  {
                    "Sailors",
                    ":/new/prefix1/content/zaglowiec.png",
                    ":/new/prefix1/content/cebulasta_godzinowa_zlota.png",
                    ":/new/prefix1/content/cebulasta_minutowa_zlota.png",
                    "0",
                    //  ":/new/prefix1/content/neon_sekundnik.png",
                    ":/new/prefix1/content/center.png",

                    0,     0.33,  0.07 ,  0.65, // os godzinowej, skala
                    QColor(0, 0, 0),   // kolor wskaz godzinowej
                    0.0,    0.25,   0.05,  0.95,// os minutowej
                    QColor(0, 0, 0),   // kolor wskaz minutowej

                    0,    0.1,
                    0.01,  // grubosc sekundnika
                    1,  // dlugosc sekundnika (w % promienia tarczy)
                    QColor(250, 10, 10),   // kolor wskaz sekundnika

                    175, 175,  // pozycja srodka tarczy
                    0.5,  0.5,   // skalowanie zaślepki
                    160,  // promien_ tarczy

                    0, 0,  // translacja do odrebnej osi sekundnika (x,y w % promienia)

                    QPoint(80, 90),    // pozycja AM_PM ( % z dlug promienia)
                    QColor(180, 180, 180),   // kolor AM_PM
                    QPoint(70, -100),    // pozycja dnia tygodnia ( % z dlug promienia)

                    QColor(240, 240, 240),   // kolor dnia tygodnia

                    QPoint(75, -70),    // pozycja datownika % z dlug promienia
                    30,   // szer okienka w % dlugosci promienia
                    12,   // wys okienka i fontu  (w % dlugosci promienia)

                    QColor(240,240,240),   // kolor daty
                    QColor(110, 110, 0, 150),   // kolor ramki_daty
                    QColor(50, 50, 50, 240),   // kolor tla_daty
                    QColor(0, 0, 0, 120) ,  // kolor cienia
                    5     // uskok cienia
                }
                );




    cyferblat.push_back(
                Tdane_cyferblatu  {
                    "Modern-design",
                    ":/new/prefix1/content/Modern-Design.png",
                    ":/new/prefix1/content/Modern-Design-hours.png",
                    ":/new/prefix1/content/Modern-Design-minutes.png",
                    ":/new/prefix1/content/Modern-Design-seconds.png",
                    //  ":/new/prefix1/content/neon_sekundnik.png",
                    ":/new/prefix1/content/center.png",

                    0,     0.0,  0.07 ,  0.5, // os godzinowej, skala
                    QColor(0, 0, 0),   // kolor wskaz godzinowej
                    0.0,    0.25,   0.05,  0.65,// os minutowej
                    QColor(0, 0, 0),   // kolor wskaz minutowej

                    0,    0.1,
                    0.01,  // grubosc sekundnika
                    0.7,  // dlugosc sekundnika (w % promienia tarczy)
                    QColor(250, 10, 10),   // kolor wskaz sekundnika

                    175, 175,  // pozycja srodka tarczy
                    1,  1,   // skalowanie zaślepki
                    160,  // promien_ tarczy

                    0, 0,  // translacja do odrebnej osi sekundnika (x,y w % promienia)

                    QPoint(80, 90),    // pozycja AM_PM ( % z dlug promienia)
                    QColor(180, 180, 180),   // kolor AM_PM
                    QPoint(70, -100),    // pozycja dnia tygodnia ( % z dlug promienia)

                    QColor(240, 240, 240),   // kolor dnia tygodnia

                    QPoint(75, -70),    // pozycja datownika % z dlug promienia
                    30,   // szer okienka w % dlugosci promienia
                    12,   // wys okienka i fontu  (w % dlugosci promienia)

                    QColor(240,240,240),   // kolor daty
                    QColor(110, 110, 0, 150),   // kolor ramki_daty
                    QColor(50, 50, 50, 240),   // kolor tla_daty
                    QColor(0, 0, 0, 70) ,  // kolor cienia
                    3     // uskok cienia
                }
                );

#endif

#if 0
#endif

}
//*************************************************************************************************************************
// UWAGA:  //  dlug wskazowki w  % promienia tarczy jest w poniższych danych  NIEISTOTNA,
// bo brana jest z cyferblatu
// NATOMIAST mimośród x, y są waże, bo są cechą konkretnej wskazowki (gdzie na niej jest oś obrotu)
//*************************************************************************************************************************
void MainWindow::wstepne_wpisanie_wskazowek_godzinowych()
{

    // Ta pierwsza nigdy nie będzie użyta, zamiast niej będzia ta (bitmapowa) która jest opisana w tarczy
    vect_godzinowych.push_back(
                Tdane_wskazowki {
                    default_hours_hand_txt.toStdString(),
                    ":/new/prefix1/content/arnehour_pion.png",
                    0,     0.23, //0.15,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    //                    1.0,  0.7       // szer, dlug w  % promienia tarczy
                    0.025,  0.65,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 0), // kolor wskazowki
                    QColor(150, 150, 150, 150), // kolor cienia
                    0.02,     0.02,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // nie chcemy podkolorowac tej bitmapy



                });

    // zwykle, arne

    vect_godzinowych.push_back(
                Tdane_wskazowki {
                    "arne-hour ",
                    ":/new/prefix1/content/arnehour_pion.png",
                    0,     0.1,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    1.0,  0.65,       // szer, dlug w  % promienia tarczy
                    //                       0.025,  0.65,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 0), // kolor wskazowki
                    QColor(150, 150, 150, 150), // kolor cienia
                    0.02,     0.02,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // nie chcemy podkolorowac tej bitmapy



                });

    //    vect_godzinowych.push_back(
    //                Tdane_wskazowki {
    //                    "CityHall-hour",
    //                    ":/new/prefix1/content/AJ-CityHall-500-hour-pion.png",
    //                    0,     0.23,     // os w stosunku do p-tu centralnego (w % prom tarczy)
    //                    1.0,  0.7,       // szer, dlug w  % promienia tarczy
    //                    QColor(0, 0, 0), // kolor wskazowki
    //                    QColor(150, 150, 150, 150), // kolor cienia
    //                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
    //                    false // nie chcemy podkolorowac tej bitmapy
    //                });

    vect_godzinowych.push_back(
                Tdane_wskazowki {
                    "CityHall-hour-JG",
                    ":/new/prefix1/content/city_hall_hours.png",
                    0,     0.22,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    1.0,  0.7,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 0), // kolor wskazowki
                    QColor(150, 150, 150, 150), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // nie chcemy podkolorowac tej bitmapy



                });


    vect_godzinowych.push_back(
                Tdane_wskazowki {
                    "barok1-kameleon",
                    ":/new/prefix1/content/barok1_godzinowa.png",
                    0,      // mimosrod X
                    0.1,     // mimosrod os y w stosunku do p-tu centralnego (w % prom tarczy)
                    1,    // szer
                    0.75,       //  dlug w  % promienia tarczy NIEISTOTNE, dlugosc brana jest z cyferblatu

                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 150, 150), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    true // flag_bitmapa



                });


    vect_godzinowych.push_back(
                Tdane_wskazowki {
                    "barok1-silver-hour",
                    ":/new/prefix1/content/barok1_godzinowa_bevel_a.png",
                    0,     0.1,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    1,  0.75,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 150, 150), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // wolno podkolorowac?
                });

    vect_godzinowych.push_back(
                Tdane_wskazowki {
                    "barok1-gold-hour",
                    ":/new/prefix1/content/barok1_godzinowa_bevel_zloty.png",
                    0,     0.1,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    1,  0.75,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 150, 150), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // wolno podkolorowac?
                });

    vect_godzinowych.push_back(
                Tdane_wskazowki {
                    "barok1-gold-shining-hour",
                    ":/new/prefix1/content/barok1_godzinowa_bevel_zloty_shining.png",
                    0,     0.1,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    1,  0.75,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 150, 150), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // wolno podkolorowac?
                });
    vect_godzinowych.push_back(
                Tdane_wskazowki {
                    "barok1-red",
                    ":/new/prefix1/content/barok_godzinowa_red.png",
                    0,     0.1,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    1,  0.75,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 150, 150), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // wolno podkolorowac?
                });


    vect_godzinowych.push_back(
                Tdane_wskazowki {
                    "barok1-dark-hour",
                    ":/new/prefix1/content/barok1_godzinowa_bevel_dark.png",
                    0,     0.1,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    1,  0.75,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 150, 150), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // wolno podkolorowac?
                });



    //    vect_godzinowych.push_back(
    //                Tdane_wskazowki {
    //                    "marble-hour",
    //                    ":/new/prefix1/content/marble_minutowa.png",
    //                    0,     0.26,     // os w stosunku do p-tu centralnego (w % prom tarczy)
    //                    1.2,  0.8,       // szer, dlug w  % promienia tarczy
    //                    QColor(0, 0, 200), // kolor wskazowki
    //                    QColor(150, 150, 150, 150), // kolor cienia
    //                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
    //                    true // wolno podkolorowac bitmape



    //                });



    vect_godzinowych.push_back(
                Tdane_wskazowki {
                    "crystal-ball-hours",
                    ":/new/prefix1/content/crystal_ball_hours.png",
                    0,     0.02,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    0.6,  0.5,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 0, 0), // kolor wskazowki
                    QColor(150, 150, 150, 150), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // // wolno podkolorowac bitmape?



                });


    vect_godzinowych.push_back(
                Tdane_wskazowki {
                    "roman-hours",
                    ":/new/prefix1/content/roman_hours.png",
                    0,     0.07,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    1.2,  0.6,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 150, 150), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    true // // wolno podkolorowac bitmape ?



                });

    vect_godzinowych.push_back(
                Tdane_wskazowki {
                    "roman-gold-hours",
                    ":/new/prefix1/content/roman_hours_gold.png",
                    0,     0.07,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    1.2,  0.6,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 150, 150), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // // wolno podkolorowac bitmape ?



                });


    vect_godzinowych.push_back(
                Tdane_wskazowki {
                    "roman-white",
                    ":/new/prefix1/content/roman_hours_white.png",
                    0,     0.07,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    1.2,  0.6,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 150, 150), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // // wolno podkolorowac bitmape ?



                });

    vect_godzinowych.push_back(
                Tdane_wskazowki {
                    "roman-grey",
                    ":/new/prefix1/content/roman_hours_grey.png",
                    0,     0.07,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    1.2,  0.6,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 150, 150), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // // wolno podkolorowac bitmape ?



                });

    vect_godzinowych.push_back(
                Tdane_wskazowki {
                    "cebulasta-gold",
                    ":/new/prefix1/content/cebulasta_godzinowa_zlota.png",
                    0,     0.33,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    1.2,  0.9,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 150, 150), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // // wolno podkolorowac bitmape ?



                });

    vect_godzinowych.push_back(
                Tdane_wskazowki {
                    "cebulasta-czarna",
                    ":/new/prefix1/content/cebulasta_godzinowa_czarna.png",
                    0,     0.33,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    1.2,  0.9,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 150, 150), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // // wolno podkolorowac bitmape ?



                });
    //----------------------
    vect_godzinowych.push_back(
                Tdane_wskazowki {
                    "cebulasta-biala",
                    ":/new/prefix1/content/cebulasta_godzinowa_biala.png",
                    0,     0.33,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    1.2,  0.9,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 150, 150), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // // wolno podkolorowac bitmape ?



                });

    //----------------------
    vect_godzinowych.push_back(
                Tdane_wskazowki {
                    "drewniana-B",
                    ":/new/prefix1/content/drewano_godzinowaB.png",
                    0,     0.175,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    1.2,  0.70,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 150, 150), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // // wolno podkolorowac bitmape ?



                });

    //----------------------
    vect_godzinowych.push_back(
                Tdane_wskazowki {
                    "drewniana-kameleon",
                    ":/new/prefix1/content/drewano_godzinowaB.png",
                    0,     0.175,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    1.2,  0.70,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 150, 150), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    true // // wolno podkolorowac bitmape ?



                });

    vect_godzinowych.push_back(
                Tdane_wskazowki {
                    "drewniana-Cwide",
                    ":/new/prefix1/content/drewno_minutowa_wide_contr.png",
                    0,     0.175,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    5,  0.7,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 250, 50), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // wolno podkolorowac bitmape ?



                });


    vect_godzinowych.push_back(
                Tdane_wskazowki {
                    "drewniana-C",
                    ":/new/prefix1/content/drewano_godzinowa_C.png",
                    0,     0.175,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    5,  0.7,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 250, 50), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // wolno podkolorowac bitmape ?



                });
    vect_godzinowych.push_back(
                Tdane_wskazowki {
                    "drewno_godzinowa_kameleon",
                    ":/new/prefix1/content/drewno_godzinowa_C_black.png",
                    0,     0.175,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    5,  0.7,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 250, 50), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    true // wolno podkolorowac bitmape ?



                });

    vect_godzinowych.push_back(
                Tdane_wskazowki {
                    "drewno_godzinowa_C_black",
                    ":/new/prefix1/content/drewno_godzinowa_C_buk.png",
                    0,     0.175,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    5,  0.7,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 250, 50), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // wolno podkolorowac bitmape ?



                });

    vect_godzinowych.push_back(
                Tdane_wskazowki {
                    "agrafa_metal",
                    ":/new/prefix1/content/agrafa_metal-h.png",
                    0,     0.07,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    5,  0.7,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 250, 50), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // wolno podkolorowac bitmape ?

                });

    vect_godzinowych.push_back(
                Tdane_wskazowki {
                    "agrafa_gold",
                    ":/new/prefix1/content/agrafa_godz.png",
                    0,     0.07,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    5,  0.7,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 250, 50), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // wolno podkolorowac bitmape ?
                });


    vect_godzinowych.push_back(
                Tdane_wskazowki {
                    "tower_empty_gold",
                    ":/new/prefix1/content/tower_empty_gold_hours.png",
                    0,     0.07,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    5,  0.7,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 250, 50), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // wolno podkolorowac bitmape ?

                });

    vect_godzinowych.push_back(
                Tdane_wskazowki {
                    "tower_full_gold",
                    ":/new/prefix1/content/tower_gold_hours.png",
                    0,     0.07,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    5,  0.7,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 250, 50), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // wolno podkolorowac bitmape ?

                });
    vect_godzinowych.push_back(
                Tdane_wskazowki {
                    "zestaw_barok_gold",
                    ":/new/prefix1/content/zestaw_barok_gold_h.png",
                    0,     0.07,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    5,  0.7,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 250, 50), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // wolno podkolorowac bitmape ?

                });



    vect_godzinowych.push_back(
                Tdane_wskazowki {
                    "tower_metal-h",
                    ":/new/prefix1/content/tower_metal-h.png",
                    0,     0.07,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    5,  0.7,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 250, 50), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // wolno podkolorowac bitmape ?

                });
    vect_godzinowych.push_back(
                Tdane_wskazowki {
                    "tower_empty_metal-h",
                    ":/new/prefix1/content/tower_empty_metal-h.png",
                    0,     0.07,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    5,  0.7,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 250, 50), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // wolno podkolorowac bitmape ?

                });

    vect_godzinowych.push_back(
                Tdane_wskazowki {
                    "tower_barock_metal-h",
                    ":/new/prefix1/content/tower_barock_metal-h.png",
                    0,     0.07,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    5,  0.7,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 250, 50), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // wolno podkolorowac bitmape ?

                });

    vect_godzinowych.push_back(
                Tdane_wskazowki {
                    "rombowe_godzinowa_zlota",
                    ":/new/prefix1/content/rombowe_godzinowa_zlota.png",
                    0,     0.11,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    5,  0.75,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 250, 50), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // wolno podkolorowac bitmape ?

                });

    vect_godzinowych.push_back(
                Tdane_wskazowki {
                    "rombowe_godzinowa_biala",
                    ":/new/prefix1/content/rombowe_godzinowa_biala.png",
                    0,     0.11,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    5,  0.75,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 250, 50), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // wolno podkolorowac bitmape ?

                });
    vect_godzinowych.push_back(
                Tdane_wskazowki {
                    "rombowe_godzinowa_ciemna",
                    ":/new/prefix1/content/rombowe_godzinowa_ciemna.png",
                    0,     0.11,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    5,  0.75,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 250, 50), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // wolno podkolorowac bitmape ?

                });

    vect_godzinowych.push_back(
                Tdane_wskazowki {
                    "rombowe_godzinowa_kameleon",
                    ":/new/prefix1/content/rombowe_godzinowa_czarna_plaska.png",
                    0,     0.11,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    5,  0.75,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 250, 50), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    true    // wolno podkolorowac bitmape ?
                });


    vect_godzinowych.push_back(
                Tdane_wskazowki {
                    "iwc_hours",
                    ":/new/prefix1/content/iwc_hours.png",
                    -1,     0.115,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    0.07,  0.7,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 250, 50), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false    // wolno podkolorowac bitmape ?
                });

    vect_godzinowych.push_back(
                Tdane_wskazowki {
                    "Morgan_hours",
                    ":/new/prefix1/content/morgan_hours.png",
                    -2,     0.26,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    0.07,  0.95,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 250, 50), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false    // wolno podkolorowac bitmape ?

                });

    vect_godzinowych.push_back(
                Tdane_wskazowki {
                    "neon_hours",
                    ":/new/prefix1/content/neon_wskazowka.png",
                    0,     0.1,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    0.04,  0.65,       // szer, dlug w  % promienia tarczy
                    QColor(0, 220, 255), // kolor wskazowki
                    QColor(150, 150, 250, 50), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false    // wolno podkolorowac bitmape ?

                });

    vect_godzinowych.push_back(
                Tdane_wskazowki {
                    "Omega-hours",
                    ":/new/prefix1/content/Omega-hours.png",
                    0,     0.18,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    0.04,  1,       // szer, dlug w  % promienia tarczy
                    QColor(0, 200, 200), // kolor wskazowki
                    QColor(150, 150, 250, 50), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false    // wolno podkolorowac bitmape ?

                });


    vect_godzinowych.push_back(
                Tdane_wskazowki {
                    "Modern-Design",
                    ":/new/prefix1/content/Modern-Design-hours.png",
                    0,     0.15,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    0.04,  1,       // szer, dlug w  % promienia tarczy
                    QColor(0, 200, 200), // kolor wskazowki
                    QColor(150, 150, 250, 50), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false    // wolno podkolorowac bitmape ?

                });


}

//**********************************************************minutowe **********************************
void MainWindow::wstepne_wpisanie_wskazowek_minutowych()
{
    // pierwsza minutowa nigdy nie będzie użyta, bo zatąpi ją ta, która jest opisana w tarczy
    vect_minutowych.push_back(
                Tdane_wskazowki {
                    default_minutes_hand_txt.toStdString(),
                    ":/new/prefix1/content/arneminute_pion.png",
                    0,     0.15,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    2,  0.85,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 0), // kolor wskazowki
                    QColor(150, 150, 150, 150), // kolor cienia
                    0.02,     0.02,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false  // wolno podkolorowac bitmape ?



                });
    vect_minutowych.push_back(
                Tdane_wskazowki {
                    "arne-minute",
                    ":/new/prefix1/content/arneminute_pion.png",
                    0,     0.10,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    2,  0.85,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 0), // kolor wskazowki
                    QColor(150, 150, 150, 150), // kolor cienia
                    0.02,     0.02,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false  // wolno podkolorowac bitmape ?



                });

    vect_minutowych.push_back(
                Tdane_wskazowki{
                    "CityHall-minutes",
                    ":/new/prefix1/content/AJ-CityHall-500-minute_pion.png",
                    0,     0.15,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    2,  0.85,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 0), // kolor wskazowki
                    QColor(150, 150, 150, 150), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    true // wolno podkolorowac bitmape ?
                }
                );



    vect_minutowych.push_back(
                Tdane_wskazowki{
                    "CityHall-minutes-JG",
                    ":/new/prefix1/content/city_hall_minutes.png",
                    0,     0.15,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    2,  0.85,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 0), // kolor wskazowki
                    QColor(150, 150, 150, 150), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // wolno podkolorowac bitmape ?
                }
                );


    vect_minutowych.push_back(
                Tdane_wskazowki {
                    "barok1-minutes",
                    ":/new/prefix1/content/barok1_minutowa.png",
                    0.001,     0.062,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    1,  0.95,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 150, 150), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    true //wolno podkolorowac bitmape ?



                });

    vect_minutowych.push_back(
                Tdane_wskazowki {
                    "barok1-gold-minutes",
                    ":/new/prefix1/content/barok1_minutowa_gold.png",
                    0,     0.062,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    1,  0.95,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 150, 150), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false //wolno podkolorowac bitmape ?
                });

    vect_minutowych.push_back(
                Tdane_wskazowki {
                    "barok1-gold-min_bv6",
                    ":/new/prefix1/content/barok1_min_bev06.png",
                    0,     0.062,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    1,  0.95,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 150, 150), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false //wolno podkolorowac bitmape ?

                });

    vect_minutowych.push_back(
                Tdane_wskazowki {
                    "barok1-gold-min_bv10",
                    ":/new/prefix1/content/barok1_min_bev10.png",
                    0,     0.062,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    1,  0.95,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 150, 150), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false //wolno podkolorowac bitmape ?

                });


    vect_minutowych.push_back(
                Tdane_wskazowki {
                    "barok1_red",
                    ":/new/prefix1/content/barok1_minutowa_red.png",
                    0,     0.062,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    1,  0.95,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 150, 150), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false //wolno podkolorowac bitmape ?

                });


    //    vect_minutowych.push_back(
    //                Tdane_wskazowki {
    //                    "marble-minutes",
    //                    ":/new/prefix1/content/marble_minutowa.png",
    //                    0,     0.25,     // os w stosunku do p-tu centralnego (w % prom tarczy)
    //                    1.0,  1,       // szer, dlug w  % promienia tarczy
    //                    QColor(0, 0, 200), // kolor wskazowki
    //                    QColor(150, 150, 150, 150), // kolor cienia
    //                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
    //                    true //wolno podkolorowac bitmape ?



    //                });

    vect_minutowych.push_back(
                Tdane_wskazowki {
                    "crystal-ball-minutes",
                    ":/new/prefix1/content/crystal_ball_minutes.png",
                    0,     0.02,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    0.5,  0.75,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 0, 0), // kolor wskazowki  (nie będzie zmiany koloru)
                    QColor(150, 150, 150, 150), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // wolno podkolorowac bitmape ?



                });



    vect_minutowych.push_back(
                Tdane_wskazowki {
                    "roman-flat",
                    ":/new/prefix1/content/roman_minut.png",
                    0,     0.07,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    1.2,  0.85,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 150, 150), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    true // wolno podkolorowac bitmape ?



                });


    vect_minutowych.push_back(
                Tdane_wskazowki {
                    "roman-gold-soft",
                    ":/new/prefix1/content/roman_gold_soft_minut.png",
                    0,     0.07,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    1.2,  0.85,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 250, 250), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // wolno podkolorowac bitmape ?



                });


    vect_minutowych.push_back(
                Tdane_wskazowki {
                    "roman-gold",
                    ":/new/prefix1/content/roman_minutgold_10.png",
                    0,     0.07,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    1.2,  0.85,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 250, 250), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // wolno podkolorowac bitmape ?



                });

    vect_minutowych.push_back(
                Tdane_wskazowki {
                    "roman-light",
                    ":/new/prefix1/content/roman_minut_light.png",
                    0,     0.07,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    1.2,  0.85,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 250, 250), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // wolno podkolorowac bitmape ?



                });

    //    vect_minutowych.push_back(
    //                Tdane_wskazowki {
    //                    "roman-new-dark-minutes",
    //                    ":/new/prefix1/content/roman_minut_dark.png",
    //                    0,     0.07,     // os w stosunku do p-tu centralnego (w % prom tarczy)
    //                    1.2,  0.85,       // szer, dlug w  % promienia tarczy
    //                    QColor(0, 0, 200), // kolor wskazowki
    //                    QColor(150, 150, 250, 250), // kolor cienia
    //                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
    //                    false // wolno podkolorowac bitmape ?
    //
    //
    //
    //                });


    vect_minutowych.push_back(
                Tdane_wskazowki {
                    "roman-black",
                    ":/new/prefix1/content/roman_min_black.png",
                    0,     0.07,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    1.2,  0.85,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 250, 250), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // wolno podkolorowac bitmape ?



                });


    vect_minutowych.push_back(
                Tdane_wskazowki {
                    "roman-grey",
                    ":/new/prefix1/content/roman_min_grey.png",
                    0,     0.07,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    1.2,  0.85,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 250, 250), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // wolno podkolorowac bitmape ?



                });


    vect_minutowych.push_back(
                Tdane_wskazowki {
                    "roman-gold",
                    ":/new/prefix1/content/roman_min_gold.png",
                    0,     0.07,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    1.2,  0.85,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 250, 250), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // wolno podkolorowac bitmape ?



                });


    vect_minutowych.push_back(
                Tdane_wskazowki {
                    "cebulasta-gold",
                    ":/new/prefix1/content/cebulasta_minutowa_zlota.png",
                    0,     0.25,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    1.2,  1.1,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 250, 250), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // wolno podkolorowac bitmape ?



                });

    vect_minutowych.push_back(
                Tdane_wskazowki {
                    "cebulasta-biala",
                    ":/new/prefix1/content/cebulasta_minutowa_biala.png",
                    0,     0.25,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    1.2,  1.1,        // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 250, 250), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // wolno podkolorowac bitmape ?



                });
    vect_minutowych.push_back(
                Tdane_wskazowki {
                    "cebulasta-czarna",
                    ":/new/prefix1/content/cebulasta_minutowa_czarna.png",
                    0,     0.25,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    1.2,  1.1,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 250, 250), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    true // wolno podkolorowac bitmape ?



                });

    vect_minutowych.push_back(
                Tdane_wskazowki {
                    "drewniana",
                    ":/new/prefix1/content/drewno_minutowa.png",
                    0,     0.16,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    5,  1.0,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 250, 250), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // wolno podkolorowac bitmape ?



                });

    vect_minutowych.push_back(
                Tdane_wskazowki {
                    "drewniana-wide",
                    ":/new/prefix1/content/drewno_minutowa_wide.png",
                    0,     0.16,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    5,  1.0,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 250, 50), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // wolno podkolorowac bitmape ?



                });


    vect_minutowych.push_back(
                Tdane_wskazowki {
                    "drewniana-Cwide",
                    ":/new/prefix1/content/drewno_minutowa_wide_contr.png",
                    0,     0.16,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    5,  1.0,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 250, 50), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // wolno podkolorowac bitmape ?



                });

    vect_minutowych.push_back(
                Tdane_wskazowki {
                    "drewniana-Coloured",
                    ":/new/prefix1/content/drewno_minutowa_wide_contr.png",
                    0,     0.16,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    5,  1.0,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(250, 0, 0, 205), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    true // wolno podkolorowac bitmape ?



                });

    vect_minutowych.push_back(
                Tdane_wskazowki {
                    "drewniana-B",
                    ":/new/prefix1/content/drewano_godzinowaB.png",
                    0,     0.16,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    1.2,  1,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 150, 150), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // // wolno podkolorowac bitmape ?



                });







    vect_minutowych.push_back(
                Tdane_wskazowki {
                    "drewno_godzinowa_C_black",
                    ":/new/prefix1/content/drewno_godzinowa_C_black.png",
                    0,     0.175,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    5,  0.9,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 250, 50), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    true // wolno podkolorowac bitmape ?



                });

    vect_minutowych.push_back(
                Tdane_wskazowki {
                    "drewno_C_buk",
                    ":/new/prefix1/content/drewno_godzinowa_C_buk.png",
                    0,     0.175,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    5,  1,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 250, 50), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // wolno podkolorowac bitmape ?

                });

    vect_minutowych.push_back(
                Tdane_wskazowki {
                    "agrafa_gold",
                    ":/new/prefix1/content/agrafa_min.png",
                    0,     0.05,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    5,  1,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 250, 50), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // wolno podkolorowac bitmape ?

                });




    vect_minutowych.push_back(
                Tdane_wskazowki {
                    "tower_empty_gold_minutes",
                    ":/new/prefix1/content/tower_empty_gold_minutes.png",
                    0,     0.045,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    5,  1,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 250, 50), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // wolno podkolorowac bitmape ?

                });


    vect_minutowych.push_back(
                Tdane_wskazowki {
                    "tower_gold_minutes",
                    ":/new/prefix1/content/tower_gold_minutes.png",
                    0,     0.045,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    5,  1,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 250, 50), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // wolno podkolorowac bitmape ?

                });

    vect_minutowych.push_back(
                Tdane_wskazowki {
                    "zestaw_barok_gold",
                    ":/new/prefix1/content/zestaw_barok_gold_m.png",
                    0,     0.045,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    5,  1,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 250, 50), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // wolno podkolorowac bitmape ?

                });


    vect_minutowych.push_back(
                Tdane_wskazowki {
                    "agrafa_metal-m",
                    ":/new/prefix1/content/agrafa_metal-m.png",
                    0,     0.045,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    5,  1,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 250, 50), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // wolno podkolorowac bitmape ?

                });

    vect_minutowych.push_back(
                Tdane_wskazowki {
                    "tower_metal-m",
                    ":/new/prefix1/content/tower_metal-m.png",
                    0,     0.045,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    5,  1,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 250, 50), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // wolno podkolorowac bitmape ?
                });

    vect_minutowych.push_back(
                Tdane_wskazowki {
                    "tower_empty_metal-m",
                    ":/new/prefix1/content/tower_empty_metal-m.png",
                    0,     0.045,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    5,  1,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 250, 50), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // wolno podkolorowac bitmape ?
                });

    vect_minutowych.push_back(
                Tdane_wskazowki {
                    "tower_barock_metal-m",
                    ":/new/prefix1/content/tower_barock_metal-m.png",
                    0,     0.045,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    5,  1,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 250, 50), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // wolno podkolorowac bitmape ?
                });


    vect_minutowych.push_back(
                Tdane_wskazowki {
                    "rombowe_minutowa_zlota",
                    ":/new/prefix1/content/rombowe_minutowa_zlota.png",
                    0,     0.08,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    5,  1.08,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 250, 50), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // wolno podkolorowac bitmape ?
                });

    vect_minutowych.push_back(
                Tdane_wskazowki {
                    "rombowe_minutowa_kameleon",
                    ":/new/prefix1/content/rombowe_minutowa_czarna_plaska.png",
                    0,     0.08,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    5,  1.08,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 250, 50), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    true // wolno podkolorowac bitmape ?
                });
    vect_minutowych.push_back(
                Tdane_wskazowki {
                    "rombowe_minutowa_czarna",
                    ":/new/prefix1/content/rombowe_minutowa_czarna.png",
                    0,     0.08,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    5,  1.08,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 250, 50), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // wolno podkolorowac bitmape ?
                });

    vect_minutowych.push_back(
                Tdane_wskazowki {
                    "rombowe_minutowa_biala",
                    ":/new/prefix1/content/rombowe_minutowa_biala.png",
                    0,     0.08,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    5,  1.08,       // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 250, 50), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // wolno podkolorowac bitmape ?
                });

    vect_minutowych.push_back(
                Tdane_wskazowki {
                    "iwc_minutes",
                    ":/new/prefix1/content/iwc_minutes.png",
                    0.0,    0.06,   // os w stosunku do p-tu centralnego (w % prom tarczy)
                    0.05,  0.9,         // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 250, 50), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // wolno podkolorowac bitmape ?
                });


    vect_minutowych.push_back(
                Tdane_wskazowki {
                    "morgan_minutes",
                    ":/new/prefix1/content/morgan_minutes.png",
                    0.0,    0.19,   // os w stosunku do p-tu centralnego (w % prom tarczy)
                    0.05,  0.95,         // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 250, 50), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // wolno podkolorowac bitmape ?
                });

    vect_minutowych.push_back(
                Tdane_wskazowki {
                    "neon_minutes",
                    ":/new/prefix1/content/neon_wskazowka.png",
                    0.0,    0.1,   // os w stosunku do p-tu centralnego (w % prom tarczy)
                    0.04,  0.8,         // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 250, 50), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // wolno podkolorowac bitmape ?
                });

    vect_minutowych.push_back(
                Tdane_wskazowki {
                    "Omega-minutes",
                    ":/new/prefix1/content/Omega-minutes.png",
                    0.0,    0.15,   // os w stosunku do p-tu centralnego (w % prom tarczy)
                    0.04,  0.55,         // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 250, 50), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // wolno podkolorowac bitmape ?
                });


    vect_minutowych.push_back(
                Tdane_wskazowki {
                    "Modern-design",
                    ":/new/prefix1/content/Modern-Design-minutes.png",
                    0.0,    0.1,   // os w stosunku do p-tu centralnego (w % prom tarczy)
                    0.04,  0.55,         // szer, dlug w  % promienia tarczy
                    QColor(0, 0, 200), // kolor wskazowki
                    QColor(150, 150, 250, 50), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // wolno podkolorowac bitmape ?
                });

    //==== SEKUNDNIKI =============================================

    //    vect_sekundnikow. push_back(
    //                Tdane_wskazowki {
    //                    default_seconds_hand_txt.toStdString(),
    //                    // tr ("default hand - for a current clock face").toStdString() ,
    //                    ":/new/prefix1/content/second.png",
    //                    0.30,     0.0,     // os w stosunku do p-tu centralnego (w % prom tarczy)
    //                    1,  0.8,       // szer, dlug w  % promienia tarczy
    //                    QColor(200, 0, 0), // kolor wskazowki
    //                    QColor(150, 150, 150, 150), // kolor cienia
    //                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
    //                    true // wolno podkolorowac bitmape ?



    //                }
    //                );

    //    vect_sekundnikow. push_back(
    //                Tdane_wskazowki {
    //                    "arne-seconds",
    //                    ":/new/prefix1/content/second.png",
    //                    0.20,     0.1,     // os w stosunku do p-tu centralnego (w % prom tarczy)
    //                    1,  0.8,       // szer, dlug w  % promienia tarczy
    //                    QColor(200, 0, 200), // kolor wskazowki
    //                    QColor(150, 150, 150, 150), // kolor cienia
    //                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
    //                    false // wolno podkolorowac bitmape ?



    //                }
    //                );



    vect_sekundnikow. push_back(
                Tdane_wskazowki {
                    "Wall Clock seconds HD",
                    ":/new/prefix1/content/Wall-sekundnik.png",
                    0.0,     0.0,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    1,  0.8,       // szer, dlug w  % promienia tarczy
                    QColor(200, 0, 0), // kolor wskazowki
                    QColor(150, 150, 150, 150), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // wolno podkolorowac bitmape ?

                }
                );

    vect_sekundnikow. push_back(
                Tdane_wskazowki {
                    "Neon seconds",
                    ":/new/prefix1/content/neon_sekundnik.png",
                    0.0,     0.19,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    0.005,  0.9,       // szer, dlug w  % promienia tarczy
                    QColor(0, 230, 230), // kolor wskazowki
                    QColor(150, 150, 150, 150), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // wolno podkolorowac bitmape ?

                }
                );

    vect_sekundnikow. push_back(
                Tdane_wskazowki {
                    "Omega-seconds.png",
                    ":/new/prefix1/content/Omega-seconds.png",
                    0.0,     0.24,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    0.005,  0.9,       // szer, dlug w  % promienia tarczy
                    QColor(0, 230, 230), // kolor wskazowki
                    QColor(150, 150, 150, 150), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // wolno podkolorowac bitmape ?
                }
                );




    vect_sekundnikow. push_back(
                Tdane_wskazowki {
                    "Modern-Design.png",
                    ":/new/prefix1/content/Modern-Design-seconds.png",
                    0.0,     0.28,     // os w stosunku do p-tu centralnego (w % prom tarczy)
                    0.005,  0.9,       // szer, dlug w  % promienia tarczy
                    QColor(0, 230, 230), // kolor wskazowki
                    QColor(150, 150, 150, 150), // kolor cienia
                    0.05,     0.05,     // przes cienia os w stosunku do p-tu centralnego (w % prom tarczy)
                    false // wolno podkolorowac bitmape ?
                }
                );

}
//***************************************************************************************************************

//***************************************************************************************************************
void MainWindow::zapis_opcji_na_dysku()
{
    // qDebug() << __PRETTY_FUNCTION__ << " nr_jezyka = " << nr_jezyka
    //          << " zapisywane do pliku " << pathed_nazwa_pliku_z_opcjami;

    ofstream plik(pathed_nazwa_pliku_z_opcjami);
    if(!plik)
    {
        cerr << "Blad zapisu pliku " << pathed_nazwa_pliku_z_opcjami << endl;
        exit(3);
    }
    plik
        << nr_jezyka << "\n"
        << przezroczystosc << "\n"
         << przezroczystosc_dla_kursora << "\n"
         << flag_na_wierzchu << "\n"
         <<  flag_ograniczenie_pozycji << "\n"
          <<  flag_blokuj_na_ekranie << "\n"
           <<  flag_uruchom_z_loginem << "\n"
            <<  flag_uruchom_przy_logowaniu<< "\n"
             << flag__kilka_kopii<< "\n"
             << flag_gskala_active << '\n'
             <<  gskala  << '\n'
              << flag_show_dzien_tygodnia << '\n'
              <<  flag_show_date << '\n'
               << flag_show_sekundnik << '\n'
               << nr_tarczy << '\n'
               << poz_zegara_x << '\n'
               << poz_zegara_y << '\n'
               << nazwa_timezone << '\n'
               << flag_show_AM_PM << '\n'
               <<   flag_wsk_godz_bitmapowa  << '\n'
                 << flag_wsk_min_bitmapowa  << '\n'
                 << flag_wsk_sek_bitmapowa  << '\n'
                 <<  nr_wsk_wektorowej_godz << '\n'
                  <<  nr_wsk_wektorowej_min << '\n'
                   <<  nr_wsk_wektorowej_sek << '\n'

                    << nr_wybranej_bitmapy_wsk_godzinowej << '\n'
                    << nr_wybranej_bitmapy_wsk_minutowej << '\n'
                    << nr_wybranej_bitmapy_wsk_sekundowej << '\n'
                    << ruler_mm << '\n' ;



       // cerr << "Po zapisie " << nazwa_pliku_z_opcjami << endl;
}
//***************************************************************************************************************
int  MainWindow::id_linux_czy_windows_version()
{
    //    cout << __func__ << endl;



#ifdef Q_OS_WIN

    //    auto current = QOperatingSystemVersion::current();
    //    cout <<  current.name().toStdString() << endl;    // pisze: "Windows"

    string product_version =
            QSysInfo::productVersion().toStdString();

    //    cout << " to " <<  " productVersion  = " <<  product_version << endl;


    if(product_version.substr(0, 1) == "7 ")
    {
        // to jest windows 7
        return 7;
    }
    else   if(product_version.substr(0, 1) == "8")
    {
        // to jest windows 8
        return 8;
    }
    else   if(product_version.substr(0, 2) == "10")
    {
        // to jest windows 10
        return 10;
    }

    // w przeciwnym razie wersja Windows nierozpoznana
    return 0;


#elif Q_OS_LINUX
    return 1;  // to jest linux
#else
#error  "Your operating system is not supported "
    return 0;
#endif
}
//***************************************************************************************************************
void MainWindow::wczytanie_opcji_z_dysku()
{

    // rozpoznanie typu systemu operacyjnego. Jeśli to LInux, to powinien istniec katalog  ~/.configure

    id_linux_czy_windows_version();


    ifstream plik(pathed_nazwa_pliku_z_opcjami);
    if(!plik)
    {
        cerr << "Error while reading-in a file  " << pathed_nazwa_pliku_z_opcjami << endl;
        return ;
    }
    plik
            >> nr_jezyka
            >> przezroczystosc
            >> przezroczystosc_dla_kursora
            >> flag_na_wierzchu
            >>  flag_ograniczenie_pozycji   // nie uzywamy
            >>  flag_blokuj_na_ekranie
            >>  flag_uruchom_z_loginem  // ???
            >>  flag_uruchom_przy_logowaniu
            >> flag__kilka_kopii
            >> flag_gskala_active
            >>  gskala
            >> flag_show_dzien_tygodnia
            >> flag_show_date
            >> flag_show_sekundnik
            >> nr_tarczy
            >> poz_zegara_x
            >> poz_zegara_y
            >> nazwa_timezone
            >>  flag_show_AM_PM
            >> flag_wsk_godz_bitmapowa
            >> flag_wsk_min_bitmapowa
            >> flag_wsk_sek_bitmapowa
            >> nr_wsk_wektorowej_godz
            >> nr_wsk_wektorowej_min
            >> nr_wsk_wektorowej_sek

            >> nr_wybranej_bitmapy_wsk_godzinowej
            >> nr_wybranej_bitmapy_wsk_minutowej
            >> nr_wybranej_bitmapy_wsk_sekundowej
            >> ruler_mm;



    this->setWindowOpacity(przezroczystosc/255.0);
    this->move(poz_zegara_x, poz_zegara_y);

    //    cout << __func__ << endl;

    zmiana_opcji_on_top(flag_na_wierzchu);



}


//***************************************************************************************************************
void MainWindow::zmiana_opcji_on_top(bool flag_top)
{
    flag_na_wierzchu = flag_top;

    //  cout << "Flaga - zegar ma byc on top ? " << boolalpha << flag_top << endl;

    setWindowFlag(Qt::WindowStaysOnTopHint, flag_top );
    setWindowFlag(Qt::X11BypassWindowManagerHint, flag_top);
    //  Informs the window system that the window should stay on top of all other windows.
    // Note that on some window managers on X11 you also have
    // to pass Qt::X11BypassWindowManagerHint for this flag to work correctly.

}



//***************************************************************************************************************
void MainWindow::zmien_przezroczystosc(int value)
{
    setWindowOpacity(value/255.0);
}
//*********************************************************************************************************
bool MainWindow::spr_rozne_typy_alarmu(Tjeden_alarm & al)
{
    // UWAGA: nr w częstotliwosci ją numerami takich nazw
    //QStringList list_czestotliwosc =
    // { "Raz" - 0
    // 1 -, "Codziennie",
    // 2- "Co tydzień",
    // 3 - "Co miesiąc",
    // 4 - "Co rok",
    // 5 - "Minuty"};


    if(al.flag_test_alarm_now)
    {
        al.flag_test_alarm_now = false;
        return true;
    }

    QTime time = QTime::currentTime();
    QDate date = QDate::currentDate();

    if( (al.godzina + ":00")  == time.toString().toStdString() )
    {
        cout << "bezacy miesiac " << date.month();
        cout << "czestotliwosc  " << al.czestotliwosc << endl;
        cout << "al.data  " << al.data << endl;
        cout << " date.toString().toStdString() =  " << date.toString().toStdString()  << endl;

        switch (al.czestotliwosc){
        case 1:    // codziennie
            return true;

        case 0:   // raz
            if(al.data == date.toString("yyyy-MM-dd").toStdString() )
            {
                cout << "Zgadza sie godzina i data " << endl;
                return true;
            }
            break;

        case 2: // co tydzien
        {
            cout << "dzien tygodnia ma byc ma byc 0x"  << hex  << (int) al.dni_tygodnia   << endl;
            int dzisejszy_dzien_tygodnia = date.dayOfWeek()-1;
            if( al.dni_tygodnia & (1 << dzisejszy_dzien_tygodnia))
            {
                cout << "zgadza sie dzien tygodnia 0x " << hex << (int) al.dni_tygodnia  << endl;
                return true;
            }
            break;
        }
        case 3:   // co miesiac
            cout << "dzien miesaca ma byc " << al.data.substr(8,2) << endl;

            if( al.data.substr(8,2) == date.toString("dd").toStdString() )
            {
                cout << "Zgadza sie godzina i dzien w miesiacu " << endl;
                return true;
            }
            break;

        case 4:  // co rok
            cout << " miesiac i dzien miesaca ma byc " << al.data.substr(5,5) << endl;
            if( al.data.substr(5,5) == date.toString("MM-dd").toStdString() )
            {
                cout << "Zgadza sie godzina i dzien w miesiacu i miesiac " << endl;
                return true;
            }
            break;

        default:   // ignorujemy na razie interwal
            break;



        } // koniec switch



    } // kon if, zgadza sie godzina
    else if (al.czestotliwosc == 5)   // czyli: co interwal minut, poczawszy od..
    {

        // cout << " co interwal, " << al.interwal << " poczawszy od " << al.godzina << endl;

        string s_godz = al.godzina.substr(0,2);
        string s_min = al.godzina.substr(3,2);
        int g = atoi(s_godz.c_str());
        int m = atoi(s_min.c_str());

        QTime start;
        start.setHMS(g, m, 0);

        if(al.interwal != 0){
            if(( time.secsTo(start) % (al.interwal*60) ) == 0)
            {
                cout << " zgodzil sie interwal "  << endl;
                return true;
            }
        }
    }
    else
        return false;

    return false;
}
//*********************************************************************************************************
string MainWindow::kosmetyka_nazwy_programu(string nazwa)
{
    auto gdzie = string::npos;
    for(int i = 0 ; i < 100 ; ++i)
    {
        gdzie = nazwa.rfind('\\', gdzie);
        if(gdzie == string::npos)
        {
            break;
        }
        nazwa.replace(gdzie, 1, R"(\\)" );
        //cout << "Obecnie nazwa = " << nazwa << ", kursor na pozycji " << gdzie << endl;
        --gdzie;



    }
    nazwa = R"(")"   + nazwa + R"(")";
    cout << "ostatecznie [" << nazwa << "]" << endl;


    return nazwa;
}
//*********************************************************************************************************
void MainWindow::player_loop_needed(QMediaPlayer::MediaStatus status)
{
    //cout << "player  zmienil status na = " << status << endl;

    switch (status)
    {
    case QMediaPlayer::LoadedMedia:
        //  cout << "--------->  QMediaPlayer::LoadedMedia " << endl;
        break;

    case QMediaPlayer::BufferedMedia:
        //        cout << " ----------->QMediaPlayer::BufferedMedia " << endl;
        break;

    case  QMediaPlayer::EndOfMedia:
    {

        //cout << "---------> player zakonczyl media" << endl;

        QMediaPlayer *wsk = dynamic_cast< QMediaPlayer*> (sender());

        wsk->play();

    }
        break;

    default:
        cout << "nieznany status " << endl;
        break;


    }
}

//*********************************************************************************************************
void MainWindow::sprawdzenie_alarmow()
{

    //cout <<  __func__ << endl;


    for(auto & al : tablica_alarmow)
    {

        //        cout <<  "\n" << al.nazwa << " ma godzine " << al.godzina.substr(0,5)
        //              <<  ", porownanie ze stringiem [" << time.toString().toStdString().substr(0,5) << "]" << endl;

        //        if(al.godzina.substr(0,5)  == time.toString().toStdString().substr(0,5) )

        if(al.flag_enable == false) continue;

        if(spr_rozne_typy_alarmu( al) )
        {

            cout << "!!!!!! ALARM !!!!!!!!" << al.nazwa
                 << ", flag okno = " << al.flag_okno
                 << endl;

            // otwarcie okienka z tekstem
            if(al.flag_okno){
                if( al.flag_alarm_jest_na_ekranie == false  ) {
                    al.flag_alarm_jest_na_ekranie = true;
                    al.dlg = new Togloszenie_alarmu(this);
                    al.dlg->adres_mojego_alarmu(&al);
                    al.dlg->wstaw_opis(al.nazwa.c_str(),  al.dodatkowy_opis.c_str());
                    al.dlg->show();
                } // if nie ma na ekranie

            }// if flag okno

            // wywołanie muzyki -----------------------------------------------------
            if(al.flag_dzwiek)
            {
                //                cout << "!!!!!! Proba uruchomienia muzyki!!!!!!!!" << al.muzyka
                //                     << ", flag dzwiek = " << al.flag_dzwiek << endl;
                if(al.muzyka.empty() == false)
                {
                    cout << "!!!!!! Przed funkcja play !!!!!!!!" << al.muzyka << endl;

                    al.player = new QMediaPlayer(this);
                    al.audioOutput = new QAudioOutput(this);    // nowy element w Qt6
                    al.player->setAudioOutput(al.audioOutput);  // powiązanie

                    al.player->setSource(QUrl::fromLocalFile(QString::fromStdString(al.muzyka)));
                    al.audioOutput->setVolume(0.5);  // zakres 0.0–1.0

                    al.player->play();

                    if(al.sekund_dzwieku > 0)
                    {
                        connect(al.player, SIGNAL(positionChanged(qint64)),
                                this, SLOT(player_position_upadate(qint64)));

                        vec_playerow_grajaych_teraz.push_back (
                                    Trojka { al.player, al.sekund_dzwieku, al.loop}
                                    );
                        //                        auto itt = vec_playerow_grajaych_teraz.front();
                        //                        vec_playerow_grajaych_teraz.erase(itt);
                    }
                    if(al.loop)
                    {
                        cout << "Wejscie do opcji loop w muzyce" << endl;
                        connect(al.player,
                                SIGNAL(
                                    mediaStatusChanged(QMediaPlayer::MediaStatus)
                                    ),
                                this,
                                SLOT(player_loop_needed(QMediaPlayer::MediaStatus )));
                    }
                    // delete player;
                }
            }

            if(al.flag_program)
            {
                cout << "!!!!!! Przed uruchomieniem zewnetrznego programu  !!!!!!!!" << al.nazwa_programu << endl;
                string komenda =  al.nazwa_programu ;
                komenda = kosmetyka_nazwy_programu(komenda);

                QProcess::startDetached(komenda.c_str()); // nie zatrzyma sekundnika
            }

            if(al.flag_wylacz_komputer)
            {
                cout << "!!!!!! Przed uruchomieniem shutdown !!!!!!!!" << endl;
                make_shutdown();
            }

        } // to alarm

    } // end petli po alarmach
}
//*******************************************************************************************************************
//************************************************************************************************************************************


void MainWindow::zapisz_ulubione_wskazowki_na_dysku()
{
    string def_filename {pathed_nazwa_pliku_z_faworytami};
    ofstream plik(def_filename);
    if(!plik){
        cout << "Can't open file for writing defaults (" << def_filename << endl;
        return;
    }


    for(auto &e : vec_pref_hands)
    {
        //        cout << "Tarcza " << e.clock_face_name << " lubi wsk godz:  "
        //             << e.hours_hand
        //             << " i wsk min: " << e.minutes_hand
        //             << " i sekundnik:  " << e.seconds_hand
        //             << endl;


#define xkeyword(n)  "\t<" << #n << ">" << e.n << "</" << #n << ">\n"

        plik
                << "<faworyt>\n"
                << xkeyword(clock_face_name)
                << xkeyword(hours_hand)
                << xkeyword(minutes_hand)
                << xkeyword(seconds_hand)
                << "</faworyt>\n"
                << endl;

        // cout << "Zapisany "   << (int) (a.dni_tygodnia) << endl;

    }
}
//******************************************************************************************************
void MainWindow::odczytaj_ulubione_wskazowki_z_dysku()
{

    ifstream plik(pathed_nazwa_pliku_z_faworytami);
    if(!plik)
    {
        cerr << "Blad otwarcia pliku z_faworytami " << pathed_nazwa_pliku_z_faworytami << endl;
        return;
    }

    string tresc;
    string linijka;
    while(
          getline(plik, linijka))
    {
        tresc += linijka + '\n';
    }

    //

    size_t kursor = 0;
    for(int entry [[__maybe_unused__]] = 0 ; ; entry++)      // petla po faworytach
    {
        // cout << "obieg nr " << entry << endl;
        // wyjecie jednego alarmu ----------------------
        auto pocz = tresc.find("<faworyt>", kursor);
        kursor = pocz;
        auto kon = tresc.find("</faworyt>", kursor);
        kursor = kon;
        if(pocz == string::npos || kon == string::npos)
        {
            // cout << "Kniec petli bo pocz = " << pocz << ", kon = " << kon << endl;
            break;
        }
        string one = tresc.substr(pocz, kon-pocz);

        // cout << "Jedno entry nr " << entry << " ma tresc " << one << endl;
        // wyjecie poszczegolnych itemow z alarmu -------------------

        auto tarcza = znajdz_txt_item(one, "clock_face_name");
        auto godzinowa = znajdz_txt_item(one, "hours_hand");
        auto minutowa = znajdz_txt_item(one, "minutes_hand");
        auto sek = znajdz_txt_item(one, "seconds_hand");

        //        cout << "tarcza " << tarcza
        //             << " | godz = " << godzinowa
        //             << " | min "  << minutowa
        //             << " | sek "  << sek << endl;

        vec_pref_hands.push_back(
                    Tpreferred_hands(
                        tarcza, godzinowa, minutowa, sek));
    }



}


//************************************************************************************************************
Tdane_cyferblatu::Tdane_cyferblatu(string nazwa_, string bitmapa_tarcza_,
                                   string bitmapa_godzinowa_, string bitmapa_minutowa_, string bitmapa_sekundnik_,
                                   string bitmapa_central_, double os_x_godzinowej_, double os_y_godzinowej_,
                                   double skala_x_godzinowej_, double skala_y_godzinowej_,
                                   QColor kolor_godzinowej_,
                                   double os_x_minutowej_, double os_y_minutowej_,
                                   double skala_x_minutowej_, double skala_y_minutowej_,
                                   QColor kolor_minutowej_,
                                   double os_x_sekundnika_, double os_y_sekundnika_,
                                   double skala_x_sekundnika_, double skala_y_sekundnika_,
                                   QColor kolor_sekundnika_,
                                   double os_x_center_, double os_y_center_,
                                   double skala_x_center_, double skala_y_center_,
                                   double promien_tarczy_, double odrebna_os_x_,
                                   double odrebna_os_y_,
                                   QPoint pozycja_AM_PM_,
                                   QColor kolor_AM_PM_, QPoint pozycja_dnia_tygodnia_,
                                   QColor kolor_dnia_tygodnia_, QPoint pozycja_daty_, double szer_daty_,
                                   double wys_daty_, QColor kolor_daty_, QColor kolor_ramki_daty_,
                                   QColor kolor_tla_daty_, QColor kolor_cienia_,
                                   double uskok_cienia_)

    :  // lista inicjalizacyjna
      nazwa(nazwa_), bitmapa_tarcza(bitmapa_tarcza_),
      bitmapa_godzinowa(bitmapa_godzinowa_),
      bitmapa_minutowa(bitmapa_minutowa_),
      bitmapa_sekundnik(bitmapa_sekundnik_),
      bitmapa_central(bitmapa_central_),

      os_x_godzinowej(os_x_godzinowej_),
      os_y_godzinowej(os_y_godzinowej_),
      szer_x_godzinowej(skala_x_godzinowej_),
      dlugosc_godzinowej(skala_y_godzinowej_),
      kolor_godzinowej(kolor_godzinowej_),

      os_x_minutowej(os_x_minutowej_),
      os_y_minutowej(os_y_minutowej_),
      szer_x_minutowej(skala_x_minutowej_),
      dlugosc_minutowej(skala_y_minutowej_),
      kolor_minutowej(kolor_minutowej_),

      os_x_sekundnika(os_x_sekundnika_),
      os_y_sekundnika(os_y_sekundnika_),
      skala_x_sekundnika(skala_x_sekundnika_),
      skala_y_sekundnika(skala_y_sekundnika_),

      kolor_sekundnika(kolor_sekundnika_),

      os_x_center(os_x_center_),
      os_y_center(os_y_center_),
      skala_x_center(skala_x_center_),
      skala_y_center(skala_y_center_),

      promien_tarczy(promien_tarczy_),

      odrebna_os_x(odrebna_os_x_),  // translacja do odrebnej osi sekundnika (x,y w % promienia)
      odrebna_os_y(odrebna_os_y_),  // translacja do odrebnej osi sekundnika (x,y w % promienia)


      pozycja_AM_PM ( pozycja_AM_PM_),
      kolor_AM_PM(  kolor_AM_PM_),

      pozycja_dzien_tygodnia(pozycja_dnia_tygodnia_),
      kolor_dnia_tygodnia(kolor_dnia_tygodnia_),
      pozycja_daty(pozycja_daty_),
      szer_daty(szer_daty_),
      wys_daty(wys_daty_),
      kolor_daty(kolor_daty_),
      kolor_ramki_daty(kolor_ramki_daty_),
      kolor_tla_daty(kolor_tla_daty_),
      kolor_cienia(kolor_cienia_),
      uskok_cienia(uskok_cienia_)
{
    // cout << "dziala konstruktor" << endl;

    // Wykryć, czy wskazowka jest bitmapowa czy wektorowa
    // wektorowa ma tylko jeden znak w nazwie bitmapy i jest to cyfra będąca
    // numerem wzoru wektorowego.

    // ustawić flagę mówiącą czy wskazowka fabryczna jest wektorowa czy bitmapowa.

    // sprawdzenie czy ulubiona wsk godzinowa jest cyfrą (czytli wektorowa) -----------------
    if( bitmapa_godzinowa.size() == 1 && isdigit(bitmapa_godzinowa[0] ) )
    {
        flag_fabryczna_wsk_godz_bitmapowa = false;
        // nr_wsk_wektorowej_godz = atoi (bitmapa_godzinowa.c_str() );
    }

    if( bitmapa_minutowa.size() == 1 && isdigit(bitmapa_minutowa[0] ) )
    {
        flag_fabryczna_wsk_min_bitmapowa = false;
    }

    if( bitmapa_sekundnik.size() == 1 && isdigit(bitmapa_sekundnik[0] ) )
    {
        flag_fabryczna_wsk_sek_bitmapowa = false;
    }
}
//**********************************************************************************************************
void MainWindow::make_shutdown()
{
    QProcess proc;

#ifdef Q_OS_WINDOWS

    // Dla Windowsow ----------------------
    QStringList argumenty1;
    // litera L - mala robi jedynie logout.
    // Do powier off trzeba chyba opcji /s
    argumenty1 << "/s";

    proc.startDetached("shutdown" , argumenty1) ;

    //cout << "Command executed " << endl;
    if(!proc.waitForStarted())
    {
        // cout << "After waitForStarded " << endl;
    }
#elif defined      Q_OS_LINUX
    // Dla linuxa ------------------------------
    QStringList argumenty2;
    argumenty2 << "--poweroff" ;

    proc.startDetached("shutdown" , argumenty2) ;

    if(!proc.waitForStarted()){
        cout << "After waitForStarded 2 " << endl;

    }
#endif
}
//**********************************************************************************************************
void MainWindow::zapisz_poligony()
{    
    int nr = 0;
    for(uint p = 0 ; p < vpoligon.size() ; ++p)
    {
        // if(vpoligon[p].empty() )continue;

        // zapisuje ten jeden
        string poligon_name = "poligon_" + to_string(nr++) + ".dat";
        ofstream plik (poligon_name);
        if(!plik) {
            cout << "Niemozliwy zapis pliku " << poligon_name << endl;
            exit(1);
        }


        for(uint i = 0 ; i < vpoligon[p].size() ; ++i)
        {
            plik << vpoligon[p][i].x() << "\t" << vpoligon[p][i].y() << "\n";
        }
        //        cout << "zapisany " << poligon_name << " ma vertexow " << vpoligon[p].size() << endl;
    }

    // na koncu wpisuje jeden pusty, żeby było wiadomo kiedy skonczyc
    string poligon_name = "poligon_" + to_string(nr++) + ".dat";
    ofstream plik (poligon_name);
    plik.close();


}
//**********************************************************************************************************
void MainWindow::wczytaj_poligony()
{
    vpoligon.clear();
    for(uint p = 0 ;  ; ++p)
    {

        // na razie wpisuje tylko ten jeden
        string poligon_name = "poligon_" + to_string(p) + ".dat";
        ifstream plik (poligon_name);
        if(!plik) {
            cout << "Niemozliwe otwarcie pliku " << poligon_name << endl;
            biezacy_poligon = -1;
            break;
        }
        typ_poligonu pol;

        double x = 0 , y = 0;
        while(plik)
        {
            plik >> x >> y ;
            if(!plik) break;

            pol.push_back(QPointF{ x, y} );
        }

        //        cout << "wcztyany " << poligon_name << " ma vertexow " << pol.size() << endl;
        if(pol.empty()) break;

        vpoligon.push_back(pol);

    }
}
//**********************************************************************************************************
void MainWindow::przelicz_wsp_eventowe_na_cyferblatowe(QPoint pt_ev, QPoint *pt_world)
{
    pt_world ->setX(0.5 + pt_ev.x() - cyf.os_x_center * gskala);
    pt_world ->setY(0.5 + pt_ev.y() - cyf.os_y_center * gskala);
}
//**********************************************************************************************************
void MainWindow::test_poligons(int linia)
{
    for(uint p = 0 ; p < vpoligon.size() ; ++p){
        cout << "LINIA "<< linia << ", Polygon " << p << " ma vertexow " << vpoligon[p].size() << endl;
        //            for(int v = 0 ; v < vpoligons[p].size() ; ++v){             }
    }
}

//
//**********************************************************************************************************
void MainWindow::dodaj_vertex()
{
    if(biezacy_poligon == -1 || vpoligon.empty())
    {
        // No poligon is currently selected
        QMessageBox::information( this, "Note",
                                  "There is no selected (yellow) vertex yet. At first click on one vertex to select it"
                                  );
        return;
    }

    typ_poligonu & poligon = vpoligon[biezacy_poligon];
    int &n = nr_przesuwanego_vertexu;

    // karuzela laczaca koniec poligonu z poczatkiem
    //    int poprzedni = (n == 0 )? (poligon.size()) - 1:
    //                               nr_przesuwanego_vertexu -1;

    int nastepny = (n +1 >= (int) poligon.size() )?  0:
                                                     nr_przesuwanego_vertexu +1;

    // obliczenie sredniej
    auto srednia_x = poligon[n].x() + (poligon[nastepny].x() -  poligon[n].x()) / 2;
    auto srednia_y = poligon[n].y() + (poligon[nastepny].y() - poligon[n].y()) / 2;

    QPoint nowy(srednia_x, srednia_y);

    //
    cout << "biezacy punkt = " ; drukuj_pt(poligon[n], true);
    cout << "nastepny punkt = " ; drukuj_pt(poligon[nastepny], true);

    cout << "nowy, sredni punkt ma byc w " ;
    drukuj_pt(nowy, true);

    auto nowy_poligon = poligon;

    auto it = nowy_poligon.begin();
    it += nastepny;
    // auto it = &nowy_poligon[nastepny];
    nowy_poligon.insert(it, nowy);

    cout << "po dodaniu" << endl;
    for(auto & el : nowy_poligon)
    {
        drukuj_pt(el, true);
    }

    poligon = nowy_poligon;
    update();
}
//**********************************************************************************************************
void MainWindow::remove_selected_vertex()
{

    if(biezacy_poligon == -1 || vpoligon.empty())
    {
        // No poligon is currently selected
        QMessageBox::information( this, "Note",
                                  "There is no selected poligon vertex yet. At first click on one vertex to select it"
                                  );
        return;
    }

    // Jeśli nie ma selected vertex -> info
    if(nr_przesuwanego_vertexu == -1)
    {
        QMessageBox::information( this, "Note",
                                  "There is no selected (yellow) vertex yet. At first click on one vertex to select it"
                                  );
        return;

    }


    int &n = nr_przesuwanego_vertexu;



    // karuzela laczaca koniec poligonu z poczatkiem
    //    int poprzedni = (n == 0 )? (poligon.size()) - 1:
    //                               nr_przesuwanego_vertexu -1;

    typ_poligonu & poligon = vpoligon[biezacy_poligon];

    auto nowy_poligon = poligon;
    nowy_poligon.clear();

    for(int i = 0 ; i < (int) poligon.size() ; ++i)
    {
        if(i != n){
            nowy_poligon.push_back(poligon[i]);

            //        drukuj_pt(el, true);
        }
    }
    poligon = nowy_poligon;
    update();
}
//**********************************************************************************************************
void MainWindow::skalowanie_podzialki()
{
    // wypisanie info o ilości pixeli na szerokosci ekranu
    int ile_pixeli = tarcza.width() *gskala;

    // zapytanie ile to milimetrow
    string info;
    info += "horizontal ruler covers "
            + to_string(ile_pixeli) +
            " pixels. Please type how many mm it is: ";

    bool ok;
    int mm = QInputDialog::getInt(this, "Scaling",
                                  info.c_str(),
                                  25, 0, 1000, 1, &ok);
    if (!ok) return;

    ruler_mm = mm;

    // przygotowanie obiektów (tekstow do wypisania przy poszczególnych kreskach skali
    //    kresek_na_skali = 6 lub 7
    //odstep_kresek =

}
//**********************************************************************************************************
void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
     {
        cout << "zmiana języka" << endl;
         ui->retranslateUi(this);  // odświeża wszystkie teksty z .ui

         updateTexts();            // jeśli masz teksty ustawiane ręcznie
     }

     QMainWindow::changeEvent(event);  // WAŻNE!
}
//**********************************************************************************************************
void MainWindow::updateTexts()
{
    // ui->labelStatus->setText(tr("Status"));
    // ui->pushButtonStart->setText(tr("Start"));
}
//**********************************************************************************************************
//**********************************************************************************************************



