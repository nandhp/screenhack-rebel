= `screenhack-rebel` =

> **rebel**, *n*.
>   A person who resists an established authority, often violently.

== What is it? ==

screenhack-rebel allows you to subvert a totalitarian screensaver
daemon (gnome-screensaver) to run custom screenhacks, without
requiring write access to /usr/share/applications/screensavers.

To use screenhack-rebel, run gnome-screensaver with

    LD_PRELOAD=/path/to/screenhack-rebel.so

Please do not put this in your global environment; it should only be
used with gnome-screensaver. You may be able to do this in System ->
Preferences -> Startup Applications by changing the command for
"Screensaver" from `gnome-screensaver` to

    sh -c 'LD_PRELOAD=/path/to/my-screensaver.so exec gnome-screensaver'

You will have to restart gnome-screensaver for this to take effect.

Once you have done this, attempts by the gnome-screensaver process to
start the floating-GNOME-logo screensaver ("Floating Feet",
`/.../gnome-screensaver/floaters`) will be diverted to start
`~/.screenhack-rebel` instead. You will need to create this file and
make it executable. A simple shell script like this will suffice:

    #!/bin/sh
    exec /path/to/xscreensaver/hacks/kumppa -root

== How does it work? ==

`LD_PRELOAD` is a feature of the dynamic linker that allows arbitrary
libraries to be loaded into programs. Libraries loaded this way are
often used to override the behavior of system calls or other library
functions.

`screenhack-rebel` works by trapping all `execve` system calls in
affected processes. It then checks if the filename to execute matches
`/.../gnome-screensaver/floaters` (one of GNOME's default screenhacks)
and, if it is, changes it to `~/.screenhack-rebel`.
