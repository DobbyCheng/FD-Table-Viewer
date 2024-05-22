#include "utils.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Function to check whether a process belongs to a user based on process ID
// (pid) and user ID (uid).
bool is_user_process(pid_t pid, uid_t uid) {
  char filePath[80],
      statusLine[300];  // Define variables for file path and line reading.
  snprintf(filePath, sizeof(filePath), "/proc/%d/status",
           pid);  // Format the file path string with pid.
  FILE* statusFile = fopen(filePath, "r");  // Open the file in read mode.
  if (!statusFile) {  // Check whether the file failed to open.
    fprintf(stderr, "Fail to open status file for process with pid %d!\n",
            pid);  // Print error message.
    exit(1);       // Exit the program with error.
  }

  while (fgets(statusLine, sizeof(statusLine),
               statusFile)) {  // Read the file line by line.
    if (strncmp(statusLine, "Uid",
                3)) {  // Check whether the line starts with "Uid".
      continue;        // If not, continue to the next line.
    }
    uid_t ruid;  // Variable to store the real user ID.
    sscanf(statusLine, "Uid:\t%d",
           &ruid);       // Parse the real user ID from the line.
    fclose(statusFile);  // Close the file.
    return uid == ruid;  // Return true if the user ID matches the real user ID,
                         // false otherwise.
  }

  fclose(statusFile);  // Close the file if no matching line is found.
  return false;        // Return false if the UID line is not found.
}

// Function to check if a character is a digit.
bool char_is_digit(char c) {
  return c >= '0' && c <= '9';
}  // Returns true if c is between '0' and '9'.
