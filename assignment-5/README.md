# Assignment 5

## 1 - Assembly programming

The deliverable for the assembly programming can be found in the [`assembly`](./assembly) directory. Please see the attached implementation details. Run the application using:

```sh
# Navigate to the `assembly` directory
cd assembly

# Run the application
make
```

## 2 - Code Generation Part I

The deliverable for the code generation can be found in the [`code`](./code/) directory. Please see the attached implementation details. Run the application using:

```sh
# Navigate to the code
cd code

# Clean and compile source code
make purge && make

# Navigate to the directory of .vsl files
cd vsl_programs

# Clean, compile the assembly code, and generate the executable
make clean && make ps5 && make ps5-assemble
```
