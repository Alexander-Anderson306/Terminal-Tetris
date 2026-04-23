all : game game-easy game-hard

game : *.c
	@gcc *.c -o game -pthread -Wall

game-easy : *.c
	@gcc -DEASY_MODE *.c -o game-easy -Wall

game-hard : *.c
	@gcc -DHARD_MODE *.c -o game-hard  -Wall

clean:
	@rm -f game game-easy game-hard