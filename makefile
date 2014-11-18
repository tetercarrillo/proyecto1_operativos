hilos: clean
	gcc -pthread ordenArchivo-t.c -o ordenArchivo-t -lm

clean:
	rm ordenArchivo-t || true
	rm a.out          || true
