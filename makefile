tree :tree2
	./tree

tree2: self_relative_binary_tree_2.c
	clang -Wall -Wextra -pedantic self_relative_binary_tree_2.c -o tree
N=10
CC=clang
test_save: main.c 
	${CC} -Wall -Wextra  -DTEST_SAVE -DNODE_POOL_CAP=${N} main.c -o tree && ./tree

test_load: main.c 
	${CC} -Wall -Wextra  -DTEST_LOAD -DNODE_POOL_CAP=${N} main.c -o tree   && ./tree

run: test_save test_load 

clean : 
	rm out.bin
	rm tree
	rm a.out
test_mmap: 
	${CC} -Wall -Wextra mmaped_tree.c -DNODE_POOL_CAP=${N} -o a.out && ./a.out
