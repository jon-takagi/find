#include <iostream>
#include <string>
#include <list>

int main(int argc, char ** argv) {
    int current_arg = 1;
    bool follow_symlinks = 0;
    std::list<std::string> starting_points {};
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
    // return parsed_output;

    // po::command_line_style::style_t style = po::command_line_style::style_t(
    //     po::command_line_style::allow_long_disguise|
    //     po::command_line_style::long_allow_next
    // );
    // po::options_description desc("Supported Options");
    // desc.add_options()
    //     ("start", po::value<std::vector<std::string>>(), "the positions to start searching from")
    //     ("name", po::value<std::string>(), "Base of file name (the path with the leading directories removed) matches shell pattern pattern. The metacharacters ('*', '?', and '[]') match a '.' at the start of the base name (this is a change in findutils-4.2.2; see section STANDARDS CONFORMANCE below). To ignore a directory and the files under it, use -prune; see an example in the description of -path. Braces are not recognised as being special, despite the fact that some shells including Bash imbue braces with a special meaning in shell patterns. The filename matching is performed with the use of the fnmatch(3) library function. Don't forget to enclose the pattern in quotes in order to protect it from expansion by the shell.")
    //     ("mtime", po::value<int>(), "File's data was last modified n*24 hours ago. See the comments for -atime to understand how rounding affects the interpretation of file modification times.")
    //     ("type", po::value<char>(), "File is of type c: \nb block (buffered) special\n c character (unbuffered) special\n d directory\n p named pipe (FIFO) \n f regular file \nl symbolic link; this is never true if the -L option or the -follow option is in effect, unless the symbolic link is broken. If you want to search for symbolic links when -L is in effect, use -xtype. \n s socket\nD door (Solaris)")
    //     ("exec", po::value<std::string>(), "Execute command; true if 0 status is returned. All following arguments to find are taken to be arguments to the command until an argument consisting of ';' is encountered. The string '{}' is replaced by the current file name being processed everywhere it occurs in the arguments to the command, not just in arguments where it is alone, as in some versions of find. Both of these constructions might need to be escaped (with a '\') or quoted to protect them from expansion by the shell. See the EXAMPLES section for examples of the use of the -exec option. The specified command is run once for each matched file. The command is executed in the starting directory. There are unavoidable security problems surrounding use of the -exec action; you should use the -execdir option instead.")
    //     ("L,L", po::bool_switch(), "Follow symbolic links")
    // ;
    // po::positional_options_description pd;
    // pd.add("start", -1);
    // po::variables_map vm;
    // auto parsed = po::command_line_parser(argc, argv).options(desc).style(style).positional(pd).run();
    // // std::cout << parsed.options << std::endl;
    // auto print = [](auto val) { std::cout << val.string_key << std::endl;};
    // std::for_each(parsed.options.begin(), parsed.options.end(), print);
    // // po::parse_command_line(argc, argv, desc, style, )
    // po::store(parsed, vm);
    // po::notify(vm);
    // if(vm.count("start")) {
    //     std::vector<std::string> directories = vm["start"].as<std::vector<std::string>>();
    //     std::for_each(directories.begin(), directories.end(), [](std::string n) { std::cout << n << "\n"; });
    //     std::cout << ".\n";
    // }
    // if(vm.count("name")) {
    //     std::cout << "name set to: " << vm["name"].as<std::string>() << ".\n";
    // }
    // if(vm.count("mtime")) {
    //     std::cout << "mtime set to: " << vm["mtime"].as<int>() << ".\n";
    // }
    // if(vm.count("type")) {
    //     std::cout << "type set to: " << vm["type"].as<char>() << ".\n";
    // }
    // if(vm.count("exec")) {
    //     std::cout << "exec set to: " << vm["exec"].as<std::string>() << ".\n";
    // }
    // if(vm.count("L")) {
    //     std::cout << "L was set to: " << vm["L"].as<bool>() << ".\n";
    // }
    // std::cout << argc << " "  << argv << std::endl;
    return 0;
}
