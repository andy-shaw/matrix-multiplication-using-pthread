matrix-multiplication-using-pthread
===================================

Previous project using pthread.h to code matrix multiplication, and reporting the time taken.  Written in C

#### Details

Author: Andy Shaw
Date: 11/26/12

### Compiling notes

* The gcc <files> -lpthread -lrt command is what was used to compile the program.
* The dimensions of the matrix are stored in a header file.
* The program needs to be recompiled after the dimensions of the matrix are changed.

### Notes

* There is a printing feature of checking if the matrices are correct.  On line 201 of the code, change the value of print from 1 to 0 to output all of the conditions.
* The correctness of the matrices are determined by comparing 20 random locations in the matrix.
* The program will compute the matrices for all number of threads between 1 and the number entered by the user.
* Some times result in a negative time in the decimal portion of the time.  This is most likely from a loss of digits from not returning trailing zeros.  There are attempts to fix this in the code by multiplying by a factor of 10 on the finishing time, but not all negative values are caught.
* If the matrix calculated using threads is incorrect, the program will terminate.
* Part of the design plans made the bonus possible from the start, so there is only 1 implementation of this lab.