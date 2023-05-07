# Assignment 6

## A general note

Because this assignment features a lot of assembly code on the ELF format, the preferred way to run this is using a Linux machine. Personally, I use a Mac, so the easiest way I found to run this is to open the entire project using **GitHub Codespaces**, and then coding and running the project in the browser or using remote Visual Studio Code. On GitHub, navigate to `Code -> Codespaces -> + -> Open in... -> Browser or Visual Studio Code`. This will open the project in a Linux environment, and you can run the code using the commands below.

## 1 - Data flow analysis

The deliverable for the theoretical questions can be found in the file [`theory/ps6.pdf`](./theory/ps6.pdf).

## 2 - Code Generation Part II

The deliverable for the code generation can be found in the [`code`](./code/) directory. Please see the attached implementation details. Run the application using:

```sh
# Navigate to the code
cd code

# Clean and compile source code
make purge && make

# Navigate to the directory of .vsl files
cd vsl_programs

# Clean, compile the assembly code, and generate the executable
make clean && make ps6 && make ps6-assemble
```

Now you can run executable code based on the demo programs provided.

```sh
# Navigate to the directory with generated executables
cd ps6-codegen2

# Run executable code
./array.out
./break.out
./for.out
./if.out
./sieve.out
./simple_if.out
./while.out
```
