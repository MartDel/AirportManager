#include "Plane.hpp"

// Landing and take off speeds
#define TAKEOFF_RUNWAY_START_SPEED 50
#define TAKEOFF_RUNWAY_END_SPEED 100
#define TAKEOFF_PARKING_ENTRANCE_SPEED 30
#define LANDING_RUNWAY_START_SPEED 50
#define LANDING_RUNWAY_END_SPEED 30
#define LANDING_PARKING_ENTRANCE_SPEED 5

/**
 * @brief A TWR manage airport parking and runway.
 * It manage all of landing and taking off planes.
 */
class TWR {
    private:
        queue<Plane*> parking; // All of parked planes
        map<Location, bool> parking_spots; // All of parking spot locations
        Trajectory takeoff, landing; // The taking off and landing trajectory
        bool is_runway_used;
        Sprite background; // The airport background image
        Plane* plane_using_runway; // The plane which is using the runway

        std::_Rb_tree_const_iterator<std::pair<const Location, bool>> getFreeParkingSpot() const;

    public:
        TWR(
            const vector<Location> &_parking_spots,
            Location &runway_start,
            Location &runway_end,
            Location &parking_entrance,
            Location &perimeter_limit
        );

        queue<Plane*> getParking() const { return this->parking; }
        bool isParkingEmpty() const { return this->parking.empty(); }
        bool isParkingFull() const { return this->parking.size() == this->parking_spots.size(); }
        Trajectory getLandingTrajectory() const { return this->landing; }
        bool isRunwayUsed() const { return this->is_runway_used; }
        void toggleIsRunwayUsed() { this->is_runway_used = !this->is_runway_used; }
        Sprite getBackground() const { return this->background; }
        void setBackground(const Sprite& bg) { this->background = bg; }
        Plane* getPlaneInRunway() const { return this->plane_using_runway; }
        void setPlaneInRunway(Plane* p) { this->plane_using_runway = p; }

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
        Plane* spawnPlane();
};
