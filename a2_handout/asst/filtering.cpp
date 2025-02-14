/* -----------------------------------------------------------------
 * File:    filtering.cpp
 * Created: 2015-09-22
 * -----------------------------------------------------------------
 *
 * Image convolution and filtering
 *
 * ---------------------------------------------------------------*/

#include "filtering.h"
#include <cassert>
#include <cmath>
#include <math.h>
#include <iostream>
#include "basicImageManipulation.h"
using namespace std;

Image boxBlur(const Image &im, int k, bool clamp) {
  // --------- HANDOUT  PS02 ------------------------------
  // Convolve an image with a box filter of size k by k
  // It is safe to asssume k is odd.
  Image newImage = Image(im.width(), im.height(), im.channels());
  float currSum = 0.0f;
  int halfK = (k - 1) / 2;

  for (int x = 0; x < im.width(); x++) {
    for (int y = 0; y < im.height(); y++) {
      for (int z = 0; z < im.channels(); z++) {
        currSum = 0.0f;

        for (int i = x - halfK; i < x + halfK + 1; i++) {
          for (int j = y - halfK; j < y + halfK + 1; j++) {
            currSum += im.smartAccessor(i, j, z, clamp);
          }
        }
        newImage(x, y, z) = currSum / (k * k);
      }
    }
  }
  return newImage;
}

Image Filter::convolve(const Image &im, bool clamp) const {
  // --------- HANDOUT  PS02 ------------------------------
  // Write a convolution function for the filter class
  Image newImage = Image(im.width(), im.height(), im.channels());
  float currSum = 0.0f;

  int halfWidth = (width() - 1) / 2;
  int halfHeight = (height() - 1) / 2;

  for (int x = 0; x < im.width(); x++) {
    for (int y = 0; y < im.height(); y++) {
      for (int z = 0; z < im.channels(); z++) {
        currSum = 0.0f;

        for (int i = x - halfWidth; i < x + halfWidth + 1; i++) {
          for (int j = y - halfHeight; j < y + halfHeight + 1; j++) {
            currSum += im.smartAccessor(i, j, z, clamp) * operator()(x - i + halfWidth, y - j + halfHeight);
          }
        }
        newImage(x, y, z) = currSum;
      }
    }
  }
  return newImage;
}

Image boxBlur_filterClass(const Image &im, int k, bool clamp) {
  // --------- HANDOUT  PS02 ------------------------------
  // Reimplement the box filter using the filter class.
  // check that your results match those in the previous function "boxBlur"
  vector<float> kernel(k * k, 1.0f / (k * k));
  Filter blurFilter(kernel, k, k);
  return blurFilter.convolve(im, clamp);
}

Image gradientMagnitude(const Image &im, bool clamp) {
  // --------- HANDOUT  PS02 ------------------------------
  // Uses a Sobel kernel to compute the horizontal and vertical components
  // of the gradient of an image and returns the gradient magnitude.
  vector<float> kernel1{-1, 0, 1, -2, 0, 2, -1, 0, 1};
  vector<float> kernel2{-1, -2, -1, 0, 0, 0, 1, 2, 1};

  Image newImage = Image(im.width(), im.height(), im.channels());
  float currSum1 = 0.0f;
  float currSum2 = 0.0f;

  for (int x = 0; x < im.width(); x++) {
    for (int y = 0; y < im.height(); y++) {
      for (int z = 0; z < im.channels(); z++) {
        currSum1 = 0.0f;
        currSum2 = 0.0f;

        for (int i = x - 1; i < x + 2; i++) {
          for (int j = y - 1; j < y + 2; j++) {
            currSum1 += im.smartAccessor(i, j, z, clamp) * kernel1[((j - y + 1) * 3) + (i - x + 1)];
            currSum2 += im.smartAccessor(i, j, z, clamp) * kernel2[((j - y + 1) * 3) + (i - x + 1)];
          }
        }
        newImage(x, y, z) = sqrt(pow((currSum1), 2) + pow((currSum2), 2));
      }
    }
  }
  return newImage;
}

vector<float> gauss1DFilterValues(float sigma, float truncate) {
  // --------- HANDOUT  PS02 ------------------------------
  // Create a vector containing the normalized values in a 1D Gaussian filter
  // Truncate the gaussian at truncate*sigma.
  vector<float> result;
  float totalSum = 0.0f;
  int mid = ceil(sigma * truncate);

  for (int x = -mid; x < mid + 1; x++) {
    float f_x = (1 / sqrt(2 * M_PI * pow(sigma, 2))) * exp(pow(x, 2) / (-2 * pow(sigma, 2)));
    totalSum += f_x;
    result.push_back(f_x);
  }
  // Normalization
  for (int x = 0; x < result.size(); x++) {
    result[x] = result[x] / totalSum;
  }
  return result;
}

Image gaussianBlur_horizontal(const Image &im, float sigma, float truncate,
                              bool clamp) {
  // --------- HANDOUT  PS02 ------------------------------
  // Gaussian blur across the rows of an image
  vector<float> kernel = gauss1DFilterValues(sigma, truncate);
  Filter gaussFilter(kernel, kernel.size(), 1);
  return gaussFilter.convolve(im, clamp);
}

vector<float> gauss2DFilterValues(float sigma, float truncate) {
  // --------- HANDOUT  PS02 ------------------------------
  // Create a vector containing the normalized values in a 2D Gaussian
  // filter. Truncate the gaussian at truncate*sigma.
  vector<float> result;
  float totalSum = 0.0f;
  int mid = ceil(sigma * truncate);

  for (int x = -mid; x < mid + 1; x++) {
    for (int y = -mid; y < mid + 1; y++) {
      float f_x = exp(-1 * (pow(x, 2) + pow(y, 2)) / (2 * pow(sigma, 2)));
      totalSum += f_x;
      result.push_back(f_x);
    }
  }
  // Normalization
  for (int x = 0; x < result.size(); x++) {
    result[x] = result[x] / totalSum;
  }
  return result;
}

Image gaussianBlur_2D(const Image &im, float sigma, float truncate,
                      bool clamp) {
  // --------- HANDOUT  PS02 ------------------------------
  // Blur an image with a full 2D rotationally symmetric Gaussian kernel
  vector<float> kernel = gauss2DFilterValues(sigma, truncate);
  Filter gauss2DFilter(kernel, (1 + (2 * sigma * truncate)), (1 + (2 * sigma * truncate)));
  return gauss2DFilter.convolve(im, clamp);
}

Image gaussianBlur_separable(const Image &im, float sigma, float truncate,
                             bool clamp) {
  // --------- HANDOUT  PS02 ------------------------------
  // Use principles of seperabiltity to blur an image using 2 1D Gaussian
  // Filters
  vector<float> kernel = gauss1DFilterValues(sigma, truncate);
  Filter gaussFilterHoriz(kernel, kernel.size(), 1);
  Image result = gaussFilterHoriz.convolve(im, clamp);
  Filter gaussFilterVert(kernel, 1, kernel.size());
  return gaussFilterVert.convolve(result, true);
}

Image unsharpMask(const Image &im, float sigma, float truncate, float strength,
                  bool clamp) {
  // --------- HANDOUT  PS02 ------------------------------
  // Sharpen an image
  Image gaussImage = gaussianBlur_separable(im, sigma, truncate, clamp);
  Image result = im - gaussImage;
  return (im + (strength * result));
}

Image bilateral(const Image &im, float sigmaRange, float sigmaDomain,
                float truncateDomain, bool clamp) {
  // --------- HANDOUT  PS02 ------------------------------
  // Denoise an image using the bilateral filter
  Image newImage = Image(im.width(), im.height(), im.channels());

  vector<float> domainGauss = gauss2DFilterValues(sigmaDomain, truncateDomain);
  int r = ceil(sigmaDomain * truncateDomain);

  for (int x = 0; x < im.width(); x++) {
    for (int y = 0; y < im.height(); y++) {
      for (int z = 0; z < im.channels(); z++) {
        float imageSum = 0.0f;
        float sumZ = 0.0f;
        for (int j = 0; j < 1 + (2*r); j++) {
          for (int i = 0; i < 1 + (2*r); i++) {
            int currX = x + r - i;
            int currY = y + r - j;
            float domainGaussValue = domainGauss[((1+(2*r)) * currY) + currX];
            float rangeGaussSum = 0.0f;
            for (int c = 0; c < im.channels(); c++) {
              rangeGaussSum += pow(im(x, y, c) - im(i, j, c), 2);
            }
            rangeGaussSum = sqrt(rangeGaussSum);

            rangeGaussSum = exp(-1 * (pow(rangeGaussSum, 2) / (2 * pow(sigmaRange, 2))));

            imageSum += domainGaussValue * rangeGaussSum * im(i, j);
            sumZ += domainGaussValue * rangeGaussSum;
          }
        }
        newImage(x, y, z) = (1 / sumZ) * (imageSum);
      }
    }
  }
  return newImage;
}

Image bilaYUV(const Image &im, float sigmaRange, float sigmaY, float sigmaUV,
              float truncateDomain, bool clamp) {
  // --------- HANDOUT  PS02 ------------------------------
  // 6.8370 only
  // Bilateral Filter an image seperatly for
  // the Y and UV components of an image
  return im;
}

/**************************************************************
 //               DON'T EDIT BELOW THIS LINE                //
 *************************************************************/

// Create an image of 0's with a value of 1 in the middle. This function
// can be used to test that you have properly set the kernel values in your
// Filter object. Make sure to set k to be larger than the size of your kernel
Image impulseImg(int k) {
  // initlize a kxkx1 image of all 0's
  Image impulse(k, k, 1);

  // set the center pixel to have intensity 1
  int center = floor(k / 2);
  impulse(center, center, 0) = 1.0f;

  return impulse;
}

// ------------- FILTER CLASS -----------------------
Filter::Filter() : width_(0), height_(0) {}
Filter::Filter(const vector<float> &fData, int fWidth, int fHeight)
    : kernel_(fData), width_(fWidth), height_(fHeight) {
  assert(fWidth * fHeight == (int)fData.size());
}

Filter::Filter(int fWidth, int fHeight)
    : kernel_(std::vector<float>(fWidth * fHeight, 0)), width_(fWidth),
      height_(fHeight) {}

const float &Filter::operator()(int x, int y) const {
  if (x < 0 || x >= width_)
    throw OutOfBoundsException();
  if (y < 0 || y >= height_)
    throw OutOfBoundsException();

  return kernel_[x + y * width_];
}

float &Filter::operator()(int x, int y) {
  if (x < 0 || x >= width_)
    throw OutOfBoundsException();
  if (y < 0 || y >= height_)
    throw OutOfBoundsException();

  return kernel_[x + y * width_];
}
// --------- END FILTER CLASS -----------------------
