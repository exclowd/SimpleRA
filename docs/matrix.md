### Page layout used to store matrices

The page layout for matrices is very simple: We are just storing matrices broken into blocks each file `MPage_<I><J>`
corresponds to the ith block rowwise and jth block columnwise

### Compression technique used for sparse matrices and the corresponding page layout

For sparse matrices we are just storing the x y and the value of the sparse matrices i.e table with 3 columns only

### Compression ratio as a function of the percentage of sparseness of the matrix (If you cannot specify this, a reasonable explanation with examples will do)

let sparseness be f i.e the number of zeros / total then compression is 3 * (1 - f)
This is suitable for matrices with f > 0.66

### Describe your in-place transpose operation and how(if) it needs to be changed or altered for

DENSE:
We can show this via an example

```
1 2 | 3
4 5 | 6
- -   - 
7 8 | 9
```

let these be the block sizes then if we want to transpose block (0,1) with block (1,0) then

```
for i in block1.rows
    for j in block1.cols
        block1[i][j] = block2[j][i]
```
We then handle diagonal block separately


SPARSE:
For sparse matrices we just swap the x and y cols. and sort the page entries for faster export and print functions


