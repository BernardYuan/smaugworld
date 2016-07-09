smaugworld: smaugworld.o smaug.o sheep.o cow.o
    gcc smaugworld.o smaug.o sheep.o cow.o -o smaugworld
smaugworld.o: smaugworld.c smaugworld.h smaug.h sheep.h
    gcc -c smaugworld.c
smaug.o: smaug.c smaugworld.h smaug.h
    gcc -c smaug.c
sheep.o: sheep.c smaugworld.h sheep.h
    gcc -c sheep.c
cow.o: cow.c smaugworld.h cow.h
    gcc -c cow.c

clean:
    @echo "cleaning project"
    rm smaugworld
    rm *.o
    @echo "clean completed"
	
