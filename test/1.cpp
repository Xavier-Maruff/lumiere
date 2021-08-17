
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
    double ftest5(double, signed long);

    const char* stest1(const char*);
    const char* stest2();
    const char* stest3();
}

//Source of truth

double ftest1_check(double a){return -a*2.0+12;}
double ftest2_check(double a){return a*2.0+-3.0/2.0;}
double ftest3_check(double a){return a-12.0;}
double ftest4_check(double a, double y, signed long p){return a*p-p*y+a/-y;}
double ftest5_check(double a, signed long b){
    double c = ftest1_check(a);
    return ftest4_check(a, c, b);
}

/*
char* stest1_check(char* a){
    return ("my name jeff";
}*/


//Run funcs, check match

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>

std::string passed, failed;

inline std::string check_res(std::string actual, std::string expected){
    if(actual == expected) return passed;
    else return failed+" (expected \""+expected+"\", got \""+actual+"\")";
}

template<typename T>
inline void push_string(std::vector<std::string>& vect, T elem){
    vect.push_back(std::to_string(elem));
}

int main(int argc, char* argv[]){
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
    push_string<double>(actual, ftest5(1.0, 2));
    push_string<double>(actual, ftest5(-1.0, -2));

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
    push_string<double>(expected, ftest5_check(1.0, 2));
    push_string<double>(expected, ftest5_check(-1.0, -2));
    //string truths
    expected.push_back("test");
    expected.push_back("my name jeff");
    expected.push_back("my name jeff");
    

    std::cout << std::endl << "\tTest Results" << std::endl;
    std::cout << "\t===============================" << std::endl;
    for(int index = 0; index < actual.size(); index++){
       std::cout << ANSI_CYAN << "\t#" << std::setfill('0') << std::setw(3) << index << ANSI_RESET
       << " | " << check_res(actual[index], expected[index])  << std::endl;
    }
    std::cout << "\t===============================" << std::endl;
    std::cout << std::endl;

    return 0;
}