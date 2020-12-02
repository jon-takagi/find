#include <iostream>
#include <filesystem>
#include <regex>
#include <string>
#include <vector>
/*
find procedure:
    1. use boost to parse input flags
        -name pattern (including wildcards)
        -mtime n (to simplify, I will only test with n=0, so don't bother with negatives or plus sign).
        -type t
        -exec command (only the ; variant).
        -print
        -L (follow symbolic links)
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

using path = std::filesystem::path;
using entry = std::filesystem::directory_entry;
using dirit = std::filesystem::directory_iterator;
using options = ?;



void do_simplifind(std::vector<path> &starting_points, options) {

}

void do_exec(std::string command, args) {
    /*
        Something like:
            1. fork
            2. if you're the parent, wait for the child, then return
                if you're the child:
                    set up argc, argv
                    run exec
        execvpe?
    */

}

namespace po = boost::program_options;
int main(int argc, char ** argv) {
    po::options_description desc("Supported Options");
    desc.add_options()
        ("name", po::value<string>(), "Base of file name (the path with the leading directories removed) matches shell pattern pattern. The metacharacters ('*', '?', and '[]') match a '.' at the start of the base name (this is a change in findutils-4.2.2; see section STANDARDS CONFORMANCE below). To ignore a directory and the files under it, use -prune; see an example in the description of -path. Braces are not recognised as being special, despite the fact that some shells including Bash imbue braces with a special meaning in shell patterns. The filename matching is performed with the use of the fnmatch(3) library function. Don't forget to enclose the pattern in quotes in order to protect it from expansion by the shell.")
        ("mtime", po::value<int>(), "File's data was last modified n*24 hours ago. See the comments for -atime to understand how rounding affects the interpretation of file modification times.")
        ("type", po::value<char>(), "File is of type c: \nb block (buffered) special\n c character (unbuffered) special\n d directory\n p named pipe (FIFO) \n f regular file \nl symbolic link; this is never true if the -L option or the -follow option is in effect, unless the symbolic link is broken. If you want to search for symbolic links when -L is in effect, use -xtype. \n s socket\nD door (Solaris)")
        ("exec", po::value<string>(), "Execute command; true if 0 status is returned. All following arguments to find are taken to be arguments to the command until an argument consisting of ';' is encountered. The string '{}' is replaced by the current file name being processed everywhere it occurs in the arguments to the command, not just in arguments where it is alone, as in some versions of find. Both of these constructions might need to be escaped (with a '\') or quoted to protect them from expansion by the shell. See the EXAMPLES section for examples of the use of the -exec option. The specified command is run once for each matched file. The command is executed in the starting directory. There are unavoidable security problems surrounding use of the -exec action; you should use the -execdir option instead.")
        ("L", "Follow symbolic links")
    ;
    po::variables_map vm;
    po::store(po::parse_command_line(ac, av, desc), vm);
    po::notify(vm);
    if(vm.count("name")) {
        std::cout << "name set to: " << vm["name"] << ".\n";
    }
    if(vm.count("mtime")) {
        std::cout << "name set to: " << vm["name"] << ".\n";
    }
    if(vm.count("type")) {
        std::cout << "name set to: " << vm["typ"] << ".\n";
    }
    if(vm.count("exec")) {
        std::cout << "name set to: " << vm["exec"] << ".\n";
    }
    if(vm.count("L")) {
        std::cout << "name set to: " << vm["L"] << ".\n";
    }
    std::cout << argc << " "  << argv << std::endl;
    return 0;
}
