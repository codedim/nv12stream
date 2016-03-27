# nv12stream

**nv12stream** is a v4l2 video device 'yuyv' frame capture 
and transcoding to 'nv12' pixelformat tool.

## Build

```
git clone '?'  
cd nv12stream  
mkdir build  
cd build  
cmake ..  
make
```


## Usage: 

```
nv12stream -f <dev_fps_value> [<dev_file_name>]
```

where

* `<dev_fps_value>` - integer fps value, supported by the device 
with the current resolution (use 'v4l2-ctl --list-formats-ext 
command to make it clear);  
* `<dev_file_name>` - alternative v4l2 device name ('/dev/video0' 
is default value).

example

```
v4l2-ctl -v pixelformat='YUYV'  
nv12stream -f 10 >> nv12video.raw
```

## Cleanup

```
cd ..  
rm -rf build
```
