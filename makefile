N=10
CC=clang
test_save1: self_relative_binary_tree.c 
	${CC} -Wall -Wextra  -DTEST_SAVE -DNODE_POOL_CAP=${N} self_relative_binary_tree.c -o tree && ./tree

test_load1: self_relative_binary_tree.c 
	${CC} -Wall -Wextra  -DTEST_LOAD -DNODE_POOL_CAP=${N} self_relative_binary_tree.c -o tree   && ./tree

run1: test_save1 test_load1

test_save2: main.c 
	${CC} -Wall -Wextra  -DTEST_SAVE -DNODE_POOL_CAP=${N} main.c -o tree && ./tree

test_load2: main.c 
	${CC} -Wall -Wextra  -DTEST_LOAD -DNODE_POOL_CAP=${N} main.c -o tree   && ./tree

run2: test_save2 test_load2
	cat ./tree.txt

clean : 
	rm  -f out.bin
	rm -f tree
	rm -f a.out
	rm -f tree.txt
	rm -f gen.bin
test_mmap: 
	${CC} -Wall -Wextra mmaped_tree.c -DNODE_POOL_CAP=${N} -o a.out && ./a.out
