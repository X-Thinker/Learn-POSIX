SUBDIRS := Chapter1/alarm_fork.c Chapter1/alarm_thread.c \
		   Chapter1/thread_error.c Chapter1/alarm.c Chapter2/lifecycle.c \
		   Chapter3/alarm_cond.c Chapter3/alarm_mutex.c Chapter3/backoff.c \
		   Chapter3/cond.c Chapter3/cond_dynamic.c Chapter3/cond_static.c \
		   Chapter3/trylock.c Chapter3/mutex_dynamic.c Chapter3/mutex_static.c \
		   Chapter4/crew.c Chapter4/pipe.c Chapter4/server.c \
		   Chapter5/cancel_async.c Chapter5/cancel_cleanup.c \
		   Chapter5/cancel_disable.c Chapter5/cancel_subcontract.c \
		   Chapter5/cancel.c Chapter5/cond_attr.c Chapter5/mutex_attr.c \
		   Chapter5/once.c Chapter5/sched_attr.c Chapter5/sched_thread.c \
		   Chapter5/tsd_destructor.c Chapter5/tsd_once.c Chapter5/thread_attr.c \
		   Chapter6/atfork.c Chapter6/flock.c \
		   Chapter6/putchar.c Chapter6/getlogin.c \
		   Chapter7/barrier Chapter7/rwlock Chapter7/workq

CFLAGS = -I$(shell pwd)
export CFLAGS

.PHONY: all clean clean-all

all:
	@for dir in $(SUBDIRS); do \
		echo "Building in $$dir"; \
		$(MAKE) -C $$dir; \
	done

clean:
	-@for dir in $(SUBDIRS); do \
		echo "Cleaning in $$dir"; \
		$(MAKE) -C $$dir clean; \
	done
