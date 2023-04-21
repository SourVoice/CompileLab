FLEX=..\tools\win_flex.exe
CC=gcc

FFLAGS = --nounistd -c --noline
CFLAGS = -Wno-implicit-function-declaration

lexical : lex.yy.c
	$(CC) $(CFLAGS) $< -o $@

lex.yy.c : lexical.l
	$(FLEX) $(FFLAGS) $< 
