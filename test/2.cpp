

extern "C" {
    double get_glob();
    double mutate_a(double);
    double mutate_glob(double);
    const char* stest(const char*);
}

#include <iostream>
#include <string>

int main(int argc, char* argv[]){
    std::cout << get_glob()
    << ", " << mutate_a(1.0)
    << ", " << mutate_glob(1.0)
    << ", " << stest(std::string("asdf").c_str())
    << std::endl;
}