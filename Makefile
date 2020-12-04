all: simplifind
	#All done!

debug: find.o
	g++ -g -o simplifind find.o -Wall -Werror -Wextra
	#All done!

simplifind: find.o
	g++ -o simplifind find.o -O3 -Wall -Werror -Wextra
po_example:
	g++ -o po_example po_example.cpp -O3 -Wall -Werror -Wextra -I /usr/local/boost_1_61_0 ../lib/libboost_program_options.a

find.o:
	g++ -g -c find.cpp -Wall -Werror -Wextra  -I /usr/local/boost_1_61_0 ../lib/libboost_program_options.a ../lib/libboost_filesystem.a

clean:
	rm *.o
