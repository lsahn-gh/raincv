/** raincv.c
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

#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/mount.h>

#include "config.h"

#include "rcv-utils.h"

char opt_debug;

#define STACK_SIZE_1MB (1024 * 1024)
static char container_stack[STACK_SIZE_1MB];

static void
usage(void)
{
  fprintf(stderr,
          "usage: "PACKAGE_NAME" [options] PROG args...\n\n"

          "- Note: PID namespace and mounting procfs is default\n\n"

          "[options]\n"
          "\t--help              : show help\n"
          "\t--hostname <NAME>   : new hostname to container\n"
          "\t--ns-mount <PATH>   : new mount namespace with <PATH> mounting to container\n"
          "\t--ns-user <UID:GID> : new user namespace with <UID:GID> mapping to container\n"
          );
}

static int
init_container_fn(void *arg)
{
  /* required namespaces are made ! */
  struct rcv_arguments *args = arg;

  /* default values */
  if (mount("none", "/proc", NULL, MS_REC | MS_PRIVATE, NULL) == -1)
    err_die(MOUNT_FAIL, "Failed to mount-private proc (%s)", strerror(errno));
  if (mount("proc", "/proc", "proc", 0, NULL) == -1)
    err_die(MOUNT_FAIL, "Failed to mount proc (%s)", strerror(errno));

  /* args */
  if (args->ns_hostname) {
    if (sethostname(args->ns_hostname, strlen(args->ns_hostname)) == -1)
      err_die(UTS_FAIL, "Failed to set new hostname (%s)", strerror(errno));
  }

  __debug("about to call 'execvp");
  execvp(args->exec_prog, args->exec_args);
}

int
main(int argc, char *argv[])
{
  pid_t child_pid;
  struct rcv_arguments *args;

  args = args_parse(--argc, ++argv);
  switch (args->err) {
    case NO_ERROR:
      if (args->show_help) {
        usage();
        die(EXIT_SUCCESS);
      }
      break;
    default:
      usage();
      die(args->err);
  }

  child_pid = clone(init_container_fn,
                    container_stack + STACK_SIZE_1MB,
                    args->ns_flags | SIGCHLD,
                    args);
  if (child_pid == -1)
    err_die(CLONE_FAIL, "Failed for calling clone (%s)", strerror(errno));

  if (waitpid(child_pid, NULL, 0) == -1)
    err_die(CHILD_FAIL, "waitpid");

  return EXIT_SUCCESS;
}

