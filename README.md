# realsensej

This is a very basic Java (JNI) adapter for Intel RealSense library. Here is an example on how to use it:
(Make sure realsensej and realsense2 libraries are in your JVM's library path)

```
System.loadLibrary("realsensej");

RealSenseJ realSenseJ = new RealSenseJ();
realSenseJ.openPipe();
 
byte[][] frames = realSenseJ.getFrames();
BufferedImage rgb = ImageIO.read(new ByteArrayInputStream(Objects.requireNonNull(frames[0])));
BufferedImage depthColor = ImageIO.read(new ByteArrayInputStream(Objects.requireNonNull(frames[1])));
BufferedImage depthZ16 = depthToImage(depthBytesToDepthMatrix(frames[2]));

realSenseJ.closePipe();
```

The native lib is built for Linux x64 and Raspberry Pi 4 but it's easy to build it for other systems.

**NOTE** Before using this, you need to make sure LibRealSense2 from Intel is installed on your system.
Follow https://github.com/IntelRealSense/librealsense/

**NOTE for Pi** When configuring CMake, make sure to set CMAKE_CXX_FLAGS to "-fPIC" to avoid compilation errors. You may need to check "Advanced" checkbox in CMake GUI. 

