objects = workq.o workq_main.o

workq : $(objects)
	cc -o workq $(objects)

workq_main.o : rwlock.h

.PHONY : clean
clean : 
	rm workq $(objects)

