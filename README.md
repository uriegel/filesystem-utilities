# filesystem-utils
File system utilities for Node.js
## Prerequisites Fedora
``` 
sudo dnf install gcc-c++ gtk3-devel
``` 

## Prerequisites Ubuntu
``` 
sudo apt-get install build-essential libgtk-3-dev
``` 

## Compile drag_helper on Linux:
```
g++ drag_helper.cpp -o drag_helper `pkg-config --cflags --libs gtk+-3.0`
xxd -i drag_helper > ../source/linux/drag_helper_exec.h
```