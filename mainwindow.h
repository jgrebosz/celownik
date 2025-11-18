#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include <QPoint>
#include <QMediaPlayer>
#include <QRubberBand>

#include <string>
#include <vector>
#include "qpainterpath.h"
#include "tjeden_alarm.h"
#include "tprzezroczystosc.h"

namespace Ui {
class MainWindow;
}

class Topcje_dlg;

//-------------------------------------------------
enum  class  tryb_wskazowek { zwykle = 0, ulubione, fabryczne };

enum  class  zestaw_wskazowek_dodatkowych { zadne = 0,
                                            wsk_dzien_miesiaca = 1,
                                            wsk_dzien_tygodnia = 1<<2,
                                            wsk_miesiaca = 1<<3,
                                            liczba_rok = 1<<4,
                                            tarcza_faza_ksiezyca = 1<<5,
                                            tarcza_noc_dzien = 1<<6
                                          };


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Dodatkowe tarcze, wskazowki
// wsk. dzien tygodnia
// wsk. dzien miesiaca
// wsk. miesiaca
// liczba: rok
// tarcza - faza ksiezyca
// tarcza - dzien/noc godzinowo

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct Tdane_cyferblatu
{
    std::string nazwa;
    std::string bitmapa_tarcza;
    std::string bitmapa_godzinowa;
    std::string bitmapa_minutowa;
    std::string bitmapa_sekundnik;
    std::string bitmapa_central;
    std::string bitmapa_moon;

    double os_x_godzinowej;
    double os_y_godzinowej;
    double szer_x_godzinowej;
    double dlugosc_godzinowej;
    QColor kolor_godzinowej;

    double os_x_minutowej;
    double os_y_minutowej;
    double szer_x_minutowej;
    double dlugosc_minutowej;
    QColor kolor_minutowej;

    double os_x_sekundnika;
    double os_y_sekundnika;
    double skala_x_sekundnika;
    double skala_y_sekundnika;
    QColor kolor_sekundnika;

    double os_x_center;  // środek tarczy x
    double os_y_center;   // srodek tarczy y
    // zaślepka osi
    double skala_x_center;
    double skala_y_center;

    double promien_tarczy;

    double  odrebna_os_x;
    double  odrebna_os_y;


    // teksty ---------------
    QPoint   pozycja_AM_PM;
    QColor kolor_AM_PM;

    QPoint   pozycja_dzien_tygodnia;
    QColor kolor_dnia_tygodnia;

    // pozycja Daty
    QPoint pozycja_daty;
    double szer_daty;
    double wys_daty; // (i fontu) w % dlugosci promienia
    QColor kolor_daty;
    QColor kolor_ramki_daty;
    QColor kolor_tla_daty;


    QColor kolor_cienia;
    double uskok_cienia;


    bool flag_fabryczna_wsk_godz_bitmapowa = true;
    bool flag_fabryczna_wsk_min_bitmapowa = true;
    bool flag_fabryczna_wsk_sek_bitmapowa = true;
    // konstruktor-----------------------------------------------------------------
    Tdane_cyferblatu( ) { };


    Tdane_cyferblatu(  std::string nazwa_,
                       std::string bitmapa_tarcza_,
                       std::string bitmapa_godzinowa_,
                       std::string bitmapa_minutowa_,
                       std::string bitmapa_sekundnik_,
                       std::string bitmapa_central_,


                       double os_x_godzinowej_,
                       double os_y_godzinowej_,
                       double skala_x_godzinowej_,
                       double skala_y_godzinowej_,
                       QColor kolor_godzinowej_,

                       double os_x_minutowej_,
                       double os_y_minutowej_,
                       double skala_x_minutowej_,
                       double skala_y_minutowej_,
                       QColor kolor_minutowej_,

                       double os_x_sekundnika_,
                       double os_y_sekundnika_,
                       double skala_x_sekundnika_,
                       double skala_y_sekundnika_,
                       QColor kolor_sekundnika_,

                       double os_x_center_,  // środek tarczy x
                       double os_y_center_,   // srodek tarczy y
                       // zaślepka osi
                       double skala_x_center_,
                       double skala_y_center_,

                       double promien_tarczy_,
                       double odrebna_os_x_,  // translacja do odrebnej osi sekundnika (x,y w % promienia)
                       double odrebna_os_y_,  // translacja do odrebnej osi sekundnika (x,y w % promienia)

                       // teksty ---------------

                       QPoint   pozycja_AM_PM_,
                       QColor kolor_AM_PM_,

                       QPoint   pozycja_dnia_tygodnia_,
                       QColor kolor_dnia_tygodnia_,

                       // pozycja Daty
                       QPoint pozycja_daty_,
                       double szer_daty_,
                       double wys_daty_,  // wys okienka i fontu  (w % dlugosci promienia)
                       QColor kolor_daty_,
                       QColor kolor_ramki_daty_,
                       QColor kolor_tla_daty_,
                       QColor kolor_cienia_ = QColor(0, 0, 0, 200),
                       double uskok_cienia_ = 5
            );

};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct Tdane_wskazowki
{
    std::string nazwa;
    std::string bitmapa;
    double os_x;        // w % w stosunku do p-tu centralnego
    double os_y;        // w % w stosunku do p-tu centralnego
    double skala_x;    // % promienia tarczy
    double skala_y;    // % promienia tarczy
    QColor kolor;
    QColor kolor_cienia;
    // przesuniecie cienia os w stosunku do p-tu centralnego (w % prom tarczy)
    double offs_x;        // w % w stosunku do p-tu centralnego
    double offs_y;        // w % w stosunku do p-tu centralnego
    bool flag_bitmapa_moze_byc_kolorowana;

};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class MainWindow : public QMainWindow
{
    Q_OBJECT

    friend class Topcje_dlg;
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    std::string sciezka;

    bool zmiana_wygladu_cyferblatu(int nr, tryb_wskazowek  tryb);    // tryb 0 = zwykly, 1 uzyj favourites (jesli sa),  2 - wróć do fabrycznych

    void resize_okna_z_tarcza()
    {
        zmiana_wygladu_cyferblatu(nr_tarczy, tryb_wskazowek::zwykle);
    }
    void zmien_przezroczystosc(int value);

    bool spr_rozne_typy_alarmu(Tjeden_alarm &a);
    string kosmetyka_nazwy_programu(string nazwa);
    void zmiana_opcji_on_top(bool flag );
    void zmiana_jezyka(int nr_jezyka);

    void zapisz_ulubione_wskazowki_na_dysku();
    void odczytaj_ulubione_wskazowki_z_dysku();


private slots:

    void mousePressEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *event)override;
    void mouseReleaseEvent ( QMouseEvent* )override;
    void sprawdzenie_alarmow();

    void wywolanie_okna_alarmow();
    void wywolanie_okna_opcji();
    void wejdz_w_rys_poligonu();
    void skoncz_rys_poligonu();
    void wejdz_w_edycje_poligonu();
    void skoncz_edycje_poligonu();
    void podmien_tarcze_na_polprzezroczysta(bool flaga); ;

    void remove_poligon();
    void dodaj_vertex();
    void remove_selected_vertex();
    void skalowanie_podzialki();
    void set_transparency();
    void transparency_slider();

    // void wywolanie_okna_wskazowek();
    void nastepna_tarcza();
    void koniec_programu();
    void empty_function(){};
    void player_position_upadate(qint64 position);

    void player_loop_needed(QMediaPlayer::MediaStatus status);

protected:
    void paintEvent(QPaintEvent *event) override;
private:
    Ui::MainWindow *ui;


    QPixmap minutowa;
    QPixmap godzinowa;
    QPixmap sekundnik;
    QPixmap tarcza;
    QPixmap central;
    //QPixmap ksiezyc;


    bool wybor_cyferblatu(string nazwa);

    double rozm_tarczy_x = 200;
    double rozm_tarczy_y = 200;

    int ruler_mm = 0;
    int transparency_while_editing_poligons = 220;


    void wstepne_zaladowanie_tablicy_alarmow();
    Tjeden_alarm wyjecie_itemow(std::string one);
    std::string znajdz_txt_item(std::string txt, std::string keyword);
    int znajdz_int_item(std::string txt, std::string keyword);

    void wstepne_wpisanie_cyferblatow();
    void wstepne_wpisanie_wskazowek_godzinowych();
    void wstepne_wpisanie_wskazowek_minutowych();
    //void wstepne_wczytanie_faz_ksiezyca();


    std::string nazwa_pliku_z_opcjami { "zegary.dat"};
    std::string pathed_nazwa_pliku_z_opcjami ;
    std::string nazwa_pliku_z_alarmami { "alarmy.dat"};
    std::string pathed_nazwa_pliku_z_alarmami ;
    std::string  nazwa_pliku_z_faworytami { "favorites.dat" };
    std::string  pathed_nazwa_pliku_z_faworytami;

    void zapis_opcji_na_dysku();
    void wczytanie_opcji_z_dysku();

    void rysuj_wskaz_wektorowa(QPainter & painter, int nr, double x, double y, double dlug, double szer,
                               QColor kolor,
                               double cien_x, double cien_y,  QColor kolor_cienia,
                               double owal_x =0, double owal_y = 0);


    void rysowanie_podzialki(QPainter & painter);

    vector<Tdane_cyferblatu> cyferblat;
    unsigned int nr_tarczy = 0;
    Tdane_cyferblatu cyf; // biezacy cyferblat;

    //  bool flag_wskazowki_bitmapowe = 0;
    bool flag_wsk_godz_bitmapowa = 0;
    bool flag_wsk_min_bitmapowa = 0;
    bool flag_wsk_sek_bitmapowa = 0;

    int nr_wybranej_bitmapy_wsk_godzinowej = 0;
    int nr_wybranej_bitmapy_wsk_minutowej = 0;
    int nr_wybranej_bitmapy_wsk_sekundowej = 0;

    vector <Tdane_wskazowki> vect_godzinowych;
    vector <Tdane_wskazowki> vect_minutowych;
    vector <Tdane_wskazowki> vect_sekundnikow;


    int nr_wsk_wektorowej_godz = 0;
    int nr_wsk_wektorowej_min = 0;
    int nr_wsk_wektorowej_sek = 0;




    // do suwania tarczy po ekranie

    int lastMouseX = 0;
    int lastMouseY = 0;


    //------------------------
    bool flag_rysowanie_poligonu = false;
    bool flag_edycja_poligonu = false;
    bool flag_move_one_vertex_of_polygon = false;

    bool flag_mouse_just_pressed = false;

    using typ_poligonu = vector<QPointF>;
    vector<typ_poligonu>   vpoligon;
    int biezacy_poligon = -1;

    void rysuj_poligon(QPainter & painter, int nr, double szer);


    QRubberBand *rubberBand = nullptr;
    bool flag_rubberband_on = false;
    QPoint starting_pt;
    QPoint current_end_pt;
    QPoint previous_end_pt;

    int nr_przesuwanego_vertexu = -1;
    void moving_polygon_vertex_handler ( QMouseEvent* e );
    QPainterPath shape_moving_vertex;  // to ma byc rubberband?

    // we remember how far was the place of clicking from the center of the vertex
    // then during moving - we can correct the movement
    int x_distance_to_vertex = 0;
    int y_distance_to_vertex = 0;
    bool flag_screen_just_repainted = false; // for refreshing during lupa mode

    // for making the rubberband in moving vertes, we want to
    // store neighbours of the moved vertex
    QPoint p_previous_vtx, p_next_vtx ;


    void przelicz_wskazowke(int nr_wybranej_wsk_any,
                            vector <Tdane_wskazowki> & vect_any,
                            QPixmap & anyowa,
                            bool flag_czy_bitmapowo,

                            // dotychczasowe dane o tej wskazowce
                            double & cyf_os_x_any,
                            double & cyf_os_y_any,
                            double & cyf_dlugosc_any,
                            double & cyf_szer_any,
                            QColor &cyf_kolor_any,
                            // rezultaty
                            double  & any_mimosrod_y,
                            double  & any_mimosrod_x,
                            double & any_length_poly,
                            double & any_szer_poly,
                            double & any_wsp_korekcji_dlugosci,
                            double &any_wsp_korekcji_szerokosci,

                            // dla bitmapowej  wsk godzinowej
                            double & any_skalowanie_x,
                            double & any_skalowanie_y

                            );


    //-- Przeliczone dane biezącego cyferblatu ---------------------------
    double promien = 100;

    // dla wektorowej wsk godzinowej // to sa wartosci stale?
    double  godz_mimosrod_y = 0;
    double  godz_mimosrod_x = 0;
    double godz_length_poly = 150;
    double godz_szer_poly = 10;
    double godz_wsp_korekcji_dlugosci = 1;
    double godz_wsp_korekcji_szerokosci = 1;

    // dla bitmapowej  wsk godzinowej
    double godz_skalowanie_x = 0;
    double godz_skalowanie_y = 0;


    //    double godz_b_dlugosc ;

    // dla wektorowej wsk minutowej ------------------------------
    double  min_mimosrod_y = 0;
    double  min_mimosrod_x = 0;
    double min_length_poly = 190;
    double min_szer_poly =  10; // 2;
    double min_wsp_korekcji_dlugosci = 1;
    double min_wsp_korekcji_szerokosci = 1;

    double min_skalowanie_x = 0;
    double min_skalowanie_y = 0;


    // dla wektorowej wsk sekundnik ---------------------------------

    double  sek_mimosrod_y = 0;
    double  sek_mimosrod_x = 0;
    double sek_length_poly = 2000;
    double sek_szer_poly = 10;
    double sek_wsp_korekcji_dlugosci = 1;
    double sek_wsp_korekcji_szerokosci = 1; // nieuzywane?


    // dla bitmapowej  wersji sekundnika
    double sek_skalowanie_x = 0;
    double sek_skalowanie_y = 0;

    int  znajdz_nr_wskazowki_o_bitmapie(string nazwa, vector<Tdane_wskazowki> & v);
    int  znajdz_nr_wskazowki_o_nazwie(string naz, vector<Tdane_wskazowki> &v);

    void wgraj_wskazowke(string nazwa,  vector <Tdane_wskazowki> & vecwsk,
                         bool *flag_bitvec, int * nr_wekt, int * nr_bitm, QPixmap *bitmapa);

    void wgraj_wskazowke_fabryczna(string nazwa,  vector <Tdane_wskazowki> & vecwsk,
                                   bool *flag_bitvec, int * nr_wekt, int * nr_bitm, QPixmap *bitmapa);
    bool czy_nazwa_naprawde_bitmapowa(string nazwa);
    bool czy_biezace_wskazowki_sa_fabryczne();
    int  id_linux_czy_windows_version();
    void make_shutdown();

    void zapisz_poligony();
    void wczytaj_poligony();
    void przelicz_wsp_eventowe_na_cyferblatowe(QPoint pt_ev, QPoint *pt_world);


    void test_poligons(int linia);

    vector<QColor> kolory_poligonow;
};

template <class typ>
void drukuj_pt(typ pt, bool flag_nl = false)
{
    cout << "[" << pt.x() << " , " <<  pt.y() << "]" ;
    if(flag_nl) cout << endl;
}

#endif // MAINWINDOW_H
