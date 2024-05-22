# FD Table Viewer

This program provides a utility to display tables used by the operating system to keep track of open file descriptors (FDs), their assignment to processes, and the system-wide FD table. It explores the Linux kernel's virtual file subsystem, particularly the `/proc` interface, to retrieve information about open files and processes.

## Installation

1. Extract the provided ZIP archive to a directory.
2. Open a terminal and navigate to the directory containing the extracted files.
3. Run `make` to compile the program. This will create the executable `fd_table_viewer`.

## Usage

`fd_table_viewer` accepts multiple command-line arguments to control its output:

- `--per-process`: Display the FD table for each process individually.
- `--systemWide`: Display the system-wide FD table.
- `--Vnodes`: Display the Vnodes FD table.
- `--composite`: Display a composite view of all tables.
- `--threshold=X`: Flag processes with FDs greater than `X`.
- `--output_TXT`: Save the composite table in ASCII format to `compositeTable.txt`.
- `--output_binary`: Save the composite table in binary format to `compositeTable.bin`.

Additionally, a positional argument specifies a particular process ID (PID). If no PID is provided, the program processes all running processes for the user.

## Default Behavior

If no arguments are passed, the program will display the composite table. This behavior mimics the `--composite` flag.

## Files

- `fd_table_viewer.c`: The main program file.
- `display_utils.c/h`: Utility functions for displaying tables.
- `proc_fd_ops.c/h`: Functions to operate on `/proc` entries and gather FD information.
- `utils.c/h`: General utility functions for the program.
- `types.h`: Definitions of structures and types used throughout the program.

## Makefile

- Use `make` to compile the program.
- Use `make clean` to remove compiled binaries and object files.

## Examples

To display the composite table:
```
./fd_table_viewer --composite
```

To display the FD table for a specific PID:
```
./fd_table_viewer 12345
```

To save the composite table in ASCII format:
```
./fd_table_viewer --output_TXT
```

## Comparison Test

### Objective

The purpose of this comparison is to evaluate the performance differences between generating the composite table in text (ASCII) format and binary format. The evaluation criteria are runtime (in seconds) and output file size (in Kilobytes).

### Methodology

We conducted a series of experiments, running the program with both `--output_TXT` and `--output_binary` flags across various cases. The experiments included both individual PID cases and cases involving all PIDs for a user. We recorded the runtime and the size of the generated files.

### Results

Data is recorded in data.xlsx, which is available in the zip file.

#### Runtime(All PIDs)

- **Text Output (`--output_TXT`)**:
  - Average Runtime: 0.02 seconds
  - Standard Deviation: 0.001673 seconds

- **Binary Output (`--output_binary`)**:
  - Average Runtime: 0.01817 seconds
  - Standard Deviation: 0.001169 seconds

#### File Size(All PIDs)

- **Text Output (`--output_TXT`)**:
  - Average File Size: 28.0 KB
  - Standard Deviation: 0 KB
- **Binary Output (`--output_binary`)**:
  - Average File Size: 6.6833 KB
  - Standard Deviation: 0.2041 KB

#### Runtime(One PID)

- **Text Output (`--output_TXT`)**:
  - Average Runtime: 0.0105 seconds
  - Standard Deviation: 0.00105 seconds

- **Binary Output (`--output_binary`)**:
  - Average Runtime: 0.00933 seconds
  - Standard Deviation: 0.000516 seconds

#### File Size(One PID)

- **Text Output (`--output_TXT`)**:
  - Average File Size: 4.266 KB
  - Standard Deviation: 0.0816 KB

- **Binary Output (`--output_binary`)**:
  - Average File Size: 0.532 KB
  - Standard Deviation: 0.1446 KB

### Conclusion

The binary format is more efficient than the text format in terms of both processing time and storage space. The `--output_binary` flag results in faster execution times and significantly smaller file sizes, making it a more efficient choice for storing and processing large amounts of data. However, the choice between formats should consider the specific needs for readability and data processing.
