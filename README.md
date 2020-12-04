Build instructions:

Change Makefile to point at the built boost library
`-I /usr/local/boost_1_61_0` is the unzipped, header-only portion
I built the boost library in the parent (my OS folder). Point the other portion to the `.a` file for the following boost options:
- libboost_program_options
- libboost_filesystem
`make all`
