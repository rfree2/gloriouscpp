#!/bin/bash

dir="$HOME/.bin/"
mkdir -p "$dir"
f="runscriptcpp11" && cp "src/$f" "$dir/$f" && chmod 755 "$dir/$f" && echo "Installed $f to $dir (as user)"

echo "Make sure that directory $dir is in your PATH, you might need to edit your .bashrc"
echo "PATH is $PATH"

