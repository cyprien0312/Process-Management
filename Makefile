EXE=allocate

$(EXE): main.c schedule.o
	cc -Wall -o $(EXE) $<

schedule.o: schedule.c
	cc -Wall -c schedule.c

format:
	clang-format -style=file -i *.c

clean:
	rm -f $(EXE) schedule.o