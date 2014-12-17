#!/bin/bash
# This is the installation (for runscript - see README.txt, also check there any licences etc)
# This script installs as user (in $HOME). Call it directly.

dir="$HOME/.local/bin/"
mkdir -p "$dir"
f="runscriptcpp11" && cp "src/$f" "$dir/$f" && chmod 755 "$dir/$f" && echo "Installed $f to $dir (as user)"

echo "Make sure that directory $dir is in your PATH, you might need to edit your .bashrc"
echo "E.g. try adding following line to top of your .bashrc file:"
echo "PATH=\"\$PATH:$dir\""

