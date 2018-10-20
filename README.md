# Tracking Objects
## App to track olympic lifting bar's path

Create a build directory inside your local repo to build the project makefiles
```
$ cd TrackingObjects
$ mkdir build
$ cd build
$ cmake .. -G "Visual Studio 15 2017 Win64"
```

Run the command below in the build directory to build the solution in Release mode:
```
$ cmake --build . --config Release
```

Note: Project is only working in Release mode.

___

In order to Build/Compile the project just save your changes and go to the build directory (_TrackingObjects/build_) and run the following command:
```
$ cmake --build . --config Release
```

If you want to run your compiled program, just run:
```
$ ./Release/trackingObjects.exe
```
![alt text](https://github.com/RubenAMtz/tracking-objects/blob/master/media/path.png "Logo Title Text 1")
