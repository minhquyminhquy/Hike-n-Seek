// The main program (provided by the course), COMP.CS.300
//
// DO ****NOT**** EDIT THIS FILE!
// (Preferably do not edit this even temporarily. And if you still decide to do so
//  (for debugging, for example), DO NOT commit any changes to git, but revert all
//  changes later. Otherwise you won't be able to get any updates/fixes to this
//  file from git!)

#include <string>
using std::string;
using std::getline;

#include <iostream>
using std::cout;
using std::cin;
using std::cerr;
using std::endl;
using std::flush;
using std::noskipws;

#include <iomanip>
using std::setfill;
using std::setw;

#include <istream>
using std::istream;

#include <ostream>
using std::ostream;

#include <fstream>
using std::ifstream;

#include <sstream>
using std::istringstream;
using std::ostringstream;
using std::stringstream;

#include <iomanip>
using std::setw;

#include <tuple>
using std::tuple;
using std::make_tuple;
using std::get;
using std::tie;

#include <regex>
using std::regex_match;
using std::regex_search;
using std::smatch;
using std::regex;
using std::sregex_token_iterator;

#include <algorithm>
using std::find_if;
using std::find;
using std::binary_search;
using std::max_element;
using std::max;
using std::min;
using std::shuffle;
using std::sort;

#include <random>
using std::minstd_rand;
using std::uniform_int_distribution;

#include <chrono>

#include <functional>
using std::function;
using std::equal_to;

#include <vector>
using std::vector;

#include <set>
using std::set;

#include <array>
using std::array;

#include <bitset>
using std::bitset;

#include <iterator>
using std::next;

#include <ctime>
using std::time;

#include <memory>
using std::move;

#include <utility>
using std::pair;
using std::make_pair;

#include <cmath>
using std::abs;

#include <cstdlib>
using std::div;

#include <algorithm>
using std::transform;

#include <iterator>
using std::back_inserter;

#include <cstddef>
#include <cassert>


#include "mainprogram.hh"

#include "datastructures.hh"

#ifdef GRAPHICAL_GUI
#include "mainwindow.hh"
#endif


// for perftests, to execute only the needed things
const std::set<std::string> place_tests = {"get_place_name", "get_place_coord", "all_places", "get_places_alphabetically", "get_places_distance_increasing", "get_places_closest_to", "find_places_with_coord", "change_place_coord", "remove_place"};
const std::set<std::string> area_tests = {"get_area_coords", "get_area_name", "all_subareas_of_area", "get_closest_common_ancestor_of_areas"};
const std::set<std::string> place_area_tests = {};



string const MainProgram::PROMPT = "> ";

template<typename C, typename F> size_t remove_if_from(C& container, F fun){
    auto old_size = container.size();
    for (auto first = container.begin();first!=container.end();){
        if (fun(*first)){
            first=container.erase(first);
        }else{
            ++first;
        }
    }
    return old_size-container.size();
}

template<typename T> void unstable_vector_erase(std::vector<T>& vector, const T& to_remove){
    auto findit = std::find(vector.begin(),vector.end(),to_remove);
    if(findit == vector.end()){
        return;
    }
    std::swap(*findit,vector.back());
    vector.pop_back();
}


MainProgram::CmdResult MainProgram::cmd_add_place(ostream& /*output*/, MatchIter begin, MatchIter end)
{
    PlaceID id = *begin++;
    string name = *begin++;
    string typestr = *begin++;
    string xstr = *begin++;
    string ystr = *begin++;
    assert( begin == end && "Impossible number of parameters!");

    PlaceType type = convert_string_to_placetype(typestr);
    if (type == PlaceType::NO_TYPE)
    {
        //output << "Impossible place type: " << typestr << endl;
        return {ResultType::IDLIST, CmdResultIDs{{}, {NO_PLACE}}};
    }

    int x = convert_string_to<int>(xstr);
    int y = convert_string_to<int>(ystr);

    bool success = ds_.add_place(id, name, type, {x, y});

    view_dirty = true;
    return {ResultType::IDLIST, CmdResultIDs{{}, {success ? id : NO_PLACE}}};
}

MainProgram::CmdResult MainProgram::cmd_get_place_name(std::ostream &/*output*/, MatchIter begin, MatchIter end)
{
    PlaceID id = *begin++;
    assert( begin == end && "Impossible number of parameters!");
    Name place_name = ds_.get_place_name(id);
    return {ResultType::NAMELIST, CmdResultNames{{id,NO_AREA},place_name}};
}

MainProgram::CmdResult MainProgram::cmd_get_place_type(std::ostream &/*output*/, MatchIter begin, MatchIter end)
{
    PlaceID id = *begin++;
    assert( begin == end && "Impossible number of parameters!");
    PlaceType place_type = ds_.get_place_type(id);
    //return {ResultType::NAMELIST, CmdResultNames{{id,NO_AREA},place_type}};
    return {ResultType::TYPELIST, CmdResultPlaceTypes{{id,NO_AREA},place_type}};
}

MainProgram::CmdResult MainProgram::cmd_get_area_name(std::ostream &/*output*/, MatchIter begin, MatchIter end)
{
    AreaID id = convert_string_to<AreaID>(*begin++);
    assert( begin == end && "Impossible number of parameters!");
    Name area_name = ds_.get_area_name(id);
    return {ResultType::NAMELIST, CmdResultNames{{NO_PLACE,id},area_name}};
}

MainProgram::CmdResult MainProgram::cmd_get_place_coord(std::ostream &/*output*/, MatchIter begin, MatchIter end)
{
    PlaceID id = *begin++;
    assert( begin == end && "Impossible number of parameters!");
    Coord place_coordinates = ds_.get_place_coord(id);
    return {ResultType::COORDLIST, CmdResultCoords{{id,NO_AREA},{place_coordinates}}};
}

MainProgram::CmdResult MainProgram::cmd_get_area_coords(std::ostream &/*output*/, MatchIter begin, MatchIter end)
{
    AreaID id = convert_string_to<AreaID>(*begin++);
    assert( begin == end && "Impossible number of parameters!");
    std::vector<Coord> area_coordinates = ds_.get_area_coords(id);
    return {ResultType::COORDLIST, CmdResultCoords{{NO_PLACE,id},area_coordinates}};
}

MainProgram::CmdResult MainProgram::cmd_change_place_coord(std::ostream& /*output*/, MainProgram::MatchIter begin, MainProgram::MatchIter end)
{
    PlaceID id = *begin++;
    string xstr = *begin++;
    string ystr = *begin++;
    assert( begin == end && "Impossible number of parameters!");

    int x = convert_string_to<int>(xstr);
    int y = convert_string_to<int>(ystr);

    bool success = ds_.change_place_coord(id, {x,y});

    view_dirty = true;
    return {ResultType::IDLIST, CmdResultIDs{{}, {success ? id : NO_PLACE}}};
}

MainProgram::CmdResult MainProgram::cmd_change_place_name(std::ostream& /*output*/, MainProgram::MatchIter begin, MainProgram::MatchIter end)
{
    PlaceID id = *begin++;
    string name = *begin++;

    assert( begin == end && "Impossible number of parameters!");

    bool success = ds_.change_place_name(id, name);

    view_dirty = true;
    return {ResultType::IDLIST, CmdResultIDs{{}, {success ? id : NO_PLACE}}};
}

bool MainProgram::test_change_place_coord(Stopwatch &stopwatch, std::ostream& output)
{
    if(random_places_added_<1){
        return true;
    }

    auto id = random_valid_place();
    Coord oldcoord=ds_.get_place_coord(id);
    if(oldcoord == NO_COORD){
        output << "Failed check! "
               << "get_place_coord returned \n"
               << "NO_COORD for a valid place id." << std::endl;
        return false;
    }
    Coord newcoord=random_coord();
    stopwatch.start();
    bool result = ds_.change_place_coord(id, newcoord);
    stopwatch.stop();
    auto find_iter = valid_coords_.find(oldcoord);
    auto vec_it = std::find(coords_.begin(),coords_.end(),oldcoord);
    if(find_iter==valid_coords_.end() || vec_it==coords_.end()){
        output << "Failed check! "
               << "get_place_coord returned\n"
               << "invalid original coordinates." << std::endl;
        return false;
    }
    valid_coords_.erase(find_iter);
    valid_coords_.insert(newcoord);
    *vec_it=newcoord;

    return result;
}

bool MainProgram::test_change_place_name(Stopwatch &stopwatch, std::ostream& output)
{
    if(random_places_added_<1){
        return true;
    }

    auto id = random_valid_place();
    Name oldname=ds_.get_place_name(id);
    if(oldname == NO_NAME){
        output << "Failed check! get_place_name \n"
               << "returned NO_NAME for a valid place id." << std::endl;
        return false;
    }
    Name newname=random_name();
    stopwatch.start();
    bool result = ds_.change_place_name(id, newname);
    stopwatch.stop();
    auto find_iter = valid_place_names_.find(oldname);
    if(find_iter==valid_place_names_.end()){
        output << "Failed check! Original name from \n"
               << "get_place_name is not valid." << std::endl;
        return false;
    }
    valid_place_names_.erase(find_iter);
    valid_place_names_.insert(newname);

    return result;
}

MainProgram::CmdResult MainProgram::cmd_add_subarea_to_area(std::ostream& output, MainProgram::MatchIter begin, MainProgram::MatchIter end)
{
    AreaID subareaid = convert_string_to<AreaID>(*begin++);
    AreaID parentid = convert_string_to<AreaID>(*begin++);
    assert( begin == end && "Impossible number of parameters!");

    bool ok = ds_.add_subarea_to_area(subareaid, parentid);
    if (ok)
    {
        try
        {
            auto subareaname = ds_.get_area_name(subareaid);
            auto parentname = ds_.get_area_name(parentid);
            output << "Added '" << subareaname << "' as a subarea of '" << parentname << "'" << endl;
        }
        catch (NotImplemented&)
        {
            output << "Added a subarea to area." << endl;
        }
        return {ResultType::IDLIST, CmdResultIDs{{subareaid, parentid}, {}}};
    }
    else
    {
        output << "Adding a subarea failed!" << endl;
        return {};
    }
}

MainProgram::CmdResult MainProgram::cmd_get_places_closest_to(std::ostream &output, MatchIter begin, MatchIter end)
{
    string xstr = *begin++;
    string ystr = *begin++;
    assert( begin == end && "Impossible number of parameters!");

    int x = convert_string_to<int>(xstr);
    int y = convert_string_to<int>(ystr);

    auto places = ds_.get_places_closest_to({x,y});
    if (places.empty())
    {
        output << "No places!" << endl;
    }

    return {ResultType::IDLIST, CmdResultIDs{{}, places}};
}

MainProgram::CmdResult MainProgram::cmd_get_closest_common_ancestor_of_areas(std::ostream &output, MatchIter begin, MatchIter end)
{
    AreaID areaid1 = convert_string_to<AreaID>(*begin++);
    AreaID areaid2 = convert_string_to<AreaID>(*begin++);
    assert( begin == end && "Impossible number of parameters!");


    auto areaid = ds_.get_closest_common_ancestor_of_areas(areaid1, areaid2);
    if (areaid == NO_AREA)
    {
        output << "No common parent area found." << endl;
        return {ResultType::IDLIST, CmdResultIDs{{}, {}}};
    }

    return {ResultType::IDLIST, CmdResultIDs{{areaid1, areaid2, areaid}, {}}};
}

MainProgram::CmdResult MainProgram::cmd_ancestor_areas_of_subarea(std::ostream& output, MainProgram::MatchIter begin, MainProgram::MatchIter end)
{
    AreaID id = convert_string_to<AreaID>(*begin++);
    assert( begin == end && "Impossible number of parameters!");

    auto result = ds_.ancestor_areas_of_subarea(id);
    if (result.empty()) { output << "Area does not belong to any area." << std::endl; }
    return {ResultType::IDLIST, CmdResultIDs{result, {}}};
}

bool MainProgram::test_ancestor_areas_of_subarea(Stopwatch &stopwatch, std::ostream& output)
{
    if(random_places_added_<1){
        return true;
    }

    auto id = random_valid_area();
    stopwatch.start();
    auto result = ds_.ancestor_areas_of_subarea(id);
    stopwatch.stop();
    std::unordered_set<AreaID> unique(result.begin(),result.end());
    if(unique.size()!=result.size()){
        output << "Failed check! Duplicate areas returned." << std::endl;
        return false;
    }
    for(const auto& rid : result){
        if(valid_area_ids_.find(rid)==valid_area_ids_.end()){
            output << "Failed check! Invalid area returned." << std::endl;
            return false;
        }
    }

    return true;
}

bool MainProgram::test_all_subareas_of_area(Stopwatch &stopwatch, std::ostream& output)
{
    if(random_areas_added_<1){
        return true;
    }

    auto id = random_root_area();
    stopwatch.start();
    auto result = ds_.all_subareas_of_area(id);
    stopwatch.stop();
    if(random_areas_added_ > 1 && (result.size()+1)*(pow(2,ROOT_BIAS_MULTIPLIER*random_areas_added_))<random_areas_added_){
        output << "Failed check! Invalid number of areas returned." << std::endl;
        return false;
    }
    std::unordered_set<AreaID> unique(result.begin(),result.end());
    if(result.size()!=unique.size()){
        output << "Failed check! Duplicate areas returned." << std::endl;
        return false;
    }
    // check validity of each id
    for(const auto& rid : result){
        if(valid_area_ids_.find(rid)==valid_area_ids_.end()){
            output << "Failed check! Invalid area returned." << std::endl;
            return false;
        }
    }

    return true;
}

MainProgram::CmdResult MainProgram::cmd_remove_place(ostream& output, MatchIter begin, MatchIter end)
{
    string id = *begin++;
    assert( begin == end && "Impossible number of parameters!");

    auto name = ds_.get_place_name(id);
    bool success = ds_.remove_place(id);
    if (success)
    {
        output << name << " removed." << endl;
        view_dirty = true;
        return {};
    }
    else
    {
        return {ResultType::IDLIST, CmdResultIDs{{}, {NO_PLACE}}};
    }
}

bool MainProgram::test_remove_place(Stopwatch &stopwatch, std::ostream& output)
{
    if(random_places_added_<1){
        return true;
    }

    PlaceID placeid = random_valid_place();
    Name name = ds_.get_place_name(placeid);
    if(name==NO_NAME){
        output << "Failed check! get_place_name \n"
               << "returned NO_NAME for a valid place id." << std::endl;
        return false;
    }
    PlaceType type = ds_.get_place_type(placeid);
    if(type==PlaceType::NO_TYPE){
        output << "Failed check! get_place_type \n"
               << "returned NO_TYPE for a valid place id." << std::endl;
        return false;
    }
    Coord coords = ds_.get_place_coord(placeid);
    if(coords==NO_COORD){
        output << "Failed check! get_place_coord \n"
               << "returned NO_COORD for a valid place id." << std::endl;
        return false;
    }
    stopwatch.start();
    ds_.remove_place(placeid);
    stopwatch.stop();

    // remove from caches
    valid_place_ids_.erase(placeid);

    unstable_vector_erase(place_ids_,placeid);

    --random_places_added_;
    return true;
}



bool MainProgram::add_random_places_areas(Stopwatch &stopwatch, unsigned int size, std::ostream& output)
{
    // first add areas so that there are #_of_places/10 areas
    unsigned long long int areas_added_this_time = 0;
    for (; random_areas_added_<(random_places_added_+size)/10; ++areas_added_this_time){
        if (!test_add_area(stopwatch, output)){
            return false;
        }
    }
    // create a binary tree from the areas
    for (unsigned int i=(random_areas_added_-areas_added_this_time); i<random_areas_added_; ++i){
        auto areaid = n_to_areaid(i);
        auto parentid = n_to_areaid(i/ 2);
        if(i==(i/2)){
            continue;
        }
        stopwatch.start();
        bool retval=ds_.add_subarea_to_area(areaid, parentid);
        stopwatch.stop();
        if (!retval){
            return false;
        }
    }

    // Add the wanted number of places
    for (unsigned int i = 0; i < size; ++i)
    {
        bool retval = test_add_place(stopwatch, output);
        if (!retval){
            return false;
        }
    }
    return true;
}

bool MainProgram::add_random_places(Stopwatch &stopwatch, unsigned int size, std::ostream& output)
{

    // Add the wanted number of places
    for (unsigned int i = 0; i < size; ++i)
    {
        bool retval = test_add_place(stopwatch, output);
        if (!retval){
            return false;
        }
    }

    return true;
}

bool MainProgram::add_random_areas(Stopwatch &stopwatch, unsigned int size, std::ostream& output)
{
    // first add areas so that there are #_of_places/10 areas
    unsigned long long int areas_added_this_time = 0;
    for (; random_areas_added_<(random_places_added_+size)/10; ++areas_added_this_time){
        if (!test_add_area(stopwatch, output)){
            return false;
        }
    }
    // create a binary tree from the areas
    for (unsigned int i=(random_areas_added_-areas_added_this_time); i<random_areas_added_; ++i){
        auto areaid = n_to_areaid(i);
        auto parentid = n_to_areaid(i/ 2);
        if(i==(i/2)){
            continue;
        }
        stopwatch.start();
        bool retval=ds_.add_subarea_to_area(areaid, parentid);
        stopwatch.stop();
        if (!retval){
            return false;
        }
    }
    return true;
}



MainProgram::CmdResult MainProgram::cmd_random_places(ostream& output, MatchIter begin, MatchIter end)
{
    string sizestr = *begin++;
    string minxstr = *begin++;
    string minystr = *begin++;
    string maxxstr = *begin++;
    string maxystr = *begin++;
    assert( begin == end && "Impossible number of parameters!");

    unsigned int size = convert_string_to<unsigned int>(sizestr);

    if (!minxstr.empty() && !minystr.empty() && !maxxstr.empty() && !maxystr.empty())
    {
        min.x = convert_string_to<unsigned int>(minxstr);
        min.y = convert_string_to<unsigned int>(minystr);
        max.x = convert_string_to<unsigned int>(maxxstr);
        max.y = convert_string_to<unsigned int>(maxystr);
    }
    else
    {
        auto places = ds_.all_places();
        if (!places.empty())
        {
            // Find out bounding box
            min = {std::numeric_limits<int>::max(), std::numeric_limits<int>::max()};
            max = {std::numeric_limits<int>::min(), std::numeric_limits<int>::min()};
            for (auto const& place : places)
            {
                auto [x,y] = ds_.get_place_coord(place);
                if (x < min.x) { min.x = x; }
                if (y < min.y) { min.y = y; }
                if (x > max.x) { max.x = x; }
                if (y > max.y) { max.y = y; }
            }
        }
    }
    Stopwatch dummy;
    add_random_places_areas(dummy, size, output);

    output << "Added: " << size << " places." << endl;

    view_dirty = true;

    return {};
}

bool MainProgram::test_random_places(Stopwatch &stopwatch, std::ostream& output)
{
    // TODO checks and possible forwarding stopwatch to called function
    return add_random_places(stopwatch,1, output);
}

bool MainProgram::test_get_place_count(Stopwatch &stopwatch, std::ostream& output)
{
    stopwatch.start();
    auto result = ds_.get_place_count();
    stopwatch.stop();

    if (result!=random_places_added_){
        output << "Failed check! Invalid place count returned." << std::endl;
        return false;
    }
    return true;
}

bool MainProgram::test_add_place(Stopwatch &stopwatch, std::ostream& output)
{
    PlaceID placeid = n_to_placeid(random_places_added_);
    Name placename = n_to_name(random_places_added_);
    Coord newcoord = n_to_coord(random_places_added_);
    PlaceType type{random(0, static_cast<int>(PlaceType::NO_TYPE)-1)};

    stopwatch.start();
    bool retval = ds_.add_place(placeid, placename, type, newcoord);
    stopwatch.stop();
    ++random_places_added_;
    if (!retval){
        output << "Failed check! Adding a place failed." << std::endl;
        return false;
    }
    return true;
}

bool MainProgram::test_get_place_name(Stopwatch &stopwatch, std::ostream& output)
{
    if(random_places_added_<1){
        return true;
    }
    PlaceID id = random_valid_place();
    stopwatch.start();
    Name retval = ds_.get_place_name(id);
    stopwatch.stop();
    if (retval == NO_NAME){
        output << "Failed check! NO_NAME returned for a valid place id." << std::endl;
        return false;
    }
    return true;
}

bool MainProgram::test_get_place_type(Stopwatch &stopwatch, std::ostream& output)
{
    if(random_places_added_<1){
        return true;
    }
    PlaceID id = random_valid_place();
    stopwatch.start();
    PlaceType retval = ds_.get_place_type(id);
    stopwatch.stop();
    if (retval == PlaceType::NO_TYPE){
        output << "Failed check! NO_TYPE returned for a valid place id." << std::endl;
        return false;
    }
    return true;
}

bool MainProgram::test_get_place_coord(Stopwatch &stopwatch, std::ostream& output)
{
    if(random_places_added_<1){
        return true;
    }
    PlaceID id = random_valid_place();
    stopwatch.start();
    Coord retval = ds_.get_place_coord(id);
    stopwatch.stop();
    if(retval == NO_COORD){
        output << "Failed check! NO_COORD returned for a valid place id." << std::endl;
        return false;
    }
    return true;
}

bool MainProgram::test_add_area(Stopwatch &stopwatch, std::ostream& output)
{
    auto areaid = n_to_areaid(random_areas_added_);
    vector<Coord> coords;
    // possible to change the number of coords by random chance
    for (int j=0; j<3; ++j)
    {
        coords.push_back(random_coord());
    }
    Name areaname=convert_to_string(areaid);
    stopwatch.start();
    bool retval=ds_.add_area(areaid, areaname, std::move(coords));
    stopwatch.stop();
    ++random_areas_added_;
    if (!retval){
        output << "Failed check! Adding an area failed." << std::endl;
        return false;
    }
    return true;
}

bool MainProgram::test_all_areas(Stopwatch &stopwatch, std::ostream& output)
{
    stopwatch.start();
    auto result = ds_.all_areas();
    stopwatch.stop();
    if(result.size()!=random_areas_added_){
        output << "Failed check! The number of areas \n"
               << "returned doesn't match the expected value." << std::endl;
        return false;
    }
    std::unordered_set<AreaID> unique(result.begin(),result.end());
    if(unique.size()!=result.size()){
        output << "Failed check! Duplicate areas returned."
               << std::endl;
        return false;
    }
    for(const auto& id : result){
        if(valid_area_ids_.find(id)==valid_area_ids_.end()){
            output << "Failed check! Invalid area \n"
                   << "returned." << std::endl;
            return false;
        }
    }
    return true;
}

bool MainProgram::test_get_area_name(Stopwatch &stopwatch, std::ostream& output)
{
    AreaID areaid = random_valid_area();
    Name areaname=convert_to_string(areaid);

    stopwatch.start();
    auto result = ds_.get_area_name(areaid);
    stopwatch.stop();
    if (result != areaname){
        output << "Failed check! Invalid area name returned." << std::endl;
        return false;
    }
    return true;
}

bool MainProgram::test_get_area_coords(Stopwatch &stopwatch, std::ostream& output)
{
    AreaID areaid = random_valid_area();

    stopwatch.start();
    auto result = ds_.get_area_coords(areaid);
    stopwatch.stop();

    if (result.size()!=3){
        output << "Failed check! Invalid coords returned." << std::endl;
        return false;
    }
    for (const auto& coord : result){
        if (coord==NO_COORD){
            output << "Failed check! NO_COORD returned for valid area." << std::endl;
            return false;
        }
    }
    return true;
}

bool MainProgram::test_add_subarea_to_area(Stopwatch &stopwatch, std::ostream& output)
{
    Stopwatch dummy;
    // add a new subarea to continue making binary tree with
    unsigned int i = random_areas_added_;
    if (!test_add_area(dummy, output)){
        output << "Failed check! Adding an area failed." << std::endl;
        return false;
    }

    auto areaid = n_to_areaid(i);
    auto parentid = n_to_areaid(i/ 2);
    stopwatch.start();
    bool retval=ds_.add_subarea_to_area(areaid, parentid);
    stopwatch.stop();
    if (!retval){
        output << "Failed check! Valid addition \n"
               << "of subarea to area failed." << std::endl;
        return false;
    }

    return true;
}

bool MainProgram::test_get_places_alphabetically(Stopwatch &stopwatch, std::ostream& output)
{
    // get the ordered thing

    stopwatch.start();
    auto result = ds_.get_places_alphabetically();
    stopwatch.stop();
    if (result.size() != random_places_added_){
        output << "Failed check! Incorrect number of places returned." << std::endl;
        return false;
    }
    std::unordered_set<PlaceID> unique(result.begin(),result.end());
    if(unique.size()!=result.size()){
        output << "Failed check! Duplicate places returned." << std::endl;
        return false;
    }
    std::vector<Name> names(result.size());
    size_t i=0;
    for(const auto& id : result){
        auto name = ds_.get_place_name(id);
        if(name == NO_NAME || (valid_place_names_.find(name)==valid_place_names_.end())){
            output << "Failed check! Invalid name \n"
                   << "returned for a valid place id." << std::endl;
            return false;
        }
        names[i++]=name;
    }
    // check for the order
    if (!std::is_sorted(names.begin(),names.end())){
        output << "Failed check! Places are not sorted alphabetically." << std::endl;
        return false;
    }
    return true;

}

bool MainProgram::test_get_places_distance_increasing(Stopwatch &stopwatch, std::ostream& output)
{
    // get the ordered thing

    stopwatch.start();
    auto result = ds_.get_places_distance_increasing();
    stopwatch.stop();
    if (result.size() != random_places_added_){
        output << "Failed check! The returned number of places \n"
               << "doesn't match the expected value." << std::endl;
        return false;
    }
    std::unordered_set<PlaceID> unique(result.begin(),result.end());
    if(unique.size()!=result.size()){
        output << "Failed check! Duplicate places found." << std::endl;
        return false;
    }
    std::vector<unsigned long long int> something(result.size());
    size_t i=0;
    for(const auto& id : result){
        auto coord = ds_.get_place_coord(id);
        if(coord == NO_COORD || (valid_coords_.find(coord)==valid_coords_.end())){
            output << "Failed check! Invalid coordinates for a PlaceID. "
                   << std::endl;

            return false;
        }

        something[i++]=coord.x*coord.x+coord.y*coord.y;
    }

    // check for the order
    auto isOK = std::is_sorted(something.begin(),something.end());
    if (!isOK){
        output << "Failed check! Places are not sorted in order of distances." << std::endl;
        return false;
    }
    return true;
}

MainProgram::CmdResult MainProgram::cmd_get_place_count(ostream& output, MatchIter begin, MatchIter end)
{
    assert( begin == end && "Impossible number of parameters!");

    output << "Number of places: " << ds_.get_place_count() << endl;

    return {};
}

MainProgram::CmdResult MainProgram::cmd_all_places(ostream& output, MatchIter begin, MatchIter end)
{
    assert( begin == end && "Impossible number of parameters!");

    auto places = ds_.all_places();
    if (places.empty())
    {
        output << "No places!" << endl;
    }

    std::sort(places.begin(), places.end());
    return {ResultType::IDLIST, CmdResultIDs{{}, places}};
}

MainProgram::CmdResult MainProgram::cmd_add_area(std::ostream& /*output*/, MatchIter begin, MatchIter end)
{
    AreaID id = convert_string_to<AreaID>(*begin++);
    string name = *begin++;
    string coordsstr = *begin++;
    assert( begin == end && "Impossible number of parameters!");

    vector<Coord> coords;
    smatch coord;
    auto sbeg = coordsstr.cbegin();
    auto send = coordsstr.cend();
    for ( ; regex_search(sbeg, send, coord, coords_regex_); sbeg = coord.suffix().first)
    {
        coords.push_back({convert_string_to<int>(coord[1]),convert_string_to<int>(coord[2])});
    }

    assert(coords.size() >= 3 && "Area with <3 coords");

    bool success = ds_.add_area(id, name, coords);

    view_dirty = true;
    return {ResultType::IDLIST, CmdResultIDs{{success ? id : NO_AREA}, {}}};
}

MainProgram::CmdResult MainProgram::cmd_all_areas(std::ostream &output, MatchIter begin, MatchIter end)
{
    assert( begin == end && "Impossible number of parameters!");

    auto areas = ds_.all_areas();
    if (areas.empty())
    {
        output << "No areas!" << endl;
    }

    std::sort(areas.begin(), areas.end());
    return {ResultType::IDLIST, CmdResultIDs{areas, {}}};
}

bool MainProgram::test_all_places(Stopwatch& stopwatch, std::ostream& output)
{
    stopwatch.start();
    auto result = ds_.all_places();
    stopwatch.stop();

    // checks for result validity:
    if (result.size() != random_places_added_){
        output << "Failed check! The returned number of places \n"
               << "does not match the expected value." << std::endl;
        return false;
    }
    std::unordered_set<PlaceID> unique(result.begin(),result.end());
    if(unique.size()!=result.size()){
        output << "Failed check! Duplicate places returned." << std::endl;
        return false;
    }
    for(const auto& id : result){
        if(valid_place_ids_.find(id)==valid_place_ids_.end()){
            output << "Failed check! Invalid place returned." << std::endl;
            return false;
        }
    }
    return true;
}

MainProgram::CmdResult MainProgram::cmd_all_subareas_of_area(std::ostream &output, MatchIter begin, MatchIter end)
{
    AreaID areaid = convert_string_to<AreaID>(*begin++);
    assert( begin == end && "Impossible number of parameters!");

    auto areas = ds_.all_subareas_of_area(areaid);
    if (areas.empty())
    {
        output << "No areas!" << endl;
    }

    std::sort(areas.begin(), areas.end());
    areas.insert(areas.begin(), areaid); // Add parameter as the first area
    return {ResultType::IDLIST, CmdResultIDs{areas, {}}};
}

Distance MainProgram::calc_distance(Coord c1, Coord c2)
{
    if (c1 == NO_COORD || c2 == NO_COORD) { return NO_DISTANCE; }

    long long int deltax = c1.x - c2.x;
    long long int deltay = c1.y - c2.y;
    return static_cast<Distance>(std::sqrt(deltax*deltax + deltay*deltay));
}

MainProgram::CmdResult MainProgram::cmd_clear_all(ostream& output, MatchIter begin, MatchIter end)
{
    assert(begin == end && "Invalid number of parameters");

    ds_.clear_all();
    init_primes();

    output << "Cleared all places" << endl;

    view_dirty = true;

    return {};
}

string MainProgram::print_place(PlaceID id, ostream& output, bool nl)
{
    try
    {
        if (id != NO_PLACE)
        {
            auto name = ds_.get_place_name(id);
            auto xy = ds_.get_place_coord(id);
            if (!name.empty())
            {
                output << name << ": ";
            }
            else
            {
                output << "*: ";
            }

            output << "pos=";
            print_coord(xy, output, false);
            output << ", id=" << id;
            if (nl) { output << endl; }

            ostringstream retstream;
            retstream << id;
            return retstream.str();
        }
        else
        {
            output << "--NO_PLACE--";
            if (nl) { output << endl; }
            return "";
        }
    }
    catch (NotImplemented const& e)
    {
        output << endl << "NotImplemented while printing place : " << e.what() << endl;
        std::cerr << endl << "NotImplemented while printing place : " << e.what() << endl;
        return "";
    }
}

string MainProgram::print_place_brief(PlaceID id, std::ostream &output, bool nl)
{
    try
    {
        if (id != NO_PLACE)
        {
            auto name = ds_.get_place_name(id);
            if (!name.empty())
            {
                output << name << " ";
            }
            else
            {
                output << "* ";
            }

            output << "(" << id << ")";
            if (nl) { output << endl; }

            ostringstream retstream;
            retstream << id;
            return retstream.str();
        }
        else
        {
            output << "--NO_PLACE--";
            if (nl) { output << endl; }
            return "";
        }
    }
    catch (NotImplemented const& e)
    {
        output << endl << "NotImplemented while printing place : " << e.what() << endl;
        std::cerr << endl << "NotImplemented while printing place : " << e.what() << endl;
        return "";
    }
}

string MainProgram::print_area(AreaID id, std::ostream &output, bool nl)
{
    try
    {
        if (id != NO_AREA)
        {
            auto name = ds_.get_area_name(id);
//            auto xy = ds_.get_area_coords(id);
            if (!name.empty())
            {
                output << name << ": ";
            }
            else
            {
                output << "*: ";
            }

            output << "id=" << id;
            if (nl) { output << endl; }

            ostringstream retstream;
            retstream << id;
            return retstream.str();
        }
        else
        {
            output << "--NO_AREA--";
            if (nl) { output << endl; }
            return "";
        }
    }
    catch (NotImplemented const& e)
    {
        output << endl << "NotImplemented while printing area : " << e.what() << endl;
        std::cerr << endl << "NotImplemented while printing area : " << e.what() << endl;
        return "";
    }
}

MainProgram::CmdResult MainProgram::cmd_find_places_with_coord(ostream& output , MatchIter begin, MatchIter end)
{
    string xstr = *begin++;
    string ystr = *begin++;
    assert( begin == end && "Impossible number of parameters!");

    int x = convert_string_to<int>(xstr);
    int y = convert_string_to<int>(ystr);

    auto result = ds_.find_places_with_coord({x,y});
    std::sort(result.begin(), result.end());

    if (result.empty())
    {
        output << "No places found!" << std::endl;
    }

    return {ResultType::IDLIST, CmdResultIDs{{}, result}};
}

MainProgram::CmdResult MainProgram::cmd_find_places_with_name(ostream& output , MatchIter begin, MatchIter end)
{
    Name name = *begin++;
    assert( begin == end && "Impossible number of parameters!");

    auto result = ds_.find_places_with_name(name);
    std::sort(result.begin(), result.end());

    if (result.empty())
    {
        output << "No places found!" << std::endl;
    }

    return {ResultType::IDLIST, CmdResultIDs{{}, result}};
}

MainProgram::CmdResult MainProgram::cmd_find_places_with_type(ostream& output , MatchIter begin, MatchIter end)
{
    string typestr = *begin++;
    assert( begin == end && "Impossible number of parameters!");

    PlaceType placetype = convert_string_to_placetype(typestr);
    auto result = ds_.find_places_with_type(placetype);
    std::sort(result.begin(), result.end());

    if (result.empty())
    {
        output << "No places found!" << std::endl;
    }

    return {ResultType::IDLIST, CmdResultIDs{{}, result}};
}



bool MainProgram::test_find_places_with_coord(Stopwatch& stopwatch, std::ostream& output)
{
    if(random_places_added_<1){
        return true;
    }
    Coord testcoord = random_valid_coord();
    stopwatch.start();
    auto result = ds_.find_places_with_coord(testcoord);
    stopwatch.stop();
    auto correct_count = valid_coords_.count(testcoord);
    if(result.size()!=correct_count){
        output << "Failed check! Incorrect number of places returned." << std::endl;
        return false;
    }
    std::unordered_set<PlaceID> unique(result.begin(),result.end());
    if(unique.size()!=result.size()){
        output << "Failed check! Duplicate places returned." << std::endl;
        return false;
    }
    for(const auto& id: result){
        if(valid_place_ids_.find(id)==valid_place_ids_.end()){
            output << "Failed check! Invalid places returned." << std::endl;
            return false;
        }
    }
    return true;
}

bool MainProgram::test_get_places_closest_to(Stopwatch &stopwatch, std::ostream& output)
{
    auto searchcoord = random_coord();
    stopwatch.start();
    auto result = ds_.get_places_closest_to(searchcoord);
    stopwatch.stop();

    unsigned int size = std::min((unsigned long int)3,random_places_added_);
    if(result.size()!=size){
        output << "Failed check! Invalid number of places returned." << std::endl;
        return false;
    }
    std::unordered_set<PlaceID> unique(result.begin(),result.end());
    if(unique.size()!=result.size()){
        output << "Failed check! Duplicate places returned." << std::endl;
        return false;
    }
    for (const auto& id : result){
        if(valid_place_ids_.find(id)==valid_place_ids_.end()){
            output << "Failed check! Invalid places returned." << std::endl;
            return false;
        }
    }
    return true;
}

bool MainProgram::test_get_closest_common_ancestor_of_areas(Stopwatch &stopwatch, std::ostream& output)
{
    if(random_areas_added_<1){
        return true;
    }

    auto id1 = random_valid_area();
    auto id2 = random_valid_area();
    stopwatch.start();
    auto result = ds_.get_closest_common_ancestor_of_areas(id1, id2);
    stopwatch.stop();

    auto isOK = result==NO_AREA || (valid_area_ids_.find(result)!=valid_area_ids_.end());
    if (!isOK){
        output << "Failed check! Invalid area returned." << std::endl;
        return false;
    }

    return true;
}

MainProgram::CmdResult MainProgram::cmd_randseed(std::ostream& output, MatchIter begin, MatchIter end)
{
    string seedstr = *begin++;
    assert(begin == end && "Invalid number of parameters");

    unsigned long int seed = convert_string_to<unsigned long int>(seedstr);

    rand_engine_.seed(seed);
    init_primes();

    output << "Random seed set to " << seed << endl;

    return {};
}

MainProgram::CmdResult MainProgram::cmd_read(std::ostream& output, MatchIter begin, MatchIter end)
{
    string filename = *begin++;
    string silentstr =  *begin++;
    assert( begin == end && "Impossible number of parameters!");

    bool silent = !silentstr.empty();
    ostream* new_output = &output;

    ostringstream dummystr; // Given as output if "silent" is specified, the output is discarded
    if (silent)
    {
        new_output = &dummystr;
    }

    ifstream input(filename);
    if (input)
    {
        output << "** Commands from '" << filename << "'" << endl;
        command_parser(input, *new_output, PromptStyle::NORMAL);
        if (silent) { output << "...(output discarded in silent mode)..." << endl; }
        output << "** End of commands from '" << filename << "'" << endl;
    }
    else
    {
        output << "Cannot open file '" << filename << "'!" << endl;
    }

    return {};
}


MainProgram::CmdResult MainProgram::cmd_testread(std::ostream& output, MatchIter begin, MatchIter end)
{
    string infilename = *begin++;
    string outfilename = *begin++;
    assert( begin == end && "Impossible number of parameters!");

    ifstream input(infilename);
    if (input)
    {
        ifstream expected_output(outfilename);
        if (output)
        {
            stringstream actual_output;
            command_parser(input, actual_output, PromptStyle::NO_NESTING);

            vector<string> actual_lines;
            while (actual_output)
            {
                string line;
                getline(actual_output, line);
                if (!actual_output) { break; }
                actual_lines.push_back(line);
            }

            vector<string> expected_lines;
            while (expected_output)
            {
                string line;
                getline(expected_output, line);
                if (!expected_output) { break; }
                expected_lines.push_back(line);
            }

            string heading_actual = "Actual output";
            unsigned int actual_max_length = heading_actual.length();
            auto actual_max_iter = max_element(actual_lines.cbegin(), actual_lines.cend(),
                                               [](string s1, string s2){ return s1.length() < s2.length(); });
            if (actual_max_iter != actual_lines.cend())
            {
                actual_max_length = actual_max_iter->length();
            }

            string heading_expected = "Expected output";
            unsigned int expected_max_length = heading_expected.length();
            auto expected_max_iter = std::max_element(expected_lines.cbegin(), expected_lines.cend(),
                                                    [](string s1, string s2){ return s1.length() < s2.length(); });
            if (expected_max_iter != expected_lines.cend())
            {
                expected_max_length = expected_max_iter->length();
            }

            auto pos_actual = actual_lines.cbegin();
            auto pos_expected = expected_lines.cbegin();
            output << "  " << heading_actual << string(actual_max_length - heading_actual.length(), ' ') << " | " << heading_expected << endl;
            output << "--" << string(actual_max_length, '-') << "-|-" << string(expected_max_length, '-') << endl;

            bool lines_ok = true;
            while (pos_expected != expected_lines.cend() || pos_actual != actual_lines.cend())
            {
                if (pos_expected != expected_lines.cend())
                {
                    if (pos_actual != actual_lines.cend())
                    {
                        bool ok = (*pos_expected == *pos_actual);
                        output << (ok ? ' ' : '?') << ' ' << *pos_actual << string(actual_max_length - pos_actual->length(), ' ')
                               << " | " << *pos_expected << endl;
                        lines_ok = lines_ok && ok;
                        ++pos_actual;
                    }
                    else
                    { // Actual output was too short
                        output << "? " << string(actual_max_length, ' ')
                               << " | " << *pos_expected << endl;
                        lines_ok = false;
                    }
                    ++pos_expected;
                }
                else
                { // Actual output was too long
                    output << "? " << *pos_actual << string(actual_max_length - pos_actual->length(), ' ')
                           << " | " << endl;
                    lines_ok = false;
                    ++pos_actual;
                }
            }
            if (lines_ok)
            {
                output << "**No differences in output.**" << endl;
                if (test_status_ == TestStatus::NOT_RUN)
                {
                    test_status_ = TestStatus::NO_DIFFS;
                }
            }
            else
            {
                output << "**Differences found! (Lines beginning with '?')**" << endl;
                test_status_ = TestStatus::DIFFS_FOUND;
            }

        }
        else
        {
            output << "Cannot open file '" << outfilename << "'!" << endl;
        }
    }
    else
    {
        output << "Cannot open file '" << infilename << "'!" << endl;
    }

    return {};
}

MainProgram::CmdResult MainProgram::cmd_stopwatch(std::ostream& output, MatchIter begin, MatchIter end)
{
    string on = *begin++;
    string off = *begin++;
    string next = *begin++;
    assert(begin == end && "Invalid number of parameters");

    if (!on.empty())
    {
        stopwatch_mode = StopwatchMode::ON;
        output << "Stopwatch: on" << endl;
    }
    else if (!off.empty())
    {
        stopwatch_mode = StopwatchMode::OFF;
        output << "Stopwatch: off" << endl;
    }
    else if (!next.empty())
    {
        stopwatch_mode = StopwatchMode::NEXT;
        output << "Stopwatch: on for the next command" << endl;
    }
    else
    {
        assert(!"Impossible stopwatch mode!");
    }

    return {};
}

std::string MainProgram::print_place_name(PlaceID id, std::ostream &output, bool nl)
{
    try
    {
        if (id != NO_PLACE)
        {
            auto name = ds_.get_place_name(id);
            if (!name.empty())
            {
                output << name;
            }
            else
            {
                output << "*";
            }

            ostringstream retstream;
            retstream << name;
            if (nl) { output << endl; }
            return retstream.str();
        }
        else
        {
            output << "--NO_PLACE--";
            if (nl) { output << endl; }
            return "";
        }
    }
    catch (NotImplemented const& e)
    {
        output << endl << "NotImplemented while printing place name : " << e.what() << endl;
        std::cerr << endl << "NotImplemented while printing place name : " << e.what() << endl;
        return "";
    }
}

std::string MainProgram::print_place_type(PlaceType placetype, std::ostream &output, bool nl)
{
    try
    {
        if (placetype != PlaceType::NO_TYPE)
        {
            //auto type = ds_.get_place_type(id);
            //if (!type.empty())
            //if (type != PlaceType::NO_TYPE)
            //{
            output <<  convert_placetype_to_string(placetype);

            //}
            /*
            else
            {
                output << "*";
            }
            */
            ostringstream retstream;
            retstream << convert_placetype_to_string(placetype);
            if (nl) { output << endl; }
            return retstream.str();
        }
        else
        {
            output << "--NO_PLACE--";
            if (nl) { output << endl; }
            return "";
        }
    }
    catch (NotImplemented const& e)
    {
        output << endl << "NotImplemented while printing place name : " << e.what() << endl;
        std::cerr << endl << "NotImplemented while printing place name : " << e.what() << endl;
        return "";
    }
}

std::string MainProgram::print_coord(Coord coord, std::ostream& output, bool nl)
{
    if (coord != NO_COORD)
    {
        output << "(" << coord.x << "," << coord.y << ")";
        ostringstream retstream;
        retstream << "(" << coord.x << "," << coord.y << ")";
        if (nl) { output << endl; }
        return retstream.str();
    }
    else
    {
        output << "(--NO_COORD--)";
        if (nl) { output << endl; }
        return "";
    }
}

string const placeidx = "([a-zA-Z0-9-]+)";
string const areaidx = "([0-9]+)";
string const namex = "([ a-zA-Z0-9-]+)";
string const typex = "([a-zA-Z0-9]+)";
string const numx = "([0-9]+)";
string const optcoordx = "\\([[:space:]]*[0-9]+[[:space:]]*,[[:space:]]*[0-9]+[[:space:]]*\\)";
string const coordx = "\\([[:space:]]*([0-9]+)[[:space:]]*,[[:space:]]*([0-9]+)[[:space:]]*\\)";
string const wsx = "[[:space:]]+";
string const optplacetimeidx = "[a-zA-Z0-9-]+:(?:(?:[01][0-9][0-5][0-9])|(?:2[0-3][0-5][0-9]))";

string const perftest_cmds_listx = "((?:[0-9]+\\*)?[0-9a-zA-Z_]+(?:;(?:[0-9]+\\*)?[0-9a-zA-Z_]+)*(?:(?::(?:[0-9]+\\*)?[0-9a-zA-Z_]+)(?:;(?:[0-9]+\\*)?[0-9a-zA-Z_]+)*(?::[0-9]+)?)?)";
string const perftest_timeoutx = "([0-9]+(?::[0-9]+)?)";
string const perftest_n_values_or_rangex = "([0-9]+(?:(?::[0-9]+){1,2}|(?:;[0-9]+)+)?)";
string const perftest_logx = "(?:[[:space:]]+(log))?";

vector<MainProgram::CmdInfo> MainProgram::cmds_ =
{
    {"get_place_count", "", "", &MainProgram::cmd_get_place_count, &MainProgram::test_get_place_count },
    {"clear_all", "", "", &MainProgram::cmd_clear_all, nullptr },
    {"all_places", "", "", &MainProgram::cmd_all_places, &MainProgram::test_all_places },
    {"add_place", "PlaceID \"Name\" Type (x,y)", placeidx+wsx+'"'+namex+'"'+wsx+typex+wsx+coordx, &MainProgram::cmd_add_place, &MainProgram::test_add_place },
    {"get_place_name", "PlaceID",placeidx,&MainProgram::cmd_get_place_name,&MainProgram::test_get_place_name},
    {"get_place_type", "PlaceID",placeidx,&MainProgram::cmd_get_place_type,&MainProgram::test_get_place_type},
    {"get_place_coord", "PlaceID",placeidx,&MainProgram::cmd_get_place_coord,&MainProgram::test_get_place_coord},
    {"get_places_alphabetically", "", "", &MainProgram::NoParListCmd<&Datastructures::get_places_alphabetically>, &MainProgram::test_get_places_alphabetically },
    {"get_places_distance_increasing", "", "", &MainProgram::NoParListCmd<&Datastructures::get_places_distance_increasing>,
                                             &MainProgram::test_get_places_distance_increasing },
    {"find_places_with_coord", "(x,y)", coordx, &MainProgram::cmd_find_places_with_coord, &MainProgram::test_find_places_with_coord },
    {"find_places_with_name", "Name", '"'+namex+'"', &MainProgram::cmd_find_places_with_name, nullptr },
    {"find_places_with_type", "Type", typex, &MainProgram::cmd_find_places_with_type, nullptr },
    {"change_place_coord", "PlaceID (x,y)", placeidx+wsx+coordx, &MainProgram::cmd_change_place_coord, &MainProgram::test_change_place_coord },
    {"change_place_name", "PlaceID (x,y)", placeidx+wsx+'"'+namex+'"', &MainProgram::cmd_change_place_name, &MainProgram::test_change_place_name },
    {"add_area", "AreaID \"Name\" (x,y) (x,y)...", areaidx+wsx+'"'+namex+'"'+"((?:"+wsx+optcoordx+")+)", &MainProgram::cmd_add_area, &MainProgram::test_add_area },
    {"all_areas", "", "", &MainProgram::cmd_all_areas, &MainProgram::test_all_areas },
    {"get_area_name", "AreaID",areaidx,&MainProgram::cmd_get_area_name,&MainProgram::test_get_area_name},
    {"get_area_coords", "AreaID",areaidx,&MainProgram::cmd_get_area_coords,&MainProgram::test_get_area_coords},
    {"add_subarea_to_area", "SubareaID AreaID", areaidx+wsx+areaidx, &MainProgram::cmd_add_subarea_to_area, &MainProgram::test_add_subarea_to_area},
    {"ancestor_areas_of_subarea", "AreaID", areaidx, &MainProgram::cmd_ancestor_areas_of_subarea, &MainProgram::test_ancestor_areas_of_subarea },
    {"all_subareas_of_area", "AreaID", areaidx, &MainProgram::cmd_all_subareas_of_area, &MainProgram::test_all_subareas_of_area },
    {"get_places_closest_to", "(x,y)", coordx, &MainProgram::cmd_get_places_closest_to, &MainProgram::test_get_places_closest_to },
    {"remove_place", "PlaceID", placeidx, &MainProgram::cmd_remove_place, &MainProgram::test_remove_place },
    {"get_closest_common_ancestor_of_areas", "AreaID1 AreaID2", areaidx+wsx+areaidx, &MainProgram::cmd_get_closest_common_ancestor_of_areas, &MainProgram::test_get_closest_common_ancestor_of_areas },
    {"quit", "", "", nullptr, nullptr },
    {"help", "", "", &MainProgram::help_command, nullptr },
    {"random_add", "number_of_places_to_add  (minx,miny) (maxx,maxy) (coordinates optional)",
     numx+"(?:"+wsx+coordx+wsx+coordx+")?", &MainProgram::cmd_random_places, &MainProgram::test_random_places },
    {"read", "\"in-filename\" [silent]", "\"([-a-zA-Z0-9 ./:_]+)\"(?:"+wsx+"(silent))?", &MainProgram::cmd_read, nullptr },
    {"testread", "\"in-filename\" \"out-filename\"", "\"([-a-zA-Z0-9 ./:_]+)\""+wsx+"\"([-a-zA-Z0-9 ./:_]+)\"", &MainProgram::cmd_testread, nullptr },
    {"perftest", "[REPEAT*]cmd1[;[REPEAT*]cmd2...][:[REPEAT*]extracmd1[;[REPEAT*]extracmd2...]:extra_every_n_test_entry] timeout[:extra_timeout] range_start:range_end[:step]|n1[;n2...] [log] (parts in [] are optional)",
     perftest_cmds_listx+wsx+perftest_timeoutx+wsx+perftest_n_values_or_rangex+perftest_logx, &MainProgram::cmd_perftest, nullptr },
    {"stopwatch", "on|off|next (alternatives separated by |)", "(?:(on)|(off)|(next))", &MainProgram::cmd_stopwatch, nullptr },
    {"random_seed", "new-random-seed-integer", numx, &MainProgram::cmd_randseed, nullptr },
    {"#", "comment text", ".*", &MainProgram::cmd_comment, nullptr },
};

MainProgram::CmdResult MainProgram::help_command(std::ostream& output, MatchIter /*begin*/, MatchIter /*end*/)
{
    output << "Commands:" << endl;
    for (auto& i : cmds_)
    {
        output << "  " << i.cmd << " " << i.info << endl;
    }

    return {};
}

bool MainProgram::init_perftest(Stopwatch &stopwatch, int n, std::string testcmd, std::ostream& output)
{

    if (place_tests.find(testcmd) != place_tests.end())
    {
        add_random_places(stopwatch, n, output);
    }
    else if (area_tests.find(testcmd) != area_tests.end())
    {
        add_random_areas(stopwatch, n, output);
    }
    else if (place_area_tests.find(testcmd) != place_area_tests.end())
    {
        add_random_places_areas(stopwatch, n, output);
    }
    else
    {
        add_random_places_areas(stopwatch, n, output);
    }
    return true;
}


MainProgram::CmdResult MainProgram::cmd_perftest(std::ostream& output, MatchIter begin, MatchIter end)
{
#ifdef _GLIBCXX_DEBUG
    output << "WARNING: Debug STL enabled, performance will be worse than expected (maybe also asymptotically)!" << endl;
#endif // _GLIBCXX_DEBUG

    string commandstr = *begin++;
    string timeoutstr = *begin++;
    string n_rangestr = *begin++;
    string logstr = *begin++;
    assert(begin == end && "Invalid number of parameters");

    // parse the commands and extra commands
    vector<pair<unsigned int,string>> testcmds;
    vector<pair<unsigned int,string>> extracmds;
    unsigned int n_extra = 1;
    // to reduce the number of unnecessary data in ram
    {
        vector<string> test_extrastr={"","",""};
        auto cbeg = commandstr.cbegin();
        auto cend = commandstr.cend();
        size_t i = 0;
        for (smatch sma;regex_search(cbeg,cend,sma,commands_divide_regex_);cbeg = sma.suffix().first){
            test_extrastr[i++]=sma[1];
        }
        // first find the regular commands
        cbeg = test_extrastr[0].cbegin();
        cend = test_extrastr[0].cend();
        for (smatch scmd; regex_search(cbeg, cend, scmd, commands_regex_); cbeg = scmd.suffix().first)
        {
            unsigned int repeat_count = scmd[1]==""?1:convert_string_to<unsigned int>(scmd[1]);
            testcmds.push_back({repeat_count,scmd[2]});
        }
        // then add the extra commands
        cbeg = test_extrastr[1].cbegin();
        cend = test_extrastr[1].cend();
        for (smatch scmd; regex_search(cbeg, cend, scmd, commands_regex_); cbeg = scmd.suffix().first)
        {
            unsigned int repeat_count = scmd[1]==""?1:convert_string_to<unsigned int>(scmd[1]);
            extracmds.push_back({repeat_count,scmd[2]});
        }
        if(test_extrastr[1]!="" && test_extrastr[2]==""){
            output <<"using extra commands requires every n parameter"<<endl;
            return {};
        }
        n_extra=test_extrastr[2]==""?1:convert_string_to<unsigned int>(test_extrastr[2]);
        if(n_extra < 1){
            output << "extra commands can not be run every 0 normal command listing"<<endl;
            return {};
        }
    }
    vector<unsigned int> timeouts;
    {
        auto cbeg = timeoutstr.cbegin();
        auto cend = timeoutstr.cend();
        for (smatch timeoutsmatch; regex_search(cbeg,cend,timeoutsmatch,timeouts_regex_);cbeg=timeoutsmatch.suffix().first){
            timeouts.push_back(convert_string_to<unsigned int>(timeoutsmatch[1]));
        }
    }
    if(!extracmds.empty() && timeouts.size()==1){
        output << "Using extra commands requires timeout for extra commands"<<endl;
        return {};
    }

    bool is_range=false;
    {
        auto cbeg = n_rangestr.cbegin();
        auto cend = n_rangestr.cend();
        smatch isrange;
        regex_search(cbeg,cend,isrange,is_range_notation_regex_);
        is_range = isrange[1]!="";
    }
    vector<unsigned int> init_ns;
    if (is_range){
        vector<unsigned int> range_limits;
        auto sbeg = n_rangestr.cbegin();
        auto send = n_rangestr.cend();
        for (smatch range; regex_search(sbeg, send, range, range_regex_); sbeg = range.suffix().first)
        {
            range_limits.push_back(convert_string_to<unsigned int>(range[1]));
        }
        if(range_limits[0]>=range_limits[1]){
            output <<  "Lower bound of range must be less than upper bound"<<endl;
            return {};
        }
        if(logstr.empty()){
            unsigned int step = 1;
            if (range_limits.size()==3){
                step=range_limits[2];
            }
            if(step<1){
                output << "Step for linear range can not be < 1"<<endl;
                return{};
            }
            // linear range
            for(unsigned int n=range_limits[0];n<=range_limits[1];n+=step){
                init_ns.push_back(n);
            }
        } else{
            unsigned int step = 10;
            if (range_limits.size()==3){
                step=range_limits[2];
            }
            if(step<2){
                output << "Base for log range can not be < 2"<<endl;
                return{};
            }
            unsigned int overflow_boundary_check = std::numeric_limits<unsigned int>::max()/step;

            for(unsigned int n=range_limits[0];n<=range_limits[1];n*=step){
                init_ns.push_back(n);
                if(overflow_boundary_check<n){
                    break;
                }
            }
        }
    }else{
        auto sbeg = n_rangestr.cbegin();
        auto send = n_rangestr.cend();
        for (smatch size; regex_search(sbeg, send, size, sizes_regex_); sbeg = size.suffix().first)
        {
            init_ns.push_back(convert_string_to<unsigned int>(size[1]));
        }
    }

    try {


        output << "Timeout for each N is " << timeouts[0] <<" sec (for ADD and test functions)";
        //perftest places_distance_increasing;places_distance_increasing:places_distance_increasing 1:1 1 1000
        if(!extracmds.empty()){
            output <<" + " <<timeouts[1]<<" sec (for extra functions)";
        }
        output << ". " << endl;
        output << "For each N perform command(s):" << endl;

        // Initialize test functions
        vector<pair<unsigned int,bool(MainProgram::*)(Stopwatch&, std::ostream&)>> testfuncs;
        vector<pair<unsigned int,bool(MainProgram::*)(Stopwatch&, std::ostream&)>> extrafuncs;

        for (auto& i : testcmds)
        {
            auto pos = find_if(cmds_.begin(), cmds_.end(), [&i](auto const& cmd){ return cmd.cmd == i.second; });
            if (pos != cmds_.end() && pos->testfunc)
            {
                output << i.second << " ";
                testfuncs.push_back({i.first,pos->testfunc});
            }
            else
            {
                output << "(cannot test " << i.second << ") ";
            }
        }
        if(!extracmds.empty()){
            output << endl << "with extra functions"<<endl;

            for (auto& i : extracmds)
            {
                auto pos = find_if(cmds_.begin(), cmds_.end(), [&i](auto const& cmd){ return cmd.cmd == i.second; });
                if (pos != cmds_.end() && pos->testfunc)
                {
                    output << i.second << " ";
                    extrafuncs.push_back({i.first,pos->testfunc});
                }
                else
                {
                    output << "(cannot add extra function " << i.second << ") ";
                }
            }
            output<< endl<<"after every " <<n_extra << " entries in test functions"<<endl;
        }
        output << endl<<endl;

        if (testfuncs.empty())
        {
            output << "No commands to test!" << endl;
            return {};
        }

#ifdef USE_PERF_EVENT
        output << setw(7) << "N" << " , " << setw(12) << "add (sec)" << " , " << setw(12) << "add (count)" << " , " << setw(12) << "cmds (sec)" << " , "
               << setw(12) << "cmds (count)"  << " , " << setw(12) << "total (sec)" << " , " << setw(12) << "total (count)" << endl;
#else
        output << setw(7) << "N" << " , " << setw(12) << "add (sec)" << " , " << setw(12) << "cmds (sec)" << " , "
               << setw(12) << "total (sec)" << endl;
#endif
        flush_output(output);

        auto stop = false;
        for (unsigned int n : init_ns)
        {
            if (stop) { break; }

            output << setw(7) << n << " , " << flush;
            ds_.clear_all();
            init_primes();

            Stopwatch stopwatch(true); // Use also instruction counting, if enabled
            Stopwatch extrasw;

            // Add random places
            // modify the add to work and add correct number of things also with lower n values
            for (unsigned int i = 0; i < n / 1000; ++i)
            {
                // TODO check return value
                add_random_places_areas(stopwatch,1000, output);
                init_perftest(stopwatch, 1000, testcmds[0].second, output);

                if (stopwatch.elapsed() >= timeouts[0])
                {
                    output << "ADD Timeout!" << endl;
                    stop = true;
                    break;
                }
                if (check_stop())
                {
                    output << "Stopped!" << endl;
                    stop = true;
                    break;
                }
            }
            if (stop) { break; }

            if (n % 1000 != 0)
            {
                // TODO check return value
                add_random_places_areas(stopwatch,n % 1000, output);
            }

#ifdef USE_PERF_EVENT
            auto addcount = stopwatch.count();
#endif
            auto addsec = stopwatch.elapsed();

#ifdef USE_PERF_EVENT
            output << setw(12) << addsec << " , " << setw(12) << addcount << " , " << flush;
#else
            output << setw(12) << addsec << " , " << flush;
#endif

            if (addsec >= timeouts[0])
            {
                output << "ADD Timeout!" << endl;
                stop = true;
                break;
            }

            // TODO reformat this to its own function -> return instead of stop and unnecessary checks

            unsigned int extra=0;
            for (auto pair : testfuncs)
            {
                auto cmdpos = pair.second;
                for (unsigned int i = 0; i<pair.first;++i){
                    if (!(this->*cmdpos)(stopwatch, output)){
                        output <<"Failed check!"<<endl;
                        stop=true;
                        break;
                    }
                    if (stopwatch.elapsed() >= timeouts[0])
                    {
                        output << "Timeout!" << endl;
                        stop = true;
                        break;
                    }
                    if (check_stop())
                    {
                        output << "Stopped!" << endl;
                        stop = true;
                        break;
                    }
                }
                if(stop){
                    break;
                }
                // run extra functions every "n_extra" elements in the command list
                if((++extra)%(n_extra<1?1:n_extra) == 0 && extra<testfuncs.size()){
                    for (auto extrapair : extrafuncs){
                        auto cmdpos = extrapair.second;
                        for (unsigned int i = 0; i<extrapair.first;++i){
                            if (!(this->*cmdpos)(extrasw, output)){
                                output <<"Extrafunc Failed check!"<<endl;
                                stop=true;
                                break;
                            }
                            if (extrasw.elapsed() >= timeouts[1])
                            {
                                output << "Extrafunc Timeout!" << endl;
                                stop = true;
                                break;
                            }
                            if (check_stop())
                            {
                                output << "Stopped!" << endl;
                                stop = true;
                                break;
                            }
                        }
                    }
                }
                if(stop){
                    break;
                }
            }
            if (stop) { break; }

#ifdef USE_PERF_EVENT
            auto totalcount = stopwatch.count();
#endif
            auto totalsec = stopwatch.elapsed();

#ifdef USE_PERF_EVENT
            output << setw(12) << totalsec-addsec << " , " << setw(12) << totalcount-addcount << " , " << setw(12) << totalsec << " , " << setw(12) << totalcount;
#else
            output << setw(12) << totalsec-addsec << " , " << setw(12) << totalsec;
#endif

            //        unsigned long int maxmem;
            //        string unit;
            //        tie(maxmem, unit) = mempeak();
            //        maxmem -=  startmem;
            //        if (maxmem != 0)
            //        {
            //            output << ", memory " << maxmem << " " << unit;
            //        }
            output << endl;
            flush_output(output);
        }

        ds_.clear_all();
        init_primes();

    }
    catch (NotImplemented const&)
    {
        // Clean up after NotImplemented
        ds_.clear_all();
        init_primes();
        throw;
    }

#ifdef _GLIBCXX_DEBUG
    output << "WARNING: Debug STL enabled, performance will be worse than expected (maybe also asymptotically)!" << endl;
#endif // _GLIBCXX_DEBUG

    return {};
}

MainProgram::CmdResult MainProgram::cmd_comment(std::ostream& /*output*/, MatchIter /*begin*/, MatchIter /*end*/)
{
    return {};
}

bool MainProgram::command_parse_line(string inputline, ostream& output)
{
    //    static unsigned int nesting_level = 0; // UGLY! Remember nesting level to print correct amount of >:s.
    //    if (promptstyle != PromptStyle::NO_NESTING) { ++nesting_level; }

    if (inputline.empty()) { return true; }

    smatch match;
    bool matched = regex_match(inputline, match, cmds_regex_);
    if (matched)
    {
        assert(match.size() == 3);
        string cmd = match[1];
        string params = match[2];

        auto pos = find_if(cmds_.begin(), cmds_.end(), [cmd](CmdInfo const& ci) { return ci.cmd == cmd; });
        assert(pos != cmds_.end());

        smatch match2;
        bool matched2 = regex_match(params, match2, pos->param_regex);
        if (matched2)
        {
            if (pos->func)
            {
                assert(!match2.empty());

                Stopwatch stopwatch;
                bool use_stopwatch = (stopwatch_mode != StopwatchMode::OFF);
                // Reset stopwatch mode if only for the next command
                if (stopwatch_mode == StopwatchMode::NEXT) { stopwatch_mode = StopwatchMode::OFF; }

                TestStatus initial_status = test_status_;
                test_status_ = TestStatus::NOT_RUN;

                if (use_stopwatch)
                {
                    stopwatch.start();
                }

                CmdResult result;
                try
                {
                    result = (this->*(pos->func))(output, ++(match2.begin()), match2.end());
                }
                catch (NotImplemented const& e)
                {
                    output << endl << "NotImplemented from cmd " << pos->cmd << " : " << e.what() << endl;
                    std::cerr << endl << "NotImplemented from cmd " << pos->cmd << " : " << e.what() << endl;
                }

                if (use_stopwatch)
                {
                    stopwatch.stop();
                }

                switch (result.first)
                {
                    case ResultType::NOTHING:
                    {
                        break;
                    }
                    case ResultType::IDLIST:
                    {
                        auto& [areas, places] = std::get<CmdResultIDs>(result.second);
                        if (places.size() == 1 && places.front() == NO_PLACE)
                        {
                            output << "Failed (NO_PLACE returned)!" << std::endl;
                        }
                        else
                        {
                            if (!places.empty())
                            {
                                if (places.size() == 1) { output << "Place:" << std::endl; }
                                else { output << "Places:" << std::endl; }

                                unsigned int num = 0;
                                for (PlaceID id : places)
                                {
                                    ++num;
                                    if (places.size() > 1) { output << num << ". "; }
                                    else { output << "   "; }
                                    print_place(id, output);
                                }
                            }
                        }

                        if (areas.size() == 1 && areas.front() == NO_AREA)
                        {
                            output << "Failed (NO_AREA returned)!" << std::endl;
                        }
                        else
                        {
                            if (!areas.empty())
                            {
                                if (areas.size() == 1) { output << "Area:" << std::endl; }
                                else { output << "Areas:" << std::endl; }

                                unsigned int num = 0;
                                for (AreaID id : areas)
                                {
                                    ++num;
                                    if (areas.size() > 1) { output << num << ". "; }
                                    else { output << "   "; }
                                    print_area(id, output);
                                }
                            }
                        }
                        break;
                    }
                    case ResultType::NAMELIST: {
                        auto& resulttype = std::get<CmdResultNames>(result.second);
                        auto& sid = resulttype.first.first;
                        auto rid = convert_to_string(resulttype.first.second);
                        auto& name = resulttype.second;
                        std::string type = sid!=NO_PLACE? "place":"area";
                        if (name == NO_NAME)
                        {
                            output << "Failed (NO_NAME returned)!" << std::endl;
                            break;
                        }
                        output << "Name for "<<type<<" with id " << (sid!=NO_PLACE? sid:rid);
                        output <<" is " << name<<endl;
                        break;
                    }
                    case ResultType::TYPELIST: {
                        auto& resulttype = std::get<CmdResultPlaceTypes>(result.second);
                        auto& sid = resulttype.first.first;
                        auto rid = convert_to_string(resulttype.first.second);
                        auto& typelist = resulttype.second;
                        std::string type = sid!=NO_PLACE? "Place":"Area";
                        if (typelist==PlaceType::NO_TYPE){
                            output << "Failed (NO_TYPE returned)!" << std::endl;
                            break;
                        }
                        output << type << " "<< (sid!=NO_PLACE? sid:rid);
                        output <<" is type of "<<endl;
                        //for (const auto& placetype : typelist){
                        print_place_type(typelist, output);
                        //}
                        break;
                    }
                    case ResultType::COORDLIST: {
                        auto& resulttype = std::get<CmdResultCoords>(result.second);
                        auto& sid = resulttype.first.first;
                        auto rid = convert_to_string(resulttype.first.second);
                        auto& coordlist = resulttype.second;
                        std::string type = sid!=NO_PLACE? "Place":"Area";
                        if (coordlist.size()==1 && coordlist.front()==NO_COORD){
                            output << "Failed (NO_COORD returned)!" << std::endl;
                            break;
                        }
                        output << type << " "<< (sid!=NO_PLACE? sid:rid);
                        output <<" has coordinates"<<endl;
                        for (const auto& coord : coordlist){
                            print_coord(coord, output);
                        }
                        break;
                    }
                default:
                {
                    assert(false && "Unsupported result type!");
                }
                }

                if (result != prev_result)
                {
                    prev_result = std::move(result);
                    view_dirty = true;
                }

                if (use_stopwatch)
                {
                    output << "Command '" << cmd << "': " << stopwatch.elapsed() << " sec";
                    output << endl;
                }

                if (test_status_ != TestStatus::NOT_RUN)
                {
                    output << "Testread-tests have been run, " << ((test_status_ == TestStatus::DIFFS_FOUND) ? "differences found!" : "no differences found.") << endl;
                }
                if (test_status_ == TestStatus::NOT_RUN || (test_status_ == TestStatus::NO_DIFFS && initial_status == TestStatus::DIFFS_FOUND))
                {
                    test_status_ = initial_status;
                }
            }
            else
            { // No function to run = quit command
                return false;
            }
        }
        else
        {
            output << "Invalid parameters for command '" << cmd << "'!" << endl;
        }
    }
    else
    {
        output << "Unknown command!" << endl;
    }

    return true; // Signal continuing
}

void MainProgram::command_parser(istream& input, ostream& output, PromptStyle promptstyle)
{
    string line;
    do
    {
//        output << string(nesting_level, '>') << " ";
        output << PROMPT;
        getline(input, line, '\n');

        if (promptstyle != PromptStyle::NO_ECHO)
        {
            output << line << endl;
        }

        if (!input) { break; }

        bool cont = command_parse_line(line, output);
        view_dirty = false; // No need to keep track of individual result changes
        if (!cont) { break; }
    }
    while (input);
    //    if (promptstyle != PromptStyle::NO_NESTING) { --nesting_level; }

    view_dirty = true; // To be safe, assume that results have been changed
}

void MainProgram::setui(MainWindow* ui)
{
    ui_ = ui;
}

#ifdef GRAPHICAL_GUI
void MainProgram::flush_output(std::ostream& output)
{
    if (ui_)
    {
        if (auto soutput = dynamic_cast<ostringstream*>(&output))
        {
            ui_->output_text(*soutput);
        }
    }
}
#else
void MainProgram::flush_output(std::ostream& /*output*/)
{
}
#endif

bool MainProgram::check_stop() const
{
#ifdef GRAPHICAL_GUI
    if (ui_)
    {
        return ui_->check_stop_pressed();
    }
#endif
    return false;
}

std::array<unsigned long int, 20> const MainProgram::primes1{4943,   4951,   4957,   4967,   4969,   4973,   4987,   4993,   4999,   5003,
                                                             5009,   5011,   5021,   5023,   5039,   5051,   5059,   5077,   5081,   5087};
std::array<unsigned long int, 20> const MainProgram::primes2{81031,  81041,  81043,  81047,  81049,  81071,  81077,  81083,  81097,  81101,
                                                             81119,  81131,  81157,  81163,  81173,  81181,  81197,  81199,  81203,  81223};

MainProgram::MainProgram()
{
    rand_engine_.seed(time(nullptr));

    //    startmem = get<0>(mempeak());

    init_primes();
    init_regexs();
}

int MainProgram::mainprogram(int argc, char* argv[])
{
    vector<string> args(argv, argv+argc);

    if (args.size() < 1 || args.size() > 2)
    {
        cerr << "Usage: " + ((args.size() > 0) ? args[0] : "<program name>") + " [<command file>]" << endl;
        return EXIT_FAILURE;
    }

    MainProgram mainprg;

    if (args.size() == 2 && args[1] != "--console")
    {
        string filename = args[1];
        ifstream input(filename);
        if (input)
        {
            mainprg.command_parser(input, cout, MainProgram::PromptStyle::NORMAL);
        }
        else
        {
            cout << "Cannot open file '" << filename << "'!" << endl;
        }
    }
    else
    {
        mainprg.command_parser(cin, cout, MainProgram::PromptStyle::NO_ECHO);
    }

    cerr << "Program ended normally." << endl;
    if (mainprg.test_status_ == TestStatus::DIFFS_FOUND)
    {
        return EXIT_FAILURE;
    }
    else
    {
        return EXIT_SUCCESS;
    }
}

PlaceType MainProgram::convert_string_to_placetype(std::string from)
{
    std::istringstream istr(from);
    std::string typestr;
    istr >> std::noskipws >> typestr;
    if (istr.fail() || !istr.eof())
    {
        throw std::invalid_argument("Cannot convert string to place type");
    }

    PlaceType result = PlaceType::NO_TYPE;
    if (typestr == "firepit") { result = PlaceType::FIREPIT; }
    else if (typestr == "shelter") { result = PlaceType::SHELTER; }
    else if (typestr == "parking") { result = PlaceType::PARKING; }
    else if (typestr == "peak") { result = PlaceType::PEAK; }
    else if (typestr == "bay") { result = PlaceType::BAY; }
    else if (typestr == "area") { result = PlaceType::AREA; }
    else if (typestr == "other") { result = PlaceType::OTHER; }
    else
    {
        throw std::invalid_argument("Cannot convert string to place type");
    }

    return result;
}

std::string MainProgram::convert_placetype_to_string(PlaceType type)
{
    switch (type)
    {
    case PlaceType::FIREPIT:
        return "firepit";
    case PlaceType::SHELTER:
        return "shelter";
    case PlaceType::PARKING:
        return "parking";
    case PlaceType::PEAK:
        return "peak";
    case PlaceType::BAY:
        return "bay";
    case PlaceType::AREA:
        return "area";
    case PlaceType::OTHER:
        return "other";
    default:
        return "!!no type!!";
    }
}

void MainProgram::init_primes()
{
    // Initialize id generator
    prime1_ = primes1[random<int>(0, primes1.size())];
    prime2_ = primes2[random<int>(0, primes2.size())];
    random_places_added_ = 0;
    random_areas_added_ = 0;
    clear_test_cache();
}

Name MainProgram::random_name(){
    Name id;
    // there might be changes in this function on grader

    // all perftest Names are 8 characters long -> (27*2)^20 possibilities
    // this results in pretty low chance of collisions when using <=10^6 ids

    // first get charcode
    auto chardistr = std::uniform_int_distribution<int>('A','Z'+('z'-'a'));
    for(unsigned int i = 0; i<NAME_LENGTH;++i){
        auto charint = chardistr(rand_engine_);
        // if charcode is above uppercase, add offset for lowercase charcodes
        id+=(char)(charint>'Z'?charint+('a'-'Z'):charint);
    }
    return id;
}

Coord MainProgram::random_coord()
{
    int x = random<int>(min.x, max.x);
    int y = random<int>(min.y, max.y);
    Coord newcoord{x,y};
    return newcoord;
}

AreaID MainProgram::random_area_id()
{
    return random<AreaID>(0,MAX_AREAID);
}

Name MainProgram::n_to_name(unsigned long n)
{
    if(n<valid_place_names_.size()){
        // this WILL result in different names for same n since valid_names_ is unordered set
        return *std::next(valid_place_names_.begin(),n);
    }
    Name name;
    for(name=random_name();valid_place_names_.find(name)!=valid_place_names_.end();name=random_name()){}
    valid_place_names_.insert(name);
    return name;
}

PlaceID MainProgram::random_place_id(){
    PlaceID id;
    // there might be changes in this function on grader

    // all perftest PlaceIDs are 20 characters long -> (27*2)^20 possibilities
    // this results in pretty low chance of collisions when using <=10^6 ids

    // first get charcode
    auto chardistr = std::uniform_int_distribution<int>('A','Z'+('z'-'a'));
    for(unsigned int i = 0; i<PLACEID_LENGTH;++i){
        auto charint = chardistr(rand_engine_);
        // if charcode is above uppercase, add offset for lowercase charcodes
        id+=(char)(charint>'Z'?charint+('a'-'Z'):charint);
    }
    return id;
}

PlaceID MainProgram::n_to_placeid(unsigned long n)
{
    if(n<place_ids_.size()){
        return *std::next(place_ids_.begin(),n);
    }
    PlaceID id;
    for(id=random_place_id();valid_place_ids_.find(id)!=valid_place_ids_.end();id=random_place_id()){}
    valid_place_ids_.insert(id);
    place_ids_.push_back(id);
    return id;


}

AreaID MainProgram::n_to_areaid(unsigned long n)
{
    if(n<area_ids_.size()){
        return *std::next(area_ids_.begin(),n);
    }
    AreaID id;
    for(id=random_area_id();valid_area_ids_.find(id)!=valid_area_ids_.end();id=random_area_id()){}
    valid_area_ids_.insert(id);
    area_ids_.push_back(id);
    return id;
}

Coord MainProgram::n_to_coord(unsigned long n)
{
    if (n<coords_.size()){
        return *std::next(coords_.begin(),n);
    }
    Coord xy;
    for(xy=random_coord();valid_coords_.find(xy)!=valid_coords_.end();xy=random_coord()){}
    valid_coords_.insert(xy);
    coords_.push_back(xy);
    return xy;

}

void MainProgram::clear_test_cache()
{
    valid_place_ids_.clear();
    valid_area_ids_.clear();
    valid_coords_.clear();
    place_ids_.clear();
    area_ids_.clear();
    coords_.clear();
    valid_place_names_.clear();

}

AreaID MainProgram::random_root_area()
{
    if(random_areas_added_<2){
        return n_to_areaid(0);
    }
    unsigned long end = ROOT_BIAS_MULTIPLIER * random_areas_added_;
    if (end == 0 ) {
        return n_to_areaid(0);
    }
    return n_to_areaid(random<decltype(random_areas_added_)>(0, end));
}

AreaID MainProgram::random_leaf_area()
{
    if(random_areas_added_<2){
        return n_to_areaid(0);
    }
    unsigned long start = LEAF_BIAS_MULTIPLIER * random_areas_added_;
    if (start == random_areas_added_) {
        start = 0;
    }
    return n_to_areaid(random<decltype(random_areas_added_)>(start, random_areas_added_));
}

PlaceID MainProgram::random_valid_place()
{
    return n_to_placeid(random<decltype(random_places_added_)>(0, random_places_added_));
}

AreaID MainProgram::random_valid_area()
{
    return n_to_areaid(random<decltype(random_areas_added_)>(0, random_areas_added_));
}

Coord MainProgram::random_valid_coord()
{
    return *std::next(coords_.begin(),random<decltype(coords_.size())>(0,coords_.size()));
}

void MainProgram::init_regexs()
{
    // Create regex <whitespace>(cmd1|cmd2|...)<whitespace>(.*)
    string cmds_regex_str = "[[:space:]]*(";
    bool first = true;
    for (auto& cmd : cmds_)
    {
        cmds_regex_str += (first ? "" : "|") + cmd.cmd;
        first = false;

        cmd.param_regex = regex(cmd.param_regex_str+"[[:space:]]*", std::regex_constants::ECMAScript | std::regex_constants::optimize);
    }
    cmds_regex_str += ")(?:[[:space:]]*$|"+wsx+"(.*))";
    cmds_regex_ = regex(cmds_regex_str, std::regex_constants::ECMAScript | std::regex_constants::optimize);
    coords_regex_ = regex(coordx+"[[:space:]]?", std::regex_constants::ECMAScript | std::regex_constants::optimize);
    commands_divide_regex_ = regex("([0-9a-zA-Z_;\\*]+):?", std::regex_constants::ECMAScript | std::regex_constants::optimize);
    timeouts_regex_=regex("([0-9]+):?",std::regex_constants::ECMAScript | std::regex_constants::optimize);
    commands_regex_ = regex("(?:([0-9]+)\\*)?([0-9a-zA-Z_]+);?", std::regex_constants::ECMAScript | std::regex_constants::optimize);
    sizes_regex_ = regex(numx+";?", std::regex_constants::ECMAScript | std::regex_constants::optimize);
    range_regex_=regex(numx+":?",std::regex_constants::ECMAScript | std::regex_constants::optimize);
    is_range_notation_regex_ =regex("([0-9]+(?::[0-9]+){1,2})",std::regex_constants::ECMAScript | std::regex_constants::optimize);
}
