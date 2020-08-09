# realsensej

This is a very basic JNI adapter for Intel RealSense library. Here is an example on how to use it:
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

The natrive lib is built for Linux x64 and Raspberry Pi 4 but it's easy to build it for other systems.
