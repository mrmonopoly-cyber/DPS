# Control Logic Operator Tests

## Requirements

- [Cmake](https://cmake.org/cmake/help/latest/guide/tutorial/index.html)
- [GCC](https://gcc.gnu.org/)
- [socketcan](https://docs.kernel.org/networking/can.html)

## Run

To set up the environment use the command:
```bash
    ./setup_test_env.sh [option]
```
To run one or all tests use the following command.
If name is empty all tests will start sequentially
```bash
    ./run_tests.sh [name]
```
If you want to add a new test run the following command:
```sh
    ./add_test.sh [name]
```
If you want to remove a test run the following command:
```sh
    ./rm_test.sh [name]
```
If you want to list the available tests run the following command 
```sh
    ./list_tests.sh
```
