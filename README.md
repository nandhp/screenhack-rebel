screenhack-rebel
================

> **rebel**, *n*.
>   A person who resists an established authority, often violently.

What is it?
-----------

screenhack-rebel allows you to subvert a totalitarian screensaver
daemon (gnome-screensaver 2.x or mate-screensaver) to run custom
screenhacks without requiring write access to
`/usr/share/applications/screensavers` or `/usr/libexec/*screensaver`.

To use screenhack-rebel, run the screensaver daemon with

    LD_PRELOAD=/path/to/screenhack-rebel.so

Please do not put this in your global environment; it should only be
used with the screensaver daemon. You may be able to do this in System
-> Preferences -> Startup Applications by changing the command for
"Screensaver" from `gnome-screensaver` to

    sh -c 'LD_PRELOAD=/path/to/screenhack-rebel.so exec gnome-screensaver'

(or similar for mate-screensaver). You will have to restart the
screensaver daemon for this to take effect.

Once you have done this, attempts by the screensaver daemon to start
the floating-GNOME-logo screensaver ("Floating Feet",
`/path/to/gnome-screensaver/floaters`) will be diverted to start
`~/.screenhack-rebel` instead. You will need to create this file and
make it executable. A simple shell script like this will suffice:

    #!/bin/sh
    exec /path/to/xscreensaver/hacks/kumppa -root

You can also make it a symlink to `xscreensaver.pl`, which will choose
a random enabled screensaver from your `~/.xscreensaver` file and run
it. It will look for the screenhacks in `/usr/lib/xscreensaver` and
`/usr/libexec/xscreensaver`, which are popular locations for Linux
distributions to put them. If they aren't installed there, set the
`XSCREENSAVER_HOME` environment variable to the location of the
compiled source distribution (or your unusual equivalent to
`/usr/lib/xscreensaver`).

How does it work?
-----------------

`LD_PRELOAD` is a feature of the dynamic linker that allows arbitrary
libraries to be loaded into programs. Libraries loaded this way are
often used to override the behavior of system calls or other library
functions.

`screenhack-rebel` works by trapping all `execve` calls in affected
processes. Newer versions of gnome-screensaver and mate-screensaver
clear the `LD_PRELOAD` environment variable prior to starting the
screenhack subprocess, so `gdk_spawn_on_screen_with_pipes` is trapped
instead. In both cases, `screenhack-rebel` checks if the filename
parameter matches `-screensaver/floaters` (one of the default
screenhacks in GNOME and MATE screensaver) and, if it does, changes it
to `~/.screenhack-rebel`.
