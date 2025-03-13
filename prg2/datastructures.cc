/**
 * @brief The Datastructures class
 * STUDENTS: Modify the code below to implement the functionality of the class.
 * Also remove comments from the parameter names when you implement an operation
 * (Commenting out parameter name prevents compiler from warning about unused
 * parameters on operations you haven't yet implemented.)
 */
 #include "datastructures.hh"
 #include "customtypes.hh"
 #include <algorithm>
 #include "stdio.h"
 #include <iostream>
 #include <map>
 #include <queue>
 #include <stack>
 #include <unordered_set>
 using namespace std;
 
 // Modify the code below to implement the functionality of the class.
 // Also remove comments from the parameter names when you implement
 // an operation (Commenting out parameter name prevents compiler from
 // warning about unused parameters on operations you haven't yet implemented.)
 Datastructures::Datastructures()
    //: places_names(NameComparator{&places_})
    //: places_names({NameComparator{&places_}})
    //, places_by_distance_({DistanceComparator{&places_}})
 {}
 
 Datastructures::~Datastructures()
 {
    clear_all();
 }
 
 unsigned int Datastructures::get_place_count()
 {
    return places_.size();
 }
 
 
 void Datastructures::clear_all()
 {
    places_.clear();
    places_coords.clear();
    places_names.clear();
    places_dists.clear();
    places_type.clear();
 
    for (auto& area: areas_){
        area.second->parent_ = nullptr;
        area.second->subareas.clear();
        area.second->area_places_.clear();
    }
 
    areas_.clear();
 }
 
 std::vector<PlaceID> Datastructures::all_places()
 {
 
    std::vector<PlaceID> place_ids = {};
 
    for (const auto& place_ptr : places_) {
        place_ids.push_back(place_ptr.second->place_id);
    }
 
    return place_ids;
 }
 
 Distance Datastructures::compute_distance(const Coord& coord)
 {
    return std::sqrt(coord.x * coord.x + coord.y * coord.y);
 }
 
 
 bool Datastructures::add_place(PlaceID id, const Name& name, PlaceType type, Coord xy)
 {
    if (places_.find(id) != places_.end()) {
        return false;
    }
 
    Distance dist = compute_distance(xy);
    // Initialize new Place with given params
    shared_ptr<Place> new_place(new Place{id, name, type, xy, dist,{}});
    // Insert new Place into database
    places_.insert({id, new_place});
    // Insert new Place into database, categorized by features
    places_names.insert({name,id});
    places_type.insert({type, id});
    //places_names.insert({{id, name}, id});
    places_dists.insert({dist,id});
    places_coords.insert({xy,id});
 
    return true;
 
 }
 
 Name Datastructures::get_place_name(PlaceID id)
 {
    auto iter = places_.find(id);
 
    if ( iter ==places_.end() )
    {
        return NO_NAME;
    }
 
    else{
        return iter->second->name;
    }
 }
 
 PlaceType Datastructures::get_place_type(PlaceID id)
 {
    auto iter = places_.find(id);
 
    if ( iter ==places_.end() )
    {
        return PlaceType::NO_TYPE;
    }
 
    return iter->second->type;
 }
 
 Coord Datastructures::get_place_coord(PlaceID id)
 {
 
    auto iter = places_.find(id);
 
    if ( iter == places_.end() )
    {
        return NO_COORD;
    }
 
    else{
        return iter->second->coord;
    }
 
 }
 
 bool comp(std::pair<Name, PlaceID>& place1, std::pair<Name, PlaceID> place2) {
    if (place1.first != place2.first){
        return place1.first < place2.first;
    }
    else{
        return place1.second < place2.second;
    }
 }
 
 std::vector<PlaceID> Datastructures::get_places_alphabetically()
 {
  vector<PlaceID> places_alphabetically = {};
 
  for (const auto& stat: places_names){
      places_alphabetically.push_back(stat.second);
  }
  /*
  vector<pair<Name, PlaceID>> v(places_names.begin(), places_names.end());
 
  sort(v.begin(), v.end(), comp);
 
  for (const auto &place : v)
  {
      places_alphabetically.push_back(place.second);
  }*/
 
  return places_alphabetically;
 }
 
 
 std::vector<PlaceID> Datastructures::get_places_distance_increasing()
 {
    std::vector<std::pair<Distance, PlaceID>> v;
 
    for (const auto& [id, place] : places_) {
        //Distance distance = std::sqrt(std::pow(place->coord.x, 2) + std::pow(place->coord.y, 2));
        Distance distance = static_cast<Distance>(place->coord.x) * place->coord.x +
                                     static_cast<Distance>(place->coord.y) * place->coord.y;
        v.emplace_back(distance, id);
    }
 
    std::sort(v.begin(), v.end(), [this](const auto& place1, const auto& place2) {
        if (place1.first != place2.first) {
            return place1.first < place2.first;
        }
        int y1 = places_.at(place1.second)->coord.y;
        int y2 = places_.at(place2.second)->coord.y;
        if (y1 != y2) {
            return y1 < y2;
        }
 
        return place1.second < place2.second;
    });
 
    std::vector<PlaceID> places_distance_increasing;
    for (const auto& [distance, id] : v) {
        places_distance_increasing.push_back(id);
    }
 
    return places_distance_increasing;
 }
 
 
 
 std::vector<PlaceID> Datastructures::find_places_with_coord(Coord target_xy)
 {
    auto iter = places_coords.find(target_xy);
 
    if (iter == places_coords.end() ){
        return {};
    }
 
    vector<PlaceID> places_with_coord = {};
 
    auto range = places_coords.equal_range(target_xy);
 
    for (auto it = range.first; it != range.second; ++it) {
            places_with_coord.push_back(it->second);
    }
 
    return places_with_coord;
 }
 
 std::vector<PlaceID> Datastructures::find_places_with_name(Name const& target_name)
 {
    auto iter = places_names.find(target_name);
    if (iter == places_names.end() ){
        return {};
    }
 
 
    vector<PlaceID> places_with_name = {};
 
    auto range = places_names.equal_range(target_name);
 
    for (auto it = range.first; it != range.second; ++it) {
            places_with_name.push_back(it->second);
    }
 
    return places_with_name;
 }
 
 std::vector<PlaceID> Datastructures::find_places_with_type(PlaceType target_type)
 {
    auto iter = places_type.find(target_type);
 
    if (iter == places_type.end() ){
        return {};
    }
 
    vector<PlaceID> places_with_type = {};
 
    auto range = places_type.equal_range(target_type);
 
    for (auto it = range.first; it != range.second; ++it) {
            places_with_type.push_back(it->second);
    }
 
    return places_with_type;
 }
 
 
 bool Datastructures::change_place_name(PlaceID id, const Name& new_name)
 {
    auto it = places_.find(id);
 
    if (it == places_.end())
    {
        return false;
    }
 
    places_names.erase(it->second->name);
    places_names.insert({new_name,id});
 
    it->second->name = new_name;
 
    return true;
 
 
 }
 
 
 bool Datastructures::change_place_coord(PlaceID id, Coord new_coord)
 {
    auto it = places_.find(id);
 
    if(it == places_.end()){
        return false;
    }
 
    places_coords.erase(it->second->coord);
    places_coords.insert({new_coord,id});
 
    // Entry in places_dists corresponding to the old distance
    auto it1 = places_dists.find(it->second->distance_from_origin);
    // Initialize another iterator it2 to track the current distance from the origin
    auto it2 = it1;
 
    // Search for the specific entry with matching distance and PlaceID to remove it from places_dists
    while( it2->first == it1->first && it2->second != id){
        ++it1;
    }
    // If an entry with the same distance and matching PlaceID is found, erase it from places_dists
    if( it2->first == it1->first && it2->second == it1->second){
        places_dists.erase(it1);
    }
 
    places_dists.insert({compute_distance(new_coord),id});
 
    it->second->coord = new_coord; // Update
 
    return true;
 
 }
 
 
 bool Datastructures::add_area(AreaID id, const Name& name, std::vector<Coord> coords)
 {
    if(areas_.find(id) != areas_.end()){
        return false;
    }
    // Initialize new_area with given parameters, then add to areas_
    shared_ptr<Area> new_area(new Area{id,name,coords,{},{},nullptr});
    areas_.insert({id,new_area});
 
    return true;
 
 }
 
 std::vector<AreaID> Datastructures::all_areas()
 {
    std::vector<AreaID> all_areas = {};
 
    for(const auto& area : areas_){
        all_areas.push_back(area.first);
    }
 
    return all_areas;
 }
 
 Name Datastructures::get_area_name(AreaID id)
 {
    const auto& area = areas_.find(id);
 
    if( area == areas_.end()){
        return NO_NAME;
    }
 
    return area->second->name;
 
 }
 
 std::vector<Coord> Datastructures::get_area_coords(AreaID id)
 {
    if(areas_.find(id) == areas_.end()){
        return {NO_COORD};
    }
 
    return areas_[id]->coords;
 }
 
 bool Datastructures::add_subarea_to_area(AreaID subarea_id, AreaID parent_area_id)
 {
    auto parent_iter = areas_.find(parent_area_id);
    if (parent_iter == areas_.end()) {
        return false;  // Parent area not found
    }
 
    auto subarea_iter = areas_.find(subarea_id);
    if (subarea_iter == areas_.end()) {
        return false;  // Subarea not found
    }
 
    if (subarea_iter->second->parent_) {
        return false;
    }
    auto current_area = parent_iter->second;
    while (current_area->parent_) {
        if (current_area == subarea_iter->second) {
            return false;
        }
        current_area = current_area->parent_;
    }
 
    parent_iter->second->subareas.push_back(subarea_iter->second);
 
    subarea_iter->second->parent_ = parent_iter->second;
 
    return true;
 
 }
 
 std::vector<AreaID> Datastructures::ancestor_areas_of_subarea(AreaID id)
 {
    const auto& child_it = areas_.find(id);
 
    if(child_it == areas_.end()){
        return {NO_AREA}; // Area not exist
    }
 
    vector<AreaID> ancestors;
    auto current_area = child_it->second;
 
    while (current_area->parent_ != nullptr) {
        auto parent = current_area->parent_;
        ancestors.push_back(parent->area_id);
        current_area = parent;
    }
 
    return ancestors;
 }
 
 void Datastructures::allsubofarea(std::shared_ptr<Area> p,std::vector<AreaID>& allsubareas){
 
    //for region in p:s subregion
    if(p->subareas.size()==0){
        return;
    }
    else {
        for(const auto& area : p->subareas){
            allsubareas.push_back(area->area_id);
            allsubofarea(area,allsubareas);
        }
        return;
    }
 }
 
 std::vector<AreaID> Datastructures::all_subareas_of_area(AreaID id)
 {
    auto area = areas_.find(id);
    if(area==areas_.end()){
        return {NO_AREA};
    }
    else{
        std::vector<AreaID> allsubregs = {};
        allsubofarea(areas_[id],allsubregs);
        return allsubregs;
    }
 }
 
 int find_distance(Coord c1,Coord c2){
    return (c1.x-c2.x)*(c1.x-c2.x) + (c1.y-c2.y)*(c1.y-c2.y);
 }
 
 std::vector<PlaceID> Datastructures::get_places_closest_to(Coord xy)
 {
    std::vector<PlaceID> closest_place_ids = {};
    std::multimap<int,PlaceID> closest = {};
    for(const auto& place : places_){
        int dist = find_distance(place.second->coord,xy);
        closest.insert({dist,place.second->place_id});    //O(log(n))
    }
 
    for(const auto& place : closest){
        if (closest_place_ids.size()==3){
            break;
        }
        else {
            closest_place_ids.push_back(place.second);    //O(1)
        }
    }
 
    return closest_place_ids;
 }
 
 bool Datastructures::remove_place(PlaceID id)
 {
    auto i = places_.find(id);
    if (i == places_.end()) {
        return false;
    }
    auto name_range = places_names.equal_range(i->second->name);
    for (auto iter = name_range.first; iter != name_range.second; ++iter) {
        if (iter->second == id) {
            places_names.erase(iter);
            break;
        }
    }
    auto coord_range = places_coords.equal_range(i->second->coord);
    for (auto iter = coord_range.first; iter != coord_range.second; ++iter) {
        if (iter->second == id) {
            places_coords.erase(iter);
            break;
        }
    }
    auto type_range = places_type.equal_range(i->second->type);
    for (auto iter = type_range.first; iter != type_range.second; ++iter) {
        if (iter->second == id) {
            places_type.erase(iter);
            break;
        }
    }
    Distance d = compute_distance(i->second->coord);
    auto dist_range = places_dists.equal_range(d);
    for (auto iter = dist_range.first; iter != dist_range.second; ++iter) {
        if (iter->second == id) {
            places_dists.erase(iter);
            break;
        }
    }
    places_.erase(i);
 
    return true;
 }
 
 AreaID Datastructures::get_closest_common_ancestor_of_areas(AreaID id1, AreaID id2)
 {
    if (areas_.find(id1) == areas_.end() || areas_.find(id2) == areas_.end()) {
        return NO_AREA;
    }
    shared_ptr<Area> area1 = areas_[id1];
    shared_ptr<Area> area2 = areas_[id2];
    unordered_set<AreaID> ancestors_of_area1;
 
    shared_ptr<Area> current_ptr = area1->parent_;
    while (current_ptr != nullptr) {
        ancestors_of_area1.insert(current_ptr->area_id);
        current_ptr = current_ptr->parent_;
    }
 
    current_ptr = area2->parent_;
    while (current_ptr != nullptr) {
        if (ancestors_of_area1.find(current_ptr->area_id) != ancestors_of_area1.end()) {
            return current_ptr->area_id;
        }
        current_ptr = current_ptr->parent_;
    }
 
    return NO_AREA;
 }
 
 
 /**
  * ============================
  * ============================
  * ========== PRG2 ============
  * ============================
  * ============================
  */
 
 void Datastructures::clear_ways()
 {
     ways_.clear();
     crossroads_.clear();
 }
 
 std::vector<WayID> Datastructures::all_ways()
 {
     std::vector<WayID> way_ids;
     for (const auto& way_pair : ways_) {
         way_ids.push_back(way_pair.first);
     }
     return way_ids;
 }
 
 bool Datastructures::add_way(WayID id, std::vector<Coord> coords)
 {
     if (ways_.count(id) > 0) return false;
 
     Distance length = calculate_way_length(coords);
     Way new_way = {id, coords, length};
     ways_[id] = new_way;
 
     if (!coords.empty()) {
         crossroads_[coords.front()].connected_ways.insert(id);
         crossroads_[coords.back()].connected_ways.insert(id);
     }
 
     return true;
 }
 
 std::vector<std::pair<WayID, Coord>> Datastructures::get_ways_from(Coord xy)
 {
     std::vector<std::pair<WayID, Coord>> result;
     if (crossroads_.find(xy) == crossroads_.end()) return result;
 
     for (const WayID& way_id : crossroads_[xy].connected_ways) {
         Way& way = ways_.at(way_id);
         if (way.coordinates.front() == xy) {
             result.emplace_back(way_id, way.coordinates.back());
         } else if (way.coordinates.back() == xy) {
             result.emplace_back(way_id, way.coordinates.front());
         }
     }
 
     return result;
 }
 
 std::vector<Coord> Datastructures::get_way_coords(WayID id)
 {
     if (ways_.find(id) == ways_.end()) return {NO_COORD};
     return ways_[id].coordinates;
 }
 
 std::vector<std::tuple<Coord, WayID, Distance>> Datastructures::route_any(Coord fromxy, Coord toxy)
 {
     if (crossroads_.find(fromxy) == crossroads_.end() || crossroads_.find(toxy) == crossroads_.end()) {
         return {{NO_COORD, NO_WAY, NO_DISTANCE}};
     }
 
     std::queue<std::tuple<Coord, WayID, Distance>> queue;
     std::unordered_map<Coord, Distance> distances;
     std::unordered_map<Coord, std::tuple<Coord, WayID>> predecessors;
 
     queue.push({fromxy, NO_WAY, 0});
     distances[fromxy] = 0;
 
     while (!queue.empty()) {
         auto [current, way_id, distance] = queue.front();
         queue.pop();
 
         if (current == toxy) break;
 
         for (const auto& [next_way, next_coord] : get_ways_from(current)) {
             Way& way = ways_.at(next_way);
 
             Distance way_distance = 0;
             if (way.coordinates.front() == current) {
 
                 way_distance = calculate_way_length({way.coordinates.begin(), std::find(way.coordinates.begin(), way.coordinates.end(), next_coord) + 1});
             } else if (way.coordinates.back() == current) {
                 way_distance = calculate_way_length({way.coordinates.rbegin(), std::find(way.coordinates.rbegin(), way.coordinates.rend(), next_coord) + 1});
             }
 
             Distance new_distance = distance + way_distance;
 
             if (distances.find(next_coord) == distances.end() || new_distance < distances[next_coord]) {
                 distances[next_coord] = new_distance;
                 queue.push({next_coord, next_way, new_distance});
                 predecessors[next_coord] = {current, next_way};
             }
         }
     }
 
     if (distances.find(toxy) == distances.end()) return {};
 
     std::vector<std::tuple<Coord, WayID, Distance>> route;
     Coord current = toxy;
     while (current != fromxy) {
         auto [prev_coord, way_id] = predecessors[current];
         route.emplace_back(current, way_id, distances[current]);
         current = prev_coord;
     }
     route.emplace_back(fromxy, NO_WAY, 0);
     std::reverse(route.begin(), route.end());
     return route;
 }
 
 
 Distance Datastructures::calculate_way_length(const std::vector<Coord>& coords)
 {
     Distance total_distance = 0;
     for (size_t i = 1; i < coords.size(); ++i) {
         int dx = coords[i].x - coords[i - 1].x;
         int dy = coords[i].y - coords[i - 1].y;
         total_distance += static_cast<Distance>(std::sqrt(dx * dx + dy * dy));
     }
     return total_distance;
 }
 
 // prg2-opt functions
 
 std::vector<std::tuple<Coord, WayID, Distance>> Datastructures::route_shortest_distance(Coord fromxy, Coord toxy)
 {
 
     // Update: remove string
     if (crossroads_.find(fromxy) == crossroads_.end() || crossroads_.find(toxy) == crossroads_.end()) {
         return {{NO_COORD, NO_WAY, NO_DISTANCE}};
     }
 
     std::priority_queue<std::tuple<Distance, Coord, WayID>,
                         std::vector<std::tuple<Distance, Coord, WayID>>,
                         std::greater<>> queue;
 
     std::unordered_map<Coord, Distance> distances;
     std::unordered_map<Coord, std::tuple<Coord, WayID, std::string>> predecessors; // Store way name as well
 
     distances[fromxy] = 0;
     queue.push({0, fromxy, NO_WAY});
 
     while (!queue.empty()) {
         auto [current_distance, current_coord, current_way] = queue.top();
         queue.pop();
 
         if (current_coord == toxy) break;
 
         for (const auto& [next_way, next_coord] : get_ways_from(current_coord)) {
             Distance way_distance = calculate_way_length(ways_[next_way].coordinates);
             Distance new_distance = current_distance + way_distance;
 
             if (distances.find(next_coord) == distances.end() || new_distance < distances[next_coord]) {
                 distances[next_coord] = new_distance;
                 queue.push({new_distance, next_coord, next_way});
                 std::string way_name = ways_[next_way].id;
                 predecessors[next_coord] = {current_coord, next_way, way_name};
             }
         }
     }
 
     if (distances.find(toxy) == distances.end()) return {{NO_COORD, NO_WAY, NO_DISTANCE}};
 
     std::vector<std::tuple<Coord, WayID, Distance>> route;
     Coord current = toxy;
     while (current != fromxy) {
         auto [prev_coord, way_id, way_name] = predecessors[current];
         route.emplace_back(current, way_id, distances[current], way_name);
         current = prev_coord;
     }
     route.emplace_back(fromxy, NO_WAY, 0, "");
     std::reverse(route.begin(), route.end());
 
     return route;
 }
 
 
 
 std::vector<std::tuple<Coord, WayID, Distance>> Datastructures::route_least_crossroads(Coord fromxy, Coord toxy)
 {
     if (crossroads_.find(fromxy) == crossroads_.end() || crossroads_.find(toxy) == crossroads_.end()) {
         return {{NO_COORD, NO_WAY, NO_DISTANCE}};
     }
 
     std::queue<std::tuple<Coord, int, WayID>> queue;
     std::unordered_map<Coord, std::tuple<Coord, WayID>> predecessors;
     std::unordered_map<Coord, int> visited_crossroads;
 
     queue.push({fromxy, 0, NO_WAY});
     visited_crossroads[fromxy] = 0;
 
     while (!queue.empty()) {
         auto [current_coord, crossroad_count, current_way] = queue.front();
         queue.pop();
 
         if (current_coord == toxy) break;
 
         for (const auto& [next_way, next_coord] : get_ways_from(current_coord)) {
             int next_crossroad_count = crossroad_count + 1;
             if (visited_crossroads.find(next_coord) == visited_crossroads.end() || next_crossroad_count < visited_crossroads[next_coord]) {
                 queue.push({next_coord, next_crossroad_count, next_way});
                 visited_crossroads[next_coord] = next_crossroad_count;
                 predecessors[next_coord] = {current_coord, next_way};
             }
         }
     }
 
     if (predecessors.find(toxy) == predecessors.end()) return {{NO_COORD, NO_WAY, NO_DISTANCE}};
 
     std::vector<std::tuple<Coord, WayID, Distance>> route;
     Coord current = toxy;
     while (current != fromxy) {
         auto [prev_coord, way_id] = predecessors[current];
         route.emplace_back(current, way_id, visited_crossroads[current]);
         current = prev_coord;
     }
     route.emplace_back(fromxy, NO_WAY, 0);
     std::reverse(route.begin(), route.end());
 
     return route;
 }
 
 std::vector<std::tuple<Coord, WayID>> Datastructures::route_with_cycle(Coord fromxy)
 {
     if (crossroads_.find(fromxy) == crossroads_.end()) return {};
 
     std::unordered_set<Coord> visited;
     std::stack<std::pair<Coord, WayID>> stack;
     std::unordered_map<Coord, Coord> parent;
 
     stack.push({fromxy, NO_WAY});
     parent[fromxy] = fromxy;
 
     while (!stack.empty()) {
         auto [current_coord, current_way] = stack.top();
         stack.pop();
 
         if (visited.find(current_coord) != visited.end()) {
             std::vector<std::tuple<Coord, WayID>> cycle;
             Coord cycle_node = current_coord;
 
             while (true) {
                 auto parent_coord = parent[cycle_node];
                 cycle.emplace_back(cycle_node, current_way);
                 if (cycle_node == fromxy && cycle.size() > 1) break;
                 cycle_node = parent_coord;
             }
 
             std::reverse(cycle.begin(), cycle.end());
             return cycle;
         }
 
         visited.insert(current_coord);
 
         for (const auto& [next_way, next_coord] : get_ways_from(current_coord)) {
             if (parent.find(next_coord) == parent.end()) {
                 stack.push({next_coord, next_way});
                 parent[next_coord] = current_coord;
             }
         }
     }
 
     return {};
 }
 
 Distance Datastructures::trim_ways()
 {
     std::unordered_set<WayID> removed_ways;
     bool has_removed = true;
 
     while (has_removed) {
         has_removed = false;
 
         for (auto it = crossroads_.begin(); it != crossroads_.end(); ) {
             if (it->second.connected_ways.size() == 1) {
                 WayID single_way = *it->second.connected_ways.begin();
                 crossroads_[ways_[single_way].coordinates.front()].connected_ways.erase(single_way);
                 crossroads_[ways_[single_way].coordinates.back()].connected_ways.erase(single_way);
                 ways_.erase(single_way);
                 it = crossroads_.erase(it);
                 removed_ways.insert(single_way);
                 has_removed = true;
             } else {
                 ++it;
             }
         }
     }
 
     Distance total_distance = 0;
     for (const auto& way_id : removed_ways) {
         total_distance += ways_[way_id].length;
     }
 
     return total_distance;
 }
 
 