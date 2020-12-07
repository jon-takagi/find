#include <typeinfo>
#include <vector>
#include <string>
#include <iostream>
// usage: expr_parser *.cpp
int main(int argc, char** argv) {
    std::vector<std::string> strs;
    for(int i = 0; i < argc; i++) {
        strs.push_back(std::string(argv[i]));
    }
    std::vector<char *> c_strs;
    for(int i = 0; i < argc; i++) {
        std::cout << typeid(strs[i].c_str()).name() << std::endl;
        // c_strs.push_back(strs[i].c_str());
    }
    return 0;
}
