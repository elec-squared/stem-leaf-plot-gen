main:
	gcc src/leafcreator.c -o bin/slpg -lm
	gcc -g src/leafcreator.c -o bin/slpg_debug -lm
	x86_64-w64-mingw32-gcc-win32 src/leafcreator.c -o bin/slpg_win.exe -lm
