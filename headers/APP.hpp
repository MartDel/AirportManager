#include "TWR.hpp"

/**
 * @brief A APP manage airport perimeter and landing.
 * It manage all of waiting planes for landing.
 */
class APP {
    private:
        vector<Plane *> waiting_planes;
        vector<Plane *> coming_planes;
        TWR* linked_twr;

    public:
        APP(TWR* _twr);
        TWR* getTWR() const { return this->linked_twr; }

        // Manage planes
        void addWaitingPlane(Plane *p) { this->waiting_planes.push_back(p); }
        void addComingPlane(Plane *p) { this->coming_planes.push_back(p); }
};