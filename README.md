# Multi Level Cache Simulator
### special for Konstantin Vladimirov C++ course
### made by Kiselev Andrey & Khasanov Amir

## 1. Contents

- [Task definition](#Task-definition)
- [Analysis combines](#Analysis)
- [Run guide](#4.run-guide)

## 2. Task definition

We were tasked with implementing a simulator for the operation of multi-level caches. The task involves writing 5 caching algorithms: LRU, LFU, LIRS, 2Q, ARC. As well as the ability to combine all these algorithms into a multi-level data storage structure. We were also required to analyze all possible combinations of algorithms, with different sizes of each level and different numbers of levels, which was implemented using a configuration file. And, of course, unit tests.

## 3. Analysis of algorithms combines

## 4. Run guide

### Build
```
chmod +x ./build.sh
./build.sh
```
### Run
```
./build/cache.x
```

### Debug build and run
```
chmod +x ./debug_build.sh
./debug_build.sh

./debug_build/cache.x
```

### Build and run tests
```
chmod +x ./tests_build.sh
./tests_build.sh

./test_build/cache.x
```