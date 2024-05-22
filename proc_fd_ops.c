#define _GNU_SOURCE
#include "proc_fd_ops.h"

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "utils.h"

// Function to insert file descriptor information into a linked list for a
// process
void insert_fd_info(procLL* procNode, int fd, const char* fileName,
                    unsigned long iNode) {
  fdInfo* fdNode = (fdInfo*)calloc(
      1,
      sizeof(fdInfo));  // Allocate memory for the new file descriptor node
  if (!fdNode) {
    fprintf(stderr, "No available memory left!\n");  // Print error if memory
                                                     // allocation fails
    exit(1);                                         // Exit program with error
  }

  fdNode->fd = fd;                     // Assign file descriptor number
  strcpy(fdNode->fileName, fileName);  // Store the file name into the node
  fdNode->iNode = iNode;               // Assign inode number
  if (!procNode->fdHead) {
    procNode->fdHead = fdNode;  // If linked list is empty, new node is the head
    return;
  }
  fdInfo* fdLast = procNode->fdHead;  // Find the last node in the linked list
  while (fdLast->next) {
    fdLast = fdLast->next;
  }
  fdLast->next = fdNode;  // Append the new node at the end of the linked list
}

// Function to read and store file descriptor information for a process
void read_fd_info(pid_t pid, procLL** procHead) {
  char fdPath[128];  // Buffer for file descriptor path string
  sprintf(fdPath, "/proc/%d/fd",
          pid);  // Construct path to process's file descriptor directory
  DIR* fdDir = opendir(fdPath);  // Open the file descriptor directory
  if (!fdDir) {
    return;  // Return if directory cannot be opened
  }

  procLL* procNode = (procLL*)calloc(
      1,
      sizeof(procLL));  // Allocate memory for the new process node
  if (!procNode) {
    fprintf(stderr, "No available memory left!\n");  // Print error if memory
                                                     // allocation fails
    exit(1);                                         // Exit program with error
  }
  procNode->pid = pid;      // Assign process ID
  procNode->fdHead = NULL;  // Initialize the file descriptor list as empty

  struct dirent* fdEntry;  // Structure to hold directory entry information
  while ((fdEntry =
              readdir(fdDir))) {  // Iterate over all entries in the directory
    if (fdEntry->d_name[0] != '.') {  // Skip '.' and '..' entries
      char dNamePath[512], linkPath[512];  // Buffers for paths
      sprintf(dNamePath, "%s/%s", fdPath,
              fdEntry->d_name);  // Construct full path to the file descriptor
      int len = readlink(
          dNamePath, linkPath,
          sizeof(linkPath) - 1);  // Read symbolic link to get actual file path
      if (len != -1) {
        linkPath[len] = '\0';  // Null-terminate the file path
        char fdInfoPath[512]; // Buffer for fd info path
        sprintf(fdInfoPath, "/proc/%d/fdinfo/%s", pid, fdEntry->d_name); // Path to fdinfo
        FILE* fdInfoFile = fopen(fdInfoPath, "r"); // Open the fdinfo file
        if(!fdInfoFile){ // Error opening the fdinfo file
          return;
        }
        char line[256];
        unsigned long iNode = 0;
        // Read lines from fdinfo file
        while (fgets(line, sizeof(line), fdInfoFile)) {
            // Parse the line for the inode information
            if (sscanf(line, "ino: %lu", &iNode) == 1) {
                break; // inode found, break out of the loop
            }
        }
        fclose(fdInfoFile); // Close the fdinfo file
        // Insert file descriptor information into list
        insert_fd_info(procNode, atoi(fdEntry->d_name), linkPath, iNode);
      }
    }
  }

  if (*procHead == NULL) {
    *procHead = procNode;  // If the list is empty, new node is the head
    return;
  }
  procLL* procLast = *procHead;  // Find the last node in the list
  while (procLast->next) {
    procLast = procLast->next;
  }
  procLast->next = procNode;  // Append the new node at the end of the list

  if (closedir(fdDir) == -1) {  // Close the directory
    fprintf(stderr, "Error closing directory %s!\n",
            fdPath);  // Print error if directory closing fails
    exit(1);          // Exit program with error
  }
}

// Function to iterate over all processes and store their file descriptor
// information
void iterate_over_processes(procLL** procHead, uid_t uid) {
  DIR* procDir = opendir("/proc");  // Open the /proc directory
  if (!procDir) {
    fprintf(stderr,
            "Error opening /proc directory!\n");  // Print error if /proc
                                                  // directory cannot be opened
    exit(1);                                      // Exit program with error
  }

  struct dirent* dirEntry;  // Structure to hold directory entry information
  while ((dirEntry = readdir(
              procDir))) {  // Iterate over all entries in /proc directory
    if (dirEntry->d_type == DT_DIR &&
        char_is_digit(
            dirEntry->d_name[0])) {  // Check whether entry is a directory and
                                     // name starts with a digit
      pid_t currentPid =
          atoi(dirEntry->d_name);  // Convert directory name to PID
      if (!is_user_process(currentPid,
                           uid)) {  // Check whether process belongs to the user
        continue;                   // Skip if process does not belong to user
      }
      read_fd_info(currentPid, procHead);  // Read and store file descriptor
                                           // information for the process
    }
  }

  if (closedir(procDir) == -1) {  // Close the /proc directory
    fprintf(stderr,
            "Error closing /proc directory!\n");  // Print error if directory
                                                  // closing fails
    exit(1);                                      // Exit program with error
  }
}
