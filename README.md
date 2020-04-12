# Wolf example app

Base of this repository was created with `wolf_win (wolf_linux)` docker image.

You can build if either with docker:
```
docker run --rm -v ${PWD}:C:\wolf wolf_win
docker run --rm -v ${PWD}:/wolf wolf_linux
```

or natively:
```
mkdir wolf-build
cd wolf-build
cmake.exe -DCMAKE_BUILD_TYPE=Release ..
cmake.exe --build . --target install --config Release
```
