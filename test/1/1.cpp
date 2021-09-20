
#include "common.hpp"


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
    double ftest9();

    extern double glob;

    const char* stest1(const char*);
    const char* stest2();
    const char* stest3();
    const char* stest4();
}

//Source of truth

double ftest1_check(double a) { return -a * 2.0 + 12.0; }
double ftest2_check(double a) { return a * 2.0 + -3.0 / 2.0; }
double ftest3_check(double a) { return a - 12.0; }
double ftest4_check(double a, double y, signed long p) { return a * p - p * y + a / -y; }
double ftest5_check(double a) { return ftest1(ftest4(a, 1.123, 200))+12.0; }
double ftest6_check(double a) { return 12.0; }
double ftest7_check(double a) { return (a+2.0)*3.0; }
double ftest8_check() { return 10.0; }
double ftest9_check() { return 1.0; }


std::vector<std::string> expected = {};
std::vector<std::string> actual = {};
float total_tests = 0;
float total_passed = 0;

//Run funcs, check match
void print_char_ptr(const char* str){
    std::cout << str << std::endl;
    return;
}

int main(int argc, char* argv[]) {
    setup_ansi();

    //run tests
    //double tests
    double_test(ftest1, 1.0);
    double_test(ftest1, -1.0);
    double_test(ftest2, 1.0);
    double_test(ftest2, -1.0);
    double_test(ftest3, 1.0);
    double_test(ftest3, -1.0);
    double_test(ftest4, 3.0, 4.0, 5);
    double_test(ftest4, -3.0, -4.0, -5);
    double_test(ftest5, 1.0);
    double_test(ftest5, -1.0);
    double_test(ftest6, 1.0);
    double_test(ftest6, -1.0);
    double_test(ftest7, 1.0);
    double_test(ftest7, -1.0);
    double_test(ftest7, 12.56);
    double_test(ftest7, -12.56);
    double_test(ftest8);
    double_test(ftest9);

    string_test(stest1, "test", "test");
    string_test(stest2, "my name jeff");
    string_test(stest3, "my name jeff");
    string_test(stest4, "test");


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