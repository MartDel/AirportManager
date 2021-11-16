#define _USE_MATH_DEFINES

#include <iostream>
#include <vector>
#include <map>
#include <queue>
#include <cmath>
#include <thread>
#include <mutex>
#include <chrono>

using namespace std;

#define DEFAULT_CONSUMPTION 0.25

class Location {
    private:
        float x, y, z, speed;

    public:
        Location(const float& _x = 0, const float& _y = 0, const float& _z = 0, const float& _speed = -1)
         : x(_x), y(_y), z(_z), speed(_speed) {}
        float getX() const { return this->x; }
        float getY() const { return this->y; }
        float getZ() const { return this->z; }
        float getSpeed() const { return this->speed; }
        void setX(const float& _x) { this->x = _x; }
        void setY(const float& _y) { this->y = _y; }
        void setZ(const float& _z) { this->z = _z; }
        void setSpeed(const float& _speed) { this->speed = _speed; }
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
        Trajectory() : reached_point(NULL) {}
        Trajectory(const vector<Location>& _points) : points(_points), reached_point(NULL) {}
        Location getPoint(const size_t& i) const { return points.at(i); }
        Location getLastPoint() const { return points.at(points.size() - 1); }
        size_t getPointPos(const Location& l) const;
        bool isCyclic() const;

        // Get the next position in the trajectory
        Location getNextLocation(const Location& from, const float& speed, const bool& verbose = false);
};

class Plane {
    private:
        const string name;
        Location location, destination;
        Trajectory trajectory;
        float speed, fuel;
        const float consumption;
        size_t parking_spot, state;
        // State codes
        // 0 : parked
        // 1 : taking off
        // 2 : taked off

    public:
        Plane(const string& _name, const Location& _spawn, const size_t& parking_spot);

        // Getters
        string getName() const { return this->name; }
        Location getLocation() const { return this->location; }
        Location getDestination() const { return this->destination; }
        float getSpeed() const { return this->speed; }
        float getFuel() const { return this->fuel; }
        float getConsumption() const { return this->consumption; }
        size_t getParkingSpot() const { return this->parking_spot; }
        size_t getState() const { return this->state; }

        // Setters
        void setLocation(const Location& l);
        void setDestination(const Location& d) { this->destination = d; }
        void setSpeed(const float& s) { this->speed = s; }
        void setFuel(const float& f) { this->fuel = f; }
        void setState(const size_t& s) { this->state = s; }
        void setParkingSpot(const size_t& i) { this->parking_spot = i; }

        bool isDestinationReached() const { return this->location == this->destination && !this->trajectory.isCyclic(); }

        void start(const Trajectory& traj);
        void updateLocation();

        friend ostream& operator<<(ostream& stream, const Plane& plane);

};
