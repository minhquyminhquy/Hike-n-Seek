// Datastructures.hh

#ifndef DATASTRUCTURES_HH
#define DATASTRUCTURES_HH

#include "customtypes.hh"
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <tuple>
#include <cmath>
#include <memory>
#include <iostream>
#include <deque>
#include <functional>
#include <map>

#include "customtypes.hh"

using namespace::std;

// Add your own STL includes below this comment
struct Way {
    WayID id;
    std::vector<Coord> coordinates;
    Distance length;
};

struct Crossroad {
    Coord location;
    std::unordered_set<WayID> connected_ways;
};

class Datastructures
{
public:
    Datastructures();
    ~Datastructures();

    // Estimate of performance:
    // Short rationale for estimate:
    unsigned int get_place_count();

    // Estimate of performance:
    // Short rationale for estimate:
    void clear_all();

    // Estimate of performance:
    // Short rationale for estimate:
    std::vector<PlaceID> all_places();

    // Estimate of performance:
    // Short rationale for estimate:
    bool add_place(PlaceID id, Name const& name, PlaceType type, Coord xy);

    // Estimate of performance:
    // Short rationale for estimate:
    Name get_place_name(PlaceID id);

    // Estimate of performance:
    // Short rationale for estimate:
    PlaceType get_place_type(PlaceID id);

    // Estimate of performance:
    // Short rationale for estimate:
    Coord get_place_coord(PlaceID id);

    // We recommend you implement the operations below only after implementing the ones above

    // Estimate of performance:
    // Short rationale for estimate:
    std::vector<PlaceID> get_places_alphabetically();

    // Estimate of performance:
    // Short rationale for estimate:
    std::vector<PlaceID> get_places_distance_increasing();

    // Estimate of performance:
    // Short rationale for estimate:
    std::vector<PlaceID> find_places_with_coord(Coord xy);

    // Estimate of performance:
    // Short rationale for estimate:
    std::vector<PlaceID> find_places_with_name(Name const& name);

    // Estimate of performance:
    // Short rationale for estimate:
    std::vector<PlaceID> find_places_with_type(PlaceType type);

    // Estimate of performance:
    // Short rationale for estimate:
    bool change_place_name(PlaceID id, const Name& newname);

    // Estimate of performance:
    // Short rationale for estimate:
    bool change_place_coord(PlaceID id, Coord newcoord);

    // We recommend you implement the operations below only after implementing the ones above

    // Estimate of performance:
    // Short rationale for estimate:
    bool add_area(AreaID id, Name const& name, std::vector<Coord> coords);

    // Estimate of performance:
    // Short rationale for estimate:
    std::vector<AreaID> all_areas();

    // Estimate of performance:
    // Short rationale for estimate:
    Name get_area_name(AreaID id);

    // Estimate of performance:
    // Short rationale for estimate:
    std::vector<Coord> get_area_coords(AreaID id);

    // Estimate of performance:
    // Short rationale for estimate:
    bool add_subarea_to_area(AreaID id, AreaID parentid);

    // Estimate of performance:
    // Short rationale for estimate:
    std::vector<AreaID> ancestor_areas_of_subarea(AreaID id);

    // Non-compulsory operations

    // Estimate of performance:
    // Short rationale for estimate:
    std::vector<AreaID> all_subareas_of_area(AreaID id);

    // Estimate of performance:
    // Short rationale for estimate:
    std::vector<PlaceID> get_places_closest_to(Coord xy);

    // Estimate of performance:
    // Short rationale for estimate:
    bool remove_place(PlaceID id);

    // Estimate of performance:
    // Short rationale for estimate:
    AreaID get_closest_common_ancestor_of_areas(AreaID id1, AreaID id2);


    // Estimate of performance: O(n)
    // Short rationale for estimate: traverse the unordermap in the function
    std::vector<WayID> all_ways();

    // Estimate of performance: O(n)
    // Short rationale for estimate: Insert + copy vector of coords
    bool add_way(WayID id, std::vector<Coord> coords);

    // Estimate of performance: O(n)
    // Short rationale for estimate: access adjacent nodes
    std::vector<std::pair<WayID, Coord>> get_ways_from(Coord xy);

    // Estimate of performance: O(n)
    // Short rationale for estimate: copy vector of n coordinates
    std::vector<Coord> get_way_coords(WayID id);

    // Estimate of performance: O(n)
    // Short rationale for estimate: call unordered_map.clear()
    void clear_ways();

    // Estimate of performance:  O(n^2 * logn)
    // Short rationale for estimate: BFS traversal, n times
    std::vector<std::tuple<Coord, WayID, Distance>> route_any(Coord fromxy, Coord toxy);




    // prg2-opt functions

    // Estimate of performance:
    // Short rationale for estimate:
    std::vector<std::tuple<Coord, WayID, Distance>> route_shortest_distance(Coord fromxy, Coord toxy);


    // Estimate of performance:
    // Short rationale for estimate:
    std::vector<std::tuple<Coord, WayID, Distance>> route_least_crossroads(Coord fromxy, Coord toxy);

    // Estimate of performance:
    // Short rationale for estimate:
    std::vector<std::tuple<Coord, WayID>> route_with_cycle(Coord fromxy);


    // Estimate of performance:
    // Short rationale for estimate:
    Distance trim_ways();


private:
    // Add stuff needed for your class implementation here
    struct Area;

    struct Place {
        PlaceID place_id = "";
        Name name = NO_NAME;
        PlaceType type = PlaceType::NO_TYPE;
        Coord coord = NO_COORD;
        Distance distance_from_origin = NO_VALUE;
        // Pointer of parent area
        shared_ptr<Area> in_area_ = nullptr;
    };

    struct Area {
        AreaID area_id = NO_VALUE;
        Name name = NO_NAME;
        // Vector containing coordinates of edge forming the Area
        std::vector<Coord> coords = {NO_COORD};
        // Vector containing pointers of sub areas of Area
        std::vector<std::shared_ptr<Area>> subareas = {};
        // Map of places in this area
        std::unordered_map<PlaceID,std::shared_ptr<Place>> area_places_ = {};
        // Pointer of parent area
        shared_ptr<Area> parent_ = nullptr;
    };

    struct NameComparator {
        //std::multimap<Name, PlaceID>* places_ref;

        bool operator()(std::pair<Name, PlaceID>& place1, std::pair<Name, PlaceID> place2) const {
            if (place1.first != place2.first){
                return place1.first < place2.first;
            }
            else{
                return place1.second < place2.second;
            }
        }
    };

    struct DistanceComparator
    {
        std::unordered_map<PlaceID, Place>* places_ref;

        bool operator()(const PlaceID& id1, const PlaceID& id2) const
        {
            const auto& place1 = (*places_ref).at(id1);
            const auto& place2 = (*places_ref).at(id2);

            if (place1.distance_from_origin != place2.distance_from_origin) {
                        return place1.distance_from_origin < place2.distance_from_origin;
                    }
            if (place1.name != place2.name) {
                        return place1.name < place2.name;
                    }

            return id1 < id2;
        }
    };

    struct cmpalpha {
        bool operator()(const std::pair<PlaceID, Name>& a, const std::pair<PlaceID, Name>& b) const {
            // Compare by Name first, and then by PlaceID if names are equal
            return (a.second == b.second && a.first < b.first) || a.second < b.second;
        }
    };

    // prg2 functions
    Distance calculate_way_length(const vector<Coord>& coords); // just added

    // Compute distance from (0,0) to coord(x,y)
    Distance compute_distance(const Coord& coord);
    // Database of all places
    std::unordered_map<PlaceID, shared_ptr<Place>> places_;

    // Database of all sth
    //std::unordered_map<PlaceID, shared_ptr<Way>> ways_;
    // Database of all sth
    //std::unordered_map<PlaceID, shared_ptr<Crossroad>> crossroad_;

    std::unordered_map<WayID, Way> ways_;                  // Stores all ways (key: WayID)
    std::unordered_map<Coord, Crossroad> crossroads_;       // Stores crossroads (key: Coord)

    // Database of all places based on coord
    std::multimap<Coord,PlaceID> places_coords;
    // Database of all places based on Type
    std::multimap<PlaceType, PlaceID> places_type;
    // Database of all places based on Name
    std::multimap<Name,PlaceID> places_names;
    //std::multimap<std::pair<PlaceID, Name>, PlaceID, cmpalpha> places_names;
    // Database of all places based on Distance
    std::multimap<unsigned int,PlaceID> places_dists;

    // Database of all Area
    std::unordered_map<AreaID,std::shared_ptr<Area>> areas_;

    void allsubofarea(std::shared_ptr<Area> p,std::vector<AreaID>& allsubareas);
};

#endif // DATASTRUCTURES_HH
