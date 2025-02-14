// hdr.cpp
// Assignment 5

#include "hdr.h"
#include "filtering.h"
#include <algorithm>
#include <math.h>

using namespace std;

/**************************************************************
 //                       HDR MERGING                        //
 *************************************************************/

Image computeWeight(const Image &im, float epsilonMini, float epsilonMaxi) {
  // --------- HANDOUT  PS04 ------------------------------
  // Generate a weight image that indicates which pixels are good to use in
  // HDR, i.e. weight=1 when the pixel value is in [epsilonMini, epsilonMaxi].
  // The weight is per pixel, per channel.
  Image newImage = Image(im.width(), im.height(), im.channels());
  for (int x = 0; x < im.width(); x++) {
    for (int y = 0; y < im.height(); y++) {
      for (int z = 0; z < im.channels(); z++) {
        if (im(x, y, z) > epsilonMini && im(x,y,z) < epsilonMaxi) {
          newImage(x, y, z) = 1.0;
        }
        else {
          newImage(x, y, z) = 0.0;
        }
      }
    }
  }
  return newImage;
}

float computeFactor(const Image &im1, const Image &w1, const Image &im2,
                    const Image &w2) {
  // --------- HANDOUT  PS04 ------------------------------
  // Compute the multiplication factor between a pair of images. This
  // gives us the relative exposure between im1 and im2. It is computed as
  // the median of im2/(im1+eps) for some small eps, taking into account
  // pixels that are valid in both images.
  float result = 0.0f;
  vector<float> valid_pixels;
  for (int x = 0; x < im1.width(); x++) {
    for (int y = 0; y < im1.height(); y++) {
      for (int z = 0; z < im1.channels(); z++) {
        if (w1(x, y, z) == 1.0 && w2(x, y, z) == 1.0) {
          valid_pixels.push_back(im2(x, y, z) / (im1(x, y, z) + pow(10, -10)));
        }
      }
    }
  }

  int valid_size = valid_pixels.size();
  sort(valid_pixels.begin(), valid_pixels.end());

  if (valid_size % 2 != 0) {
    return valid_pixels[valid_size / 2];
  }

  return (valid_pixels[(valid_size - 1) / 2] + valid_pixels[valid_size / 2]) / 2.0;
}

Image makeHDR(vector<Image> &imSeq, float epsilonMini, float epsilonMaxi) {
  // --------- HANDOUT  PS04 ------------------------------
  // Merge images to make a single hdr image
  // For each image in the sequence, compute the weight map (special cases
  // for the first and last images).
  // Compute the exposure factor for each consecutive pair of image.
  // Write the valid pixel to your hdr output, taking care of rescaling them
  // properly using the factor.
  Image newImage = Image(imSeq[0].width(), imSeq[0].height(), imSeq[0].channels());
  Image wImage = Image(imSeq[0].width(), imSeq[0].height(), imSeq[0].channels());
  Image wkiImage = Image(imSeq[0].width(), imSeq[0].height(), imSeq[0].channels());
  
  Image prevImage = imSeq[0];
  Image prevWeight = computeWeight(imSeq[0], epsilonMini, 1.0f);

  float currK = 1.0f;

  for (int x = 0; x < imSeq[0].width(); x++) {
    for (int y = 0; y < imSeq[0].height(); y++) {
      for (int z = 0; z < imSeq[0].channels(); z++) {
        wImage(x, y, z) = prevWeight(x, y, z);
        wkiImage(x, y, z) = prevWeight(x, y, z) * prevImage(x, y, z);
      }
    }
  }

  for (int i = 1; i < imSeq.size(); i++) {
    Image currImage = imSeq[i];
    Image currWeight = Image(imSeq[0].width(), imSeq[0].height(), imSeq[0].channels());
    if (i == imSeq.size() - 1) {
      currWeight = computeWeight(imSeq[i], 0.0f, epsilonMaxi);
    }
    else {
      currWeight = computeWeight(imSeq[i], epsilonMini, epsilonMaxi);
    }

    currK *= computeFactor(prevImage, prevWeight, currImage, currWeight);

    for (int x = 0; x < imSeq[0].width(); x++) {
      for (int y = 0; y < imSeq[0].height(); y++) {
        for (int z = 0; z < imSeq[0].channels(); z++) {
          wImage(x, y, z) += currWeight(x, y, z);
          wkiImage(x, y, z) += currWeight(x, y, z) * (1 / currK) * currImage(x, y, z);
        }
      }
    }

    prevImage = currImage;
    prevWeight = currWeight;
  }

  for (int x = 0; x < imSeq[0].width(); x++) {
    for (int y = 0; y < imSeq[0].height(); y++) {
      for (int z = 0; z < imSeq[0].channels(); z++) {
        if (wImage(x, y, z) == 0.0) {
          newImage(x, y, z) = imSeq[0](x, y, z);
        }
        else {
          newImage(x, y, z) = (1.0f / wImage(x, y, z)) * wkiImage(x, y, z);
        }
      }
    }
  }

  return newImage;
}

/**************************************************************
 //                      TONE MAPPING                        //
 *************************************************************/

Image toneMap(const Image &im, float targetBase, float detailAmp, bool useBila,
              float sigmaRange) {
  // --------- HANDOUT  PS04 ------------------------------
  // tone map an hdr image
  // - Split the image into its luminance-chrominance components.
  // - Work in the log10 domain for the luminance
  Image newImage = Image(im.width(), im.height(), im.channels());
  Image lumi = lumiChromi(im)[0];
  Image chromi = lumiChromi(im)[1];
 
  Image log10Lumi = log10Image(lumi);
  float sigmaDomain = max(im.width(), im.height()) / 50.0;

  if (useBila) {
    newImage = bilateral(log10Lumi, sigmaRange, sigmaDomain, 3.0);
  }
  else if (!useBila) {
    newImage = gaussianBlur_separable(log10Lumi, sigmaDomain, 3.0);
  }

  float k = log10(targetBase) / (newImage.max() - newImage.min());
  Image logNewImage = k * newImage;
  logNewImage = logNewImage - logNewImage.max();

  Image imageDetailAmp = log10Lumi - newImage;
  Image logDetailAmp = detailAmp * imageDetailAmp;

  Image finalLog = logNewImage + logDetailAmp;
  Image lumiResult = exp10Image(finalLog);
  Image result = lumiChromi2rgb(vector<Image>{lumiResult, chromi});
  return result;
}

/*********************************************************************
 *                       Tone mapping helpers                        *
 *********************************************************************/

// image --> log10Image
Image log10Image(const Image &im) {
  // --------- HANDOUT  PS04 ------------------------------
  // Taking a linear image im, transform to log10 scale.
  // To avoid infinity issues, make any 0-valued pixel be equal the the
  // minimum non-zero value. See image_minnonzero(im).
  float minnonzero = image_minnonzero(im);
  Image newImage = Image(im.width(), im.height(), im.channels());
  for (int x = 0; x < im.width(); x++) {
    for (int y = 0; y < im.height(); y++) {
      for (int z = 0; z < im.channels(); z++) {
        if (im(x, y, z) == 0.0f) {
          newImage(x, y, z) = log10(minnonzero);
        }
        else {
          newImage(x, y, z) = log10(im(x, y, z));
        }
      }
    }
  }
  return newImage;
}

// Image --> 10^Image
Image exp10Image(const Image &im) {
  // --------- HANDOUT  PS04 ------------------------------
  // take an image in log10 domain and transform it back to linear domain.
  // see pow(a, b)
  Image newImage = Image(im.width(), im.height(), im.channels());
  for (int x = 0; x < im.width(); x++) {
    for (int y = 0; y < im.height(); y++) {
      for (int z = 0; z < im.channels(); z++) {
        newImage(x, y, z) = pow(10.0, im(x, y, z));
      }
    }
  }
  return newImage;
}

// min non-zero pixel value of image
float image_minnonzero(const Image &im) {
  // --------- HANDOUT  PS04 ------------------------------
  // return the smallest value in the image that is non-zeros (across all
  // channels too)
  float min = FLT_MAX;
  for (int i = 0; i < im.number_of_elements(); i++) {
    if (im(i) < min && im(i) > 0.0) {
      min = im(i);
    }
  }
  return min;
}
