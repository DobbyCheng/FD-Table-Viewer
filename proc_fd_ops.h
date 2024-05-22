#ifndef PROC_FD_OPS_H
#define PROC_FD_OPS_H

#include "types.h"  // Assuming a separate types.h file for shared type definitions

// Function to read and store file descriptor information for a process
void read_fd_info(pid_t pid, procLL** procHead);
// Function to iterate over all processes and store their file descriptor
// information
void iterate_over_processes(procLL** procHead, uid_t uid);

#endif  // PROC_FD_OPS_H
