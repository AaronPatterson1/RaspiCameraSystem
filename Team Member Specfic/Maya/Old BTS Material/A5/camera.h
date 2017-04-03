#ifndef CAMERA_H
#define CAMERA_H

#include<iostream>

class Camera{
    bool landFly = true; //Has to be flying
    bool com_buffer; //No command executing
    int     vid_quality; //Video quality option
      int   asp_ratio; //Aspect ratio option
       int  filter; //filter option


public:
    void options();
    void display();
    void record();




};



#endif // CAMERA_H
