# Bob-Rust-Generator
This project generates and optimizes the borst files

# Building VisualStudio
To build this project you will need to install `node.js` `v14.0.0` and build it
https://github.com/nodejs/node/blob/master/BUILDING.md

For this project to work and for visual studio to find all the include
folders you will need to add a new environment variable `NODE_ROOT` that
points to the root folder of the downloaded `node.js` project.

When building the `node.js` project make sure you run the builder on
the same target as you want it to compile. Example if you want to build
it for `Debug` and `x64` you should build it for these versions.
```
vcbuild.bat Debug
vcbuild.bat x64
```

To build this addon for `electron` run the folowing:
```
node-gyp rebuild --target=12.0.0 --arch=x64 --dist-url=https://atom.io/download/atom-shell
```
