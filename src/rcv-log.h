/** rcv-log.h
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

#ifndef LOG_H
#define LOG_H

extern char opt_debug;
#define __debug(fmt, ...) \
  if (opt_debug) \
    fprintf(stderr, fmt "\n", ##__VA_ARGS__)

#define die(retcode) exit(-retcode)
enum {
  NO_ERROR = 0,
  EXIT_FAIL = 1,
  ARGS_FAIL,
  CLONE_FAIL,
  CHILD_FAIL,
  MOUNT_FAIL,
  UTS_FAIL,

  NOT_REACH,
};

#define err_die(code, fmt, ...) \
  do { \
    fprintf(stderr, fmt "\n", ##__VA_ARGS__); \
    die(code); \
  } while (0)

#define err_ret(retval, fmt, ...) \
  do { \
    fprintf(stderr, fmt "\n", ##__VA_ARGS__); \
    return (retval); \
  } while (0)

#define retval_if(expr, retval) \
  if ((expr)) { return (retval); }

#define ret_if(expr) \
  if ((expr)) { return; }

#endif /* LOG_H */
