# ProiectPCD

## Initial run
Use following command to install all dependencies and generate Makefile. If it's not running, run first `chmod +x build_util.sh`
```bash
./build_util.sh all
```

## Compiling project
Instead using standard `gcc -o file file.c`, run the following command. This will compile all existing C files. Find the executables in **build/**
```bash
./build_util.sh
```

## Adding new C files
To be compiled, new C files are needed to be specified in `CMakeLists.txt` by the following syntax: `add_executable(new_file new_file.c)`. After adding them run the next two commands:
```bash
./build.sh all
./build.sh
```
