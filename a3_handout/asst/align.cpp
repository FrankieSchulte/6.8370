/* --------------------------------------------------------------------------
 * File:    align.cpp
 * Created: 2015-10-01
 * --------------------------------------------------------------------------
 *
 *
 *
 * ------------------------------------------------------------------------*/

#include "align.h"
#include <cmath>

using namespace std;

Image denoiseSeq(const vector<Image> &imSeq) {
  // // --------- HANDOUT  PS03 ------------------------------
  // Basic denoising by computing the average of a sequence of images
  Image newImage = Image(imSeq[0].width(), imSeq[0].height(), imSeq[0].channels());

  for (int i = 0; i < imSeq.size(); i++) {
    for (int x = 0; x < imSeq[i].width(); x++) {
      for (int y = 0; y < imSeq[i].height(); y++) {
        for (int z = 0; z < imSeq[i].channels(); z++) {
          newImage(x, y, z) += imSeq[i](x, y, z);
        }
      }
    }
  }

  for (int x = 0; x < imSeq[0].width(); x++) {
    for (int y = 0; y < imSeq[0].height(); y++) {
      for (int z = 0; z < imSeq[0].channels(); z++) {
        newImage(x, y, z) = newImage(x, y, z) / imSeq.size();
      }
    }
  }
  return newImage;
}

Image logSNR(const vector<Image> &imSeq, float scale) {
  // // --------- HANDOUT  PS03 ------------------------------
  // returns an image visualizing the per-pixel and
  // per-channel log of the signal-to-noise ratio scaled by scale.
  Image SNRImage = Image(imSeq[0].width(), imSeq[0].height(), imSeq[0].channels());
  Image denoiseSeqIm = denoiseSeq(imSeq);

  for (int i = 0; i < imSeq.size(); i++) {
    for (int x = 0; x < imSeq[i].width(); x++) {
      for (int y = 0; y < imSeq[i].height(); y++) {
        for (int z = 0; z < imSeq[i].channels(); z++) {
          SNRImage(x, y, z) += pow((imSeq[i](x, y, z) - denoiseSeqIm(x, y, z)), 2);
        }
      }
    }
  }

  for (int x = 0; x < imSeq[0].width(); x++) {
    for (int y = 0; y < imSeq[0].height(); y++) {
      for (int z = 0; z < imSeq[0].channels(); z++) {
        SNRImage(x, y, z) = SNRImage(x, y, z) / (imSeq.size() - 1);
        SNRImage(x, y, z) = 10 * log10(pow(denoiseSeqIm(x, y, z), 2) / (SNRImage(x, y, z) + pow(10, -6)));
        SNRImage(x, y, z) = SNRImage(x, y, z);
      }
    }
  }

  return SNRImage * scale;
}

vector<int> align(const Image &im1, const Image &im2, int maxOffset) {
  // // --------- HANDOUT  PS03 ------------------------------
  // returns the (x,y) offset that best aligns im2 to match im1.
  float minSum = 9999999.9;
  vector<int> result = { 0, 0 };
  for (int xOff = maxOffset; xOff >= -maxOffset; xOff--) {
    for (int yOff = maxOffset; yOff >= -maxOffset; yOff--) {
      float currSum = 0;
      for (int x = 0; x < im1.width(); x++) {
        for (int y = 0; y < im1.height(); y++) {
          for (int z = 0; z < im1.channels(); z++) {
            currSum += pow((im1.smartAccessor(x, y, z, true) - im2.smartAccessor(x + xOff, y + yOff, z, true)), 2);
          }
        }
      }
      if (currSum < minSum) { 
        minSum = currSum; 
        result = { xOff, yOff };
      }
    }
  }
  return result;
}

Image alignAndDenoise(const vector<Image> &imSeq, int maxOffset) {
  // // --------- HANDOUT  PS03 ------------------------------
  // Registers all images to the first one in a sequence and outputs
  // a denoised image even when the input sequence is not perfectly aligned.
  Image newImage = Image(imSeq[0].width(), imSeq[0].height(), imSeq[0].channels());
  vector<vector<int>> offsets(imSeq.size() - 1, { 0, 0 });
  for (int i = 1; i < imSeq.size(); i++) {
    vector<int> currOffset = align(imSeq[0], imSeq[i], maxOffset);
    offsets[i - 1] = currOffset;
  }

  vector<Image> newImSeq(imSeq.size(), imSeq[0]);
  for (int i = 1; i < imSeq.size(); i++) {
    for (int x = 0; x < imSeq[0].width(); x++) {
      for (int y = 0; y < imSeq[0].height(); y++) {
        for (int z = 0; z < imSeq[0].channels(); z++) {
          int currXOffset = offsets[i - 1][0];
          int currYOffset = offsets[i - 1][1];
          newImSeq[i](x, y, z) = imSeq[i].smartAccessor(x + currXOffset, y + currYOffset, z, true);
        }
      }
    }
  }
  return denoiseSeq(newImSeq);
}

Image split(const Image &sergeyImg) {
  // --------- HANDOUT  PS03 ------------------------------
  // 6.865 only:
  // split a Sergey images to turn it into one 3-channel image.
  return Image(1, 1, 1);
}

Image sergeyRGB(const Image &sergeyImg, int maxOffset) {
  // // --------- HANDOUT  PS03 ------------------------------
  // 6.865 only:
  // aligns the green and blue channels of your rgb channel of a sergey
  // image to the red channel. This should return an aligned RGB image
  return Image(1, 1, 1);
}

/**************************************************************
 //               DON'T EDIT BELOW THIS LINE                //
 *************************************************************/

// This circularly shifts an image by xRoll in the x direction and
// yRoll in the y direction. xRoll and yRoll can be negative or postive
Image roll(const Image &im, int xRoll, int yRoll) {

  int xNew, yNew;
  Image imRoll(im.width(), im.height(), im.channels());

  // for each pixel in the original image find where its corresponding
  // location is in the rolled image
  for (int x = 0; x < im.width(); x++) {
    for (int y = 0; y < im.height(); y++) {

      // use modulo to figure out where the new location is in the
      // rolled image. Then take care of when this returns a negative
      // number
      xNew = (x + xRoll) % im.width();
      yNew = (y + yRoll) % im.height();
      xNew = (xNew < 0) * (imRoll.width() + xNew) + (xNew >= 0) * xNew;
      yNew = (yNew < 0) * (imRoll.height() + yNew) + (yNew >= 0) * yNew;

      // assign the rgb values for each pixel in the original image to
      // the location in the new image
      for (int z = 0; z < im.channels(); z++) {
        imRoll(xNew, yNew, z) = im(x, y, z);
      }
    }
  }

  // return the rolled image
  return imRoll;
}
