#!/bin/bash
#!/bin/bash
# This is the installation (for runscript - see README.txt, also check there any licences etc)
# This script installs as user (in $HOME). Call it directly.

f="runscriptcpp11" && cp "src/$f" "/usr/local/bin/$f" && chmod 755 "/usr/local/bin/$f" && echo "Installed $f (system wide)"

