#include "homography.h"
#include "matrix.h"

using namespace std;

void applyHomography(const Image &source, const Matrix &H, Image &out,
                     bool bilinear) {
  // --------- HANDOUT  PS06 ------------------------------
  // Transform image source using the homography H, and composite in onto out.
  // if bilinear == true, using bilinear interpolation. Use nearest neighbor
  // otherwise.
  Matrix invH = H.inverse();
  for (int x = 0; x < out.width(); x++) {
    for (int y = 0; y < out.height(); y++) {
      for (int z = 0; z < out.channels(); z++) {
        Matrix currMatrix(3, 1);
        currMatrix(0, 0) = x;
        currMatrix(1, 0) = y;
        currMatrix(2, 0) = 1;
        Matrix finalMatrix = invH * currMatrix;
        float newX = finalMatrix(0, 0) / finalMatrix(2, 0);
        float newY = finalMatrix(1, 0) / finalMatrix(2, 0);
        if (newX >= 0 && newX < source.width() - 1 && newY >= 0 && newY < source.height() - 1) {
          float pixel;
          if (bilinear) { pixel = interpolateLin(source, newX, newY, z, false); }
          else { pixel = source(round(newX), round(newY), z); }

          out(x, y, z) = pixel;
        }
      }
    }
  }
}

Matrix computeHomography(const CorrespondencePair correspondences[4]) {
  // --------- HANDOUT  PS06 ------------------------------
  // Compute a homography from 4 point correspondences.
  Matrix result(9, 9);
  result(8, 8) = 1;

  for (int i = 0; i < 4; i++) {
    Vec3f p1 = correspondences[i].point1;
    Vec3f p2 = correspondences[i].point2;

    result(i * 2, 0) = p1.y();
    result(i * 2 + 1, 0) = 0;

    result(i * 2, 1) = p1.x();
    result(i * 2 + 1, 1) = 0;

    result(i * 2, 2) = 1;
    result(i * 2 + 1, 2) = 0;

    result(i * 2, 3) = 0;
    result(i * 2 + 1, 3) = p1.y();

    result(i * 2, 4) = 0;
    result(i * 2 + 1, 4) = p1.x();

    result(i * 2, 5) = 0;
    result(i * 2 + 1, 5) = 1;

    result(i * 2, 6) = p2.y() * -p1.y();
    result(i * 2 + 1, 6) = p1.y() * -p2.x();

    result(i * 2, 7) = p2.y() * -p1.x();
    result(i * 2 + 1, 7) = p1.x() * -p2.x();

    result(i * 2, 8) = -p2.y();
    result(i * 2 + 1, 8) = -p2.x();
  }

  Matrix newResult = Matrix::Zero(9, 1);
  newResult(8, 0) = 1;

  Matrix homography = result.fullPivLu().solve(newResult);

  Matrix finalMatrix = Matrix::Zero(3, 3);
  finalMatrix(0, 0) = homography(4, 0);
  finalMatrix(0, 1) = homography(3, 0);
  finalMatrix(0, 2) = homography(5, 0);
  finalMatrix(1, 0) = homography(1, 0);
  finalMatrix(1, 1) = homography(0, 0);
  finalMatrix(1, 2) = homography(2, 0);
  finalMatrix(2, 0) = homography(7, 0);
  finalMatrix(2, 1) = homography(6, 0);
  finalMatrix(2, 2) = homography(8, 0);

  return finalMatrix;
}

BoundingBox computeTransformedBBox(int imwidth, int imheight, Matrix H) {
  // --------- HANDOUT  PS06 ------------------------------
  // Predict the bounding boxes that encompasses all the transformed
  // coordinates for pixels frow and Image with size (imwidth, imheight)
  Vec3f lowerLeft(0, imheight, 1);
  Vec3f upperLeft(0, 0, 1);
  Vec3f lowerRight(imwidth, imheight, 1);
  Vec3f upperRight(imwidth, 0, 1);

  lowerLeft = H * lowerLeft;
  upperLeft = H * upperLeft;
  lowerRight = H * lowerRight;
  upperRight = H * upperRight;

  float lowerLeftX = lowerLeft.x() / lowerLeft.z();
  float upperLeftX = upperLeft.x() / upperLeft.z();
  float lowerRightX = lowerRight.x() / lowerRight.z();
  float upperRightX = upperRight.x() / upperRight.z();

  float lowerLeftY = lowerLeft.y() / lowerLeft.z();
  float upperLeftY = upperLeft.y() / upperLeft.z();
  float lowerRightY = lowerRight.y() / lowerRight.z();
  float upperRightY = upperRight.y() / upperRight.z();

  float a = min(lowerLeftX, upperLeftX);
  float b = max(lowerRightX, upperRightX);
  float c = min(upperLeftY, upperRightY);
  float d = max(lowerLeftY, lowerRightY);
  return BoundingBox(a, b, c, d);
}

BoundingBox bboxUnion(BoundingBox B1, BoundingBox B2) {
  // --------- HANDOUT  PS06 ------------------------------
  // Compute the bounding box that tightly bounds the union of B1 an B2.
  return BoundingBox(min(B1.x1, B2.x1), max(B1.x2, B2.x2), min(B1.y1, B2.y1), max(B1.y2, B2.y2));
}

Matrix makeTranslation(BoundingBox B) {
  // --------- HANDOUT  PS06 ------------------------------
  // Compute a translation matrix (as a homography matrix) that translates the
  // top-left corner of B to (0,0).
  float newX = -B.x1;
  float newY = -B.y1;
  Matrix result = Matrix::Zero(3, 3);
  result(0, 0) = 1;
  result(1, 1) = 1;
  result(2, 2) = 1;
  result(0, 2) = newX;
  result(1, 2) = newY;
  return result;
}

Image stitch(const Image &im1, const Image &im2,
             const CorrespondencePair correspondences[4]) {
  // --------- HANDOUT  PS06 ------------------------------
  // Transform im1 to align with im2 according to the set of correspondences.
  // make sure the union of the bounding boxes for im2 and transformed_im1 is
  // translated properly (use makeTranslation)
  Matrix H = computeHomography(correspondences);
  Matrix I = Matrix::Zero(3, 3);
  I(0, 0) = 1;
  I(1, 1) = 1;
  I(2, 2) = 1;

  BoundingBox B1 = computeTransformedBBox(im1.width(), im1.height(), H);
  BoundingBox B2 = computeTransformedBBox(im2.width(), im2.height(), I);
  BoundingBox unionB = bboxUnion(B1, B2);
  Matrix translatedB = makeTranslation(unionB);

  Image newImage = Image(unionB.x2 - unionB.x1, unionB.y2 - unionB.y1, im1.channels());
  applyHomography(im1, translatedB * H, newImage, true);
  applyHomography(im2, translatedB, newImage, true);

  return newImage;
}

// debug-useful
Image drawBoundingBox(const Image &im, BoundingBox bbox) {
  // --------- HANDOUT  PS06 ------------------------------
  /*
    ________________________________________
   / Draw me a bounding box!                \
   |                                        |
   | "I jumped to my                        |
   | feet, completely thunderstruck. I      |
   | blinked my eyes hard. I looked         |
   | carefully all around me. And I saw a   |
   | most extraordinary small person, who   |
   | stood there examining me with great    |
   | seriousness."                          |
   \              Antoine de Saint-Exupery  /
    ----------------------------------------
           \   ^__^
            \  (oo)\_______
               (__)\       )\/\
                   ||----w |
                   ||     ||
  */
  return im;
}

void applyHomographyFast(const Image &source, const Matrix &H, Image &out,
                         bool bilinear) {
  // --------- HANDOUT  PS06 ------------------------------
  // Same as apply but change only the pixels of out that are within the
  // predicted bounding box (when H maps source to its new position).

  Matrix invH = H.inverse();
  BoundingBox bbox = computeTransformedBBox(source.width(), source.height(), H);

  for (int x = bbox.x1; x < bbox.x2 + 1; x++) {
    for (int y = bbox.y1; y < bbox.y2 + 1; y++) {
      for (int z = 0; z < out.channels(); z++) {
        Matrix currMatrix(3, 1);
        currMatrix(0, 0) = x;
        currMatrix(1, 0) = y;
        currMatrix(2, 0) = 1;
        Matrix finalMatrix = invH * currMatrix;
        float newX = finalMatrix(0, 0) / finalMatrix(2, 0);
        float newY = finalMatrix(1, 0) / finalMatrix(2, 0);
        if (newX >= 0 && newX < source.width() - 1 && newY >= 0 && newY < source.height() - 1) {
          float pixel;
          if (bilinear) { pixel = interpolateLin(source, newX, newY, z, true); }
          else { pixel = source(round(newX), round(newY), z); }

          out(x, y, z) = pixel;
        }
      }
    }
  }
}
