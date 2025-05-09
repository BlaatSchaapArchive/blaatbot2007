# Project: bscp-cbot
# Makefile created by Dev-C++ 4.9.9.2
# Edited for *NIX compilation

CPP  = g++
CC   = gcc
OBJ  = main.o osinfo.o bot.o general.o irc.o fcom.o
LINKOBJ  = main.o osinfo.o bot.o general.o irc.o fcom.o
BIN  = cbot
CXXFLAGS = $(CXXINCS)  
CFLAGS = $(INCS)  
RM = rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before cbot all-after


clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CPP) $(LINKOBJ) -o "cbot" $(LIBS)

main.o: main.cpp
	$(CPP) -c main.cpp -o main.o $(CXXFLAGS)

osinfo.o: osinfo.cpp
	$(CPP) -c osinfo.cpp -o osinfo.o $(CXXFLAGS)

bot.o: bot.cpp
	$(CPP) -c bot.cpp -o bot.o $(CXXFLAGS)

general.o: general.cpp
	$(CPP) -c general.cpp -o general.o $(CXXFLAGS)

irc.o: irc.cpp
	$(CPP) -c irc.cpp -o irc.o $(CXXFLAGS)

fcom.o: fcom.cpp
	$(CPP) -c fcom.cpp -o fcom.o $(CXXFLAGS)
