objects = alarm.o

alarm : $(objects)
	cc -o alarm $(objects)

alarm.o : errors.h

.PHONY : clean
clean : 
	rm alarm $(objects)

