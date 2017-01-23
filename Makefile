all:
	gcc `pkg-config --cflags python3` -fPIC -c Calmodule.c -o Calmodule.o
	gcc -shared Calmodule.o -o Cal.so
	gcc -std=c11 -Wall -DNDEBUG calutil.c caltool.c -o caltool
	chmod +x xcal.py

caltool:
	gcc caltool.c calutil.c -std=c11 -Wall -DNDEBUG -o caltool

clean:
	rm -f *.o *.so caltool
