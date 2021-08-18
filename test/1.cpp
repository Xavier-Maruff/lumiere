
#define ANSI_RESET "\033[0m"
#define ANSI_CYAN "\033[36m"
#define ANSI_RED "\033[31m"
#define ANSI_GREEN "\033[32m"
#define ANSI_YELLOW "\033[33m"


//functions to test
extern "C" {
    double ftest1(double);
    double ftest2(double);
    double ftest3(double);
    double ftest4(double, double, signed long);
    double ftest5(double);
    double ftest6(double);
    double ftest7(double);
    double ftest8();

    const char* stest1(const char*);
    const char* stest2();
    const char* stest3();
}

//Source of truth

double ftest1_check(double a) { return -a * 2.0 + 12; }
double ftest2_check(double a) { return a * 2.0 + -3.0 / 2.0; }
double ftest3_check(double a) { return a - 12.0; }
double ftest4_check(double a, double y, signed long p) { return a * p - p * y + a / -y; }
double ftest5_check(double a) { return ftest1(ftest4(a, 1.0123, 200))+12.0; }
double ftest6_check(double a) {return 12;}
double ftest7_check(double a) {return (a+2)*3;}
double ftest8_check() {return 10;}


//Run funcs, check match

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <cmath>
#include <memory>

#define MAX_STR_LEN 100

float total_tests = 0;
float total_passed = 0;

std::string passed, failed;

//levenshtein distance
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
    return dist[l2][l1];
    delete[] dist;
}

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

int main(int argc, char* argv[]) {
    std::stringstream passed_str, failed_str;
    passed_str << ANSI_GREEN << "Passed" << ANSI_RESET;
    failed_str << ANSI_RED << "Failed" << ANSI_RESET;

    passed = passed_str.str();
    failed = failed_str.str();

    std::vector<std::string> expected = {};
    std::vector<std::string> actual = {};

    //run tests
    //double tests
    push_string<double>(actual, ftest1(1.0));
    push_string<double>(actual, ftest1(-1.0));
    push_string<double>(actual, ftest2(1.0));
    push_string<double>(actual, ftest2(-1.0));
    push_string<double>(actual, ftest3(1.0));
    push_string<double>(actual, ftest3(-1.0));
    push_string<double>(actual, ftest4(3.0, 4.0, 5));
    push_string<double>(actual, ftest4(-3.0, -4.0, -5));
    push_string<double>(actual, ftest5(1.0));
    push_string<double>(actual, ftest5(-1.0));
    push_string<double>(actual, ftest6(1.0));
    push_string<double>(actual, ftest6(-1.0));
    push_string<double>(actual, ftest7(1.0));
    push_string<double>(actual, ftest7(-1.0));
    push_string<double>(actual, ftest7(12.56));
    push_string<double>(actual, ftest7(-12.56));
    push_string<double>(actual, ftest8());

    //string tests
    actual.emplace_back(stest1("test"));
    actual.emplace_back(stest2());
    actual.emplace_back(stest3());

    //sources of truth
    //double truths
    push_string<double>(expected, ftest1_check(1.0));
    push_string<double>(expected, ftest1_check(-1.0));
    push_string<double>(expected, ftest2_check(1.0));
    push_string<double>(expected, ftest2_check(-1.0));
    push_string<double>(expected, ftest3_check(1.0));
    push_string<double>(expected, ftest3_check(-1.0));
    push_string<double>(expected, ftest4_check(3.0, 4.0, 5));
    push_string<double>(expected, ftest4_check(-3.0, -4.0, -5));
    push_string<double>(expected, ftest5_check(1.0));
    push_string<double>(expected, ftest5_check(-1.0));
    push_string<double>(expected, ftest6_check(1.0));
    push_string<double>(expected, ftest6_check(-1.0));
    push_string<double>(expected, ftest7_check(1.0));
    push_string<double>(expected, ftest7_check(-1.0));
    push_string<double>(expected, ftest7_check(12.56));
    push_string<double>(expected, ftest7_check(-12.56));
    push_string<double>(expected, ftest8_check());
    //string truths
    expected.push_back("test");
    expected.push_back("my name jeff");
    expected.push_back("my name jeff");


    std::cout << std::endl << "\tTest Results" << std::endl;
    std::cout << "\t===============================" << std::endl;
    std::cout << "\t### | status | lev dist " << std::endl;
    std::cout << "\t===============================" << std::endl;
    for (int index = 0; index < actual.size(); index++) {
        std::cout << ANSI_CYAN << "\t" << std::setfill('0') << std::setw(3) << index << ANSI_RESET
            << " | " << check_res(actual[index], expected[index]) << std::endl;
    }
    std::cout << "\t===============================" << std::endl;

    float passed_perc = (total_passed/total_tests)*100;

    std::cout << "\tTotal passed: " << (passed_perc >= 50 ? ANSI_GREEN : ANSI_RED)
    << passed_perc << "%" << ANSI_RESET << std::endl << std::endl;

    return 0;
}