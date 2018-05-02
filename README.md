# TrackingObjects
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
## Text Below is Outdated
### Replicate the hierarchy below to reproduce the project in Visual Studio
Visual Studio Project Hierarchy:
<pre>
.  
+-- _External Dependencies  
+-- _Header Files  
|    +-- globals.h  
|    +-- Header.h  
+-- _Resource Files  
+-- _Source Files  
|    +-- helpers.cpp  
|    +-- Source.cpp  
</pre>

