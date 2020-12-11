all: simplifind
	#All done!

simplifind:
	g++-8 -o simplifind.bin simplifind.cpp -O3 -Wall -Werror -Wextra --std=c++17 -lstdc++fs
clean:
	rm *.bin
	rm test_find
