#include <iostream>
#include <regex>
#include <string>
#include <vector>
#include <filesystem>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <chrono>
#include <stdlib.h>

/*Global TODO:
There are two issues stopping simplifind from compiling into a working file:
    We now have bugs in parse_command_line, but it compiles and runs (for some inputs)
*/

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

void do_exec(const std::string& command, std::vector<std::string>& args) {
    pid_t id = fork();
    if(id == 0) {

        std::vector<char *> c_strs;
        for (std::string s: args) {
            c_strs.push_back((char*) s.c_str());
        }
        c_strs.push_back((char*) NULL);
        execvp(command.c_str(), c_strs.data());

        //std::cout << "I'm the child! " << command << args[0] << std::endl;
    } else if (id < 0) {
        // std::cout << "Failed to start child process" << std::endl;
        exit(-1);
    } else {
        waitpid(-1, &id, 0);
    }
}
// using options = ?;
std::tuple<bool, bool, std::vector<std::string>, std::vector<std::function<bool(fs::path)>>> parse_command_line(int argc, char** argv) {
    // std::cout<<"starting parse_command_line\n";
    int current_arg = 1;
    bool follow_symlinks = 0;
    bool has_action = 0;
    std::vector<std::string> starting_points {};
    std::vector<std::function<bool(fs::path)>> expr = {};
    std::string current_dir = ".";

    if(argc == 1) {
        std::tuple<bool, bool, std::vector<std::string>, std::vector<std::function<bool(fs::path)>>> parsed_output = std::make_tuple(
            follow_symlinks,
            has_action,
            starting_points,
            expr
        );
    }

    const char* name = "-name";
    const char* type = "-type";
    const char* mtime = "-mtime";
    const char* exec = "-exec";
    const char* print = "-print";
    // std::cout<<"checking symlink options\n";
    if(!strcmp(argv[current_arg],"-L")) {
        // std::cout<<"following symlinks\n";
        follow_symlinks = 1;
        current_arg++;
    } else {
        // std::cout<<"not following symlinks\n";
        follow_symlinks = 0;
    }
    // std::cout<<"checking for starting points\n";
    // std::cout << current_arg << ": " << argv[current_arg] << std::endl;
    if(argv[current_arg][0] == '-') {
        // std::cout<<"no starting points specified; using default\n";
        starting_points.push_back(current_dir);
    }
    while (current_arg < argc && argv[current_arg][0] != '-') {
        // std::cout<<"non-default starting points found\n";
        starting_points.push_back(std::string(argv[current_arg]));
        current_arg++;
    };
    // std::cout<<"now entering expression_parsing loop\n";

    while(current_arg < argc) {
        if (*(argv[current_arg]) != '-') {
            // for (int i = 0; i < argc; i++) {
            //     std::cout<<argv[i];
            // }
            std::cout<<"find: paths must precede expression: `" << argv[current_arg] << "'" << std::endl;
            std::cout<<"find: possible unquoted pattern after predicate `-name'?" << std::endl; //TODO: will this give a different result for different predicates?
            exit(1);
        }
        // std::cout<<"in expression parsing loop\n";
        // std::cout<<"checking name\n";
        else if(!strcmp(argv[current_arg], name)) {
            // std::cout<<"doing name\n";
            current_arg++;
            if (current_arg >= argc) {
                std::cout << "find: missing argument to `-name'" << std::endl;
                exit(1);
            }
            std::regex pattern(argv[current_arg], std::regex::grep);
            std::function<bool(fs::path)> func = [=](fs::path path) {
                // std::cout << "testing regex" << std::endl;
                return std::regex_match(path.filename().c_str(), pattern);
            };
            expr.push_back(func);
        }
        // std::cout<<"checking type\n";
        else if(!strcmp(argv[current_arg], type)) {
            // std::cout<<"doing type\n";
            current_arg++;
            if (current_arg >= argc) {
                std::cout << "find: missing argument to `-type'" << std::endl;
                exit(1);
            }
            fs::file_type wanted;

            if (!strcmp(argv[current_arg],"b")){
                wanted = fs::file_type::block;
            } else if (!strcmp(argv[current_arg], "c")){
                wanted = fs::file_type::character;
            } else if (!strcmp(argv[current_arg] , "d")){
                wanted = fs::file_type::directory;
            } else if (!strcmp(argv[current_arg] , "p")){
                wanted = fs::file_type::fifo;
            } else if (!strcmp(argv[current_arg] , "f")){
                wanted = fs::file_type::regular;
            } else if (!strcmp(argv[current_arg] , "l")){
                wanted = fs::file_type::symlink;
            } else if (!strcmp(argv[current_arg] , "s")){
                wanted = fs::file_type::socket;
            } else {
                //cry about it
                std::cout<<"find: Unknown argument to -type: " << *(argv[current_arg]) << std::endl;
                exit(1);
            }

            std::function<bool(fs::path)> func = [=](fs::path path) {
                if(follow_symlinks){
                    return wanted == fs::symlink_status(path).type();
                } else {
                    return wanted == fs::status(path).type();
                }
            };
            expr.push_back(func);
        }
        // std::cout<<"checking mtime\n";
        else if(!strcmp(argv[current_arg], mtime)) {
            // std::cout<<"doing mtime\n";
            current_arg++;
            if (current_arg >= argc) {
                std::cout << "find: missing argument to `-name'" << std::endl;
                exit(1);
            }
            int age = 0;
            try {
                age = std::stoi(argv[current_arg]);
            } catch (...) {
                std::cout<<"find: Unknown argument to -mtime: " << *(argv[current_arg]) << std::endl;
                exit(1);
            }
            // find: invalid argument `<arg>' to `-mtimte'
            std::function<bool(fs::path)> func = [=](fs::path path) {
                /*
                I think I know what's going on.  A time_point is a clock (with an epoch reference),
                    and time since the epoch.  In order to make a time_point, therefore, we need a clock.
                fs::last_write_time builds a time_point from a clock called std::filesystem::__file_clock.
                This clock is different from std::chrono::steady_clock, so if I build a now time point from
                that, I won't be able to compare it to the write time without c++20 operations (conversion
                of time points between clocks).  So, I need to build now from the file clock.
                    It type checks!
                */
                auto file_time = fs::last_write_time(path);
                auto now = std::filesystem::__file_clock::now();
                return (now - file_time).count() / 86400 == age;
            };
            expr.push_back(func);
        }
        // std::cout<<"checking exec\n";
        else if(!strcmp(argv[current_arg], exec)) {
            // std::cout<<"doing exec\n";
            has_action = 1;
            current_arg++;
            std::string exec_command(argv[current_arg]);
            std::vector<std::string> raw_exec_args;
            while (strcmp(argv[current_arg], ";")) {
                raw_exec_args.push_back(std::string(argv[current_arg]));
                current_arg++;
            }
            std::function<bool(fs::path)> func = [=](fs::path path) {
                std::vector<std::string> these_exec_args;
                std::transform(std::begin(raw_exec_args), std::end(raw_exec_args), std::back_inserter(these_exec_args),
                    [path](std::string s) -> std::string {
                        std::string rs = std::string(s);
                        std::string file_name = path.string();
                        for (size_t pos = rs.find("{}"); pos != std::string::npos; pos = rs.find("{}", pos)) {
                            rs.replace(pos, 2, file_name);
                            pos += file_name.size();
                        }
                        return rs;
                    }
                );
                do_exec(exec_command, these_exec_args);
                return 1;
            };
            expr.push_back(func);
        }
        // std::cout<<"checking print\n";
        else if(!strcmp(argv[current_arg], print)) {
            // std::cout<<"doing print\n";
            has_action = 1;
            std::function<bool(fs::path)> func = [](fs::path path) { std::cout << path.string() << std::endl; return 1;};
            expr.push_back(func);
        }
        else {
            std::cout << "find: unknown predicate `" << argv[current_arg]<<"'" << std::endl;
            exit(1);

        }
        current_arg++;
    }
    // std::cout<<"outside of arg_parsing loop\n";
    std::tuple<bool, bool, std::vector<std::string>, std::vector<std::function<bool(fs::path)>>> parsed_output = std::make_tuple(follow_symlinks, has_action, starting_points, expr);
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

void do_simplifind(bool follow_symlinks, bool has_action,
                    std::vector<std::string>& starting_points,
                    std::vector<std::function<bool(fs::path)> >& instructions){
    fs::directory_options option;
    if (follow_symlinks) {
        option = fs::directory_options::follow_directory_symlink;
    } else {
        option = fs::directory_options::none;
    }
    for(std::string s : starting_points) {
        std::cout << s << std::endl;
    }
    /*
    TODO: correct our handling of the '.' and '..' special directories.
        std::filesystem recursive_directory_iterators don't see them, so we will
        need to find a way to correct that (in the cases where find sees those
        directories, which seem to be limited)

    find .. -name .. should find ..
    find -name . should find .
    these are the only two cases where it finds these directories
    */
    for (auto &start: starting_points) {
        std::string root = start;
        fs::path path_to_root(root);
        if(!fs::exists(root)) {
            std::cout << "find: '" << root << "': No such file or directory" << std::endl;
        } else {
            fs::recursive_directory_iterator it(path_to_root, option);
            for(auto &path: it) {
                bool passes_tests = true;
                for (auto &instruction: instructions) {
                    if (!instruction(path)) {
                        passes_tests = false;
                        break;
                    }
                }
                if(!has_action && passes_tests) {
                    std::cout << path << std::endl;
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


int main(int argc, char ** argv) {
    auto parsed_output = parse_command_line(argc, argv);
    bool follow_symlinks = std::get<0>(parsed_output);
    bool has_action = std::get<1>(parsed_output);
    std::vector<std::string> roots = std::get<2>(parsed_output);
    std::vector<std::function<bool(fs::path)>> exprs = std::get<3>(parsed_output);
    do_simplifind(follow_symlinks, has_action, roots, exprs);
    return 0;
}
