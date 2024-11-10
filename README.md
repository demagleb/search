# Baby search indexes

## Requirements

* clang++ >= 17
* cmake >= 3.20

## Build

### Debug:
```bash
CXX=clang++-17 cmake -DCMAKE_BUILD_TYPE=Debug -B build-debug
```

### Release:
```bash
CXX=clang++-17 cmake -DCMAKE_BUILD_TYPE=Release -B build-release
```

### Address Sanitizer:
```bash
CXX=clang++-17 cmake -DCMAKE_BUILD_TYPE=Debug -DASAN=1 -B build-asan
```

### UB Sanitizer:
```bash
CXX=clang++-17 cmake -DCMAKE_BUILD_TYPE=Debug -DUBSAN=1 -B build-ubsan
```
