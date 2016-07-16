CC ?= $(shell command -v gcc)
CXX ?= $(shell command -v g++)

CXX_FLAGS := -x c++ -Wno-write-strings

SHELL := $(shell echo $$SHELL)
.DEFAULT_GOAL := all

all: .build_prod
clean: .clean_prod

.bison:
	@bison -d grammar.y
	$(info Parser Generated)
.flex:
	@flex --header-file=lex.yy.h lexer.l
	$(info Scanner Generated)

.clean_prod:
	@rm -f grammar.tab.* & rm -f lex.yy.* & rm -f silk
.build_prod: .bison .flex
	@$(CXX) $(CXX_FLAGS) lex.yy.c grammar.tab.c util.c main.cpp -o silk -ll -ly
	$(info Build Success)