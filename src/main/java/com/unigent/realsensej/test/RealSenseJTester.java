package com.unigent.realsensej.test;

import com.unigent.realsensej.RealSenseJ;

import javax.imageio.ImageIO;
import java.awt.image.BufferedImage;
import java.io.ByteArrayInputStream;
import java.util.Objects;

public class RealSenseJTester {

    public static void main(String [] args) throws Exception {

        ImageViewer rgbViewer = new ImageViewer(640, 480);
        ImageViewer depthViewer = new ImageViewer(640, 480);

        RealSenseJ realSenseJ = new RealSenseJ();
        realSenseJ.openPipe();
        System.out.println("Pipe opened");

        long start = System.currentTimeMillis();

        while(System.currentTimeMillis() - start < 10000) {
            byte[][] frames = realSenseJ.getFrames();

            BufferedImage rgb = ImageIO.read(new ByteArrayInputStream(Objects.requireNonNull(frames[0])));
            BufferedImage depthColor = ImageIO.read(new ByteArrayInputStream(Objects.requireNonNull(frames[1])));

            rgbViewer.show(rgb, "RGB");
            depthViewer.show(depthColor, "Depth");


            System.out.println("Depth length: " + frames[2].length);
        }

        realSenseJ.closePipe();
        System.out.println("Pipe closed");
    }

}
