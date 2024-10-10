# Building/Running (Only tested on Linux)
## dependencies:
- [CGLM](https://github.com/recp/cglm)
    - [CMake](https://cmake.org/)
- [GLFW](https://www.glfw.org/)
- [GNU Make](https://www.gnu.org/software/make/)

If you got all dependencies installed, simply build like so:

```
git clone https://github.com/rodrigolitzius/OpenGL
cd OpenGL
make
```

Then to run:
```
./main
```

If you get a `Cannot open shared object file` make sure LD (The dynamic linker) is searching for libraries in the path CGLM is installed.

Run this to find where CGLM is installed:
```
find /usr/ 2>/dev/null | grep libcglm
```

Then run this to tell LD where to find CGLM
```
LD_LIBRARY_PATH=<the path cglm is installed>
```
- Note that this will only set the variable for your current shell session