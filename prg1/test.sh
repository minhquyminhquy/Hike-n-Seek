#!/bin/bash

declare -ir RANDOM_SEED=5182943142687546
declare -r EXECUTABLE_PROGRAM="prg1"

copy_pro_file() {
    if [ ! -f ${EXECUTABLE_PROGRAM}.pro.${1} ]; then
        echo "ERROR: Unable to find ${EXECUTABLE_PROGRAM}.pro.${1} compilation aborted."
        exit 1
    fi

    cp ${EXECUTABLE_PROGRAM}.pro ${EXECUTABLE_PROGRAM}.pro.original &>/dev/null
    cp ${EXECUTABLE_PROGRAM}.pro.${1} ${EXECUTABLE_PROGRAM}.pro &>/dev/null
}

restore_pro_file() {
    [ -f ${EXECUTABLE_PROGRAM}.pro.original ] && mv ${EXECUTABLE_PROGRAM}.pro.original ${EXECUTABLE_PROGRAM}.pro &>/dev/null
}

clean_files() {
    restore_pro_file
    [ -f Makefile ] && make clean &>/dev/null
    rm -f Makefile ${EXECUTABLE_PROGRAM} *.user .qmake.stash *.o *~ ~* i-test.txt test_output.txt test_errors.txt perftest.txt &>/dev/null
}

init() {
    # Check if the executable 'prg1' exists
    if [ ! -f "${EXECUTABLE_PROGRAM}" ]; then
        # TODO: Make sure that perftest flags are enabled before compiling

        #clean_files
        copy_pro_file "console"
        qmake
        make

        if [ $? -ne 0 ]; then
            echo "Compilation failed!"
            clean_files
            exit 1
        fi

        restore_pro_file

        # print empty line after compilation output
        echo
    fi
}

init_valgrind() {
    # Check if the executable 'prg1' exists
    if [ ! -f "${EXECUTABLE_PROGRAM}" ]; then
        # Run qmake and make to build the executable
        # debug flags must be ON!!!

        clean_files
        copy_pro_file "debug"
        qmake
        make

        if [ $? -ne 0 ]; then
            echo "Compilation failed!"
            clean_files
            exit 1
        fi

        restore_pro_file

        # print empty line after compilation output
        echo
    fi
}


create_test_file() {
    local infile=$1
    local outfile=$2
    local file=$3

    printf 'testread "%s" "%s"\n' "${infile}" "${outfile}" >./${file}
}


clean_test_files() {
    [ -f test_output.txt ] && rm test_output.txt
    [ -f test_errors.txt ] && rm test_errors.txt
    [ -f i-test.txt ] && rm i-test.txt
}

extra_lines_check() {
    local -i line_count=$1

    if [[ $line_count -ne 1 ]]; then
        echo "Your program's output appears to contain extra lines."
        echo "Please remove all extra debugging prints."
        printf "Your program should not print anything extra.\n\n"

        cat test_output.txt

        clean_test_files
        exit 1
    fi
}

run_tests() {
    # Run all tests from the given directory
    local test_dir=$1
    local file="i-test.txt"

    for counter in {0..20}; do
        printf -v num "%02d" ${counter}
        # Find test input and output files and read the names to 'test_files' array.
        readarray -t test_files < <(find ${test_dir} -maxdepth 1 -type f -readable -name "test-${num}-*-*.txt" -print | sort -u -)

        if [ ${#test_files[@]} -ne 2 ]; then
            # found less or more than 2 matching names
            continue
        fi

        create_test_file "${test_files[@]}" ${file}
        timeout 120 ./${EXECUTABLE_PROGRAM} "${file}" >test_output.txt 2>test_errors.txt
        local exit_code=$?

        # if test timed out (exit_code 124) return immediately
        if [ $exit_code -eq 124 ]; then
            cat test_output.txt
            if [ -s test_errors.txt ]; then
                echo
                echo "Below is what your program printed into the stderr (std::cerr) stream:"
                echo
                cat test_errors.txt
            fi
            clean_test_files

            echo
            echo "Running tests took over 120 seconds. Testing aborted."
            exit $exit_code
        fi

        # check that the second line of the result output is in correct format
        let -i actual_output=$(head -n 2 test_output.txt | grep -ciE "^\s*actual output\s+.*expected output\s*$")
        extra_lines_check $actual_output

        # check that there is only one line containing strings "Actual output" and "Expected output"
        actual_output=$(grep -ciE "^\s*actual output\s+.*expected output\s*$" test_output.txt)
        extra_lines_check $actual_output

        let -i error_count=$(grep -c '^\?' test_output.txt)

        if [[ $exit_code -eq 0 ]] && [[ $error_count -eq 0 ]]; then
            # successful tests, do not print the diff and continue to the next test
            echo "${test_files[0]}: OK"
            clean_test_files
            continue
        fi

        # print the diff for failed tests and then immediately exit
        echo "${test_files[0]}: FAIL"
        echo
        cat test_output.txt
        if [ -s test_errors.txt ]; then
            echo
            echo "Below is what your program printed into the stderr (std::cerr) stream:"
            echo
            cat test_errors.txt
        fi
        clean_test_files
        exit $((error_count))
    done
}

run_functionality_tests() {
    local test_dir="functionality-compulsory"
    [[ -v OPTIONAL ]] && test_dir="functionality-optional"

    printf "\nRunning functionality tests...\n\n"
    run_tests "${test_dir}"
}


run_integration_tests() {
    local test_dir="integration-compulsory"
    [[ -v OPTIONAL ]] && test_dir="integration-optional"

    printf "\nRunning integration tests...\n\n"
    run_tests "${test_dir}"
}


run_valgrind() {
    printf "\nRunning valgrind...\n\n"

    # Run the executable with the specified arguments if it exists
    if [ -f "${EXECUTABLE_PROGRAM}" ]; then
        local file="i-test.txt"
        echo testread \"integration-compulsory/test-00-example-in.txt\" \"integration-compulsory/test-00-example-out.txt\" >$file
        timeout 120 valgrind --tool=memcheck --leak-check=full --track-origins="yes" ./${EXECUTABLE_PROGRAM} $file
        rm $file
    fi
}

# Function to run performance tests
run_perf_tests() {
    printf "\nRunning compulsory performance tests...\n\n"

    # Run the executable with the specified arguments if it exists
    if [ -f "${EXECUTABLE_PROGRAM}" ]; then
        local file="perftest.txt"
        local first=10000
        local last=100000
        local step=20000
        local first2=20000
        local step2=20000
        echo "random_seed ${RANDOM_SEED}" >$file
        echo "perftest 20*get_place_count 5 $first:$last:$step" >>$file
        echo "perftest 20*all_places 5 $first:$last:$step" >>$file
        echo "perftest 20*get_place_name 5 $first:$last:$step" >>$file
        echo "perftest 20*add_place 5 $first:$last:$step" >>$file
        echo "perftest 20*get_place_coord 5 $first:$last:$step" >>$file
        echo "perftest 20*find_places_with_coord 5 $first2:$last:$step2" >>$file
        echo "perftest 20*get_places_distance_increasing 20 $first:$last:$step" >>$file
        echo "perftest 20*get_places_alphabetically 20 $first:$last:$step" >>$file
        echo "perftest 20*all_areas 5 $first2:$last:$step2" >>$file
        echo "perftest 20*get_area_name 5 $first:$last:$step" >>$file
        echo "perftest 20*get_area_coords 5 $first:$last:$step" >>$file
        echo "perftest 20*change_place_coord 5 $first:$last:$step" >>$file
        echo "perftest 20*change_place_name 5 $first:$last:$step" >>$file
        echo "perftest 20*ancestor_areas_of_subarea 10 $first:$last:$step" >>$file
        timeout 300 ./${EXECUTABLE_PROGRAM} $file
        rm $file
    else
        echo "prg1 executable could not be found or built."
    fi
}

# Function to run performance tests
run_opt_perf_tests() {
    printf "\nRunning optional performance tests...\n\n"

    # Run the executable with the specified arguments if it exists
    if [ -f "${EXECUTABLE_PROGRAM}" ]; then
        local file="perftest.txt"
        local first=1000
        local last=10000
        local step=1000
        echo "random_seed ${RANDOM_SEED}" >$file
        echo "perftest 20*all_subareas_of_area 5 $first:$last:$step" >>$file
        echo "perftest 20*get_closest_common_ancestor_of_areas 5 $first:$last:$step" >>$file
        echo "perftest 20*remove_place 5 $first:$last:$step" >>$file
        echo "perftest 20*get_places_closest_to 5 $first:$last:$step" >>$file
        timeout 120 ./${EXECUTABLE_PROGRAM} $file
        # ./${EXECUTABLE_PROGRAM} $file
        rm $file
    else
        echo "prg1 executable could not be found or built."
    fi
}


usage() {
    echo "Usage: $0 [-f] [-i] [-p] [-o] [-v] [-c]"
    echo
    echo "  -f   Run funtionality tests"
    echo "  -i   Run integration tests"
    echo "  -p   Run perftests"
    echo "  -o   Run optional tests (requires also one of: -f, -i, or -p)"
    echo "  -v   Run valgrind (must be run without: -f, -i, or -p)"
    echo "  -c   Force a fresh compile even if program is already compiled"
    echo
    echo "Examples:"
    echo "  $0 -cfi  (Fresh compile and then run compulsory functionality and integration tests)"
    echo "  $0 -cio  (Fresh compile and then run optional integration tests)"
    echo "  $0 -p  (run compulsory perftests, compile only if necessary)"
}



# Parse command-line arguments
while getopts "hfipovc" opt; do
    case ${opt} in
        h )
            usage
            exit 1
            ;;
        f )
            # run_functionality_tests
            FUNCTIONALITY_TESTS=1
            ;;
        i )
            # run_integration_tests
            INTEGRATION_TESTS=1
            ;;
        v )
            # run_valgrind
            if [ -v PERFTEST ]; then
                echo "ERROR: Running both valgrind and perftest at the same time is not supported"
                usage
                exit 1
            fi

            VALGRIND=1
            ;;
        p )
            # run_perf_tests
            if [ -v VALGRIND ]; then
                echo "ERROR: Running both valgrind and perftest at the same time is not supported"
                usage
                exit 1
            fi

            PERFTEST=1
            ;;
        o )
            # run_opt_perf_tests
            OPTIONAL=1
            ;;
        c )
            printf "\nClean files. Force a fresh compile.\n\n"
            clean_files
            ;;
        \? )
            usage
            exit 1
            ;;
    esac
done

# Commands/functions to run based on the commandline options provided
COMMANDS=("init")

# If no options were passed, show usage information
if [ $OPTIND -eq 1 ]; then
    usage
    exit 1
fi

if [[ -v VALGRIND ]]; then
    init_valgrind
    run_valgrind
    exit 0
fi

if [[ -v FUNCTIONALITY_TESTS ]]; then
    COMMANDS+=("run_functionality_tests")
fi

if [[ -v INTEGRATION_TESTS ]]; then
    COMMANDS+=("run_integration_tests")
fi

if [[ -v PERFTEST ]]; then
    if [[ -v OPTIONAL ]]; then
        COMMANDS+=("run_opt_perf_tests")
    else
        COMMANDS+=("run_perf_tests")
    fi
fi

# Run desired commands in order
if [[ ${#COMMANDS[@]} -gt 1 ]]; then
    for cmd in "${COMMANDS[@]}"; do
        ${cmd}
    done

    # print empty line after last commmand's output
    echo
else
    usage
    exit 1
fi
