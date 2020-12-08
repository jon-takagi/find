#include <iostream>
#include <regex>
#include <string>
#include <vector>
#include <filesystem>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <unistd.h>
#include <sys/wait.h>
/*
find procedure:
    1. use boost to parse input flags
        -name pattern (including wildcards)
        -mtime n (to simplify, I will only test with n=0, so don't bother with negatives or plus sign).
        -type t
        -exec command (only the ; variant).
        -print
        -L (follow symbolic links)

    done
    2. run find (navigate the file structure)
        start with a list of starting points to traverse.  while some remain, pop one and:
            1. create a directory iterator to that path (using error code checking)
                -if there's an error, cry about it and quit
            2. iterate through the directory contents
                -if the item is a file, check if it matches - and perform the necessary action!
                -if the item is a directory, add it to the list of paths to traverse
                -if the item is a symlink:
                    ???
        print stuff out!

        use std::regex::regex for checking the pattern.

*/

namespace fs = std::filesystem;

// using options = ?;
std::tuple<bool, std::vector<std::string>, std::vector<std::string>> parse_command_line(int argc, char** argv) {
    int current_arg = 1;
    bool follow_symlinks = 0;
    std::vector<std::string> starting_points {};
    std::string current_dir = ".";
    if(argv[current_arg] == "-L") {
        follow_symlinks = 1;
        current_arg++;
    } else {
        follow_symlinks = 0;
        current_arg++;
    }
    // std::cout << current_arg << ": " << argv[current_arg] << std::endl;
    if(argv[current_arg][0] == '-') {
        starting_points.push_back(current_dir);
        current_arg++;
    }
    while (argv[current_arg][0] != '-') {
        starting_points.push_back(std::string(argv[current_arg]));
        current_arg++;
    };
    std::vector<std::string> expr;
    while(current_arg < argc) {
        std::string line(argv[current_arg]);
        expr.push_back(line);
        current_arg++
    }
    std::tuple<bool, std::list<std::string>, std::vector<std::string>> parsed_output = std::make_tuple(follow_symlinks, starting_points, expr);
    return parsed_output;
}

/*
void name(bool& passed, fs::path path) {};
*/

/*tests:
- name (does it match the given pattern)
- type (is the file of the given type)
- mtime (file was last modified n days ago)

actions:
- print
- exec
*/
// void do_simplifind(std::vector<path> &starting_points, options) {
//
// }
//

void do_simplifind(bool follow_symlinks, std::vector<std::string>& starting_points, std::vector<std::function<bool(fs::path)> >& instructions){
    fs::directory_options option;
    if (follow_symlinks) {
        option = fs::directory_options::follow_directory_symlink;
    } else {
        option = fs::directory_options::none;
    }
    for (auto &start: starting_points.begin()) {
        std::string root = start;
        fs::path path_to_root(root);
        fs::recursive_directory_iterator it(path_to_root, option);
        for(auto &path: it) {
            for (auto &instruction: instructions.begin()) {
                if (!instruction(path)) {
                    break;
                }
            }
        }
    }
}

/*
if (print) {
    std::cout << p.path() << std::endl;
}
if (exec) { //TODO: set up exec_command, these_exec_args (or pass them in). . .
    std::vector<std::string> these_exec_args;
    std::transform(std::begin(exec_args), std::end(exec_args), std::back_inserter(these_exec_args),
        [](s){
            rs = std::string(s);
            std::string file_name = p.path().string();
            for (size_t pos = rs.find("{}"); pos != std::string::npos; pos = rs.find("{}", pos)) {
                rs.replace(pos, 2, file_name);
                pos += file_name.size();
            }
            return rs;
        }
    );
    do_exec(&exec_command, &these_exec_args);
}
*/

void do_exec(std::string& command, std::vector<std::string>& args) { //execvp
    /*
        Something like:
            1. fork
            2. if you're the parent, wait for the child, then return
                if you're the child:
                    set up argv
                    run exec
        execvpe?
    */
    pid_t id = fork();
    if(id == 0) {
        std::vector<char *> c_strs;
        std::transform(std::begin(args), std::end(args), std::back_inserter(c_strs), std::mem_fn(&std::string::c_str));
        c_strs.push_back((char*) NULL);
        execvp(command.c_str(), c_strs.data());
    } else if (id < 0) {
        cout << "Failed to start child process" << std::endl;
        exit(-1);
    } else {
        waitpid(id);
    }
}

int main(int argc, char ** argv) {
    auto parsed_output = parse_command_line(argc, argv);
    do_simplifind(std::get<0>(parsed_output), &std::get<1>(parsed_output), &std::get<2>(parsed_output));
    return 0;
}
