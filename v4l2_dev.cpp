#include "v4l2_dev.h"

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
v4l2_dev::v4l2_dev(string dev_name)
{
    devname = dev_name;
}

// error handler
void v4l2_dev::errno_exit(string err_str)
{
    cerr << "Error: " << err_str << ": " << errno << ", " << strerror(errno) << endl;
    exit(EXIT_FAILURE);
}

// ioctl request handler
int v4l2_dev::xioctl(int df, int request, void *arg)
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

void v4l2_dev::openDevice()
{
    devfile = open(devname.c_str(), O_RDWR /* required */ | O_NONBLOCK, 0);

    if (devfile == -1)
    {
        stringstream errstr;
        errstr << "Error: Cannot open '" << devname << "'";
        errno_exit(errstr.str());
    }

    getDeviceInfo();
}

void v4l2_dev::closeDevice()
{
    if (close(devfile) == -1)
    {
        stringstream errstr;
        errstr << "Error: Cannot close '" << devname << "'";
        errno_exit(errstr.str());
    }

    devfile = -1;
//    cerr << "Close device " <<  devname << endl;
}

// get device format information
void v4l2_dev::getDeviceInfo() {
    struct v4l2_format format_params;
    format_params.type =  V4L2_BUF_TYPE_CAPTURE;
    xioctl(devfile, VIDIOC_G_FMT, &format_params);

    // only YUYV pixelformat is allowed
    if (format_params.fmt.pix.pixelformat !=  V4L2_PIX_FMT_YUYV)
    {
        stringstream errstr;
        errstr << "Fatal Error: Invalid Pixel Format!";
        errno_exit(errstr.str());
    }

    framewidth = format_params.fmt.pix.width;
    frameheight = format_params.fmt.pix.height;
}

