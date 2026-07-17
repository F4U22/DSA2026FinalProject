run: main.o
	out/main

main.o:
	gcc src/main.c -o out/main


clean:
	del src/main.exe
