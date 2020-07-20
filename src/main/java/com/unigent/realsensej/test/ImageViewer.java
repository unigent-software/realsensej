package com.unigent.realsensej.test;

import javax.swing.*;
import java.awt.*;
import java.awt.image.BufferedImage;

public class ImageViewer {
    JFrame frame;
    ImageIcon icon;
    JLabel titleText;

    static final int MIN_WIDTH = 120;

    public ImageViewer(int imageWidth, int imageHeight) {

        int width = Math.max(imageWidth, MIN_WIDTH);

        this.icon=new ImageIcon();

        frame = new JFrame();

        FlowLayout layout = new FlowLayout(FlowLayout.CENTER);

        frame.setLayout(layout);
        frame.setSize(width, imageHeight + 80);
        frame.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
        JLabel lbl=new JLabel();
        lbl.setIcon(icon);
        frame.add(lbl);

        titleText = new JLabel("", JLabel.CENTER);
        titleText.setPreferredSize(new Dimension(width, 40));
        titleText.setForeground(Color.BLACK);
        frame.add(titleText);
    }

    public void show(BufferedImage image, String title) {
        this.icon.setImage(image);
        this.titleText.setText(title);
        if(this.frame.isVisible()) {
            this.frame.repaint();
        }
        else {
            frame.setVisible(true);
        }
    }

    public static ImageViewer showImage(BufferedImage image) {
        return showImage(image, "", null);
    }

    public static ImageViewer showImage(BufferedImage image, String title) {
        return showImage(image, title, null);
    }

    public static ImageViewer showImage(BufferedImage image, String title, ImageViewer viewer) {

        if(viewer == null) {
            viewer = new ImageViewer(image.getWidth(), image.getHeight());
        }

        viewer.show(image, title);
        return viewer;
    }
}

