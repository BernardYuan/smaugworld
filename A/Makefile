smaugworld: smaugworld.o smaug.o sheep.o cow.o hunter.o thief.o
	gcc smaugworld.o smaug.o sheep.o cow.o hunter.o thief.o -o smaugworld
smaugworld.o: smaugworld.c smaugworld.h smaug.h sheep.h hunter.h thief.h
	gcc -c smaugworld.c
smaug.o: smaug.c smaugworld.h smaug.h
	gcc -c smaug.c
sheep.o: sheep.c smaugworld.h sheep.h
	gcc -c sheep.c
cow.o: cow.c smaugworld.h cow.h
	gcc -c cow.c
hunter.o: hunter.c smaugworld.h hunter.h
	gcc -c hunter.c
thief.o: thief.c smaugworld.h thief.h
	gcc -c thief.c

clean:
	@echo "cleaning project"
	rm smaugworld
	rm *.o
	@echo "clean completed"
	
