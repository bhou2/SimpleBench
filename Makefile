simplebench: simplebench.o utility.o trace.o stat.o thread.o parameter.o 
	gcc -o simplebench simplebench.o utility.o trace.o stat.o thread.o parameter.o -lpthread
simplebench.o: simplebench.h simplebench.c 
	gcc -c simplebench.c -lpthread
utility.o: simplebench.h utility.c
	gcc -c utility.c
trace.o: simplebench.h trace.c
	gcc -c trace.c
stat.o: simplebench.h stat.c
	gcc -c stat.c
thread.o: simplebench.h thread.c
	gcc -c thread.c
parameter.o: simplebench.h parameter.c
	gcc -c parameter.c
clean:
	rm -rf *.o simplebench
