Build instructions:

This code was built using boost_1_61_0, but also tested with boost_1_73_0, the most recent version at time of submission. The getting started guide is labled as version 1.74, but refers to 1.73 internally. 
Create two environment variables:

`BOOST_PATH` should point to the header files (eg, the ones that unzip). Following the boost ("Getting Started")[https://www.boost.org/doc/libs/1_74_0/more/getting_started/unix-variants.html] guide, mine points to `/usr/local/boost_1_61_0`
`BOOST_LIB` should point to the built library archive files created by running `./b2 install`. Mine points to `/vagrant/os/lib` since I used the installation prefix `--prefix=/vagrant/os`


`make all`
