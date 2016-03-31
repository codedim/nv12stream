#include "main.h"
#include "v4l2_dev.h"
#include "v4l2_frame.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


int main(int argc,char* argv[])
{
    string dev_name = "/dev/video0"; // default dev name
    int fps;

    // parsing cli arguments
    if (argc == 4) dev_name = argv[3];
    if (argc > 2 && 
        (argv[1][0] == '-' && argv[1][1] == 'f'))
    {
        fps = atoi(argv[2]);
    } 
    else 
    {
        cerr << HELP << endl;
        exit(EXIT_FAILURE);
    }

    // open the v4l2 device
    v4l2_dev *video_dev = new v4l2_dev(dev_name);
    video_dev->openDevice();
    cerr << "capturing frames from '" << dev_name << "' with " << 
        video_dev->framewidth << "x" << video_dev->frameheight << 
        " " << fps << "fps" << endl;

    void *yuyv_frame; // 'yuv' frame from the v4l2 device
    void *nv12_frame; // transcoded 'nv12' pixelfomat frame

    // init frame buffers
    v4l2_frame *fr = new v4l2_frame(video_dev->devfile, 
        video_dev->framewidth, video_dev->frameheight);
    fr->initFrameBuffers();
    fr->startCapturing();

    // start capturing
    while (1)
    {
        yuyv_frame = fr->getFrame();
        nv12_frame = fr->getNV12Frame((dev_buffer *) yuyv_frame);
        writeToStdout((raw_frame *) nv12_frame);
    }

    fr->stopCapturing();
    fr->freeFrameBuffers();
    cerr << frame_nmbr << " frames was cuptured" << endl;
    video_dev->closeDevice();

    return EXIT_SUCCESS;
}


// appends frame data to the specified file
void saveToFile(string file_name, raw_frame * frame)
{
    FILE *out_file = fopen(file_name.c_str(), "a");
    fwrite(frame->start, frame->length, 1, out_file);
    fclose(out_file);

    cerr << "write frame(" << ++frame_nmbr << 
        ") to file: " << file_name << endl;
}

// writes frame data to the standard output
void writeToStdout(raw_frame * frame)
{
    fwrite(frame->start, frame->length, 1, stdout);
    cerr << "write frame(" << ++frame_nmbr << 
        ") to stdout" << endl;
}
