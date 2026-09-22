# Cache
### for Vladimirov C++ course
### made by Kiselev Andrey & Khasanov Amir

# Build and run debug version
## Build if needed and run
### -j8 to run jobs in parallel
```
make run -j8
```
## Build
```
make -j8
```
## Get `iwyu` diagnostics about include dependencies
```
make diagnose
`````
## Run
```
./debug_build/cache.x
```
# Build and run release version
## Build if needed and run
```
make VERSION=Release run -j8
```
## Build
```
make VERSION=Release -j8
```

## Run
```
./build/cache.x
`````