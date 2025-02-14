all: vecmix

CC=${LLVM_DIR}/bin/clang

vecmix: vecmix.c
	$(CC) -O3 -mepi -fno-vectorize $^ -o $@.x
