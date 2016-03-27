#ifndef V4L2_FRAME_H
#define V4L2_FRAME_H

#include <iostream>

using namespace std;

struct dev_buffer {
    void   *start;
    size_t  length;
};

class v4l2_frame
{
public:
    v4l2_frame(int dfile, int fwidth, int fheight);
    void initFrame();
    void freeFrame();

    // pointer to the dev_buffer (packed YUV 4:2:2)
    void* getFrame();
    // pointer to the dev_buffer (planed YUV 4:2:0)
    void* getNV12Frame(dev_buffer *db); 

private:
    void errno_exit(string err_str);
    int xioctl(int fd, int request, void *arg);
    void sleep_ms(unsigned long ms);
//    void initMMAP();
    void startCapturing();
    int waitFrame();
    void stopCapturing();
//    void freeMMAP();

    int devfile; // dev file
    struct dev_buffer *devbuf;
    size_t devbuf_size;

    int framewidth;
    int frameheight;

    struct dev_buffer *nv12buf;
    void *nv12frame;
};

#endif // V4L2_FRAME_H
