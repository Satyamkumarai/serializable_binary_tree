# Self relative Serializable bin Tree.
Features:
    Binary tree
    Relative Pointers :
        Customizable ptr sizes
        Self Relative thus take less space
    Uses a Fixed NodePool 
## Run:
```
make run1
cat tree.txt
```
## Test Save
```
make clean
make test_save1
```
## Test Load
```
make clean
mak test_load1
```
# Serializable Binary Tree 
Serializable binary tree using a relative pointer and a fixed static Node Pool
## Run : 

```
make run2
cat tree.txt
```
## Test Save
```
make clean
make test_save2
```
## Test Load
```
make clean
mak test_load2
```

# Memory Map the Serializable tree
An attempt to create a memory mapped file.
This file loads a memory mapped file to load a tree directly into memory.
## Test mmap
```
make clean
make test_save2
make test_mmap
```



