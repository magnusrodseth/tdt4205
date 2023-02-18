# Assignment 3

## 1 - Bottom-up parsing tables

The deliverable for the theoretical questions can be found in the file [`theory/ps3.pdf`](./theory/ps3.pdf).

## 2 - Tree simplification

Please see the attached implementation details. Run the application using:

```sh
# Navigate to the code
cd code

# Clean and compile source code
make purge && make

# Navigate to the directory of .vsl files
cd vsl_programs

# Clean and generate the Graphwiz representation of the AST
make clean && make ps3 && make ps3-graphviz
```

### Valgrind and memory correctness

To detect common memory errors, we use the Valgrind tool. To run Valgrind, use the following command:

```sh
# Navigate to the code
cd code

# Clean and compile source code
make purge && make

# Run Valgrind on a specific .vsl file
valgrind src/vslc < vsl_programs/ps3-simplify/*.vsl
```
