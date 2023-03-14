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
make clean && make ps4-graphviz

# Clean and generate the symbol table, string list and bound syntax tree
make clean && make ps4
```

### Valgrind and memory correctness

To detect common memory errors, we use the Valgrind tool. To run Valgrind, use the following command:

```sh
# Navigate to the code
cd code

# Clean and compile source code
make purge && make

# Run Valgrind on a specific .vsl file
valgrind src/vslc < vsl_programs/ps4-symbols/<filename>.vsl
```

### Important to note

The delivered code is **almost** fully correct. The two discrepancies I have noticed are:

- The function reference in the `BOUND SYNTAX TREE` in the `*.symbols` file refer to the symbol table entry. This does not happen in the `suggested/*.symbols` files.
- Some of the references in the `shadowing.symbols` file are not correct, compared to what is defined in `suggested/shadowing.symbols`.

Besides these two discrepancies, I have not been able to find any other errors.
