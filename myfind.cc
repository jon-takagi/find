// Authors: Jon Takagi, Robert McCaull

#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <chrono>
#include <stdlib.h>
#include <fnmatch.h>
#include <functional>
#include <cstring>


namespace fs = std::filesystem;
// do_exec is a wrapper for the execvp syscall that allows us to do some error handling and type conversion.
// turns out there are a lot of ways not to turn an std::vector<std::string> into a char **

bool do_exec(const std::string& command, std::vector<std::string>& args) {
    int wstatus = 0;
    pid_t id = fork();
    if(id == 0) {
        char** c_strs = (char**) calloc(args.size() + 1, sizeof(char*));
        for (long unsigned int i = 0; i < args.size(); i++) {
            c_strs[i] = (char*) args[i].c_str();
        }
        execvp(command.c_str(), c_strs);
    } else if (id < 0) {
        exit(-1);
    } else {
        waitpid(id, &wstatus, 0);
    }
    return (wstatus == 0);
}


// input: argc and argv from the command line input
// parses the input and returns (as a tuple):
//   - whether or not to follow symlinks (-L)
//   - whether or not the command line has an action (if not, then we need to use the default -print)
//   - the starting points for the recursive search (as strings, not as fs::paths)
//   - a vector of lambda functions.

// the functions (in order) correspond to the tests input on the command line
// ex: myfind -name README.md -type f
// will return two functions (call them expr1 and expr2)
// expr1(/path/to/something) will return true if the filename matches README.md (using the same fnmatch syscall that find uses)
// expr2(/path/to/something) will return true if the `something` is a regular file.
// print and exec always return true, since find always evaluates the expression after an action

std::tuple<bool, bool, std::vector<std::string>, std::vector<std::function<bool(fs::path)>>> parse_command_line(int argc, char** argv) {
    int current_arg = 1;
    bool follow_symlinks = 0;
    bool has_action = 0;
    std::vector<std::string> starting_points {};
    std::vector<std::function<bool(fs::path)>> expr = {};
    std::string current_dir = ".";
    const char* name = "-name";
    const char* type = "-type";
    const char* mtime = "-mtime";
    const char* exec = "-exec";
    const char* print = "-print";

    // Bail out if no command-line arguments were given

    if(argc == 1) {
        starting_points.push_back(current_dir);
        std::tuple<bool, bool, std::vector<std::string>, std::vector<std::function<bool(fs::path)>>> parsed_output = std::make_tuple(
            follow_symlinks,
            has_action,
            starting_points,
            expr
        );
        return parsed_output;
    }

    // Check for -L (follow symlinks)
    if(!strcmp(argv[current_arg],"-L")) {
        follow_symlinks = 1;
        current_arg++;
    } else {
        follow_symlinks = 0;
    }

    // Check whether the next command-line argument is a predicate in the
    //  expression, or a directory, and either parse the user's starting
    //  directories or use the default accordingly.

    if(argv[current_arg][0] == '-') {
        starting_points.push_back(current_dir);
    }
    while (current_arg < argc && argv[current_arg][0] != '-') {
        starting_points.push_back(std::string(argv[current_arg]));
        current_arg++;
    };

    // Parse the expression.
    while(current_arg < argc) {
        // Make sure that the current command-line argument is a predicate
        if (*(argv[current_arg]) != '-') {
            std::cout<<"find: paths must precede expression: `" << argv[current_arg] << "'" << std::endl;
            std::cout<<"find: possible unquoted pattern after predicate `-name'?" << std::endl;
            exit(1);
        }
        else if(!strcmp(argv[current_arg], name)) {
            current_arg++;
            if (current_arg >= argc) {
                std::cout << "find: missing argument to `-name'" << std::endl;
                exit(1);
            }
            std::string pattern(argv[current_arg]);
            std::function<bool(fs::path)> func = [=](fs::path path) {
                return !fnmatch(pattern.c_str(), path.filename().c_str(), 0);
            };
            expr.push_back(func);
        }
        else if(!strcmp(argv[current_arg], type)) {
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
        else if(!strcmp(argv[current_arg], mtime)) {
            current_arg++;
            if (current_arg >= argc) {
                std::cout << "find: missing argument to `-name'" << std::endl;
                exit(1);
            }
            int age = 0;
            try {
                age = std::stoi(argv[current_arg]);
            } catch (...) {
                std::cout<<"find: invalid argument `" << argv[current_arg] << "' to `-mtime'" << std::endl;
                exit(1);
            }
            std::function<bool(fs::path)> func = [=](fs::path path) {
                auto file_time = fs::last_write_time(path);
                auto now = fs::file_time_type::clock::now();
                return (now - file_time).count() / 86400 > age;
            };
            expr.push_back(func);
        }
        else if(!strcmp(argv[current_arg], exec)) {
            has_action = 1;
            current_arg++;

            if (current_arg >= argc) {
                std::cout << "find: missing argument to `-exec'" << std::endl;
                exit(1);
            }

            std::string exec_command(argv[current_arg]);
            std::vector<std::string> raw_exec_args;
            while (current_arg < argc && strcmp(argv[current_arg], ";")) {
                raw_exec_args.push_back(std::string(argv[current_arg]));
                current_arg++;
            }
            if (current_arg >= argc) {
                std::cout << "find: missing argument to `-exec'" << std::endl;
                exit(1);
            }

            // This function copy-captures raw_exec_args.  When called by
            //  do_simplifind, it uses the passed path to build a vector of
            //  command-line arguments for execvp, and then hands them off to
            //  do_exec.
            std::function<bool(fs::path)> func = [=](fs::path path) {
                std::vector<std::string> these_exec_args;
                // The command-line arguments we pass to execvp should be what
                //  the user indicated, but with instances of '{}' replaced with
                //  the name of the file under consideration.
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
                return do_exec(exec_command, these_exec_args);
            };
            expr.push_back(func);
        }
        else if(!strcmp(argv[current_arg], print)) {
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
    std::tuple<bool, bool, std::vector<std::string>, std::vector<std::function<bool(fs::path)>>> parsed_output = std::make_tuple(follow_symlinks, has_action, starting_points, expr);
    return parsed_output;
}

/* do_simplifind carries out a search mimicking find, using the output of
    parse_command_line.

    follow_symlinks: a bool that indicates whether do_simplifind should follow
        symlinks (true) or not (false)

    has_action: a bool indicating whether or not the user gave an explicit
        action to take on found files as part of the expression (in the
        command-line input).  (true when they have, false when they haven't)
        If no explicit action was called for, do_simplifind defaults to print.

    starting_points: a vector of strings indicating all of the root directories
        do_simplifind should search.  do_simplifind applies its instructions
        to those directories and all of their children.

    instructions: a vector of function objects, corresponding to the parsed
        expression.  Each function takes as input a file path, returns true or
        false, and may have a side effect.  do_simplifind calls each function on
        each file it encounters, in order, until it exhausts them or until one
        returns false.  Thus, a function can represent either a test (by
        returning false if the file path doesn't pass, thus stopping the rest
        of the instructions from being called), or an action (by having some
        side effect when called, such as printing the path).
*/

void do_simplifind(bool follow_symlinks, bool has_action,
                    std::vector<std::string>& starting_points,
                    std::vector<std::function<bool(fs::path)> >& instructions){
    fs::directory_options option;

    //follow symlinks if we are told to do so
    if (follow_symlinks) {
        option = fs::directory_options::follow_directory_symlink;
    } else {
        option = fs::directory_options::none;
    }
    for (auto &start: starting_points) {
        std::string root = start;
        fs::path path_to_root(root);
        if(!fs::exists(root)) {
            std::cout << "find: ‘" << path_to_root.string() << "’: No such file or directory" << std::endl;
        } else {

            //assemble a list of file paths to which to apply our instructions
            fs::recursive_directory_iterator it(path_to_root, option);
            std::vector<fs::path> paths;
            paths.push_back(path_to_root);
            for(auto &path: it) {
                paths.push_back(path.path());
            }

            //apply our instructions to each path
            for(auto &path: paths) {
                bool passes_tests = true;
                for (auto &instruction: instructions) {
                    if (!instruction(path)) {
                        passes_tests = false;
                        break;
                    }
                }
                if(!has_action && passes_tests) {
                    std::cout << path.string() << std::endl;
                }
            }
        }
    }
}

/* Main invokes parse_command_line to to interpret command-line arguments, then
    passes the results to do_simplifind to carry out the search.
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
