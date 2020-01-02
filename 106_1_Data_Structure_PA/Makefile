CFLAGS = -g -Wall -std=c++11
CXX = g++
ECHO = echo
MAIN = main
EXEC = myBST

all: myBST
	@$(ECHO) "Clearing outputs..."
	@rm -rf output/*.txt

myBST: BST.o main.o
	@$(ECHO) "Checking $(MAIN)..."
	@$(CXX) BST.o main.o -o $@ $(CFLAGS)

main.o   : src/main.cpp src/BST.h
	@$(ECHO) "> compiling: $<..."
	@$(CXX) -c $< -o $@ $(CFLAGS)

BST.o	 : src/BST.cpp src/BST.h
	@$(ECHO) "> compiling: $<..."
	@$(CXX) -c $< -o $@ $(CFLAGS)

BSTstd.o : src/BSTstd.cpp src/BST.h
	@$(ECHO) "> compiling: $<..."
	@$(CXX) -c $< -o $@ $(CFLAGS)

clean	 : 
	@$(ECHO) "Cleaning .o files..."
	@rm -rf *.o
	@$(ECHO) "Removing myBST..."
	@rm -rf myBST
	@rm -rf bin/myBST
	@$(ECHO) "Removing output files..."
	@rm -rf output/*.txt