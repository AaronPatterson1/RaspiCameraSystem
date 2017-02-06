#include "landtakeoff.h"

bool LandTakeoff::status(){
    if (flyLand == true){ //drone is flying
        //Change dialog to Land (maybe seperate argument?
        //if button pressed
        //slowly lower
        //receive accepted packet
        flyLand = false;
        return true;
    }else{//drone is landed
        //change dialog to Fly
        //if button pressed
        //start propellors and if lifted
        // receive packet
        flyLand = true;
        return true;

    }
    return false;
}

void LandTakeoff::display(){
    //display status and buttons

}
