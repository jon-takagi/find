all: simplifind
	#All done!

simplifind:
	g++-9 -o simplifind.bin simplifind.cpp -O3 -Wall -Werror -Wextra --std=c++17 -lstdc++fs
po_example:
	g++-9 -o po_example.bin po_example.cpp -O3 -Wall -Werror -Wextra --std=c++17 -lstdc++fs
clean:
	rm *.bin
