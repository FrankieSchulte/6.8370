/* -----------------------------------------------------------------
 * File:    a1.cpp
 * Created: 2015-09-15
 * Updated: 2019-08-10
 * -----------------------------------------------------------------
 *
 * Assignment 01
 *
 * ---------------------------------------------------------------*/

#include "a1.h"
#include <cmath>
using namespace std;

// Create a surprise image
Image create_special() {
  // // --------- HANDOUT  PS01 ------------------------------
  // create the image outlined in the handout

  // Create an image of size 290 × 150 × 3
  Image specialImage = Image(290, 150, 3);

  // Set all pixels to white
  specialImage.set_color(1.0, 1.0, 1.0);
  cout << "Hello?";

  // Draw rectangles
  specialImage.create_rectangle(0, 0, 31, 149, 0.64, 0.12, 0.20);
  specialImage.create_rectangle(52, 0, 83, 102, 0.64, 0.12, 0.20);
  specialImage.create_rectangle(103, 0, 134, 149, 0.64, 0.12, 0.20);
  specialImage.create_rectangle(155, 0, 186, 30, 0.64, 0.12, 0.20);
  specialImage.create_rectangle(155, 48, 186, 149, 0.55, 0.55, 0.55);
  specialImage.create_rectangle(207, 0, 289, 30, 0.64, 0.12, 0.20);
  specialImage.create_rectangle(207, 48, 238, 149, 0.64, 0.12, 0.20);
  return specialImage;
}

// Change the brightness of the image
// const Image & means a reference to im will get passed to the function,
// but the compiler won't let you modify it within the function.
// So you will return a new image
Image brightness(const Image &im, float factor) {
  // --------- HANDOUT  PS01 ------------------------------
  // Image output(im.width(), im.height(), im.channels());
  // Modify image brightness
  Image newImage = Image(im.width(), im.height(), im.channels());
  for (int i = 0; i < im.number_of_elements(); i++) {
    newImage(i) = im(i) * factor;
  }
  return newImage;
}

Image contrast(const Image &im, float factor, float midpoint) {
  // --------- HANDOUT  PS01 ------------------------------
  // Image output(im.width(), im.height(), im.channels());
  // Modify image contrast
  Image newImage = Image(im.width(), im.height(), im.channels());
  for (int i = 0; i < im.number_of_elements(); i++) {
    newImage(i) = factor * (im(i) - midpoint) + midpoint;
  }
  return newImage;
}

Image color2gray(const Image &im, const std::vector<float> &weights) {
  // --------- HANDOUT  PS01 ------------------------------
  // Image output(im.width(), im.height(), 1);
  // Convert to grayscale
  Image newImage = Image(im.width(), im.height(), 1);
  for (int i = 0; i < im.width(); i++) {
    for (int j = 0; j < im.height(); j++) {
      newImage(i, j) = ((im(i, j, 0) * weights[0]) + 
                        (im(i, j, 1) * weights[1]) +
                        (im(i, j, 2) * weights[2]));
    }
  }
  return newImage;
}

// For this function, we want two outputs, a single channel luminance image
// and a three channel chrominance image. Return them in a vector with
// luminance first
std::vector<Image> lumiChromi(const Image &im) {
  // --------- HANDOUT  PS01 ------------------------------
  // Create the luminance image
  Image luminance = color2gray(im);
  // Create the chrominance image
  Image chrominance = Image(im.width(), im.height(), 3);
  for (int i = 0; i < im.width(); i++) {
    for (int j = 0; j < im.height(); j++) {
      chrominance(i, j, 0) = im(i, j, 0) / luminance(i, j, 0);
      chrominance(i, j, 1) = im(i, j, 1) / luminance(i, j, 0);
      chrominance(i, j, 2) = im(i, j, 2) / luminance(i, j, 0);
    }
  }
  // Create the output vector as (luminance, chrominance)
  std::vector<Image> result{luminance, chrominance};
  return result;
}

// Modify brightness then contrast
Image brightnessContrastLumi(const Image &im, float brightF, float contrastF,
                             float midpoint) {
  // --------- HANDOUT  PS01 ------------------------------
  // Modify brightness, then contrast of luminance image
  std::vector<Image> lumiChromiResult = lumiChromi(im);
  lumiChromiResult[0] = brightness(lumiChromiResult[0], brightF);
  lumiChromiResult[0] = contrast(lumiChromiResult[0], contrastF);
  return lumiChromiResult[0] * lumiChromiResult[1];
}

Image rgb2yuv(const Image &im) {
  // --------- HANDOUT  PS01 ------------------------------
  // Create output image of appropriate size
  // Change colorspace
  Image newImage = Image(im.width(), im.height(), 3);
  for (int i = 0; i < im.width(); i++) {
    for (int j = 0; j < im.height(); j++) {
      newImage(i, j, 0) = (0.299 * im(i, j, 0)) + (0.587 * im(i, j, 1)) + (0.114 * im(i, j, 2));
      newImage(i, j, 1) = (-0.147 * im(i, j, 0)) + (-0.289 * im(i, j, 1)) + (0.436 * im(i, j, 2));
      newImage(i, j, 2) = (0.615 * im(i, j, 0)) + (-0.515 * im(i, j, 1)) + (-0.1 * im(i, j, 2));
    }
  }
  return newImage;
}

Image yuv2rgb(const Image &im) {
  // --------- HANDOUT  PS01 ------------------------------
  // Create output image of appropriate size
  // Change colorspace
  Image newImage = Image(im.width(), im.height(), 3);
  for (int i = 0; i < im.width(); i++) {
    for (int j = 0; j < im.height(); j++) {
      newImage(i, j, 0) = (1.0 * im(i, j, 0)) + (1.14 * im(i, j, 2));
      newImage(i, j, 1) = (1.0 * im(i, j, 0)) + (-0.395 * im(i, j, 1)) + (-0.581 * im(i, j, 2));
      newImage(i, j, 2) = (1.0 * im(i, j, 0)) + (2.032 * im(i, j, 1));
    }
  }
  return newImage;
}

Image saturate(const Image &im, float factor) {
  // --------- HANDOUT  PS01 ------------------------------
  // Create output image of appropriate sizes
  // Saturate image
  Image newImage = rgb2yuv(im);
  for (int i = 0; i < im.width(); i++) {
    for (int j = 0; j < im.height(); j++) {
      newImage(i, j, 1) = newImage(i, j, 1) * factor;
      newImage(i, j, 2) = newImage(i, j, 2) * factor;
    }
  }
  return yuv2rgb(newImage);
}

// Gamma codes the image
Image gamma_code(const Image &im, float gamma) {
  // // --------- HANDOUT  PS01 ------------------------------
  // Image output(im.width(), im.height(), im.channels());
  // Gamma encodes the image
  // return output;
  Image newImage = Image(im.width(), im.height(), 3);
  for (int i = 0; i < im.width(); i++) {
    for (int j = 0; j < im.height(); j++) {
      newImage(i, j, 0) = pow(im(i, j, 0), (1 / gamma));
      newImage(i, j, 1) = pow(im(i, j, 1), (1 / gamma));
      newImage(i, j, 2) = pow(im(i, j, 2), (1 / gamma));
    }
  }
  return newImage;
}

// Quantizes the image to 2^bits levels and scales back to 0~1
Image quantize(const Image &im, int bits) {
  // // --------- HANDOUT  PS01 ------------------------------
  // Image output(im.width(), im.height(), im.channels());
  // Quantizes the image to 2^bits levels
  // return output;
  Image newImage = Image(im.width(), im.height(), 3);
  float range = pow(2, bits);
  for (int i = 0; i < im.width(); i++) {
    for (int j = 0; j < im.height(); j++) {
      newImage(i, j, 0) = floor(im(i, j, 0) * range) / range;
      newImage(i, j, 1) = floor(im(i, j, 1) * range) / range;
      newImage(i, j, 2) = floor(im(i, j, 2) * range) / range;
    }
  }
  return newImage;
}

// Compare between first quantize then gamma_encode and first gamma_encode
// then quantize
std::vector<Image> gamma_test(const Image &im, int bits, float gamma) {
  // --------- HANDOUT  PS01 ------------------------------
  // im1 = quantize then gamma_encode the image
  Image im1 = quantize(gamma_code(im, bits), gamma);
  // im2 = gamma_encode then quantize the image
  Image im2 = gamma_code(quantize(im, bits), gamma);
  // Remember to create the output images and the output vector
  // Push the images onto the vector
  // Do all the required processing
  // Return the vector, color image first
  std::vector<Image> result{im1, im2};
  return result;
}

// Return two images in a C++ vector
std::vector<Image> spanish(const Image &im) {
  // --------- HANDOUT  PS01 ------------------------------
  // Remember to create the output images and the output vector
  // Push the images onto the vector
  // Do all the required processing
  // Return the vector, color image first
  Image newImage = rgb2yuv(im);
  for (int i = 0; i < im.width(); i++) {
    for (int j = 0; j < im.height(); j++) {
      newImage(i, j, 0) = 0.5;
      newImage(i, j, 1) = newImage(i, j, 1) * -1.0;
      newImage(i, j, 2) = newImage(i, j, 2) * -1.0;
    }
  }
  std::vector<Image> result{yuv2rgb(newImage), color2gray(im)};
  return result;
}

// White balances an image using the gray world assumption
Image grayworld(const Image &im) {
  // --------- HANDOUT  PS01 ------------------------------
  // Implement automatic white balance by multiplying each channel
  // of the input by a factor such that the three channels of the output
  // image have the same mean value. The mean value of the green channel
  // is taken as reference.
  Image newImage = Image(im.width(), im.height(), 3);
  float averageR;
  float averageG;
  float averageB;
  for (int i = 0; i < im.width(); i++) {
    for (int j = 0; j < im.height(); j++) {
      averageR += im(i, j, 0);
      averageG += im(i, j, 1);
      averageB += im(i, j, 2);
    }
  }
  averageR = averageR / (im.width() * im.height());
  averageG = averageG / (im.width() * im.height());
  averageB = averageB / (im.width() * im.height());
  for (int i = 0; i < im.width(); i++) {
    for (int j = 0; j < im.height(); j++) {
      newImage(i, j, 0) = (((averageG * im(i, j, 0)) / averageR));
      newImage(i, j, 1) = im(i, j, 1);
      newImage(i, j, 2) = (((averageG * im(i, j, 2)) / averageB));
    }
  }
  return newImage;
}
