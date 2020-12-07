#include <string>
#include <iostream>
#include <vector>
#include <filesystem>
// #include <boost/filesystem.hpp>


using path = std::filesystem::path;
using entry = std::filesystem::directory_entry;
using dirit = std::filesystem::directory_iterator;
namespace fs = std::filesystem;

int main(int argc, char *argv[]) {
    std::vector<std::string> roots = {".", "/vagrant", ".."};
    while(!roots.empty()) {
        std::string root = roots.back();
        roots.pop_back();
        std::cout << root << std::endl;
        fs::path path_to_root(root);
        fs::directory_options option = (
            fs::directory_options::follow_directory_symlink
        );
        fs::recursive_directory_iterator it(path_to_root, options);
        for(auto &p: it) {
            std::cout << p.path() << std::endl;
        }
    }
    return 0;
}
