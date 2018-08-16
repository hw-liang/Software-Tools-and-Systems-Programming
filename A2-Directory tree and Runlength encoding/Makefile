all : myfind compress_file uncompress_file cycle

myfind : myfind.c
	gcc -Wall -g -std=gnu99 -o myfind myfind.c

uncompress_file : uncompress_file.c
	gcc -Wall -g -std=gnu99 -o uncompress_file uncompress_file.c

compress_file : compress_file.c
	gcc -Wall -g -std=gnu99 -o compress_file compress_file.c

cycle: cycle.c
	gcc -Wall -g -std=gnu99 -o cycle cycle.c
clean : 
	-rm myfind compress_file uncompress_file cycle
