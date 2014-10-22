hilos: clean
	gcc -pthread ordenArchivo-t.c -o ordenArchivo-t

clean:
	rm ordenArchivo-t || true
	rm a.out          || true
