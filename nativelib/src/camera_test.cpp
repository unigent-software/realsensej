#include <iostream>

//
// AgentBase Robot Platform, Unigent Robotics, <a href="http://unigent.com">Unigent</a>
// @author terranfounder
//

#include <iostream>
#include <librealsense2/rs.hpp>
#include <thread>

int main(int argc, char * argv[]) try {

    rs2::log_to_console(RS2_LOG_SEVERITY_WARN);

    auto pipeline = new rs2::pipeline();
    pipeline->start();


//    pipeline->get_active_profile().get_device().hardware_reset();
//    for(std::vector<rs2::sensor>::iterator i = sensors.begin(); i != sensors.end(); ++i) {
//        std::cout << "Sensor" << (*i) << std::endl;
//    }


    int total_tries = 100;
    int success_cnt = 0;
    for(int i=0; i<total_tries; i++) {
        std::cout << "Try " << i << std::endl;
        try {
            rs2::frameset frames = pipeline->wait_for_frames(200);
            std::cout << "Got frames:" << frames.size() << std::endl;

            rs2::depth_frame depth_frame = frames.get_depth_frame();
            std::cout << "Depth frame: h=" << depth_frame.get_height() << " w=" << depth_frame.get_width() << std::endl;

            rs2::video_frame rgb_frame = frames.get_color_frame();
            std::cout << "RGB frame: h=" << rgb_frame.get_height() << " w=" << rgb_frame.get_width() << std::endl;


            // Aligned frames
            rs2::align align(RS2_STREAM_COLOR);
            auto aligned_frames = align.process(frames);
            rs2::video_frame aligned_color_frame = aligned_frames.first(RS2_STREAM_COLOR);
            rs2::depth_frame aligned_depth_frame = aligned_frames.get_depth_frame();
            std::cout << "Aligned (meters): 236,116: " << aligned_depth_frame.get_distance(236, 116) << std::endl;

            success_cnt ++;

            std::this_thread::sleep_for( std::chrono::milliseconds(200));
        }
        catch (const rs2::error & e)
        {
            std::cerr << "RealSense error calling " << e.get_failed_function() << "(" << e.get_failed_args() << "):\n    " << e.what() << std::endl;
        }
    }

    std::cout << "Success count: " << success_cnt << " of " << total_tries << std::endl;

//    rs2::frameset* frames;
//
//    bool hasData = false;
//    for(int i=0; i<500; i++) {
//        std::cout << "Try #" << i << std::endl;
//        hasData = pipeline->poll_for_frames(frames);
//        if(hasData) {
//            std::cout << "Got frames!!!!" << std::endl;
//            break;
//        }
//
//        std::this_thread::sleep_for( std::chrono::milliseconds(500));
//    }

//    if(!hasData) {
//        std::cout << "NO FRAMES :(" << std::endl;
//    }

    pipeline->stop();
    delete pipeline;

    return EXIT_SUCCESS;
}
catch (const rs2::error & e)
{
    std::cerr << "RealSense error calling " << e.get_failed_function() << "(" << e.get_failed_args() << "):\n    " << e.what() << std::endl;
    return EXIT_FAILURE;
}
catch (const std::exception& e)
{
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
}


