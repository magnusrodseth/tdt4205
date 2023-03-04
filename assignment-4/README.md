# Assignment 4

## 1 - Three-Address Code (TAC)

The deliverable for the theoretical questions can be found in the file [`theory/ps4.pdf`](./theory/ps4.pdf).

## 2 - Symbol Table Creation

Please see the attached implementation details. Run the application using:

```sh
# Navigate to the code
cd code

# Clean and compile source code
make purge && make

# Navigate to the directory of .vsl files
cd vsl_programs

# Clean and generate the Graphwiz representation of the AST
make clean && make
```

### Valgrind and memory correctness

To detect common memory errors, we use the Valgrind tool. To run Valgrind, use the following command:

```sh
# Navigate to the code
cd code

# Clean and compile source code
make purge && make

# Run Valgrind on a specific .vsl file
valgrind src/vslc < vsl_programs/ps3-simplify/<filename>.vsl
```
