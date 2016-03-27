#include <iostream>
#include "v4l2_dev.h"
#include "v4l2_frame.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

using namespace std;

struct raw_frame
{
    void *start;
    size_t length;
};

int fnmb = 0;

void saveToFile(string file_name, raw_frame * frame);
void writeToStdout(raw_frame * frame);

string help = "nv12stream - is a v4l2 video device 'yuyv' frame \
capture\n  and transcoding to 'nv12' pixelformat tool.\nUsage: \
\n  nv12stream -f <dev_fps_value> [<dev_file_name>]\nwhere\n \
 <dev_fps_value> - integer fps value, supported by the device \
\n   with the current resolution (use 'v4l2-ctl --list-formats-ext' \
\n   command to make it clear);\n  <dev_file_name> - alternative \
v4l2 device name ('/dev/video0'\n   is default value).\n";


int main(int argc,char* argv[])
{
    string dev_name = "/dev/video0"; // default dev name
    int fps;
    void *yuyv_frame;
    void *nv12_frame;

    // parsing cli arguments
    if (argc == 4) dev_name = argv[3];
    if (argc > 2 && 
        (argv[1][0] == '-' && argv[1][1] == 'f'))
    {
        fps = atoi(argv[2]);
    } 
    else 
    {
        cerr << help << endl;
        exit(EXIT_FAILURE);
    }

    // open the v4l2 device
    v4l2_dev *vd = new v4l2_dev(dev_name);
    vd->openDevice();
    cerr << "starting capture frames from '" << dev_name <<
        "' with " << vd->framewidth << "x" << vd->frameheight <<
        " " << fps << "fps" << endl;

    // init frame buffer
    v4l2_frame *fr = new v4l2_frame(vd->devfile, 
        vd->framewidth, vd->frameheight);
    fr->initFrame();

    // start capturing
    while (1) { 
//        snprintf(file_name, sizeof(file_name), "%d", ++frame_nmbr);
//        strcat(file_name, file_ext.c_str());

        yuyv_frame = fr->getFrame();
        nv12_frame = fr->getNV12Frame((dev_buffer *) yuyv_frame);
        writeToStdout((raw_frame *) nv12_frame);

//        cerr << "Frame captured" << endl;
//        saveToFile(string(file_name, 10), (raw_frame *)vd->getNV12Frame());
//        saveToFile(file_ext, (raw_frame *)nv12_frame);
//        saveToFile(file_ext, (raw_frame *)vd->getNV12Frame());
    }

    fr->freeFrame();
    vd->closeDevice();
    return 0;
}


// appends frame data to the specified file
void saveToFile(string file_name, raw_frame * frame)
{
    FILE *out_file = fopen(file_name.c_str(), "a");
    fwrite(frame->start, frame->length, 1, out_file);
    fclose(out_file);

    cerr << "write frame(" << ++fnmb << ") to file: " << file_name << endl;
}

// writes frame data to the standard output
void writeToStdout(raw_frame * frame)
{
    fwrite(frame->start, frame->length, 1, stdout);
    cerr << "write frame(" << ++fnmb << ") to stdout" << endl;
}
