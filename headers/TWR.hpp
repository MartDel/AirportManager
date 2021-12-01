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
        map<Location, bool> parking_spots; // All of parking spot locations and if the spot is used or not
        Trajectory takeoff, landing; // The taking off and landing trajectory
        bool is_runway_used;
        Plane* plane_using_runway; // The plane which is using the runway

        /**
         * @brief Get the first occurence of a free parking spot.
         * Loop through the parking_spots map to find a free spot.
         * @return std::_Rb_tree_const_iterator<std::pair<const Location, bool>> The found free spot.
         * Return parking_spots.end() if there isn't free spot available.
         */
        std::_Rb_tree_const_iterator<std::pair<const Location, bool>> getFreeParkingSpot() const;

    public:
        TWR(
            const vector<Location> &_parking_spots,
            Location &runway_start,
            Location &runway_end,
            Location &parking_entrance,
            Location &perimeter_limit
        );

        /* --------------------------------- Getters -------------------------------- */

        // Read the TWR parking
        queue<Plane*> getParking() const { return this->parking; }
        bool isParkingEmpty() const { return this->parking.empty(); }
        bool isParkingFull() const { return this->parking.size() == this->parking_spots.size(); }
       
        Trajectory getLandingTrajectory() const { return this->landing; }
        bool isRunwayUsed() const { return this->is_runway_used; }
        Plane* getPlaneInRunway() const { return this->plane_using_runway; }
        
        /* --------------------------------- Setters -------------------------------- */

        void toggleIsRunwayUsed() { this->is_runway_used = !this->is_runway_used; }
        void setPlaneInRunway(Plane* p) { this->plane_using_runway = p; }

        /* ----------------------------- Public methods ----------------------------- */

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
