CC = gcc
CFLAGS = `pkg-config --cflags gtk+-3.0 gtksourceview-3.0`  -Iinclude -Wno-deprecated-declarations
LDFLAGS = `pkg-config --libs gtk+-3.0 gtksourceview-3.0`
LIBS = `pkg-config --libs gtk+-3.0`
SRC = src/main.c src/defines.c src/file_handling.c src/fonts.c src/process.c src/overlays.c src/menu_bar.c src/search_replace.c
OUT = ./Nilgiri

all:
	$(CC) $(SRC) -o $(OUT) $(CFLAGS) $(LDFLAGS) $(LIBS) 

clean:
	rm -f $(OUT)

