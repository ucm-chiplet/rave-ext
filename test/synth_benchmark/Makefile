all: vecmix

vecmix: vecmix.c
	clang -O3 -mepi -fno-vectorize $^ -o $@.x
