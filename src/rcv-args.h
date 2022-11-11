/** rcv-args.h
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

#ifndef ARGS_H
#define ARGS_H

#include <dzf/dzf-vector.h>

typedef dzf_vec_t(char *) vec_string_t;

struct rcv_arguments
{
  char *exec_prog;
  char **exec_args;
  unsigned char err;
  unsigned char show_help;

  /* NS */
  int ns_flags;
  char * ns_hostname;
  vec_string_t ns_mnt_points;
  struct {
    unsigned short uid;
    unsigned short gid;
  } ns_user;
};

struct rcv_arguments * args_parse(int argc, char *argv[]);

#endif /* ARGS_H */
