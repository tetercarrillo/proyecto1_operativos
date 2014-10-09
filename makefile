hilos: clean
	gcc ordenArchivo-t.c -o ordenArchivo-t

clean:
	rm ordenArchivo-t || true
	rm a.out          || true
