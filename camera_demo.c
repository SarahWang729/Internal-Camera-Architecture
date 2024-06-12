#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
int main()
{
      //1.open the device
      int fd = open("/dev/video0",O_RDWR);
      if(fd < 0)
      {
            perror("fail to open");
            return -1;
      }
      //2.get the supported format
      struct v4l2_fmtdesc v4fmt;
      int ret = ioctl(fd, VIDIOC_ENUM_FMT, &v4fmt);
      if(ret < 0)
      {
            perror("fail to get format");
      }
      //3.set the format
      struct v4l2_format vfmt;
      vfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; //camera capture
      vfmt.fmt.pix.width = 1280;
      vfmt.fmt.pix.height = 720;
      vfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG; //set the captured format
      int retur = ioctl(fd, VIDIOC_S_FMT, &vfmt);
      if(retur < 0)
      {
            perror("fail to set format");
      }
      //4.request the buffer from kernel
      struct v4l2_requestbuffers req;
      req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      req.count = 4; //at most 4 buffers
      req.memory = V4L2_MEMORY_MMAP; //assign the memory map method
      retur = ioctl(fd, VIDIOC_REQBUFS, &req);
      if(retur < 0)
      {
            perror("fail to request buffer");
      }
      //5.map the buffer to user space
      unsigned char *mpointer[4]; //save the addr in user space after mapping
      unsigned int size[4];
      struct v4l2_buffer buf;
      buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      for(int i=0; i<4; i++)
      {
            buf.index = i;
            retur = ioctl(fd, VIDIOC_QUERYBUF, &buf); //get a buffer from kernal space and ready for mapping
            if(retur < 0)
            {
                  perror("fail to query");
            }
            mpointer[i] = (unsigned char *)mmap(NULL, buf.length, PROT_READ|PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
            size[i] = buf.length;
            //put buffer back to kernel space after finishing
            retur = ioctl(fd, VIDIOC_QBUF, &buf);
            if(retur < 0)
            {
                  perror("fail to put back");
            }
      }
      //6.start streaming
      int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      retur = ioctl(fd, VIDIOC_STREAMON, &type);
      if(retur < 0)
      {
            perror("fail to stream");
      }
      //get data from queueing buffer
      struct v4l2_buffer readbuf;
      readbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      retur = ioctl(fd, VIDIOC_DQBUF, &readbuf);
      if(retur < 0)
      {
            perror("fail to get data");
      }
      FILE *file=fopen("test.jpg", "w+");
      fwrite(mpointer[readbuf.index], readbuf.length, 1, file);
      fclose(file);
      //notify kernel
      retur = ioctl(fd, VIDIOC_QBUF, &readbuf);
      if(retur < 0)
      {
            perror("fail to put buffer back to kernel space");
      }
      //8.stop streaming
      retur = ioctl(fd, VIDIOC_STREAMOFF, &type);
      //9.release mapping space
      for(int i=0; i<4; i)
      {
      munmap(mpointer[i], size[i]);
      }
      //10.close the device
      close(fd);
      return 0;
}