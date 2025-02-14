#include "a1.h"
#include <iostream>

using namespace std;

// This is a way for you to test your functions.
// We will only grade the contents of a1.cpp and Image.cpp
int main() {
  cout << "nothing done in a1_main.cpp, debug me !" << endl;
  Image test = create_special();
  test.write("./Output/create_special_result.png");

  // Brightness
  Image brightnessIm("./Input/Boston_underexposed.png");
  brightnessIm = brightness(brightnessIm, 2.0);
  brightnessIm.write("./Output/Boston_overexposed.png");

  // Contrast
  Image contrastIm("./Input/Boston_low_contrast.png");
  contrastIm = contrast(contrastIm, 5.0, 0.5);
  contrastIm.write("./Output/Boston_high_contrast.png");

  // Color2Gray
  Image grayscaleIm("./Input/castle_small.png");
  grayscaleIm = color2gray(grayscaleIm);
  grayscaleIm.write("./Output/grayscale_castle.png");

  // rgb2yuv
  Image rgbTest("./Input/castle_small.png");
  rgbTest = yuv2rgb(rgb2yuv(rgbTest));
  rgbTest.write("./Output/rgb2yuv_castle.png");

  // Saturate
  Image saturateIm("./Input/castle_small.png");
  saturateIm = saturate(saturateIm, 5.0);
  saturateIm.write("./Output/saturated_castle.png");

  // Example
  Image im("./Input/castle_small.png");
  std::vector<Image> LC = lumiChromi(im);
  LC[0].write("./Output/castle_luminance.png");
  LC[1].write("./Output/castle_chrominance.png");

  // Gamma Test
  Image gammaIm("./Input/skies_and_trees.png");
  std::vector<Image> GQ = gamma_test(gammaIm, 6, 2.2);
  GQ[0].write("./Output/quant_to_gamma.png");
  GQ[1].write("./Output/gamma_to_quant.png");

  // Spanish Test
  Image spanishIm("./Input/zebra.png");
  std::vector<Image> spanishVector = spanish(spanishIm);
  spanishVector[0].write("./Output/zebra1.png");
  spanishVector[1].write("./Output/zebra2.png");

  // White Balance Test
  Image whiteBalanceIm("./Input/flower.png");
  whiteBalanceIm = grayworld(whiteBalanceIm);
  whiteBalanceIm.write("./Output/whitebalanced_flower.png");
}
