#include "TWR.hpp"

class APP {
    private:
        map<float, Plane*> waiting_planes;
        TWR* linked_twr;

    public:
        APP(const TWR* _twr);
};