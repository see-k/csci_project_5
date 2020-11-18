all: Project5 run

Project5: Project5.cpp
	g++ -std=gnu++11 -pthread -o Project5 Project5.cpp

run: Project5
	./Project5

clean:
	-rm Project5