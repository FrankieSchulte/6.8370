/* --------------------------------------------------------------------------
 * File:    a12_main.cpp
 * Created: 2021-10-28
 * --------------------------------------------------------------------------
 *
 *
 *
 * ------------------------------------------------------------------------*/

#include "Image.h"
#include "basicImageManipulation.h"
#include "npr.h"
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

// This is a way for you to test your functions.
// We will only grade the contents of npr.cpp
void testBrush() {
  Image newBrush("Input/longBrush.png");
  Image photo("Input/DSC_0579.png");
  vector<float> color = {1, 0, 0};
  brush(photo, photo.width() / 2, photo.height() / 2, color, newBrush);
  photo.write("Output/DSC-0579_brushed.png");
}

void testSingleScalePaint() {
  Image newBrush("Input/brush.png");
  Image photo("Input/villeperdue.png");
  Image newPhoto = Image(photo.width(), photo.height(), photo.channels());
  singleScalePaint(photo, newPhoto, newBrush, 60, 10000, 0.15f);
  newPhoto.write("Output/villeperdue_scalePaint.png");
}

void testSingleScalePaintImportance() {
  Image newBrush("Input/brush.png");
  Image photo("Input/villeperdue.png");
  Image newPhoto = Image(photo.width(), photo.height(), photo.channels());
  Image importance = Image(photo.width(), photo.height(), photo.channels());
  for (int i = 0; i < importance.number_of_elements(); i++) {
    importance(i) = 0.01f;
  }
  singleScalePaintImportance(photo, importance, newPhoto, newBrush, 60, 10000, 0.15f);
  newPhoto.write("Output/villeperdue_singleScalePaintImportance.png");
}

void testPainterly() {
  Image newBrush("Input/brush.png");
  Image photo("Input/villeperdue.png");
  Image newPhoto = Image(photo.width(), photo.height(), photo.channels());
  painterly(photo, newPhoto, newBrush, 500000, 10, 0.3f);
  newPhoto.write("Output/villeperdue_painterly.png");
}

void testComputeTensor() {
  Image round("Input/china.png");
  Image newPhoto = Image(round.width(), round.height(), round.channels());
  newPhoto = computeTensor(round);
  newPhoto.write("Output/china_compute_tensor.png");
}

void testTestAngle() {
  Image round("Input/round.png");
  Image newPhoto = Image(round.width(), round.height(), round.channels());
  newPhoto = testAngle(round);
  newPhoto.write("Output/round_test_angle.png");
}

void testRotateBrushes() {
  Image newBrush("Input/brush.png");
  vector<Image> nBrushes = rotateBrushes(newBrush, 6);
  nBrushes[0].write("Output/rotate_brushes_0.png");
  nBrushes[1].write("Output/rotate_brushes_1.png");
  nBrushes[2].write("Output/rotate_brushes_2.png");
  nBrushes[3].write("Output/rotate_brushes_3.png");
  nBrushes[4].write("Output/rotate_brushes_4.png");
  nBrushes[5].write("Output/rotate_brushes_5.png");
}

void testSingleScaleOrientedPaint() {
  Image newBrush("Input/brush.png");
  Image china("Input/china.png");
  Image newPhoto = Image(china.width(), china.height(), china.channels());
  Image importance = Image(china.width(), china.height(), china.channels());
  for (int i = 0; i < importance.number_of_elements(); i++) {
    importance(i) = 0.01f;
  }
  Image tensor = computeTensor(china);

  singleScaleOrientedPaint(china, importance, newPhoto, tensor, newBrush, 15, 1000000, 0.3f);
  newPhoto.write("Output/china-oriented-paint.png");

}

void testOrientedPaint() {
  Image newBrush("Input/longBrush.png");
  Image photo("Input/villeperdue.png");
  Image newPhoto = Image(photo.width(), photo.height(), photo.channels());
  orientedPaint(photo, newPhoto, newBrush, 1000000, 10, 0.3f);
  newPhoto.write("Output/villeperdue-oriented-paint-long.png");
}

int main() {
  cout << "nothing done in a12_main.cpp, debug me !" << endl;

  // testBrush();
  // testSingleScalePaint();
  // testSingleScalePaintImportance();
  // testPainterly();
  // testComputeTensor();
  // testTestAngle();
  // testRotateBrushes();
  // testSingleScaleOrientedPaint();
  testOrientedPaint();
  
  return 0;
}
