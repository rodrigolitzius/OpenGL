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

# "Playing"
## Cross-eye 3D
This project has a "fake 3D" mode that allows you to perceive depth from 2D images. If you want to know how see this effect i recommend [this Youtube video](https://www.youtube.com/watch?v=Tt0d9DhnmGc). (It looks really cool, it's worth the try)
## Key bindings
| Key | Action |
| -- | -- |
| WASD | Forwards/backwards, left/right|
| Space/LShift | Up/down |
| Right Click | Grab/release cursor|
| Mouse Wheel | Change FOV |
| C | Enable wireframe mode |
| V | Change VAO |
| 3 | Enable cross-eye 3D mode |
