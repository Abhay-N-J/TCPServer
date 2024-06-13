SERIAL_DIR = src/serial

SERIAL_OBJECTS = $(SERIAL_DIR)/main.o $(SERIAL_DIR)/server.o $(SERIAL_DIR)/data_store.o

SERIAL_HEADERS = $(SERIAL_DIR)/server.hpp $(SERIAL_DIR)/data_store.hpp

serial_debug: $(SERIAL_OBJECTS)
	g++ -fsanitize=address $(SERIAL_OBJECTS) -o bin/serial_server_debug

serial: $(SERIAL_OBJECTS)
	g++ $(SERIAL_OBJECTS) -o bin/serial_server

$(SERIAL_DIR)/%.o: $(SERIAL_DIR)/%.cpp $(SERIAL_HEADERS)
	g++ -c $< -o $@


all: serial parallel
	echo "Made all"

parallel_debug: src/parallel/main.cpp
	g++ -o bin/parallel_server_debug src/parallel/main.cpp -fsanitize=address -g

test_serial: serial
	./bin/serial_server 8080 > /dev/null &
	PARALLEL=0 ./test.sh
	killall serial_server 

test_parallel: parallel
	./bin/parallel_server 8080 > /dev/null &
	PARALLEL=1 ./test.sh
	killall parallel_server

parallel: src/parallel/main.cpp
	g++ -std=c++11 -o bin/parallel_server -pthread src/parallel/main.cpp

clean:
	rm bin/*
	rm src/serial/*.o

test: test_serial test_parallel
	echo "Made test"

