#include "Airport.hpp"

/* ------------------------------ Plane defines ----------------------------- */
#define DEFAULT_CONSUMPTION 0.25
#define PLANE_COLOR_DEFAULT Color::Blue
#define PLANE_COLOR_URGENCY Color::Red
#define PLANE_CIRCLE_RADIUS 5.f
#define ALTITUDE_LABEL_X 15
#define ALTITUDE_LABEL_Y 10
#define ALTITUDE_LABEL_SIZE 10
#define NAME_LABEL_X 15
#define NAME_LABEL_Y 25
#define NAME_LABEL_SIZE 15

// Thread timeout (in ms)
#define WORLD_INTERVAL 250

/**
 * @brief A Location is a 3D point.
 * It can be linked to a speed in order to update the plane speed during a trajectory.
 */
class Location {
    private:
        float x, y, z, phi, theta, speed;
        ReferenceFrame ref_frame;

    public:
        Location(
            const float &_x = 0,
            const float &_y = 0,
            const float &_z = 0,
            const float &_speed = -1,
            const ReferenceFrame _ref = ReferenceFrame::APP1
        ) : x(_x), y(_y), z(_z), speed(_speed), phi(0), theta(0), ref_frame(_ref) {}

        /* --------------------------------- Getters -------------------------------- */

        float getX() const { return this->x; }
        float getY() const { return this->y; }
        float getZ() const { return this->z; }
        float getPhi() const { return this->phi; }
        float getTheta() const { return this->theta; }
        float getSpeed() const { return this->speed; }
        ReferenceFrame getRefFrame() const { return this->ref_frame; }

        /* --------------------------------- Setters -------------------------------- */
        
        void setX(const float& _x) { this->x = _x; }
        void setY(const float& _y) { this->y = _y; }
        void setZ(const float &_z) { this->z = _z; }
        void setPhi(const float &_phi) { this->phi = _phi; }
        void setTheta(const float &_theta) { this->theta = _theta; }
        void setSpeed(const float &_speed) { this->speed = _speed; }
        void setRefFrame(const ReferenceFrame& _ref) { this->ref_frame = _ref; }
        void setLocation(const float& _x, const float& _y, const float& _z);
        
        /**
         * @brief Convert the location to a SFML vector
         * @return Vector2f A SMFL Vector object
         */
        Vector2f toVector() const;

        /**
         * @brief Get the angle between this point and the given point in the (0, Ux, Uy) plan
         * @param l The target location
         * @return float The phi angle
         */
        float getPhiTo(const Location& l) const;

        /**
         * @brief Get the angle between this point and the given point in the (0, Uz, Uxy) plan
         * @param l The target location
         * @return float The theta angle
         */
        float getThetaTo(const Location& l) const;

        /**
         * @brief Get euclidian distance with x and y (2D)
         * @param l The target location
         * @return float The 2D distance between those points
         */
        float get2dDistanceTo(const Location& l) const;

        /**
         * @brief Get euclidian distance with x, y and z (3D)
         * @param l The target location
         * @return float The 3D distance between those points
         */
        float get3dDistanceTo(const Location& l) const;

        // Operators
        bool operator==(const Location& l) const;
        bool operator!=(const Location& l) const;
        bool operator<(const Location& l) const;
        friend ostream& operator<<(ostream& stream, const Location& l);
};

/**
 * @brief A Trajectory is list of Location.
 * It's used to move a plane to a specific destination with a custom trajectory.
 */
class Trajectory {
    private:
        vector<Location> points; // The Trajectory points
        Location* reached_point; // The last point reached (default : NULL)
        int cutting_pos; // If the trajectory must be cutted, cutting_pos contains the stop point pos, else cutting_pos = -1

        /**
         * @brief Get the given point position in the array (vector) points
         * @param l The location to search
         * @return size_t The position of the given location in the array
         */
        size_t getPointPos(const Location &l) const;

    public:
        Trajectory() : reached_point(NULL), cutting_pos(-1) {}
        Trajectory(const vector<Location>& _points) : points(_points), reached_point(NULL), cutting_pos(-1) {}
        void setCuttingPos(const int& _pos) { this->cutting_pos = _pos < this->points.size() ? _pos : -1; }
        
        /* --------------------------- Getters and setters -------------------------- */

        // Read or manage points
        Location getPointAt(const size_t& i) const { return this->points.at(i); }
        Location getLastPoint() const { return this->points.at(points.size() - 1); }
        void addPoint(const Location& new_point) { this->points.push_back(new_point); }
        
        /**
         * @brief Check if the plane has reached the first point
         * @return bool The plane started the trajectory or not
         */
        bool isStarted() const { return this->reached_point != NULL; }

        /**
         * @brief Check if the trajectory is cylic.
         * The size of list of points must be < 2 and the first point must be equal to the last.
         */
        bool isCyclic() const;

        /**
         * @brief Set all trajectory points altitude
         * @param altitude The altitude of all points
         */
        void setAltitude(const float& altitude);

        /**
         * @brief Get the near cyclic trajectory point from a given location
         * @param from The plane location
         * @return Location* The near point
         */
        size_t getNearPointPos(const Location &from);

        /* ----------------------------- Public methods ----------------------------- */

        /**
         * @brief Get the next position in the trajectory
         * @param from The start location (the plane location)
         * @param speed The plane speed (the distance to travel)
         * @param verbose If I want to print some debug data
         * @return Location The new step location
         */
        Location getNextLocation(const Location& from, const float& speed, const bool& verbose = false);

        /**
         * @brief Cut the trajectory to remove all of points after the pos argument
         * @param pos The id of the last point
         */
        void cutTrajectory(const size_t& pos);
};

/**
 * @brief A Plane is a virtual plane in our simulation.
 */
class Plane {
    private:
        string name;
        string company;
        Location location, destination;
        Trajectory trajectory;
        float speed, fuel;
        const float consumption;
        CircleShape graphical_plane;
        Text altitude_label, name_label;

    public:
        Plane(const string &_name, const Location &_spawn);
        Plane(const Location &_spawn);

        /* --------------------------------- Getters -------------------------------- */

        string getName() const { return this->name; }
        Location &getLocation() { return this->location; }
        Location getLocation() const { return this->location; }
        Location getDestination() const { return this->destination; }
        float getSpeed() const { return this->speed; }
        float getFuel() const { return this->fuel; }
        float getConsumption() const { return this->consumption; }
        Text getAltitudeLabel();
        Text getNameLabel();

        /**
         * @brief Get the plane SFML version
         * @return CircleShape The plane to display
         */
        CircleShape toSFML();

        /* --------------------------------- Setters -------------------------------- */

        void setLocation(const Location& l);
        void setDestination(const Location& d) { this->destination = d; }
        void setSpeed(const float& s) { this->speed = s; }
        void setFuel(const float& f) { this->fuel = f; }
        void setTrajectoryAltitude(const float& alt) { this->trajectory.setAltitude(alt); }

        /* ---------------------------- Public methods ---------------------------- */

        /**
         * @brief Check if the plane reached the first point
         * @return bool The plane started its trajectory
         */
        bool isTrajectoryStarted() const { return this->trajectory.isStarted(); }

        /**
         * @brief Check if the plane has reached its destination
         */
        bool isDestinationReached() const { return this->location == this->destination && !this->trajectory.isCyclic(); }

        /**
         * @brief Give a new trajectory to the plane and start it
         * @param traj The new plane trajectory
         */
        void start(const Trajectory& traj);

        /**
         * @brief Update the plane location. Manage its trajectory and fuel.
         */
        void updateLocation();

        /**
         * @brief Cut the plane trajectory with the given point pos
         * @param stop_pos The stopping point pos
         */
        void stopAt(const size_t& stop_pos);

        /* -------------------------------- Operators ------------------------------- */

        friend ostream& operator<<(ostream &stream, const Plane &plane);

        /* ---------------------- Static attributes and methods --------------------- */

        /**
         * @brief Manage plane locations each second.
         * @param planes Planes to manage
         * @param stop_prgm If the simulation must stoped
         */
        static void world(vector<Plane *> &planes, bool &stop_prgm);

        static mutex cout_lock;
        static Font default_font;
};
