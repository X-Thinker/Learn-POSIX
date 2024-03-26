objects = barrier.o barrier_main.o

barrier : $(objects)
	cc -o barrier $(objects)

barrier.o : barrier.h

.PHONY : clean
clean : 
	rm barrier $(objects)

