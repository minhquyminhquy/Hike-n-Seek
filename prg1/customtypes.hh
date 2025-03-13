#include <string>
#include <limits>
#include <tuple>
#include <exception>

#ifndef CUSTOMTYPES_HH
#define CUSTOMTYPES_HH

// Types for IDs
using PlaceID = std::string;
using AreaID = unsigned long long int;
using Name = std::string;

// Return values for cases where required thing was not found
static PlaceID const NO_PLACE = "---";
static AreaID const NO_AREA = -1;
static Name const NO_NAME = "!NO_NAME!";

// Return value for cases where integer values were not found
long long int const NO_VALUE = std::numeric_limits<long long int>::min();

// Enumeration for different place types
// !!Note since this is a C++11 "scoped enumeration", you'll have to refer to
// individual values as PlaceType::SHELTER etc.
enum class PlaceType { OTHER=0, FIREPIT, SHELTER, PARKING, PEAK, BAY, AREA, NO_TYPE };

// Type for a coordinate (x, y)
struct Coord
{
    long long int x = NO_VALUE;
    long long int y = NO_VALUE;
    // equality operator for usage as unordered_map key or unordered_set item
    bool operator==(Coord c2) const { return x == c2.x && y == c2.y; }
    bool operator!=(Coord c2) const { return !(Coord{x,y}==c2); }
    // Comparison function(s) to allow usage as map key and set item
    // please notice that these will not result the required order for
    // distance_increasing
    bool operator<(Coord c2) const
    {
        return std::tie(y,x)<std::tie(c2.y,c2.x);
    }
    bool operator>(Coord c2) const
    {
        return std::tie(y,x)>std::tie(c2.y,c2.x);
    }

    bool operator>=(Coord c2) const
    {
        return !(c2<Coord{x,y});
    }
    bool operator<=(Coord c2) const
    {
        return !(Coord{x,y}>c2);
    }

};

// hash function for Coordinate so that it can be used as unordered_map key or
// unordered_set item
template <> struct std::hash<Coord>
{
    size_t operator()(const Coord& xy) const
    {
        auto hasher = std::hash<int>();
        auto xhash = hasher(xy.x);
        auto yhash = hasher(xy.y);
        // Combine hash values (magic!)
        return xhash ^ (yhash + 0x9e3779b9 + (xhash << 6) + (xhash >> 2));
    }
};

// Return value for cases where coordinates were not found
Coord const NO_COORD = {NO_VALUE, NO_VALUE};

// Type for a distance (in metres)
using Distance = long long int;

// Return value for cases where Distance is unknown
Distance const NO_DISTANCE = NO_VALUE;

// This exception class is there just so that the user interface can notify
// about operations which are not (yet) implemented
class NotImplemented : public std::exception
{
public:
    NotImplemented() : msg_{} {}
    explicit NotImplemented(std::string const& msg):
        msg_{msg + " not implemented"} {}

    virtual const char* what() const noexcept override
    {
        return msg_.c_str();
    }
private:
    std::string msg_;
};


#endif // CUSTOMTYPES_HH
