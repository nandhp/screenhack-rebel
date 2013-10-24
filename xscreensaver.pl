#!/usr/bin/perl
#
# xscreensaver.pl - Parse ~/.xscreensaver and run a random screenhack
#
# Copyright (c) 2013 nandhp <nandhp@gmail.com>
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in
#    the documentation and/or other materials provided with the
#    distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#

use Text::ParseWords;
use Cwd;
use warnings;
use strict;

# Add xscreensaver to the PATH
my @PATH = ();
foreach my $dir (
    # User-set environment variable: path to XScreenSaver (source tree
    # or installed binaries)
    $ENV{XSCREENSAVER_HOME},
    # This is where my copy of the source tree lives. Lucky me, I
    # don't need to set XSCREENSAVER_HOME!
    "$ENV{HOME}/opt/xscreensaver",
    # Popular locations used by common Linux distributions
    '/usr/lib/xscreensaver', '/usr/libexec/xscreensaver'
    ) {
    next unless $dir and -d $dir;
    foreach my $subdir ( qw(hacks hacks/glx driver) ) {
        push @PATH, "$dir/$subdir" if -d "$dir/$subdir";
    }
    push @PATH, $dir;
}
$ENV{PATH} = join(':', @PATH, $ENV{PATH});

# Load the configuration file
open F, '<', "$ENV{HOME}/.xscreensaver"
    or die "Can't open ~/.xscreensaver: $!";
my %config = ();
my $cont = '';
while (<F>) {
    my $line = $_;
    if ( $cont ) { $_ = $cont . $_ }
    elsif ( m/^[#!;]|^\s*$/ ) { next }
    if ( $line =~ m/\S/ && m/^(.+)\\\s*$/ ) { $cont = $1; next }
    m/^(\w+):\s*(.+?)\s*$/ or die "Parse error in ~/.xscreensaver";
    my ($key, $value) = ($1, $2);
    $value =~ s/\\(.)/($1 eq 'n')?"\n":(($1 eq 'r')?"\r":(($1 eq 't')?"\t":$1))/ge;
    $config{$key} = $value;
    $cont = '';
}
close F;

# Ignore most of the options, as they don't apply
# FIXME: Consider handling options like nice, mode, selected

# Parse $PATH, dereferencing symlinks (not generally necessary).
my @path = ();
foreach ( split ':', $ENV{PATH} ) {
    my $dir = Cwd::abs_path($_);
    push @path, $dir if $dir and -d $dir;
}

# Parse @ARGV
my $opt_window = 0;
my $opt_echo = 0;
my @opt_forward = ();
foreach ( @ARGV ) {
    if ( $_ eq '-help' ) {
        print STDERR <<EOF;
$0 [-echo|-print] [-window]
Emulate behavior xscreensaver for screenhack-rebel: run a random
screenhack from ~/.xscreensaver.
EOF
        exit 1; #/
    }
    elsif ( $_ eq '-echo' or $_ eq '-print' ) { $opt_echo = 1 }
    elsif ( $_ eq '-window' ) { $opt_window = 1 }
    # FIXME: This doesn't work when replacing Floaters
    # else { push @opt_forward, $_ }
    # [...]
    # "Unrecognized options are passed to the chosen screenhack."
}

# Load the list of enabled screensavers
my @programs = ();
$config{programs} or die "No programs in ~/.xscreensaver";
foreach my $program ( split "\n", $config{programs} ) {
    $program =~ m/^\s*(-)?\s*(\w+:)?\s*(.+?)\s*$/
        or die "Parse error in ~/.xscreensaver:programs at $program";
    my ($disabled, $visual, $cmdline) = ($1, $2, $3);
    next if $disabled;
    $cmdline =~ s/-root/-window/ if $opt_window;
    my @cmdline = shellwords($cmdline);
    if ( $cmdline[0] !~ m/\// ) {
        foreach my $dir ( @path ) {
            if ( -x "$dir/$cmdline[0]" ) {
                $cmdline[0] = "$dir/$cmdline[0]";
                last;
            }
        }
    }
    push @programs, [@cmdline] if -x $cmdline[0];
}

# Choose a screensaver and run it
@programs or die "No valid programs in ~/.xscreensaver";
my $i = int(rand(@programs));
push @{$programs[$i]}, @opt_forward;
if ( $opt_echo ) {
    print join("\n", "PATH=$ENV{PATH}", @{$programs[$i]}),"\n";
    exit 0;
}
exec @{$programs[$i]};
