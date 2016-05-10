/*
 * screenhack-rebel.c - Subvert a totalitarian screensaver daemon.
 *
 * Copyright (c) 2013, 2016 nandhp <nandhp@gmail.com>
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
#include <gdk/gdk.h>

#define TRIGGER "-screensaver/floaters"
#define FILENAME ".screenhack-rebel"
#define SELFNAME "screenhack-rebel"

#if defined(HOOK_GDK_SPAWN)
gboolean
(*old_gdk_spawn_on_screen_with_pipes)(GdkScreen *screen,
                                      const gchar *working_directory,
                                      gchar **argv,
                                      gchar **envp,
                                      GSpawnFlags flags,
                                      GSpawnChildSetupFunc child_setup,
                                      gpointer user_data,
                                      gint *child_pid,
                                      gint *standard_input,
                                      gint *standard_output,
                                      gint *standard_error,
                                      GError **error);
gboolean
gdk_spawn_on_screen_with_pipes (GdkScreen *screen,
                                const gchar *working_directory,
                                gchar **argv,
                                gchar **envp,
                                GSpawnFlags flags,
                                GSpawnChildSetupFunc child_setup,
                                gpointer user_data,
                                gint *child_pid,
                                gint *standard_input,
                                gint *standard_output,
                                gint *standard_error,
                                GError **error) {
  old_gdk_spawn_on_screen_with_pipes = dlsym(RTLD_NEXT,
                                             "gdk_spawn_on_screen_with_pipes");

  if ( argv && argv[0] ) {
    fprintf(stderr, "%s: gdk_spawn_on_screen_with_pipes hook: %s\n",
            SELFNAME, argv[0]);

    /* Check for our signal and override the filename if found */
    if ( strstr(argv[0], TRIGGER) ) {
      gchar *new_argv0 = g_strdup_printf("%s/%s", getenv("HOME"), FILENAME);
      if ( argv[0] ) {
        g_free(argv[0]);
        argv[0] = new_argv0;
      }
      else
        fprintf(stderr, "%s: g_strdup_printf error\n", SELFNAME);
      fprintf(stderr, "%s: actually execing: %s\n", SELFNAME, argv[0]);
    }
  }
  return old_gdk_spawn_on_screen_with_pipes (screen, working_directory,
                                             argv, envp, flags, child_setup,
                                             user_data, child_pid,
                                             standard_input, standard_output,
                                             standard_error, error);
}
#elif defined(HOOK_EXECVE)
#define BUFSIZE 256

static int (*old_execve)(const char *filename, char *const argv[],
                         char *const envp[]);

int execve(const char *filename, char *const argv[], char *const envp[]) {
  char alternate[BUFSIZE];
  old_execve = dlsym(RTLD_NEXT, "execve");

  if ( filename ) {
    fprintf(stderr, "%s: execve hook: %s\n", SELFNAME, filename);

    /* Check for our signal and override the filename if found */
    if ( strstr(filename, TRIGGER) ) {
      int rc = snprintf(alternate, BUFSIZE, "%s/%s", getenv("HOME"), FILENAME);
      if ( rc > 0 && rc < BUFSIZE )
        filename = alternate;
      else
        fprintf(stderr, "%s: snprintf error: %d\n", SELFNAME, rc);
      fprintf(stderr, "%s: actually execing: %s\n", SELFNAME, filename);
    }
  }
  return old_execve(filename, argv, envp);
}
#else
#error Must specify function to hook.
#endif
