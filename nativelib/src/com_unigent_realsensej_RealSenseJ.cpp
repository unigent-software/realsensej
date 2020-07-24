

/**
 *
 * README:
 *
 * https://github.com/IntelRealSense/librealsense/wiki/API-How-To
 * https://software.intel.com/en-us/articles/robotics-development-kit-r200-depth-data-interpretation
 * https://www.mouser.com/pdfdocs/intel_realsense_camera_sr300.pdf
 *
 *
 */

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <librealsense2/rs.hpp>
#include <iostream>
#include "include/stb_image_write.h"
#include "include/com_unigent_realsensej_RealSenseJ.h"

rs2::pipeline *pipeline;

using namespace std;

void image_to_jbyte_array_writer(void* context, void *data, int len) {

    if(len <= 0) {
        return;
    }

    auto v = (vector<jbyte>*) context;
    auto d = (unsigned char*) data;

    auto dataChunk = new vector<jbyte>();
    dataChunk->assign(d, d + len);
    v->reserve(v->size() + len);
    v->insert(v->end(), dataChunk->begin(),dataChunk->end());
    delete dataChunk;

    // std::cout << "[RSJ] Assigned=" << len << " elements to BMP vector. Vector size:" << v->size() << std::endl;
}

vector<jbyte>* frame_to_png(const rs2::video_frame &frame) {
    auto png_data = new vector<jbyte>();
    //std::cout << "[RSJ] frame_to_png#  " << frame.get_width() << " x " << frame.get_height() << std::endl;

    //vf.get_width(), vf.get_height(), vf.get_bytes_per_pixel(), vf.get_data(), vf.get_stride_in_bytes()

    stbi_write_png_to_func(
            image_to_jbyte_array_writer,
            png_data,
            frame.get_width(),
            frame.get_height(),
            frame.get_bytes_per_pixel(),
            frame.get_data(),
            frame.get_stride_in_bytes()
    );
    //std::cout << "[RSJ] frame_to_png#  png_data.size=" << png_data->size() << std::endl;
    return png_data;
}

rs2::frameset read_frameset() {
    rs2_error *err;
    for(int i=0; i<500; i++) {
        //std::cout << "[RSJ] Try " << i << std::endl;
        try {
            rs2::frameset frames = pipeline->wait_for_frames(1000);
            //std::cout << "[RSJ] Got " << frames.size() << " frames" << std::endl;
            return frames;
        }
        catch (const rs2::error & e) {
            std::cout << "[RSJ] Try " << i << "RealSense error calling " << e.get_failed_function() << "(" << e.get_failed_args() << "):\n    " << e.what() << std::endl;
        }
    }
    throw std::logic_error("Unable to read frames after 500 tries!");
}

JNIEXPORT void JNICALL Java_com_unigent_realsensej_RealSenseJ_openPipe
        (JNIEnv *, jobject ) {

    rs2::log_to_console(RS2_LOG_SEVERITY_INFO);

    std::cout << "[RSJ] Starting RS2 pipeline (rgb align)..." << std::endl;

    rs2::config pipeline_config;
    pipeline_config.enable_stream(rs2_stream::RS2_STREAM_COLOR, -1, 640, 480, rs2_format::RS2_FORMAT_RGB8, 30);
    pipeline_config.enable_stream(rs2_stream::RS2_STREAM_DEPTH, -1, 640, 480, rs2_format::RS2_FORMAT_Z16, 30);

    pipeline = new rs2::pipeline;
    auto pipeline_profile = pipeline->start(pipeline_config);
    std::cout << "[RSJ] Started" << std::endl;

    // Check streams
    auto depth_profile = pipeline_profile.get_stream(rs2_stream::RS2_STREAM_DEPTH);
    std::cout << "[RSJ] Depth stream format: #" << depth_profile.stream_index() << ": " << depth_profile.format() << " at " << depth_profile.fps() << " fps" << std::endl;
    auto color_profile = pipeline_profile.get_stream(rs2_stream::RS2_STREAM_COLOR);
    std::cout << "[RSJ] Color stream format: #" << color_profile.stream_index() << ": " << color_profile.format() << " at " << color_profile.fps() << " fps" << std::endl;

    // test frame!
    rs2::frameset frame_data = read_frameset();
    rs2::depth_frame depth_frame = frame_data.get_depth_frame();
    std::cout << "[RSJ] Depth frame size: " << depth_frame.get_width() << "x" << depth_frame.get_height() << std::endl;

    rs2::video_frame color_frame = frame_data.get_color_frame();
    std::cout << "[RSJ] Color frame size: " << color_frame.get_width() << "x" << color_frame.get_height() << std::endl;
}

JNIEXPORT void JNICALL Java_com_unigent_realsensej_RealSenseJ_closePipe
        (JNIEnv *, jobject ) {
    std::cout << "[RSJ] Stopping RS2 pipeline..." << std::endl;
    pipeline->stop();
    delete pipeline;
}

JNIEXPORT jobjectArray JNICALL Java_com_unigent_realsensej_RealSenseJ_getFrames
        (JNIEnv *env, jobject) {

    rs2::frameset frame_data = read_frameset();
    rs2::colorizer colorizer;
    jclass arrayElemType = env->FindClass("[B");
    jobjectArray result = env->NewObjectArray(3, arrayElemType, nullptr);


    // Align frames to Depth pixels match RGB
    rs2::align align(RS2_STREAM_COLOR);
    auto aligned_frames = align.process(frame_data);
    rs2::video_frame color_frame = aligned_frames.first(RS2_STREAM_COLOR);
    rs2::depth_frame aligned_depth_frame = aligned_frames.get_depth_frame();

    // 1. RGB
    vector<jbyte>* data_rgb = frame_to_png(color_frame);
    auto length_rgb = (int) data_rgb->size();
    jbyteArray rgbArray = env->NewByteArray(length_rgb);
    env->SetByteArrayRegion(rgbArray, 0, length_rgb, data_rgb->data());
    delete data_rgb;

    // 2. Depth

    // 2.1 Depth raw (Z16)
    int depth_frame_width = aligned_depth_frame.get_width();
    int depth_frame_height = aligned_depth_frame.get_height();
    int num_of_depth_pixels = depth_frame_width * depth_frame_height;
    auto * distanceImage = new jbyte[num_of_depth_pixels * 2];

    int idx = 0;
    for(int r=0; r<aligned_depth_frame.get_height(); r++) {
        for(int c=0; c<aligned_depth_frame.get_width(); c++) {
            auto depth_mm = (unsigned short int) (aligned_depth_frame.get_distance(c, r) * 1000);
            distanceImage[idx] = (char) (depth_mm >> 8);
            distanceImage[idx + 1] = (char) (depth_mm & 0xFF);
            idx += 2;
        }
    }
    int numOfDepthBytes = num_of_depth_pixels * 2;
    jbyteArray depthDataArray = env->NewByteArray(numOfDepthBytes);
    env->SetByteArrayRegion(depthDataArray, 0, numOfDepthBytes, distanceImage);
    delete distanceImage;

    // 2.1 Depth colorized
    rs2::video_frame colorizedDepthFrame = colorizer.colorize(aligned_depth_frame);
    vector<jbyte>* data_depth_colorized = frame_to_png(colorizedDepthFrame);
    auto length_depth_colorized = (int) data_depth_colorized->size();
    jbyteArray depthColorizedArray = env->NewByteArray(length_depth_colorized);
    env->SetByteArrayRegion(depthColorizedArray, 0, length_depth_colorized, data_depth_colorized->data());
    delete data_depth_colorized;

    env->SetObjectArrayElement(result, 0, rgbArray);
    env->SetObjectArrayElement(result, 1, depthColorizedArray);
    env->SetObjectArrayElement(result, 2, depthDataArray);

    return result;
}



