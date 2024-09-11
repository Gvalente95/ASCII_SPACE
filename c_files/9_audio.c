
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

pid_t play_sound(const char *filename, int loop) {
  pid_t pid = fork();
  if (pid == 0) {
    // Child process: Replace it with the afplay process
    if (loop)
      execlp("afplay", "afplay", "-t", "9999", filename, (char *)NULL);
    else
      execlp("afplay", "afplay", filename, (char *)NULL);
    // If execlp fails
    perror("execlp failed");
    exit(EXIT_FAILURE);
  }
  return pid; // Return the process ID of the child
}

void stop_sound(pid_t pid) {
  if (pid > 0) {
    kill(pid, SIGTERM); // Send SIGTERM to the afplay process to stop it
  }
}