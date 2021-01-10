# speedy-panda

![Tests and Build](https://github.com/giulioz/speedy-panda/workflows/Tests%20and%20Build/badge.svg)

A fast implementation of the PaNDa+ algorithm for mining Top-K Binary patterns in transactional data ([C. Lucchese, S. Orlando, R. Perego – A Unifying Framework for Mining Approximate Top-K Binary Patterns](https://ieeexplore.ieee.org/document/6682889)).

## Building

```bash
mkdir build
cd build
cmake ..
make
```

## Usage

```bash
./speedy-panda [filename] {OPTIONS}

  Finds approximate patterns in datasets with noise.

OPTIONS:

    -h, --help                        Display this help menu
    -k[8]                             Max number of patterns
    -w[0.8]                           Pattern complexity weight
    -x[1.0]                           Row tollerance ratio
    -y[1.0]                           Column tollerance ratio
    filename                          The dataset to process
```

### Example

Perform Top-K Binary pattern mining with 8 threads on the file `accidents_stripped2.dat`, looking for 8 patterns with a pattern complexity weight of 0.5, row tollerance ratio of 0.4 and column tollerance ratio of 0.4.

```bash
OMP_NUM_THREADS=8 ./speedy-panda ../dataset/accidents_stripped2.dat -w 0.5 -k 5 -x 0.4 -y 0.4

Patterns:
12 16 17 18 21 27 29 31 43  (60833)
8 14 15 22 24 25 28 41 59  (48565)
1 17 28 38 43 63  (43529)
10 17 22 25 43 307  (22971)
23 30 35 41  (25569)

________________________________________________________
Executed in  148.52 secs   fish           external
   usr time  1159.12 secs  136.00 micros  1159.12 secs
   sys time    7.57 secs  913.00 micros    7.57 secs
```
