all: simplifind
	#All done!

debug: find.o
	g++ -g -o simplifind find.o -Wall -Werror -Wextra
	#All done!

simplifind: find.o
	g++ -o simplifind find.o -O3 -Wall -Werror -Wextra

find.o:
	g++ -g -c find.cpp -Wall -Werror -Wextra

clean:
	rm *.o
