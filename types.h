#ifndef TYPES_H
#define TYPES_H

#include <sys/types.h>

// Define a struct to store file descriptor information
typedef struct fdInformation {
  int fd; // File descriptor number
  char fileName[256]; // Name of the file associated with the file descriptor
  unsigned long iNode; // Inode number of the file
  struct fdInformation* next; // Pointer to the next node in the linked list
} fdInfo;

// Define a struct for a linked list of processes
typedef struct processLinkedList {
  pid_t pid; // Process ID
  fdInfo* fdHead; // Pointer to the head of a linked list of file descriptors for this process
  struct processLinkedList* next; // Pointer to the next node in the linked list
} procLL;

#endif  // TYPES_H
