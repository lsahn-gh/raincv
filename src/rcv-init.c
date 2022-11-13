/** rcv-init.c
 *
 * Copyright 2022 Leesoo Ahn <lsahn@ooseel.net>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* simple init program for container */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "rcv-log.h"

enum {
  RFD = 0,
  WFD = 1,
  N_FD
};
static int pipefd[N_FD];

static void sig_handler(int signo)
{
  int old = errno;
  unsigned char sn = signo;
  if (write(pipefd[WFD], &sn, 1) == -1)
    err("failed to write (%s)", strerror(errno));
  errno = old;
}

static void
init_tasks(pid_t child_pid)
{
  int ret, maxfd;
  unsigned char signo;
  fd_set rfds;

  if (pipe(pipefd) == -1)
    /* error */;

  signal(SIGUSR1, SIG_IGN);
  signal(SIGUSR2, SIG_IGN);
  signal(SIGTERM, sig_handler);
  signal(SIGCHLD, sig_handler);

  for (;;)
  {
    FD_ZERO(&rfds);
    FD_SET(pipefd[RFD], &rfds);
    maxfd = pipefd[RFD];

    ret = select(maxfd + 1, &rfds, NULL, NULL, NULL);
    if (ret > 0 && FD_ISSET(pipefd[RFD], &rfds)) {
      /* try reading */
      do {
        ret = read(pipefd[RFD], &signo, 1);
      } while (ret == -1 && errno == EINTR);

      /* no data! */
      if (ret == 0)
        continue;

      switch (signo) {
        case SIGTERM:
          __debug("SIGTERM");
          kill(-1, SIGTERM);
          /* TODO wait until all process are stopped */
          break;
        case SIGCHLD:
          /* TODO improvement */
          __debug("SIGCHLD");
          ret = waitpid(-1, NULL, WNOHANG);
          if (ret > 0) /* there is a child state has changed */
            goto get_die;
          /* no children state changed or error occurred */
          break;
      }
    } else if (ret == -1 && errno == EINTR) {
      continue;
    } else /* timeout, nothing to do */ {
      continue;
    }
  }

get_die:
  die(EXIT_SUCCESS);
}

int
spawn_container_init(void)
{
  pid_t pid;

  pid = fork();
  if (pid == -1)
    return INIT_FAIL;

  if (pid > 0) { /* parents! */
    init_tasks(pid);
  }

  /* Now ready to new container! */
  return NO_ERROR;
}

