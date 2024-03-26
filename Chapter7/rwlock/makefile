objects = rwlock.o rwlock_main.o

rwlock : $(objects)
	cc -o rwlock $(objects)

rwlock_main.o : rwlock.h

.PHONY : clean
clean : 
	rm rwlock $(objects)

