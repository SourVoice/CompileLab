ifeq ($(OS),Windows_NT)
FLEX=..\tools\win_flex.exe
BISON=..\tools\win_bison.exe
else
FLEX=flex
BISON=bison
endif
CC=gcc

FFLAGS = -c --noline
BFLAGS = -d --no-lines -v 
CFLAGS_1 = -Wno-implicit-function-declaration
# CFLAGS_2 = -Wno-implicit-function-declaration -Wno-incompatible-pointer-types -Wno-implicit-int
CFLAGS_2 = -g 

parser : syntax.tab.c 
	$(CC) $^ $(CFLAGS_2) -o $@

lex : lex.yy.h
	$(CC) $(CFLAGS_1) $< -o $@

syntax.tab.c : syntax.y lex.yy.h
	$(BISON) $(BFLAGS) $<

lex.yy.h : lexical.L syntax.tab.h
	$(FLEX) $(FFLAGS) -o $@ $<

syntax.tab.h : syntax.y
	$(BISON) $(BFLAGS) $<

.PONEY : clean

# if windows
ifeq ($(OS),Windows_NT)
clean : 
	del /Q /F *.o *.exe *.tab.* *.output *.yy.c
else
clean :
	rm -f *.o *.exe *.tab.* *.output *.yy.c parser *.yy.h
endif