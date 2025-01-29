GPP=g++ -Wall -Werror -Wextra

all: rebuild

build:
	$(GPP) server.cpp -o server
	$(GPP) client.cpp -o client

rebuild: clean build

clean:
	rm -rf server client result.txt 