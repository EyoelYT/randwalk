
randwalk: main.c
	gcc main.c -std=c99 -Wunused -Wall -Wextra -I/opt/homebrew/include/SDL2 -L/opt/homebrew/lib -lSDL2 -lm -o randwalk.exe
