#include <iostream>

using namespace std;

//#define BUFCOUNT 2

#define HELP "nv12stream - is a v4l2 video device 'yuyv' frame \
capture\n  and transcoding to 'nv12' pixelformat tool.\nUsage: \
\n  nv12stream -f <dev_fps_value> [<dev_file_name>]\nwhere\n \
 <dev_fps_value> - integer fps value, supported by the device \
\n   with the current resolution (use 'v4l2-ctl --list-formats-ext' \
\n   command to make it clear);\n  <dev_file_name> - alternative \
v4l2 device name ('/dev/video0'\n   is default value).\n"

struct raw_frame
{
    void *start;
    size_t length;
};

unsigned int frame_nmbr = 0;

void saveToFile(string file_name, raw_frame * frame);
void writeToStdout(raw_frame * frame);
