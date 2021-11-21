#include "Plane.hpp"

// Number of parking spots in airports
#define NB_PARKING_SPOTS 3

/**
 * @brief A TWR manage airport parking and runway.
 * It manage all of landing and taking off planes.
 */
class TWR {
    private:
        queue<Plane*> parking; // All of parked planes
        vector<Location> parking_spots; // All of parking spot locations
        Trajectory takeoff, landing; // The taking off and landing trajectory
        bool is_runway_used;
        Sprite background; // The airport background image

    public:
        TWR(
            const vector<Location> &_parking_spots,
            const Location &runway_start,
            const Location &runway_end,
            const Location &parking_entrance,
            const Location &perimeter_limit
        );
        queue<Plane*> getParking() const { return this->parking; }
        bool isParkingEmpty() const { return this->parking.empty(); }
        bool isRunwayUsed() const { return this->is_runway_used; }
        Sprite getBackground() const { return this->background; }
        void setBackground(const Sprite& bg) { this->background = bg; }

        /**
         * @brief Manage a plane to land it and park it
         * @param plane The plane to land
         */
        void landPlane(Plane* plane);

        /**
         * @brief Manage the first arrived plane to take it off
         */
        void takeOffPlane();

        /**
         * @brief Spawn a new plane in the parking
         * @param name The plane name
         * @return Plane* The spawned plane
         */
        Plane* spawnPlane(const string& name);
};
