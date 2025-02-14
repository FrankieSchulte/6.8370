/* --------------------------------------------------------------------------
 * File:    demosaic.cpp
 * Created: 2015-10-01
 * --------------------------------------------------------------------------
 *
 *
 *
 * ------------------------------------------------------------------------*/

#include "demosaic.h"
#include <cmath>

using namespace std;

Image basicGreen(const Image &raw, int offset) {
  // --------- HANDOUT  PS03 ------------------------------
  // Takes as input a raw image and returns a single-channel
  // 2D image corresponding to the green channel using simple interpolation
  Image greenRaw = Image(raw.width(), raw.height(), 1);
  for (int x = 1; x < raw.width() - 1; x++) {
    for (int y = 1; y < raw.height() - 1; y++) {
      if (((x + offset) % 2 == 1) && ((y + offset) % 2 == 0)) {
        greenRaw(x, y, 0) = raw(x, y, 0);
      }
      else if (((x + offset) % 2 == 0) && ((y + offset) % 2 == 1)) {
        greenRaw(x, y, 0) = raw(x, y, 0);
      }
      else {
        float leftValue = raw(x - 1, y, 0);
        float rightValue = raw(x + 1, y, 0);
        float upValue = raw(x, y - 1, 0);
        float downValue = raw(x, y + 1, 0);
        float average = (leftValue + rightValue + upValue + downValue) / 4.0;
        greenRaw(x, y, 0) = average;
      }
    }
  }

  // Edge values
  for (int x = 0; x < raw.width(); x++) {
    greenRaw(x, 0, 0) = raw(x, 0, 0);
    greenRaw(x, raw.height() - 1, 0) = raw(x, raw.height() - 1, 0);
  }
  for (int y = 0; y < raw.height(); y++) {
    greenRaw(0, y, 0) = raw(0, y, 0);
    greenRaw(raw.width() - 1, y, 0) = raw(raw.width() - 1, y, 0);
  }
  
  return greenRaw;
}

Image basicRorB(const Image &raw, int offsetX, int offsetY) {
  // --------- HANDOUT  PS03 ------------------------------
  //  Takes as input a raw image and returns a single-channel
  //  2D image corresponding to the red or blue channel using simple
  //  interpolation
  Image newRaw = Image(raw.width(), raw.height(), 1);
  for (int x = 0; x < raw.width(); x++) {
    for (int y = 0; y < raw.height(); y++) {
      newRaw(x, y, 0) = raw(x, y, 0);
    }
  }

  for (int x = 2; x < raw.width() - 2; x++) {
    for (int y = 2; y < raw.height() - 2; y++) {
      // Side to side pixels (odd, even)
      if (((x + offsetX) % 2 == 0) && ((y + offsetY) % 2 == 1)){
        float leftValue = raw(x - 1, y, 0);
        float rightValue = raw(x + 1, y, 0);
        float average = (leftValue + rightValue) / 2.0;
        newRaw(x, y, 0) = average;
      }
      // Up and down pixels (even, odd)
      else if (((x + offsetX) % 2 == 1) && ((y + offsetY) % 2 == 0)){
        float upValue = raw(x, y - 1, 0);
        float downValue = raw(x, y + 1, 0);
        float average = (upValue + downValue) / 2.0;
        newRaw(x, y, 0) = average;
      }
      // Diagonal pixels (odd, odd)
      else if (((x + offsetX) % 2 == 0) && ((y + offsetY) % 2 == 0)){
        float leftUpValue = raw(x - 1, y - 1, 0);
        float rightUpValue = raw(x + 1, y - 1, 0);
        float leftDownValue = raw(x - 1, y + 1, 0);
        float rightDownValue = raw(x + 1, y + 1, 0);
        float average = (leftUpValue + rightUpValue + leftDownValue + rightDownValue) / 4.0;
        newRaw(x, y, 0) = average;
      }
    }
  }
  return newRaw;
}

Image basicDemosaic(const Image &raw, int offsetGreen, int offsetRedX,
                    int offsetRedY, int offsetBlueX, int offsetBlueY) {
  // --------- HANDOUT  PS03 ------------------------------
  // takes as input a raw image and returns an rgb image
  // using simple interpolation to demosaic each of the channels
  Image newImage = Image(raw.width(), raw.height(), raw.channels());
  Image red = basicRorB(raw, offsetRedX, offsetRedY);
  Image green = basicGreen(raw, offsetGreen);
  Image blue = basicRorB(raw, offsetBlueX, offsetBlueY);
  for (int x = 0; x < raw.width(); x++) {
    for (int y = 0; y < raw.height(); y++) {
      newImage(x, y, 2) = red(x, y, 0);
      newImage(x, y, 1) = green(x, y, 0);
      newImage(x, y, 0) = blue(x, y, 0);
    }
  }
  return newImage;
}

Image edgeBasedGreen(const Image &raw, int offset) {
  // --------- HANDOUT  PS03 ------------------------------
  // Takes a raw image and outputs a single-channel
  // image corresponding to the green channel taking into account edges
  Image greenRaw = Image(raw.width(), raw.height(), 1);
  for (int x = 1; x < raw.width() - 1; x++) {
    for (int y = 1; y < raw.height() - 1; y++) {
      if (((x + offset) % 2 == 1) && ((y + offset) % 2 == 0)) {
        greenRaw(x, y, 0) = raw(x, y, 0);
      }
      else if (((x + offset) % 2 == 0) && ((y + offset) % 2 == 1)) {
        greenRaw(x, y, 0) = raw(x, y, 0);
      }
      else {
        float leftValue = raw(x - 1, y, 0);
        float rightValue = raw(x + 1, y, 0);
        float upValue = raw(x, y - 1, 0);
        float downValue = raw(x, y + 1, 0);
        
        float horizDiff = abs(leftValue - rightValue);
        float vertDiff = abs(upValue - downValue);
        if (horizDiff <= vertDiff) {
          greenRaw(x, y, 0) = (leftValue + rightValue) / 2.0;
        }
        else if (horizDiff > vertDiff) {
          greenRaw(x, y, 0) = (upValue + downValue) / 2.0;
        }
      }
    }
  }

  // Edge values
  for (int x = 0; x < raw.width(); x++) {
    greenRaw(x, 0, 0) = raw(x, 0, 0);
    greenRaw(x, raw.height() - 1, 0) = raw(x, raw.height() - 1, 0);
  }
  for (int y = 0; y < raw.height(); y++) {
    greenRaw(0, y, 0) = raw(0, y, 0);
    greenRaw(raw.width() - 1, y, 0) = raw(raw.width() - 1, y, 0);
  }
  
  return greenRaw;
}

Image edgeBasedGreenDemosaic(const Image &raw, int offsetGreen, int offsetRedX,
                             int offsetRedY, int offsetBlueX, int offsetBlueY) {
  // --------- HANDOUT  PS03 ------------------------------
  // Takes as input a raw image and returns an rgb image
  // using edge-based green demosaicing for the green channel and
  // simple interpolation to demosaic the red and blue channels
  Image newImage = Image(raw.width(), raw.height(), raw.channels());
  Image red = basicRorB(raw, offsetRedX, offsetRedY);
  Image edgeGreen = edgeBasedGreen(raw, offsetGreen);
  Image blue = basicRorB(raw, offsetBlueX, offsetBlueY);
  for (int x = 0; x < raw.width(); x++) {
    for (int y = 0; y < raw.height(); y++) {
      newImage(x, y, 2) = red(x, y, 0);
      newImage(x, y, 1) = edgeGreen(x, y, 0);
      newImage(x, y, 0) = blue(x, y, 0);
    }
  }
  return newImage;
}

Image greenBasedRorB(const Image &raw, Image &green, int offsetX, int offsetY) {
  // --------- HANDOUT  PS03 ------------------------------
  // Takes as input a raw image and returns a single-channel
  // 2D image corresponding to the red or blue channel using green based
  // interpolation
  Image newRaw = Image(raw.width(), raw.height(), 1);
  for (int x = 0; x < raw.width(); x++) {
    for (int y = 0; y < raw.height(); y++) {
      newRaw(x, y, 0) = raw(x, y, 0);
    }
  }

  for (int x = 2; x < raw.width() - 2; x++) {
    for (int y = 2; y < raw.height() - 2; y++) {
      // Side to side pixels (odd, even)
      if (((x + offsetX) % 2 == 0) && ((y + offsetY) % 2 == 1)){
        float leftValue = raw(x - 1, y, 0) - green(x - 1, y, 0);
        float rightValue = raw(x + 1, y, 0) - green(x + 1, y, 0);
        float average = (leftValue + rightValue) / 2.0;
        newRaw(x, y, 0) = average + green(x, y, 0);
      }
      // Up and down pixels (even, odd)
      else if (((x + offsetX) % 2 == 1) && ((y + offsetY) % 2 == 0)){
        float upValue = raw(x, y - 1, 0) - green(x, y - 1, 0);
        float downValue = raw(x, y + 1, 0) - green(x, y + 1, 0);
        float average = (upValue + downValue) / 2.0;
        newRaw(x, y, 0) = average + green(x, y, 0);
      }
      // Diagonal pixels (odd, odd)
      else if (((x + offsetX) % 2 == 0) && ((y + offsetY) % 2 == 0)){
        float leftUpValue = raw(x - 1, y - 1, 0) - green(x - 1, y - 1, 0);
        float rightUpValue = raw(x + 1, y - 1, 0) - green(x + 1, y - 1, 0);
        float leftDownValue = raw(x - 1, y + 1, 0) - green(x - 1, y + 1, 0);
        float rightDownValue = raw(x + 1, y + 1, 0) - green(x + 1, y + 1, 0);
        float average = (leftUpValue + rightUpValue + leftDownValue + rightDownValue) / 4.0;
        newRaw(x, y, 0) = average + green(x, y, 0);
      }
    }
  }
  return newRaw;
}

Image improvedDemosaic(const Image &raw, int offsetGreen, int offsetRedX,
                       int offsetRedY, int offsetBlueX, int offsetBlueY) {
  // // --------- HANDOUT  PS03 ------------------------------
  // Takes as input a raw image and returns an rgb image
  // using edge-based green demosaicing for the green channel and
  // simple green based demosaicing of the red and blue channels
  Image newImage = Image(raw.width(), raw.height(), raw.channels());
  Image edgeGreen = edgeBasedGreen(raw, offsetGreen);
  Image red = greenBasedRorB(raw, edgeGreen, offsetRedX, offsetRedY);
  Image blue = greenBasedRorB(raw, edgeGreen, offsetBlueX, offsetBlueY);
  for (int x = 0; x < raw.width(); x++) {
    for (int y = 0; y < raw.height(); y++) {
      newImage(x, y, 2) = red(x, y, 0);
      newImage(x, y, 1) = edgeGreen(x, y, 0);
      newImage(x, y, 0) = blue(x, y, 0);
    }
  }
  return newImage;
}
