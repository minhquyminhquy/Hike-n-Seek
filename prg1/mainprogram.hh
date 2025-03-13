#ifndef MAINPROGRAM_HH
#define MAINPROGRAM_HH


#ifdef QT_CORE_LIB
#ifndef NO_GRAPHICAL_GUI
#define GRAPHICAL_GUI
#endif
#endif


#include <string>
#include <random>
#include <regex>
#include <chrono>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <array>
#include <functional>
#include <utility>
#include <variant>
#include <bitset>
#include <cassert>
#include <unordered_set>

#include "datastructures.hh"

class MainWindow; // In case there's UI

const Coord DEFAULT_MIN_COORD={1,1};
const Coord DEFAULT_MAX_COORD={10000,10000};
const double ROOT_BIAS_MULTIPLIER = 0.05;
const double LEAF_BIAS_MULTIPLIER = 0.5;

class MainProgram
{
public:
    MainProgram();


    class Stopwatch;

    enum class PromptStyle { NORMAL, NO_ECHO, NO_NESTING };
    enum class TestStatus { NOT_RUN, NO_DIFFS, DIFFS_FOUND };

    bool command_parse_line(std::string input, std::ostream& output);
    void command_parser(std::istream& input, std::ostream& output, PromptStyle promptstyle);

    void setui(MainWindow* ui);

    void flush_output(std::ostream& output);
    bool check_stop() const;

    static int mainprogram(int argc, char* argv[]);

private:
    Datastructures ds_;
    MainWindow* ui_ = nullptr;

    static std::string const PROMPT;

    std::minstd_rand rand_engine_;

    static std::array<unsigned long int, 20> const primes1;
    static std::array<unsigned long int, 20> const primes2;
    unsigned long int prime1_ = 0; // Will be initialized to random value from above
    unsigned long int prime2_ = 0; // Will be initialized to random value from above
    unsigned long int random_places_added_ = 0; // Counter for random places added
    unsigned long int random_areas_added_ = 0; // Counter for random areas added
    const unsigned int NAME_LENGTH = 8;
    const unsigned int PLACEID_LENGTH = 20;
    const unsigned long long int MAX_AREAID=std::numeric_limits<unsigned long int>::max();

    Coord min=DEFAULT_MIN_COORD;
    Coord max=DEFAULT_MAX_COORD;
    void init_primes();
    Name n_to_name(unsigned long int n);
    PlaceID n_to_placeid(unsigned long int n);
    AreaID n_to_areaid(unsigned long int n);
    Coord n_to_coord(unsigned long int n);


    std::unordered_set<PlaceID> valid_place_ids_;
    std::unordered_set<AreaID> valid_area_ids_;
    std::unordered_multiset<Coord> valid_coords_;
    std::vector<PlaceID> place_ids_;
    std::vector<AreaID> area_ids_;
    std::vector<Coord> coords_;

    std::unordered_multiset<Name> valid_place_names_;
    void clear_test_cache();

    AreaID random_root_area();
    AreaID random_leaf_area();
    PlaceID random_valid_place();
    AreaID random_valid_area();
    Coord random_valid_coord();

    enum class StopwatchMode { OFF, ON, NEXT };
    StopwatchMode stopwatch_mode = StopwatchMode::OFF;

    enum class ResultType { NOTHING, IDLIST, NAMELIST, TYPELIST, COORDLIST };
    using CmdResultIDs = std::pair<std::vector<AreaID>, std::vector<PlaceID>>;
    using CmdResultNames = std::pair<std::pair<PlaceID,AreaID>,Name>;
    using CmdResultPlaceTypes = std::pair<std::pair<PlaceID,AreaID>,PlaceType>;
    using CmdResultCoords = std::pair<std::pair<PlaceID,AreaID>,std::vector<Coord>>;
    using CmdResult = std::pair<ResultType, std::variant<CmdResultIDs, CmdResultNames, CmdResultPlaceTypes, CmdResultCoords>>;
    CmdResult prev_result;
    bool view_dirty = true;

    TestStatus test_status_ = TestStatus::NOT_RUN;

    using MatchIter = std::smatch::const_iterator;
    struct CmdInfo
    {
        std::string cmd;
        std::string info;
        std::string param_regex_str;
        CmdResult(MainProgram::*func)(std::ostream& output, MatchIter begin, MatchIter end);
        bool(MainProgram::*testfunc)(Stopwatch& stopwatch, std::ostream&);
        std::regex param_regex = {};
    };
    static std::vector<CmdInfo> cmds_;
    // Regex objects and their initialization
    std::regex cmds_regex_;
    std::regex coords_regex_;
    std::regex placetimes_regex_;
    std::regex times_regex_;
    std::regex commands_divide_regex_;
    std::regex timeouts_regex_;
    std::regex commands_regex_;
    std::regex sizes_regex_;
    std::regex range_regex_;
    std::regex is_range_notation_regex_;
    void init_regexs();

    CmdResult cmd_get_place_count(std::ostream& output, MatchIter begin, MatchIter end);
    CmdResult cmd_clear_all(std::ostream& output, MatchIter begin, MatchIter end);
    CmdResult cmd_all_places(std::ostream& output, MatchIter begin, MatchIter end);
    CmdResult cmd_add_place(std::ostream& output, MatchIter begin, MatchIter end);

    CmdResult cmd_get_place_name(std::ostream& output, MatchIter begin, MatchIter end);
    CmdResult cmd_get_area_name(std::ostream& output, MatchIter begin, MatchIter end);
    CmdResult cmd_get_place_type(std::ostream& output, MatchIter begin, MatchIter end);
    CmdResult cmd_get_place_coord(std::ostream& output, MatchIter begin, MatchIter end);
    CmdResult cmd_get_area_coords(std::ostream& output, MatchIter begin, MatchIter end);

    CmdResult cmd_find_places_with_coord(std::ostream& output, MatchIter begin, MatchIter end);
    CmdResult cmd_find_places_with_name(std::ostream& output, MatchIter begin, MatchIter end);
    CmdResult cmd_find_places_with_type(std::ostream& output, MatchIter begin, MatchIter end);
    CmdResult cmd_change_place_coord(std::ostream& output, MatchIter begin, MatchIter end);
    CmdResult cmd_change_place_name(std::ostream& output, MatchIter begin, MatchIter end);
    CmdResult cmd_add_area(std::ostream& output, MatchIter begin, MatchIter end);
    CmdResult cmd_all_areas(std::ostream& output, MatchIter begin, MatchIter end);
    CmdResult cmd_add_subarea_to_area(std::ostream& output, MatchIter begin, MatchIter end);
    CmdResult cmd_ancestor_areas_of_subarea(std::ostream& output, MatchIter begin, MatchIter end);
    CmdResult cmd_all_subareas_of_area(std::ostream& output, MatchIter begin, MatchIter end);
    CmdResult cmd_get_places_closest_to(std::ostream& output, MatchIter begin, MatchIter end);
    CmdResult cmd_remove_place(std::ostream& output, MatchIter begin, MatchIter end);
    CmdResult cmd_get_closest_common_ancestor_of_areas(std::ostream& output, MatchIter begin, MatchIter end);

    CmdResult help_command(std::ostream& output, MatchIter begin, MatchIter end);
    CmdResult cmd_randseed(std::ostream& output, MatchIter begin, MatchIter end);
    CmdResult cmd_random_places(std::ostream& output, MatchIter begin, MatchIter end);
    CmdResult cmd_read(std::ostream& output, MatchIter begin, MatchIter end);
    CmdResult cmd_testread(std::ostream& output, MatchIter begin, MatchIter end);
    CmdResult cmd_stopwatch(std::ostream& output, MatchIter begin, MatchIter end);
    CmdResult cmd_perftest(std::ostream& output, MatchIter begin, MatchIter end);
    CmdResult cmd_comment(std::ostream& output, MatchIter begin, MatchIter end);

    bool test_all_places(Stopwatch &stopwatch,std::ostream& output);
    bool test_find_places_with_coord(Stopwatch& stopwatch, std::ostream& output);
    bool test_find_places_with_name(Stopwatch& stopwatch, std::ostream& output);
    bool test_find_places_with_type(Stopwatch& stopwatch, std::ostream& output);
    bool test_change_place_coord(Stopwatch& stopwatch, std::ostream& output);
    bool test_change_place_name(Stopwatch& stopwatch, std::ostream& output);
    bool test_ancestor_areas_of_subarea(Stopwatch& stopwatch, std::ostream& output);
    bool test_all_subareas_of_area(Stopwatch& stopwatch, std::ostream& output);
    bool test_get_places_closest_to(Stopwatch& stopwatch, std::ostream& output);
    bool test_remove_place(Stopwatch& stopwatch, std::ostream& output);
    bool test_get_closest_common_ancestor_of_areas(Stopwatch& stopwatch, std::ostream& output);
    bool test_random_places(Stopwatch& stopwatch, std::ostream& output);

    bool test_get_place_count(Stopwatch& stopwatch, std::ostream& output);
    bool test_add_place(Stopwatch& stopwatch, std::ostream& output);
    bool test_get_place_name(Stopwatch& stopwatch, std::ostream& output);
    bool test_get_place_type(Stopwatch& stopwatch, std::ostream& output);
    bool test_get_place_coord(Stopwatch& stopwatch, std::ostream& output);
    bool test_add_area(Stopwatch& stopwatch, std::ostream& output);
    bool test_all_areas(Stopwatch& stopwatch, std::ostream& output);
    bool test_get_area_name(Stopwatch& stopwatch, std::ostream& output);
    bool test_get_area_coords(Stopwatch& stopwatch, std::ostream& output);
    bool test_add_subarea_to_area(Stopwatch& stopwatch, std::ostream& output);

    bool test_get_places_alphabetically(Stopwatch &stopwatch, std::ostream& output);
    bool test_get_places_distance_increasing(Stopwatch& stopwatch, std::ostream& output);
    PlaceID random_place_id();
    Name random_name();
    Coord random_coord();
    AreaID random_area_id();

    bool init_perftest(Stopwatch &stopwatch, int n, std::string testcmd, std::ostream& output);
    bool add_random_places_areas(Stopwatch & stopwatch,unsigned int size, std::ostream& output);
    bool add_random_places(Stopwatch & stopwatch,unsigned int size, std::ostream& output);
    bool add_random_areas(Stopwatch & stopwatch,unsigned int size, std::ostream& output);
    Distance calc_distance(Coord c1, Coord c2);
    std::string print_place(PlaceID id, std::ostream& output, bool nl = true);
    std::string print_place_brief(PlaceID id, std::ostream& output, bool nl = true);
    std::string print_area(AreaID id, std::ostream& output, bool nl = true);
    std::string print_place_name(PlaceID id, std::ostream& output, bool nl = true);
    std::string print_place_type(PlaceType placetype, std::ostream& output, bool nl = true);
    std::string print_coord(Coord coord, std::ostream& output, bool nl = true);
    template <typename Type>
    Type random(Type start, Type end);
    template <typename To>
    static To convert_string_to(std::string from);
    template <typename From>
    static std::string convert_to_string(From from);

    static PlaceType convert_string_to_placetype(std::string from);
    static std::string convert_placetype_to_string(PlaceType type);

    template<PlaceID(Datastructures::*MFUNC)()>
    CmdResult NoParPlaceCmd(std::ostream& output, MatchIter begin, MatchIter end);

    template<std::vector<PlaceID>(Datastructures::*MFUNC)()>
    CmdResult NoParListCmd(std::ostream& output, MatchIter begin, MatchIter end);

    template<PlaceID(Datastructures::*MFUNC)()>
    void NoParPlaceTestCmd();

    template<std::vector<PlaceID>(Datastructures::*MFUNC)()>
    void NoParListTestCmd();

    friend class MainWindow;
};

template <typename Type>
Type MainProgram::random(Type start, Type end)
{
    auto range = end-start;
    assert(range != 0 && "random() with zero range!");

    auto num = std::uniform_int_distribution<unsigned long int>(0, range-1)(rand_engine_);

    return static_cast<Type>(start+num);
}

template <typename To>
To MainProgram::convert_string_to(std::string from)
{
    std::istringstream istr(from);
    To result;
    istr >> std::noskipws >> result;
    if (istr.fail() || !istr.eof())
    {
        throw std::invalid_argument("Cannot convert string to required type");
    }
    return result;
}

template <typename From>
std::string MainProgram::convert_to_string(From from)
{
    std::ostringstream ostr;
    ostr << from;
    if (ostr.fail())
    {
        throw std::invalid_argument("Cannot convert type to string");
    }
    return ostr.str();
}

template<PlaceID(Datastructures::*MFUNC)()>
MainProgram::CmdResult MainProgram::NoParPlaceCmd(std::ostream& /*output*/, MatchIter /*begin*/, MatchIter /*end*/)
{
    auto result = (ds_.*MFUNC)();
    return {ResultType::IDLIST, CmdResultIDs{{}, {result}}};
}

template<std::vector<PlaceID>(Datastructures::*MFUNC)()>
MainProgram::CmdResult MainProgram::NoParListCmd(std::ostream& /*output*/, MatchIter /*begin*/, MatchIter /*end*/)
{
    auto result = (ds_.*MFUNC)();
    return {ResultType::IDLIST, CmdResultIDs{{}, result}};
}

template<PlaceID(Datastructures::*MFUNC)()>
void MainProgram::NoParPlaceTestCmd()
{
    (ds_.*MFUNC)();
}

template<std::vector<PlaceID>(Datastructures::*MFUNC)()>
void MainProgram::NoParListTestCmd()
{
    (ds_.*MFUNC)();
}


#ifdef USE_PERF_EVENT
extern "C"
{
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/perf_event.h>
#include <asm/unistd.h>

static long
perf_event_open(struct perf_event_attr *hw_event, pid_t pid,
                int cpu, int group_fd, unsigned long flags)
{
    int ret;

    ret = syscall(__NR_perf_event_open, hw_event, pid, cpu,
                   group_fd, flags);
    return ret;
}
}
#endif

class MainProgram::Stopwatch
{
public:
    using Clock = std::chrono::high_resolution_clock;

    Stopwatch(bool use_counter = false) : use_counter_(use_counter)
    {
#ifdef USE_PERF_EVENT
        if (use_counter_)
        {
            memset(&pe_, 0, sizeof(pe_));
            pe_.type = PERF_TYPE_HARDWARE;
            pe_.size = sizeof(pe_);
            pe_.config = PERF_COUNT_HW_INSTRUCTIONS;
            pe_.disabled = 1;
            pe_.exclude_kernel = 1;
            pe_.exclude_hv = 1;

            fd_ = perf_event_open(&pe_, 0, -1, -1, 0);
            if (fd_ == -1) {
                throw "Couldn't open perf events!";
            }
        }
#endif
        reset();
    }

    ~Stopwatch()
    {
#ifdef USE_PERF_EVENT
        if (use_counter_)
        {
            close(fd_);
        }
#endif
    }

    void start()
    {
        running_ = true;
        starttime_ = Clock::now();
#ifdef USE_PERF_EVENT
        if (use_counter_)
        {
            ioctl(fd_, PERF_EVENT_IOC_RESET, 0);
            read(fd_, &startcount_, sizeof(startcount_));
            ioctl(fd_, PERF_EVENT_IOC_ENABLE, 0);
        }
#endif
    }

    void stop()
    {
        running_ = false;
#ifdef USE_PERF_EVENT
        if (use_counter_)
        {
            ioctl(fd_, PERF_EVENT_IOC_DISABLE, 0);
            long long c; read(fd_, &c, sizeof(c));
            counter_ += (c - startcount_);
        }
#endif
        elapsed_ += (Clock::now() - starttime_);
    }

    void reset()
    {
        running_ = false;
#ifdef USE_PERF_EVENT
        if (use_counter_)
        {
            ioctl(fd_, PERF_EVENT_IOC_DISABLE, 0);
            ioctl(fd_, PERF_EVENT_IOC_RESET, 0);
            counter_ = 0;
        }
#endif
        elapsed_ = elapsed_.zero();
    }

    double elapsed()
    {
        if (!running_)
        {
            return static_cast<std::chrono::duration<double>>(elapsed_).count();
        }
        else
        {
            auto total = elapsed_ + (Clock::now() - starttime_);
            return static_cast<std::chrono::duration<double>>(total).count();
        }
    }

#ifdef USE_PERF_EVENT
    long long count()
    {
        if (use_counter_)
        {
            if (!running_)
            {
                return counter_;
            }
            else
            {
                long long c;
                read(fd_, &c, sizeof(c));
                return counter_ + (c - startcount_);
            }
        }
        else
        {
            assert(!"perf_event not enabled during StopWatch creation!");
        }
    }
#endif

private:
    std::chrono::time_point<Clock> starttime_;
    Clock::duration elapsed_ = Clock::duration::zero();
    bool running_ = false;

    bool use_counter_;
#ifdef USE_PERF_EVENT
    struct perf_event_attr pe_;
    long long startcount_ = 0;
    long long counter_ = 0;
    int fd_ = 0;
#endif
};


#endif // MAINPROGRAM_HH
