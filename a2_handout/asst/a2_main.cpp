/* --------------------------------------------------------------------------
 * File:    a2_main.cpp
 * Created: 2015-09-23
 * --------------------------------------------------------------------------
 *
 *
 *
 * ------------------------------------------------------------------------*/

#include "Image.h"
#include "filtering.h"
#include <ctime>
#include <iostream>
#include <vector>

using namespace std;

// This is a way for you to test your functions.
// We will only grade the contents of filter.cpp and Image.cpp
int main() {
  cout << "nothing done in a2_main.cpp, debug me !" << endl;

  // ------- Example tests, change them ! --------------
  Image imCambridge("./Input/Cambridge2.png");
  Image im = impulseImg(10);

  Image test = Image(3, 3, 3);
  test.set_color(1.0f, 1.0f, 1.0f);

  cout << "smart accessor 1: " << test.smartAccessor(5, 5, 1, true)
       << endl;

  cout << "smart accessor at (1,3,0): " << im.smartAccessor(1, 3, 0, true)
       << endl;

  // Image blurred = boxBlur(im, 1, true);
  Image imBlurred2 = boxBlur(imCambridge, 9, true);
  cout << "blurred impulse image" << endl;
  imCambridge.write("./Output/boxBlurTest.png");
  imBlurred2.write("./Output/boxBlurTest2.png");

  cout << "keep testing..." << endl;
  // ---------------------------------------------------

  // ---------------------------------------------------
  // Test the filter class on an impulse image
  Image dirac = impulseImg(31);

  // Test kernel
  vector<float> kernel{0, 0, 1, 0, 1, 0, 1, 0, 0}; // C++11 syntax
  Filter testFilter(kernel, 3, 3);
  Image testOutput = testFilter.convolve(dirac);
  // The output should be an exact copy of the kernel in the center of the
  // image
  testOutput.write("./Output/testKernel.png");

  Image boxBlurKernel = boxBlur_filterClass(imCambridge, 9, true);

  boxBlurKernel.write("./Output/testCambridgeKernel.png");
  // ---------------------------------------------------

  // ---------------------------------------------------
  // E.g. test the sobelKernel
  // create Sobel Filter that extracts horizontal gradients
  // [ -1 0 1 ]
  // [ -2 0 2 ]
  // [ -1 0 1 ]
  vector<float> fDataX{-1.0, 0.0, 1.0, -2.0, 0.0, 2.0, -1.0, 0.0, 1.0};
  Filter sobelX(fDataX, 3, 3);

  // verify that your filter is correct by using it to filter an impulse image
  Image impulse = impulseImg(11); // create an image containing an impulse
  // convolve the impulse image with the Sobel kernel. We divide the output
  // by 4 and add 0.5 to make the range of the image between 0 and 1
  Image verifyKernel = sobelX.convolve(impulse) / 4 + 0.5;
  verifyKernel.write("./Output/verifySobelKernel.png");

  // filter an image using the sobel kernel
  Image im2("./Input/lounge_view.png");
  Image sobelFiltered = sobelX.convolve(im2);
  Image sobelFiltered2 = gradientMagnitude(im2, true);
  sobelFiltered2.write("./Output/sobelFiltered2.png");

  // make the range of the output image from 0 to 1 for visualization
  // since the Sobel filter changes the range of a (0,1) image to (-4,4)
  Image sobelOut = sobelFiltered / 8 + 0.5;
  sobelOut.write("./Output/sobelFiltered.png");

  Image gaussian1DIm = gaussianBlur_horizontal(im2, 5.0, 3.0, true);
  gaussian1DIm.write("./Output/1DgaussianTest.png");

  Image gaussian2DIm = gaussianBlur_2D(im2, 5.0, 3.0, true);
  gaussian2DIm.write("./Output/2DgaussianTest.png");

  Image gaussianSeperableIm = gaussianBlur_separable(im2, 5.0, 3.0, true);
  gaussianSeperableIm.write("./Output/seperableGaussianTest.png");

  Image sharpenedIm = unsharpMask(im2, 20.0, 3.0, true);
  sharpenedIm.write("./Output/sharpenTest.png");

  Image lensIm("./Input/lens.png");
  Image bilateralIm = bilateral(lensIm, 0.1, 1.0, 3.0, true);
  bilateralIm.write("./Output/bilateralLens.png");
  // ---------------------------------------------------

  // --- Timer example ---------------------------------
  clock_t start = clock();
  float sigma = 2.0f;
  Image blurHorizontal = gaussianBlur_2D(im2, sigma, 6.0f);
  clock_t end = clock();
  double duration = (end - start) * 1.0f / CLOCKS_PER_SEC;
  cout << "2D gaussian took: " << duration << "s" << endl;
  // ---------------------------------------------------
}
