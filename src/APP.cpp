#include "APP.hpp"

APP::APP(TWR* _twr) : linked_twr(_twr) {
    vector<Plane*> tmp1, tmp2;
    this->waiting_planes = tmp1;
    this->coming_planes = tmp2;
}