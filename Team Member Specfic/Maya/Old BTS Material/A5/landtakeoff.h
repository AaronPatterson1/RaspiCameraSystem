#ifndef LANDTAKEOFF_H
#define LANDTAKEOFF_H

#include<iostream>

class LandTakeoff{
    bool flyLand = false; //Indicates status of drone, true if flying, false otherwise
    bool com_buffer; //If command buffer is true, drone is busy executing another command

public:
    bool status();
    void display();
};


#endif // LANDTAKEOFF_H
