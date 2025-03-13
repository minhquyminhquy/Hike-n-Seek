// Datastructures.hh

#ifndef DATASTRUCTURES_HH
#define DATASTRUCTURES_HH

#include <vector>

// Add your own STL includes below this comment
#include <unordered_map>
#include <map>
#include <set>
#include <cmath>
#include <memory>
#include <iostream>
#include <deque>
#include <functional>
// Add your own STL includes above this comment

#include "customtypes.hh"

using namespace std;

// This is the class you are supposed to implement

class Datastructures
{
public:
    Datastructures();
    ~Datastructures();

    // Estimate of performance: O(1)
    // Short rationale for estimate: The size() function of an unordered_map has constant time complexity,
    // as it simply returns the stored size of the container.
    unsigned int get_place_count();

    // Estimate of performance: O(n)
    // Short rationale for estimate: The clear() operations on containers (unordered_map, vector) each have linear time complexity
    // relative to the number of elements in the container (n). The loop through areas_ also iterates over all areas (n), performing
    // constant-time operations for each area.
    void clear_all();

    // Estimate of performance: O(n)
    // Short rationale for estimate: The loop iterates over all the places stored in the places_ unordered_map, and for each place,
    // the place_id is added to the vector. The time complexity of iterating through all elements in an unordered_map is O(n), where
    // n is the number of places.
    std::vector<PlaceID> all_places();

    // Estimate of performance: O(log(n))
    // Short rationale for estimate: find check is O(1), insertions into maps are O(log(n))
    bool add_place(PlaceID id, Name const& name, PlaceType type, Coord xy);

    // Estimate of performance: O(log(n))
    // Short rationale for estimate: find operation in the map is O(log(n))
    Name get_place_name(PlaceID id);


    // Estimate of performance: O(log(n))
    // Short rationale for estimate: find operation in the map is O(log(n))
    PlaceType get_place_type(PlaceID id);

    // Estimate of performance: O(log(n))
    // Short rationale for estimate: find operation in the map is O(log(n))
    Coord get_place_coord(PlaceID id);

    // We recommend you implement the operations below only after implementing the ones above

    // Estimate of performance: O(n * log(n))
    // Short rationale for estimate: sorting the vector of pairs takes O(n * log(n))
    std::vector<PlaceID> get_places_alphabetically();

    // Estimate of performance: O(n * log(n))
    // Short rationale for estimate: sorting the vector of pairs takes O(n * log(n)), and inserting pairs takes O(n)
    std::vector<PlaceID> get_places_distance_increasing();

    // Estimate of performance: O(n)
    // Short rationale for estimate: equal_range takes O(log(n)) and iterating over the range takes O(n) in the worst case
    std::vector<PlaceID> find_places_with_coord(Coord xy);

    // Estimate of performance: O(n)
    // Short rationale for estimate: equal_range takes O(log(n)) and iterating over the range takes O(n) in the worst case
    std::vector<PlaceID> find_places_with_name(Name const& name);

    // Estimate of performance: O(n)
    // Short rationale for estimate: equal_range takes O(log(n)) and iterating over the range takes O(n) in the worst case
    std::vector<PlaceID> find_places_with_type(PlaceType type);

    // Estimate of performance: O(n)
    // Short rationale for estimate: equal_range takes O(log(n)) and iterating over the range takes O(n) in the worst case
    bool change_place_name(PlaceID id, const Name& newname);

    // Estimate of performance: O(log(n)) + O(log(n))
    // Short rationale for estimate: places_names.erase and places_names.insert are O(log(n)), and find is O(log(n))
    bool change_place_coord(PlaceID id, Coord newcoord);

    // We recommend you implement the operations below only after implementing the ones above

    // Estimate of performance: O(log(n))
    // Short rationale for estimate: places_coords.erase and places_coords.insert are O(log(n)), places_dists.erase and places_dists.insert are O(log(n)), and find operations are O(log(n))
    bool add_area(AreaID id, Name const& name, std::vector<Coord> coords);

    // Estimate of performance: O(n)
    // Short rationale for estimate: areas_.find and areas_.insert are O(log(n)) due to the use of a map (or multimap).
    std::vector<AreaID> all_areas();

    // Estimate of performance: O(n)
    // Short rationale for estimate: Finding the area by ID in the unordered_map takes constant time, O(1).e areas_ map is O(n), where n is the number of areas.
    Name get_area_name(AreaID id);

    // Estimate of performance: O(n)
    // Short rationale for estimate: Finding the area by ID in the unordered_map takes constant time, O(1),
    // and accessing the vector of coordinates is O(1).
    std::vector<Coord> get_area_coords(AreaID id);

    // Estimate of performance: O(n)
    // Short rationale for estimate: The function performs a search through the areas_ map, which takes O(m),
    // then traverses the parent chain of the subarea, which can take up to O(n) in the worst case where n is the depth of the hierarchy.
    bool add_subarea_to_area(AreaID id, AreaID parentid);

    // Estimate of performance: O(n)
    // Short rationale for estimate: The function traverses the chain of parent areas, which takes O(n) where n is the depth of the hierarchy.
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


    /*
    struct NameComparator
    {
        std::unordered_map<PlaceID, Place>* places_ref;

        if (*places_ref.size() == 0)
        {
            return false;
        }

        bool operator()(const PlaceID& id1, const PlaceID& id2) const
        {
            const Name& name1 = (*places_ref)[id1].name;
            const Name& name2 = (*places_ref)[id2].name;
            return name1 < name2 || (name1 == name2 && id1 < id2);
        }
    };*/
    /*
    struct NameComparator {
        std::unordered_map<PlaceID, std::shared_ptr<Place>>* places_ref;

        bool operator()(const PlaceID& id1, const PlaceID& id2) const {
            // Check if both IDs exist in the map
            auto it1 = places_ref->find(id1);
            auto it2 = places_ref->find(id2);

            // If either of the IDs is not found, return a fallback order
            if (it1 == places_ref->end()) {
                return false;  // id1 doesn't exist, consider it "smaller" (or use other behavior)
            }
            if (it2 == places_ref->end()) {
                return false; // id2 doesn't exist, consider id2 "smaller"
            }

            // Dereference the shared_ptr to access the Name
            const Name& name1 = it1->second->name;
            const Name& name2 = it2->second->name;

            if (name1 == name2){
                return id1 < id2;
            }

            else {
                return name1 < name2;
            }

            // Compare by Name, and if they are equal, compare by PlaceID
            //return name1 < name2 || (name1 == name2 && id1 < id2);
        }
    };
    */

    /*
    struct NameComparator {
        std::unordered_map<PlaceID, std::shared_ptr<Place>>* places_ref;

        bool operator()(const PlaceID& id1, const PlaceID& id2) const {
            // Check if both IDs exist in the map
            auto it1 = places_ref->find(id1);
            auto it2 = places_ref->find(id2);

            // Assume all IDs in the map are valid. If IDs are missing, ensure data integrity before insertion.
            if (it1 == places_ref->end() || it2 == places_ref->end()) {
                return id1 < id2;  // Default fallback: compare by PlaceID if data is missing
            }

            // Dereference the shared_ptr to access the Name
            const Name& name1 = it1->second->name;
            const Name& name2 = it2->second->name;

            // First, compare by name (lexicographical order)
            if (name1 < name2) {
                return true;
            }
            if (name1 > name2) {
                return false;
            }
            if (name1 == name2)
            {

            }
            // If names are equal, compare by PlaceID
            const Name& id1_ = it1->second->place_id;
            const Name& id2_ = it2->second->place_id;
            return (id1_ > id2_);
        } */
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



    // Compute distance from (0,0) to coord(x,y)
    Distance compute_distance(const Coord& coord);
    // Database of all places
    std::unordered_map<PlaceID, shared_ptr<Place>> places_;
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
