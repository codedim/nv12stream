#ifndef V4L2_DEV_H
#define V4L2_DEV_H

#include <iostream>

using namespace std;

class v4l2_dev
{
public:
    v4l2_dev(string dev_name);
    void openDevice();
    void closeDevice();
    void getDeviceInfo();

    int devfile;
    int framewidth;
    int frameheight;

private:
    void errno_exit(string err_str);
    int xioctl(int fd, int request, void *arg);
    string devname;
};

#endif // V4L2_DEV_H
