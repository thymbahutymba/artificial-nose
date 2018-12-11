CC = gcc
LDFLAGS = -pthread -lrt -ltensorflow
CFLAGS = -Wall -Wextra
ALLEGRO =  `allegro-config --libs`
OBJECTS = sensor.o interface.o main.o ptask.o keyboard.o neural_network.o
DOCKERFLAGS = -it --rm --privileged --cap-add=sys_nice -e DISPLAY="10.10.10.10:0.0"

all: $(OBJECTS)
	$(CC) $(CFLAGS) -o main $^ $(LDFLAGS) $(ALLEGRO)

%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm *.o
	rm main

docker: build run

build:
	docker build --rm -f "Dockerfile" -t artificial-nose:latest .

run:
	docker run $(DOCKERFLAGS) artificial-nose
	