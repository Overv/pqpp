CC = clang++
CFLAGS =
LDFLAGS = -lpq

example: example.cc pq.hpp
	$(CC) $(CFLAGS) -std=c++11 -o example example.cc $(LDFLAGS)

.PHONY: clean
clean:
	rm -f example
