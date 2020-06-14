CC = clang
CFLAGS = -Wall -Werror -std=c99 -g -pthread -O1
LFLAGS = -pthread -lrt

OBJ = scheduler.o queue.o logger.o
EXEC = lift_sim_A

$(EXEC): $(OBJ)
	$(CC) $(OBJ) $(LFLAGS) -o $@

scheduler.o: scheduler.c queue.h logger.h thread_info.h 
	$(CC) $(CFLAGS) -c $< -o $@

queue.o : queue.c queue.h
	$(CC) $(CFLAGS) -c $< -o $@

logger.o : logger.c logger.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm $(OBJ) $(EXEC) sim_out


