#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "display_utils.h"
#include "proc_fd_ops.h"
#include "utils.h"

// Function to delete the linked list
void free_all(procLL* procHead) {
  procLL* procNode = procHead;  // Start at the head of the process linked list
  procLL* procNext = NULL;  // Temporary pointer to store the next process node
  while (procNode !=
         NULL) {  // Iterate until the end of the process linked list
    procNext =
        procNode->next;  // Save the next process node before freeing current
    fdInfo* fdNode = procNode->fdHead;  // Get the head of the file descriptor
                                        // linked list for the current process
    fdInfo* fdNext =
        NULL;        // Temporary pointer to store the next file descriptor node
    free(procNode);  // Free the memory allocated for the current process node
    while (fdNode !=
           NULL) {  // Iterate until the end of the file descriptor linked list
      fdNext = fdNode->next;  // Save the next file descriptor node before
                              // freeing current
      free(fdNode);           // Free the memory allocated for the current file
                              // descriptor node
      fdNode = fdNext;        // Move to the next file descriptor node
    }
    procNode = procNext;  // Move to the next process node
  }
}

int main(int argc, char** argv) {
  bool displayPerProcess = false,
       displaySystemWide = false;  // Flags for display options
  bool displayVnodes = false,
       displayComposite = false;  // More flags for display options
  bool saveIntoTxt = false,
       saveIntoBinary = false;  // Flags for output format options
  int threshold = -1;    // Threshold value for displaying offending processes
  pid_t targetPid = -1;  // Target process ID for operation
  uid_t uid = getuid();  // Get and store the user ID of the current user
  int len_threshold = strlen(
      "--threshold=");  // Calculate length of threshold argument identifier
  procLL* procHead =
      NULL;  // Head pointer for process list, initialized to NULL

  // Parse command line arguments
  for (int i = 1; i < argc; ++i) {
    if (!strcmp(argv[i], "--per-process")) {  // Check whether per-process
                                              // display option is specified
      displayPerProcess = true;
    } else if (!strcmp(argv[i],
                       "--systemWide")) {  // Check whether systemWide display
                                           // option is specified
      displaySystemWide = true;
    } else if (!strcmp(argv[i], "--Vnodes")) {  // Check whether Vnode display
                                                // option is specified
      displayVnodes = true;
    } else if (!strcmp(argv[i],
                       "--composite")) {  // Check whether composite display
                                          // option is specified
      displayComposite = true;
    } else if (!strncmp(argv[i], "--threshold=",
                        len_threshold)) {  // Check whether threshold option is
                                           // specified
      threshold = atoi(
          argv[i] +
          len_threshold);  // Convert threshold value from string to integer
      if (!threshold) {    // Validate threshold value
        fprintf(stderr, "Integer for threshold not found!\n");
        exit(1);  // Exit with error if threshold is invalid
      }
    } else if (!strcmp(argv[i], "--output_TXT")) {  // Check whether TXT output
                                                    // option is specified
      saveIntoTxt = true;
    } else if (!strcmp(argv[i],
                       "--output_binary")) {  // Check whether binary output
                                              // option is specified
      saveIntoBinary = true;
    } else {                  // Handle target PID or invalid arguments
      if (targetPid != -1) {  // Check whether target PID is already set
        fprintf(stderr, "Invalid command line arguments!\n");
        exit(1);  // Exit with error if command line arguments are invalid
      }
      targetPid = atoi(argv[i]);  // Convert target PID from string to integer
      if (!targetPid) {
        fprintf(stderr, "Invalid command line arguments!\n");
        exit(1);  // Exit with error if target PID is invalid
      }
      if (!is_user_process(
              targetPid,
              uid)) {  // Check whether the target process is owned by the user
        fprintf(stderr, "Process with pid %d is not owned by the user!\n",
                targetPid);
        exit(1);  // Exit with error if process is not owned by the user
      }
    }
  }

  // Default to display composite if no display option is specified
  if (!displayPerProcess && !displaySystemWide && !displayVnodes) {
    displayComposite = true;  // Set display composite flag
  }

  // Perform operations based on target PID
  if (targetPid != -1) {
    printf(">>> Target PID: %d\n", targetPid);
    read_fd_info(targetPid,
                 &procHead);  // Read file descriptor information for target PID
  } else {
    iterate_over_processes(
        &procHead,
        uid);  // Iterate over all processes if no target PID is specified
  }

  // Display tables based on specified options
  if (displayComposite) {
    display_table(procHead, 3, (targetPid != -1));  // Display composite table
  }
  if (displayPerProcess) {
    display_table(procHead, 0, (targetPid != -1));  // Display per-process table
  }
  if (displaySystemWide) {
    display_table(procHead, 1, (targetPid != -1));  // Display systemWide table
  }
  if (displayVnodes) {
    display_table(procHead, 2, (targetPid != -1));  // Display Vnodes table
  }

  // Save composite table to files
  if (saveIntoTxt) {
    save_composite_to_file(
        procHead, "compositeTable.txt", true,
        (targetPid != -1));  // Save the composite table to txt file
  }
  if (saveIntoBinary) {
    save_composite_to_file(
        procHead, "compositeTable.bin", false,
        (targetPid != -1));  // Save the composite table to binary file
  }

  // Display processes exceeding the threshold
  if (threshold != -1) {
    if (targetPid != -1) {
      free_all(procHead);  // delete the linked list
      procHead = NULL;
      iterate_over_processes(
          &procHead,
          uid);  // Iterate over all processes if no target PID is specified
    }
    display_offending(threshold,
                      procHead);  // Display processes exceeding the threshold
  }

  free_all(procHead);  // delete the linked list
  procHead = NULL;
  return 0;  // Exit with success
}