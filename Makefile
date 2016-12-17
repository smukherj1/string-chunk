CC = g++

all: test

chunk_test: chunk_manager_sys.h chunk_manager_test.cpp
	$(CC) -O2 -Wall -std=c++11 chunk_manager_test.cpp -o chunk_test

test: chunk_test
	@./chunk_test

clean:
	-rm chunk_test

format:
	clang-format -i chunk_manager_sys.h chunk_manager_test.cpp