server: server.cpp
	g++ -Wall -Wextra -O2 -g server.cpp -o server

client: server.cpp
	g++ -Wall -Wextra -O2 -g client.cpp -o client
