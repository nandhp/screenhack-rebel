/*
 * screenhack-rebel.c - Subvert a totalitarian screensaver daemon.
 *
 * Copyright (c) 2013 nandhp <nandhp@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * screenhack-rebel.c was written using concepts from code produced by
 * FILE_PRELOAD: https://github.com/j0k/fp
 *
 */

#define _GNU_SOURCE
#include <dlfcn.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define BUFSIZE 256

static int (*old_execve)(const char *filename, char *const argv[],
                         char *const envp[]);

int execve(const char *filename, char *const argv[], char *const envp[]) {
  char alternate[BUFSIZE];
  old_execve = dlsym(RTLD_NEXT, "execve");
  printf("screenhack-rebel: execve hook: %s\n", filename);

  /* Check for our signal and override the filename if found */
  if ( strstr(filename, "gnome-screensaver/floaters") ) {
    int rc = snprintf(alternate, BUFSIZE, "%s/%s", getenv("HOME"),
                      ".screenhack-rebel");
    if ( rc > 0 && rc < BUFSIZE )
      filename = alternate;
    else
      printf("screenhack-rebel: snprintf error: %d\n", rc);
    printf("screenhack-rebel: actually execing: %s\n", filename);
  }
  return old_execve(filename, argv, envp);
}
