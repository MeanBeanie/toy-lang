CC = g++
NAME = toy-lang
FLAGS = -std=c++20 -o $(NAME)

debug:
	$(CC) *.cpp $(FLAGS) -g
release:
	$(CC) *.cpp $(FLAGS)
