#include "Plane.hpp"

#define NB_PARKING_SPOTS 3

class TWR {
    private:
        queue<Plane*> parking;
        vector<Location> parking_spots;
        Trajectory takeoff, landing;
        bool is_runway_used;

    public:
        TWR(
            const vector<Location>& _parking_spots,
            const Location& runway_start,
            const Location& runway_end,
            const Location& parking_entrance,
            const Location& perimeter_limit
        );
        queue<Plane*> getParking() const { return this->parking; }
        bool isParkingEmpty() const { return this->parking.empty(); }
        bool isRunwayUsed() const { return this->is_runway_used; }

        // Manage a plane to land and park it
        void landPlane(Plane* plane);

        // Manage a plane to take it off
        void takeOffPlane();

        // Spawn a plane in the parking
        Plane* spawnPlane(const string& name);
};
