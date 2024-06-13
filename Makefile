SERIAL_DIR = src/serial

SERIAL_OBJECTS = $(SERIAL_DIR)/main.o $(SERIAL_DIR)/server.o $(SERIAL_DIR)/data_store.o

SERIAL_HEADERS = $(SERIAL_DIR)/server.hpp $(SERIAL_DIR)/data_store.hpp

PARALLEL_DIR = src/parallel

PARALLEL_OBJECTS = $(PARALLEL_DIR)/main.o $(PARALLEL_DIR)/server.o $(PARALLEL_DIR)/data_store.o $(PARALLEL_DIR)/thread_pool.o

PARALLEL_HEADERS = $(PARALLEL_DIR)/server.hpp $(PARALLEL_DIR)/data_store.hpp $(PARALLEL_DIR)/thread_pool.hpp

parallel_debug: $(PARALLEL_OBJECTS)
	g++ -fsanitize=address $(PARALLEL_OBJECTS) -o bin/parallel_server_debug

parallel: $(PARALLEL_OBJECTS)
	g++ $(PARALLEL_OBJECTS) -o bin/parallel_server

$(PARALLEL_DIR)/%.o: $(PARALLEL_DIR)/%.cpp $(PARALLEL_HEADERS)
	g++ -c $< -o $@

serial_debug: $(SERIAL_OBJECTS)
	g++ -fsanitize=address $(SERIAL_OBJECTS) -o bin/serial_server_debug

serial: $(SERIAL_OBJECTS)
	g++ $(SERIAL_OBJECTS) -o bin/serial_server

$(SERIAL_DIR)/%.o: $(SERIAL_DIR)/%.cpp $(SERIAL_HEADERS)
	g++ -c $< -o $@


all: serial parallel
	echo "Made all"

test_serial: serial
	./bin/serial_server 8080 > /dev/null &
	PARALLEL=0 ./test.sh
	killall serial_server 

test_parallel: parallel
	./bin/parallel_server 8080 > /dev/null &
	PARALLEL=1 ./test.sh
	killall parallel_server

clean:
	rm bin/* &
	rm src/serial/*.o &
	rm src/parallel/*.o &

test: test_serial test_parallel
	echo "Made test"

