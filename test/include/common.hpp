#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <cmath>
#include <memory>

#define MAX_STR_LEN 100

#define ANSI_RESET "\033[0m"
#define ANSI_CYAN "\033[36m"
#define ANSI_RED "\033[31m"
#define ANSI_GREEN "\033[32m"
#define ANSI_YELLOW "\033[33m"

//test generators
#define double_test(x, args...)\
push_string<double>(actual, x(args));\
push_string<double>(expected, x##_check(args));

#define string_test(x, y, args...)\
actual.push_back(x(args));\
expected.push_back(y);

//test globals
extern float total_tests;
extern float total_passed;
extern std::vector<std::string> expected;
extern std::vector<std::string> actual;

std::string passed, failed;

//levenstein distance
int lev_dist(std::string s1, std::string s2) {
    int i, j, t, track;
    const int l1 = s1.size();
    const int l2 = s2.size();
    auto dist = new int[MAX_STR_LEN][MAX_STR_LEN];
    for (int i = 0; i <= l1; i++) {
        dist[0][i] = i;
    }
    for (int j = 0; j <= l2; j++) {
        dist[j][0] = j;
    }
    for (int j = 1; j <= l1; j++) {
        for (int i = 1; i <= l2; i++) {
            if (s1[i - 1] == s2[j - 1]) {
                track = 0;
            }
            else {
                track = 1;
            }
            t = std::min((dist[i - 1][j] + 1), (dist[i][j - 1] + 1));
            dist[i][j] = std::min(t, (dist[i - 1][j - 1] + track));
        }
    }
    int ret = dist[l2][l1];
    delete[] dist;
    return ret;
}

//check pass or fail
std::string check_res(std::string actual, std::string expected) {
    int lev_dist_cmp = lev_dist(actual, expected);
    std::stringstream lev_dist_stream;
    lev_dist_stream << std::setfill('0') << std::setw(3) << lev_dist_cmp;
    total_tests++;
    if (actual == expected || lev_dist_cmp <= (actual.size()/3)) {
        total_passed++;
        return passed + " | " + lev_dist_stream.str() + (actual == expected ? "" : " (within bounds: " + expected + ", " + actual + ")");
    }
    else return failed + " | " + lev_dist_stream.str() + " (expected \"" + expected + "\", got \"" + actual + "\")";
}

template<typename T>
inline void push_string(std::vector<std::string>& vect, T elem) {
    vect.push_back(std::to_string(elem));
}

void setup_ansi(){
    std::stringstream passed_str, failed_str;
    passed_str << ANSI_GREEN << "Passed" << ANSI_RESET;
    failed_str << ANSI_RED << "Failed" << ANSI_RESET;

    passed = passed_str.str();
    failed = failed_str.str();
}