#include "v4l2_frame.h"

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sstream>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include <linux/videodev2.h>
#ifndef V4L2_BUF_TYPE_CAPTURE
#define V4L2_BUF_TYPE_CAPTURE V4L2_BUF_TYPE_VIDEO_CAPTURE
#endif

// constructor
v4l2_frame::v4l2_frame(int dfile, int fwidth, int fheight)
{
    devfile = dfile;
    framewidth = fwidth;
    frameheight = fheight;
    devbuf_size = 0;
}

// error handler
void v4l2_frame::errno_exit(string err_str)
{
    cerr << "Error: " << err_str << ": " << errno << ", " << strerror(errno) << endl;
    exit(EXIT_FAILURE);
}

// ioctl request handler
int v4l2_frame::xioctl(int df, int request, void *arg)
{
       int res = ioctl(df, request, arg);
       if(res == -1)
       {
           if (errno == EAGAIN) return EAGAIN;

           stringstream errstr;
           errstr << "IoCtl Code: " << request << " ";
           errno_exit(errstr.str());
       }

       return res;
}

// retrievs frame from the device
void* v4l2_frame::getFrame()
{
    startCapturing();

    long int i = 0;
    while(!waitFrame())
    {
        sleep_ms(10); // let's the other jobs to execute
        i++;
    }
    cerr << "debug iter == " << i << endl; // for debug

    stopCapturing();
    return (void *)devbuf;
}

// transcode device YUV4:2:2 packed frame to YUV4:2:0 planar frame
void* v4l2_frame::getNV12Frame(dev_buffer *db)
{
    if (!devbuf_size) 
    {   // this is the first time we are getting a Frame 
        // from the device
        devbuf_size = db->length;

        // allocate memory for nv12 pixelformat buffer structure
        nv12buf = (dev_buffer*) calloc(1, sizeof(dev_buffer));
        nv12buf->length = devbuf_size / 4 * 3; // 16 to 12 bits per pixel

        // allocate memory for nv12 pixelformat frame data
        nv12buf->start = (void *) calloc(nv12buf->length, sizeof(char));
    }

    unsigned int *src_macropixel = (unsigned int *)db->start;
    char *dst_y = (char *) nv12buf->start;
    char *dst_uv = (char *) nv12buf->start + (devbuf_size / 2);
    int avrg_u, avrg_v; // average values of chroma samples (U and V)

    // transcoding frame, line by line
    for (int col = 0; col < frameheight; ++col)
    {
        for (int line = 0; line < framewidth / 2; ++line) 
        {
            // Y plane 
            // 'src_macropix' is 32bit value with reversed bytes
            *dst_y++ = (char)(*src_macropixel & 0xFF);
            *dst_y++ = (char)(*src_macropixel >> 16 & 0xFF);

            if (!(line % 2)) 
            { // even line
                // nv12 chroma plane
                *dst_uv++ = (char)(*src_macropixel >> 8 & 0xFF);
                *dst_uv++ = (char)(*src_macropixel >> 24);
            }
            else
            { // odd line
                // TODO: the U & V values must be averaged
            }

            ++src_macropixel;
        }
    }

    return (void *) nv12buf;
}

int v4l2_frame::waitFrame()
{
    struct v4l2_buffer buf;
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;

    if (xioctl(devfile, VIDIOC_DQBUF, &buf) == EAGAIN)
            return 0;

    return 1;
}

/////////////////////////////////////////////////////////////////////////////

void v4l2_frame::startCapturing()
{
    struct v4l2_buffer buf;
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = 0;
    xioctl(devfile, VIDIOC_QBUF, &buf);

    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    xioctl(devfile, VIDIOC_STREAMON, &type);
}

void v4l2_frame::stopCapturing()
{
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    xioctl(devfile, VIDIOC_STREAMOFF, &type);
}

void v4l2_frame::initFrame()
{
    struct v4l2_requestbuffers req;
    req.count = 1;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    xioctl(devfile, VIDIOC_REQBUFS, &req);

    devbuf = (dev_buffer*) calloc(req.count, sizeof(dev_buffer));

    struct v4l2_buffer buf;
    buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.index       = 0;
    xioctl(devfile, VIDIOC_QUERYBUF, &buf);

    devbuf->length = buf.length;
    devbuf->start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, 
        MAP_SHARED, devfile, buf.m.offset);

    if (devbuf->start == MAP_FAILED)
        errno_exit("Fatal Error: Init Frame fault!");
}

void v4l2_frame::freeFrame()
{
    if (munmap(devbuf->start, devbuf->length) == -1)
        errno_exit("Fatal Error: Free Frame fault!");

    free(devbuf);
}

// sleep(ms) function
void v4l2_frame::sleep_ms(unsigned long ms)
{
    struct timespec req = {0};
    req.tv_sec = (int)(ms/1000);
    req.tv_nsec = (ms - (req.tv_sec * 1000)) * 1000000L;
    while(nanosleep(&req, &req) == -1)
        continue;
    return;
}
