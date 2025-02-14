/* -----------------------------------------------------------------
 * File:    a3_main.cpp
 * Author:  Michael Gharbi <gharbi@mit.edu>
 * Created: 2015-09-30
 * -----------------------------------------------------------------
 *
 *
 *
 * ---------------------------------------------------------------*/

#include "Image.h"
#include "align.h"
#include "basicImageManipulation.h"
#include "demosaic.h"
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

// This is a way for you to test your functions.
// We will only grade the contents of demosaic.cpp and align.cpp
int main() {
  cout << "nothing done in a3_main.cpp, debug me !" << endl;
  // // Denoise ---------------------------
  // // Load sequence
  vector<Image> seq;
  int n_images = 16;
  for (int i = 1; i <= n_images; ++i) {
      ostringstream fname;
      fname << "./Input/aligned-ISO400/1D2N-iso400-";
      // fname << "./Input/aligned-ISO3200/1D2N-iso3200-";
      fname << i;
      fname << ".png";
      seq.push_back(Image(fname.str()));
  }
  //
  // // Denoise
  Image out = denoiseSeq(seq);
  out.write("./Output/denoised.png");
  //
  Image SNRIm = logSNR(seq,1/30.0);
  SNRIm.write("./Output/snr_map.png");

  Image im1 = Image(100, 100, 1);
  im1.set_color(0.0, 0.0, 0.0);
  im1.create_rectangle(40, 40, 60, 50, 1.0, 1.0, 1.0);
  Image im2 = Image(100, 100, 1);
  im2.set_color(0.0, 0.0, 0.0);
  im2.create_rectangle(50, 60, 70, 70, 1.0, 1.0, 1.0);
  cout << align(im1, im2, 20)[0] << endl;
  cout << align(im1, im2, 20)[1] << endl;

  // alignAndDenoise
  vector<Image> seq2;
  int n_images2 = 18;
  for (int i = 1; i <= n_images2; ++i) {
      ostringstream fname;
      fname << "./Input/green/noise-small-";
      fname << i;
      fname << ".png";
      seq2.push_back(Image(fname.str()));
  }

  Image alignAndDenoiseResult = alignAndDenoise(seq2, 3);
  alignAndDenoiseResult.write("./Output/alignAndDenoise3.png");



  //
  // Demosaic ---------------------------
  Image raw("./Input/raw/signs-small.png");
  Image green = basicGreen(raw, 1);
  Image edgeGreen = edgeBasedGreen(raw, 1);
  green.write("./Output/demosaic_green.png");
  edgeGreen.write("./Output/demosaic_edge_green.png");
  Image red = basicRorB(raw, 1, 1);
  red.write("./Output/demosaic_red.png");
  Image blue = basicRorB(raw, 0, 0);
  blue.write("./Output/demosaic_blue.png");

  Image rgb = basicDemosaic(raw, 1, 1,1,0,0);
  Image edgeRgb = edgeBasedGreenDemosaic(raw, 1, 1,1,0,0);
  rgb.write("./Output/demosaiced.png");
  edgeRgb.write("./Output/edge_green_demosaiced.png");

  Image greenBasedRed = greenBasedRorB(raw, edgeGreen, 1, 1);
  Image greenBasedBlue = greenBasedRorB(raw, edgeGreen, 0, 0);
  Image finalDemosaic = improvedDemosaic(raw, 1, 1, 1, 0, 0);
  finalDemosaic.write("./Output/improved_demosaic.png");

  

  //
  //
  // // Sergey ---------------------------
  // Image sergeyImg("./Input/Sergey/00911v_third.png");
  // Image rgb2 = split(sergeyImg);
  // rgb2.write("./Output/Sergey_split.png");
  // Image rgbAlign = sergeyRGB(sergeyImg,10);
  // rgbAlign.write("./Output/Sergey_aligned.png");

  return 0;
}
