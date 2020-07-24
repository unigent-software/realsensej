package com.unigent.realsensej.test;

import com.unigent.realsensej.RealSenseJ;

import javax.imageio.ImageIO;
import java.awt.*;
import java.awt.image.BufferedImage;
import java.io.ByteArrayInputStream;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.Objects;

public class RealSenseJTester {

    static int width = 640;
    static int height = 480;

    public static void main(String [] args) throws Exception {

        ImageViewer rgbViewer = new ImageViewer(width, height);
        ImageViewer depthColorViewer = new ImageViewer(width, height);
        ImageViewer depthZ16Viewer = new ImageViewer(width, height);

        RealSenseJ realSenseJ = new RealSenseJ();
        realSenseJ.openPipe();
        System.out.println("Pipe opened");

        long start = System.currentTimeMillis();

        while(System.currentTimeMillis() - start < 60000) {
            byte[][] frames = realSenseJ.getFrames();

            BufferedImage rgb = ImageIO.read(new ByteArrayInputStream(Objects.requireNonNull(frames[0])));
            BufferedImage depthColor = ImageIO.read(new ByteArrayInputStream(Objects.requireNonNull(frames[1])));
            BufferedImage depthZ16 = depthToImage(depthBytesToDepthMatrix(frames[2]));

            rgbViewer.show(rgb, "RGB");
            depthColorViewer.show(depthColor, "Depth Color");
            depthZ16Viewer.show(depthZ16, "Depth Z16");
        }

        realSenseJ.closePipe();
        System.out.println("Pipe closed");
    }

    private static BufferedImage depthToImage(int [][] depthData) {
        BufferedImage img = new BufferedImage(width, height, BufferedImage.TYPE_INT_RGB);
        for(int r=0; r<height; r++) {
            for(int c=0; c<width; c++) {
                if(depthData[r][c] > 0 && depthData[r][c] < 20000) {
                    img.setRGB(c, r, Color.getHSBColor(depthData[r][c] / 5000.0f, 0.8f, 0.8f).getRGB());
                }
            }
        }
        return img;
    }

    private static int [][] depthBytesToDepthMatrix(byte [] depthData) {
        int [][] result = new int[height][width];
        int i = 0;
        for(int r=0; r<height; r++) {
            for(int c=0; c<width; c++) {
                int d = fromBytes(depthData[i], depthData[i+1]);
                if(d != 0 && d < 65535){
                    result[r][c] = d;
                }
                i += 2;
            }
        }

//        System.out.println("236,116: " + result[116][236]);

        return result;
    }

    private static int fromBytes(byte b2, byte b3) {
        ByteBuffer byteBuffer = ByteBuffer.allocateDirect(4);
        // by choosing big endian, high order bytes must be put
        // to the buffer before low order bytes
        byteBuffer.order(ByteOrder.BIG_ENDIAN);
        byteBuffer.put((byte)0);
        byteBuffer.put((byte)0);
        byteBuffer.put(b2);
        byteBuffer.put(b3);
        byteBuffer.flip();
        return byteBuffer.getInt();
    }

    static int map(int x, int in_min, int in_max, int out_min, int out_max) {
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }
}
