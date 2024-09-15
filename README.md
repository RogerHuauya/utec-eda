# EDA - Advanced Data Structures

This repository contains the assignments for the **Estructura de Datos
Avanzados** (Advanced Data Structures) course taught at UTEC. It implements
various data
structures and algorithms, and the project includes automated tests using the *
*Google Test** framework.

## Project Structure

- `include/` - Contains header files for the various data structures and
  classes.
- `src/` - Contains the implementation files.
- `Google_tests/` - Contains test files using Google Test for validating the
  implementation.
- `lib/googletest/` - Contains the Google Test framework as a submodule.

## Prerequisites

- **CMake** 3.29 or higher
- A C++20 compatible compiler (e.g., `g++`, `clang++`)

## Instructions to Build and Run Tests

### 1. Clone the Repository

Clone the repository to your local machine:

```bash
git clone https://github.com/RogerHuauya/utec-eda.git
cd utec-eda
```

### 2. Initialize and Update the Google Test Submodule

Google Test is used as a submodule in this project. Initialize and update the
submodule by running:

```bash
git submodule init
git submodule update
```

### 3. Create a Build Directory

Create a build directory to keep the build files separate from the source code:

```bash
mkdir build
cd build
```

### 4. Run CMake

Generate the build system by running CMake:

```bash
cmake ..
```

### 5. Build the Project

After running CMake, build the project using:

```bash
cmake --build .
```

### 6. Run the Tests

The project includes two sets of tests: `quadtree_test` and `bsptree_test`. To
run them:

- **Run the Quadtree Tests**:
    ```bash
    ./quadtree_test
    ```

- **Run the BSP Tree Tests**:
    ```bash
    ./bsptree_test
    ```

These commands will run the corresponding tests and display the results in the
terminal.
