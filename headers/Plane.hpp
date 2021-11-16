#define _USE_MATH_DEFINES

#include <iostream>
#include <vector>
#include <map>
#include <queue>
#include <cmath>

using namespace std;

#define MAX_FUEL 1000
#define DEFAULT_CONSUMPTION 0.5

class Location {
    private:
        float x, y, z;

    public:
        Location(const float& _x = 0, const float& _y = 0, const float& _z = 0) : x(_x), y(_y), z(_z) {}
        float getX() const { return this->x; }
        float getY() const { return this->y; }
        float getZ() const { return this->z; }
        void setX(const float& _x) { this->x = _x; }
        void setY(const float& _y) { this->y = _y; }
        void setZ(const float& _z) { this->z = _z; }
        void setLocation(const float& _x, const float& _y, const float& _z);

        // Get the angle between this point and the given point in the (x, y) plan
        float getPhiTo(const Location& l) const;
        // Get the angle between this point and the given point in the (z, sqrt(x²+y²)) plan
        float getThetaTo(const Location& l) const;
        // Get euclidian distance with x and y
        float get2dDistanceTo(const Location& l) const;
        // Get euclidian distance with x, y and z
        float get3dDistanceTo(const Location& l) const;

        // Operators
        bool operator==(const Location& l) const;
        bool operator!=(const Location& l) const;
        friend ostream& operator<<(ostream& stream, const Location& l);
};

class Trajectory {
    private:
        vector<Location> points;
        Location* reached_point;
    public:
        Trajectory(const vector<Location>& _points) : points(_points), reached_point(NULL) {}
        Location getPoint(const size_t& i) const { return points.at(i); }
        Location getLastPoint() const { return points.at(points.size() - 1); }
        size_t getPointPos(const Location& l) const;
        bool isCyclic() const { return this->points.at(0) == this->getLastPoint(); }

        // Get the next position in the trajectory
        Location getNextLocation(const Location& from, const float& speed, const bool& verbose = false);
};

class Plane {
    private:
        const string name;
        Location location, destination;
        Trajectory trajectory;
        float speed, fuel, consumption;

    public:
        Plane(const string& _name, const Location& _spawn, const Trajectory& _traj);
        bool isDestinationReached() const { return this->location == this->destination && !trajectory.isCyclic(); }

        void start();
        void updateLocation();
};
