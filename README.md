Clone Raylib in the parent directory of this repo (..). Then in the `src`
directory run:

```sh
emcc -c rcore.c -Os -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2
emcc -c rshapes.c -Os -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2
emcc -c rtextures.c -Os -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2
emcc -c rtext.c -Os -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2
emcc -c rmodels.c -Os -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2
emcc -c utils.c -Os -Wall -DPLATFORM_WEB
emcc -c raudio.c -Os -Wall -DPLATFORM_WEB

emar rcs libraylib.a rcore.o rshapes.o rtextures.o rtext.o rmodels.o utils.o raudio.o
```

Now in the repo run:
```sh
emcc -o game.html main.c -Os -Wall ../raylib/src/libraylib.a  -I. -I../raylib/src -L. -L../raylib/src/libraylib.a -s USE_GLFW=3 -s TOTAL_MEMORY=67108864 -s -s ALLOW_MEMORY_GROWTH=1 -s FORCE_FILESYSTEM=1 --shell-file ../raylib/src/minshell.html -DPLATFORM_WEB --preload-file assets
```

Used Resources: 
- https://opengameart.org/content/underwater-diving-pack
- https://opengameart.org/content/magical-theme
- https://opengameart.org/content/female-gruntwork-1
- https://opengameart.org/content/40-cc0-water-splash-slime-sfx
