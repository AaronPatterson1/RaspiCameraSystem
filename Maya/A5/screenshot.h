#ifndef SCREENSHOT_H
#define SCREENSHOT_H

#include<iostream>

class Screenshot{
    bool flyLand = true; //has to be flying to be true
    bool com_buffer; //is command executing
       int  pic_quality; //screenshot quality
     int    format; //file format option
    bool clipboard;//copy to clipboard?

public:

    void display();
    void screenshot();

};




#endif // SCREENSHOT_H
