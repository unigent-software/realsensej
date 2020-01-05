package com.unigent.realsensej;

/**
 * Intel RealSense JNI Adaptor 
 *
 * AgentBase Robot Platform, Unigent Robotics, <a href="http://unigent.com">Unigent</a>
 *
 * @author terranfounder
 *
 */

public class RealSenseJ {

    static {
        try {
            System.loadLibrary("realsensej");

        } catch (UnsatisfiedLinkError e) {
            System.err.println("Native code library failed to load. Using library path: " + System.getProperty("java.library.path") + ". Error:" + e.getMessage());
            throw e;
        }
    }

    public native void openPipe();

    public native void closePipe();

    // [0] - RGB, [1] - depth (colorized), [2] - depth (RS2_FORMAT_Z16)
    public native byte [][] getFrames();

}
