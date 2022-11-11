/** rcv-args.c
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
#include <string.h>

#include "rcv-log.h"
#include "rcv-args.h"

static void
rcv_arguments_init(struct rcv_arguments *args, size_t len)
{
  memset(args, 0, len);

  args->err = ARGS_FAIL;
  dzf_vec_new(&(args->ns_mnt_points), sizeof(args->ns_mnt_points.data[0]));
}

struct rcv_arguments *
args_parse(int argc, char *argv[])
{
  static struct rcv_arguments rcv_args;
  struct argument *p;

  rcv_arguments_init(&rcv_args, sizeof(struct rcv_arguments));

  if (argc == 0)
    goto out;

  /* default */
  rcv_args.ns_flags |= CLONE_NEWPID;
  rcv_args.ns_flags |= CLONE_NEWNS; /* procfs */

  for (;
       *argv != NULL && (*argv)[0] == '-' && (*argv)[1] == '-';
       argv++)
  {
    if (strcmp(*argv, "--help") == 0)
    {
      rcv_args.show_help = 1;
      goto success;
    }
    else if (strcmp(*argv, "--debug") == 0)
    {
      opt_debug = 1;
    }
    else if (strcmp(*argv, "--hostname") == 0)
    {
      rcv_args.ns_flags |= CLONE_NEWUTS;
      rcv_args.ns_hostname = *(++argv);
    }
    else if (strncmp(*argv, "--ns-mount", strlen("--ns-mount")) == 0)
    {
      /* TODO mount path parser */
      dzf_vec_add_tail(&rcv_args.ns_mnt_points, *(++argv));
    }
    else if (strcmp(*argv, "--ns-user") == 0)
    {
      /* TODO uid:gid parser */
      rcv_args.ns_flags |= CLONE_NEWUSER;
    }
    else
    {
      /* unrecognized options */
      goto out;
    }
  }

  /* parse exec PROG and its arguments */
  if (*argv == NULL)
    goto out;

  rcv_args.exec_prog = *argv;
  rcv_args.exec_args = argv;

success:
  rcv_args.err = NO_ERROR;

out:
  return &rcv_args;
}




