objects = alarm_fork.o

alarm_fork : $(objects)
	cc -o alarm_fork $(objects)

alarm_fork.o : errors.h

.PHONY : clean
clean : 
	rm alarm $(objects)

