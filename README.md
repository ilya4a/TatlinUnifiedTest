# TatlinUnifiedTest
Test assignment for the Tatlin.Unified DataPath team.
## Overview

The solution idea is described in algorithm.md.

The project emulates a tape device using files.  

The parallel read‑sort‑write pipeline is built on a thread‑safe bounded queue and std::thread.


## Build

### Configure and build the main application

```bash
git submodule update --init --recursive
cmake -S . -B build
cmake --build build
````

### Build tests

Tests are enabled by default. If you want to configure them explicitly:

```bash
cmake -S . -B build -DBUILD_TESTS=ON
cmake --build build
```

## Run

```bash
TatlinUnifiedTest -i input.txt -o output.txt -m 1048576
```

## Command-line options

* `-i, --input` — path to the input tape file
* `-o, --output` — path to the output tape file
* `-m, --memory-limit` — memory limit in bytes
* `--memory-factor` — memory utilization factor
* `--tmp-dir` — directory for temporary tapes
* `--tape-config` — path to tape configuration file
* `--seq` — use sequential sorting mode
* `-p, --print-result` — print the result to console
* `-t, --time` — print execution time



