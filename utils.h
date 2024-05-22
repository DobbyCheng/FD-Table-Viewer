#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <sys/types.h>

#include "types.h"

// Function to check whether a process belongs to a user based on process ID
// (pid) and user ID (uid).
bool is_user_process(pid_t pid, uid_t uid);
// Function to check if a character is a digit.
bool char_is_digit(char c);

#endif  // UTILS_H
