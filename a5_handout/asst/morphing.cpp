/* -----------------------------------------------------------------
 * File:    morphing.cpp
 * Created: 2015-09-25
 * -----------------------------------------------------------------
 *
 *
 *
 * ---------------------------------------------------------------*/

#include "morphing.h"
#include <cassert>

using namespace std;

Vec2f operator+(const Vec2f &a, const Vec2f &b) {
  // --------- HANDOUT  PS05 ------------------------------
  // Return the vector sum of a an b
  return Vec2f(a.x + b.x, a.y + b.y);
}

Vec2f operator-(const Vec2f &a, const Vec2f &b) {
  // --------- HANDOUT  PS05 ------------------------------
  // Return a-b
  return Vec2f(a.x - b.x, a.y - b.y);
}

Vec2f operator*(const Vec2f &a, float f) {
  // --------- HANDOUT  PS05 ------------------------------
  // Return a*f
  return Vec2f(a.x * f, a.y * f);
}

Vec2f operator/(const Vec2f &a, float f) {
  // --------- HANDOUT  PS05 ------------------------------
  // Return a/f
  return Vec2f(a.x / f, a.y / f);
}

float dot(const Vec2f &a, const Vec2f &b) {
  // --------- HANDOUT  PS05 ------------------------------
  // Return the dot product of a and b.
  return (a.x * b.x) + (a.y * b.y);
}

float length(const Vec2f &a) {
  // --------- HANDOUT  PS05 ------------------------------
  // Return the length of a.
  return sqrt(pow(a.x, 2) + pow(a.y, 2));
}

Vec2f perpendicular(const Vec2f &a) {
  // --------- HANDOUT  PS05 ------------------------------
  // Return a vector that is perpendicular to a.
  // Either direction is fine.
  return Vec2f(a.y, -a.x);
}

// The Segment constructor takes in 2 points P(x1,y1) and Q(x2,y2) corresponding
// to the ends of a segment and initialize the local reference frame e1,e2.
Segment::Segment(Vec2f P_, Vec2f Q_) : P(P_), Q(Q_) {
  // // --------- HANDOUT  PS05 ------------------------------
  // // The initializer list above ": P(P_), Q(Q_)" already copies P_
  // // and Q_, so you don't have to do it in the body of the constructor.
  // You should:
  // * Initialize the local frame e1,e2 (see header file)
  e1 = (Q - P) * (1 / length(Q - P));
  e2 = perpendicular(Q - P) * (1 / length(Q - P));
}

Vec2f Segment::XtoUV(Vec2f X) const {
  // --------- HANDOUT  PS05 ------------------------------
  // Compute the u,v coordinates of a point X with
  // respect to the local frame of the segment.
  // e2 ^
  //    |
  // v  +  * X
  //    | /
  //    |/
  //    *--+------>-----*
  //    P  u     e1     Q
  //                    u=1
  //
  // * Be careful with the different normalization for u and v
  float u = dot(X - P, (Q - P) / pow(length(Q - P), 2));
  float v = dot(X - P, perpendicular((Q - P) / length(Q - P)));
  return Vec2f(u, v);
}

Vec2f Segment::UVtoX(Vec2f uv) const {
  // --------- HANDOUT  PS05 ------------------------------
  // compute the (x, y) position of a point given by the (u,v)
  // location relative to this segment.
  // * Be careful with the different normalization for u and v
  // float u = uv.x;
  // float v = uv.y;
  // Vec2f intermediate = e1 * length(Q - P) * uv.x;
  Vec2f X = (e1 * length(Q - P) * uv.x) + (e2 * uv.y) + P;
  return X;
}

float Segment::distance(Vec2f X) const {
  // --------- HANDOUT  PS05 ------------------------------
  // Implement distance from a point X(x,y) to the segment. Remember the 3
  // cases from class.
  float squaredLength = pow(length(Q - P), 2);
  float lineProjection = dot((X - P), (Q - P)) / squaredLength;
  if (lineProjection < 0) {
    return length(X - P);
  }
  else if (lineProjection > 1) {
    return length(X - Q);
  }
  else if (squaredLength == 0) {
    return length(Q - X);
  }
  Vec2f projectedVector = P + ((Q - P) * lineProjection);
  return length(projectedVector - X);
}

Image warpBy1(const Image &im, const Segment &segBefore,
              const Segment &segAfter) {
  // --------- HANDOUT  PS05 ------------------------------
  // Warp an entire image according to a pair of segments.
  Image newImage = Image(im.width(), im.height(), im.channels());
  for (int x = 0; x < im.width(); x++) {
    for (int y = 0; y < im.height(); y++) {
      Vec2f uv = segAfter.XtoUV(Vec2f(x, y));
      Vec2f X = segBefore.UVtoX(uv);
      for (int z = 0; z < im.channels(); z++) {
        newImage(x, y, z) = interpolateLin(im, X.x, X.y, z, true);

      }
    }
  }
  return newImage;
}

float Segment::weight(Vec2f X, float a, float b, float p) const {
  // --------- HANDOUT  PS05 ------------------------------
  // compute the weight of a segment to a point X(x,y) given the weight
  // parameters a,b, and p (see paper for details).
  float weight = pow(pow(length(X), p) / (a + distance(X)), b);
  return weight;
}

Image warp(const Image &im, const vector<Segment> &src_segs,
           const vector<Segment> &dst_segs, float a, float b, float p) {
  // --------- HANDOUT  PS05 ------------------------------
  // Warp an image according to a vector of before and after segments using
  // segment weighting
  Image newImage = Image(im.width(), im.height(), im.channels());
  for (int x = 0; x < im.width(); x++) {
    for (int y = 0; y < im.height(); y++) {
      Vec2f dSum = Vec2f(0.0f, 0.0f);
      float weightSum = 0;
      for (int s = 0; s < src_segs.size(); s++) {
        Vec2f uv = dst_segs[s].XtoUV(Vec2f(x, y));
        Vec2f X = src_segs[s].UVtoX(uv);
        Vec2f displacement = X - Vec2f(x, y);
        float weight = dst_segs[s].weight(Vec2f(x, y), a, b, p);
        dSum = dSum + (displacement * weight);
        weightSum += weight;
      }
      dSum = Vec2f(x, y) + (dSum / weightSum);
      for (int z = 0; z < im.channels(); z++) {
        newImage(x, y, z) += interpolateLin(im, dSum.x, dSum.y, z, true);
      }
    }
  }
  return newImage;
}

vector<Image> morph(const Image &im_before, const Image &im_after,
                    const vector<Segment> &segs_before,
                    const vector<Segment> &segs_after, int N, float a, float b,
                    float p) {
  // --------- HANDOUT  PS05 ------------------------------
  // return a vector of N+2 images: the two inputs plus N images that morphs
  // between im_before and im_after for the corresponding segments. im_before
  // should be the first image, im_after the last.
  vector<Image> result{im_before};
  for (int n = 1; n <= N; n++) {
    float t = float(n) / (N + 1);

    Image newImage = Image(im_before.width(), im_before.height(), im_before.channels());
    for (int x = 0; x < im_before.width(); x++) {
      for (int y = 0; y < im_before.height(); y++) {
        Vec2f dSum = Vec2f(0.0f, 0.0f);
        float weightSum = 0;
        for (int s = 0; s < segs_before.size(); s++) {
          Vec2f uv = segs_after[s].XtoUV(Vec2f(x, y));
          Vec2f X = segs_before[s].UVtoX(uv);
          Vec2f displacement = (X - Vec2f(x, y));
          float weight = segs_after[s].weight(Vec2f(x, y), a, b, p);
          dSum = dSum + (displacement * weight);
          weightSum += weight;
        }
        dSum = Vec2f(x, y) + (dSum / weightSum);
        for (int z = 0; z < im_before.channels(); z++) {
          newImage(x, y, z) += (1 - t) * interpolateLin(im_before, dSum.x, dSum.y, z, true);
        }
      }
    }
    result.push_back(newImage);
  }
  result.push_back(im_after);
  return result;
}
