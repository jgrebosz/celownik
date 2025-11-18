#ifndef TDEFAULT_HANDS_H
#define TDEFAULT_HANDS_H

#include <string>

class Tpreferred_hands
{
public:
    std::string clock_face_name;
    std::string hours_hand;
    std::string minutes_hand;
    std::string seconds_hand;

    Tpreferred_hands(
            std::string clock_face_name_arg,
            std::string hours_hand_arg,
            std::string minutes_hand_arg,
            std::string seconds_hand_arg
            ); ;

//    void zapisz_na_dysku();
//    void odczytaj_z_dysku() {}

};

#endif // TDEFAULT_HANDS_H
