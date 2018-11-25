CC = gcc
LDFLAGS = -pthread -lrt
CFLAGS = -Wall -Wextra `allegro-config --libs`
DOCKERFLAGS = -ti --rm --privileged -e DISPLAY="10.10.10.10:0.0"

.PHONY: all
all:
	$(CC) $(LDFLAGS) -o graphics graphics.c $(CFLAGS)

.PHONY: clean
clean:
	rm *.o

.PHONY: docker
docker:
	docker build --rm -f "Dockerfile" -t artificial-nose:latest .
	docker run $(DOCKERFLAGS) artificial-nose
