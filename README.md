# CPU Core Management Demo

A simple project to demonstrate the CPU Core management in C language.

Main libraries:
- `pthread.h`: Thread management
- `sched.h`: Processor afinity

This is a final project for the Information Systems program's "Operational Systems" course, in the [Federal University of Uberlândia](http://www.ufu.br/).

## Group Members
- Henrique Tornelli
- Gabriel Dal Belo
- João Vitor Izidoro

## Getting Started

1. Compile the matriz generator

```bash
$ gcc genMat.c -o genMat
```

2. Use the matriz generator

```bash
# ./genMat [LINES] [COLUMNS] [LOWEST_NUMBER] [HIGHEST_NUMBER] [OUTPUT_FILE]

$ ./genMat 100 100 1 2 m.dat

$ ./genMat 100 100 1 2 n.dat
```

3. Compile and execute the `v1.c` code

```bash
$ clear; gcc v1.c; ./a.out
```

4. Compile and execute the `v2.c` code

```bash
$ clear; gcc v2.c -lpthread; ./a.out
```
