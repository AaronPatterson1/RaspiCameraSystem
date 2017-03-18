#!/usr/bin/python

# ========================================================================
# $File: vid_files.py $
# $Date: March 18 2017 $
# $Revision: v1.0 $
# $Creator: Marko Radmanovic $
# ========================================================================

import os
import glob

names = [os.path.basename(x) for x in glob.glob("/home/pi/Desktop/BTS/Vids/*.mp4")]
print names

