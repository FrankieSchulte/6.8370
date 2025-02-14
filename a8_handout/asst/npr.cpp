// npr.cpp
// Assignment 8

#include "npr.h"
#include "filtering.h"
#include "matrix.h"
#include <algorithm>
#include <math.h>

using namespace std;

/**************************************************************
 //                       NPR                                //
 *************************************************************/

void brush(Image &im, int x, int y, vector<float> color, const Image &texture) {
	// Draws a brushstroke defined by texture and color at (x,y) in im
	// // --------- HANDOUT  PS12 ------------------------------
	float textureWidthHalf = texture.width() / 2;
	float textureHeightHalf = texture.height() / 2;
	if (x <= textureWidthHalf || y <= textureHeightHalf ||
		x >= im.width() - textureWidthHalf || y >= im.height() - textureHeightHalf) {
			return;
		}
	for (int i = x - textureWidthHalf + 1; i < x + textureWidthHalf - 1; i++) {
		for (int j = y - textureHeightHalf + 1; j < y + textureHeightHalf - 1; j++) {
			float currTexture = texture(i - x + textureWidthHalf, j - y + textureHeightHalf);
			im(i, j, 0) = (im(i, j, 0) * (1.0 - currTexture)) + (color[0] * currTexture);
			im(i, j, 1) = (im(i, j, 1) * (1.0 - currTexture)) + (color[1] * currTexture);
			im(i, j, 2) = (im(i, j, 2) * (1.0 - currTexture)) + (color[2] * currTexture);
		}
	}
	return;
}

void singleScalePaint(const Image &im, Image &out, const Image &texture, int size, int N, float noise) {
	// Create painted rendering by splatting brushstrokes at N random locations
	// in your ouptut image
	// // --------- HANDOUT  PS12 ------------------------------
	float scaleValue = static_cast<float>(size) / static_cast<float>(texture.width());
	Image newBrush = scaleLin(texture, scaleValue);
	for (int n = 0; n < N; n++) {
		int randX = rand() % im.width();
		int randY = rand() % im.height();
		
		// float randomFactor = (1.0f - noise / 2.0f + noise * (static_cast<float>(rand()) / RAND_MAX));

		float colorR = im(randX, randY, 0) * (1.0f - noise / 2.0f + noise * (static_cast<float>(rand()) / RAND_MAX));
		float colorG = im(randX, randY, 1) * (1.0f - noise / 2.0f + noise * (static_cast<float>(rand()) / RAND_MAX));
		float colorB = im(randX, randY, 2) * (1.0f - noise / 2.0f + noise * (static_cast<float>(rand()) / RAND_MAX));

		vector<float> color = {colorR, colorG, colorB};

		brush(out, randX, randY, color, newBrush);
	}
	return;
}

void singleScalePaintImportance(const Image &im, const Image &importance,
						Image &out, const Image &texture, int size, int N, float noise) {
	// Create painted rendering but vary the density of the strokes according to
	// an importance map
	// // --------- HANDOUT  PS12 ------------------------------
	float scaleValue = static_cast<float>(size) / static_cast<float>(texture.width());
	Image newBrush = scaleLin(texture, scaleValue);
	for (int n = 0; n < N; n++) {
		int randX = rand() % im.width();
		int randY = rand() % im.height();

		float importanceValue = importance(randX, randY, 0);
		float randomImportance = static_cast<float>(rand()) / RAND_MAX;

		if (randomImportance <= importanceValue) {
			// float randomFactor = (1.0f - noise / 2.0f + noise * (static_cast<float>(rand()) / RAND_MAX));

			float colorR = im(randX, randY, 0) * (1.0f - noise / 2.0f + noise * (static_cast<float>(rand()) / RAND_MAX));
			float colorG = im(randX, randY, 1) * (1.0f - noise / 2.0f + noise * (static_cast<float>(rand()) / RAND_MAX));
			float colorB = im(randX, randY, 2) * (1.0f - noise / 2.0f + noise * (static_cast<float>(rand()) / RAND_MAX));

			vector<float> color = {colorR, colorG, colorB};

			brush(out, randX, randY, color, newBrush);
		}
	}
	return;
}

Image sharpnessMap(const Image &im, float sigma) {
	// Calculate sharpness mask 
	// // --------- HANDOUT  PS12 ------------------------------
	Image lumi = lumiChromi(im)[0];
  	Image gaussBlur = gaussianBlur_separable(lumi, sigma);
	Image result = lumi - gaussBlur;
	result = result * result;
	result = gaussianBlur_separable(result, 4 * sigma);
	float maxVal = result.max();
	if (maxVal > 0) {
		result = result / maxVal;
	}
	return result;
}

void painterly(const Image &im, Image &out, const Image &texture, int N, int size, float noise) {
	// Create painterly rendering using a first layer of coarse strokes followed
	// by smaller strokes in high detail areas
	// // --------- HANDOUT  PS12 ------------------------------
	Image layer1 = Image(im.width(), im.height(), im.channels());
	singleScalePaint(im, layer1, texture, 50, 10000, 0.15f);
	Image sharpness = sharpnessMap(im);

	Image layer2 = Image(im.width(), im.height(), im.channels());
	singleScalePaintImportance(im, sharpness, layer2, texture, size, N, noise);
	out = layer2;

	for (int x = 0; x < layer2.width(); x++) {
		for (int y = 0; y < layer2.height(); y++) {
			layer1(x, y, 0) = (layer1(x, y, 0) > layer2(x, y, 0)) ? layer1(x, y, 0) : layer2(x, y, 0);
			layer1(x, y, 1) = (layer1(x, y, 1) > layer2(x, y, 1)) ? layer1(x, y, 1) : layer2(x, y, 1);
			layer1(x, y, 2) = (layer1(x, y, 2) > layer2(x, y, 2)) ? layer1(x, y, 2) : layer2(x, y, 2);
		}
	}
	out = layer1;
	return;
}

Image computeTensor(const Image &im, float sigmaG, float factorSigma) {
 	// Compute xx/xy/yy Tensor of an image. (stored in that order)
 	// // --------- HANDOUT  PS07 ------------------------------
	Image newImage = Image(im.width(), im.height(), im.channels());

	Image lumi = lumiChromi(im)[0];
	Image chromi = lumiChromi(im)[1];
	Image gaussBlur = gaussianBlur_separable(lumi, sigmaG);
	Image X = gradientX(gaussBlur);
	Image Y = gradientY(gaussBlur);

	for (int x = 0; x < im.width(); x++) {
		for (int y = 0; y < im.height(); y++) {
		newImage(x, y, 0) = pow(X(x, y), 2);
		newImage(x, y, 1) = X(x, y) * Y(x, y);
		newImage(x, y, 2) = pow(Y(x, y), 2);
		}
	}

	return gaussianBlur_separable(newImage, sigmaG * factorSigma);
}


Image testAngle(const Image &im, float sigmaG, float factor) {
	// Extracts orientation of features in im. Angles should be mapped
	// to [0,1]
	// // --------- HANDOUT  PS12 ------------------------------
    Image tensor = computeTensor(im, sigmaG, factor);
	Image newImage = Image(im.width(), im.height(), 1);

	for (int x = 0; x < im.width(); x++) {
        for (int y = 0; y < im.height(); y++) {
            float Txx = tensor(x, y, 0);
            float Txy = tensor(x, y, 1);
            float Tyy = tensor(x, y, 2);

            Matrix T = Matrix::Zero(2, 2);
			T(0, 0) = Txx;
			T(0, 1) = Txy;
			T(1, 0) = Txy;
			T(1, 1) = Tyy;

            Eigen::EigenSolver<Matrix> solver(T);
            Vec2f eigenvalues = solver.eigenvalues().real();
            Matrix eigenvectors = solver.eigenvectors().real();
            Vec2f smallestEigenvector = eigenvectors.col((eigenvalues[0] < eigenvalues[1]) ? 0 : 1);
            float angle = atan2(smallestEigenvector[1], smallestEigenvector[0]);

            angle = fmod(angle + 2 * M_PI, 2 * M_PI);
            newImage(x, y, 0) = 1 - angle / (2 * M_PI); 
        }
    }
    return newImage;
}

vector<Image> rotateBrushes(const Image &im, int nAngles) {
	// helper function
	// Returns list of nAngles images rotated by 1*2pi/nAngles
	// // --------- HANDOUT  PS12 ------------------------------
	// vector<Image> result;
	// float angleFactor = 2 * M_PI / nAngles;

	// for (int i = 0; i < nAngles; i++) {
	// 	float angle = i * angleFactor;
	// 	Image currBrush = rotate(im, angle);
	// 	result.push_back(currBrush);
	// }

	// return result;
	vector<Image> result;
    float angleFactor = 2 * M_PI / nAngles;

    int originalWidth = im.width();
    int originalHeight = im.height();
	int maxLength = max(originalWidth, originalHeight);
    int center = maxLength / 2;

    for (int i = 0; i < nAngles; i++) {
        float angle = i * angleFactor;
        float cosAngle = cos(angle);
        float sinAngle = sin(angle);

        Image currBrush = Image(maxLength, maxLength, im.channels());

        for (int x = 0; x < maxLength; x++) {
            for (int y = 0; y < maxLength; y++) {
                float originalX = cos(angle) * (x - center) + sin(angle) * (y - center) + (originalWidth / 2);
                float originalY = -sin(angle) * (x - center) + cos(angle) * (y - center) + (originalHeight / 2);

                if (originalX >= 0 && originalX < originalWidth - 1 && originalY >= 0 && originalY < originalHeight - 1) {
                    for (int z = 0; z < im.channels(); z++) {
                        int x0 = floor(originalX);
                        int x1 = x0 + 1;
                        int y0 = floor(originalY);
                        int y1 = y0 + 1;

                        float top = (1 - originalX + x0) * im(x0, y0, z) + (originalX - x0) * im(x1, y0, z);
                        float bottom = (1 - originalX + x0) * im(x0, y1, z) + (originalX - x0) * im(x1, y1, z);
                        currBrush(x, y, z) = (1 - originalY + y0) * top + (originalY - y0) * bottom;
                    }
                }
            }
        }
        result.push_back(currBrush);
    }

    return result;
}

void singleScaleOrientedPaint(const Image &im, const Image &importance,
		Image &out, const Image &tensor, const Image &texture,int size, int N, 
		float noise, int nAngles) {
	// Similar to singleScalePaintImportant but brush strokes are oriented
	// according to tensor
	// // --------- HANDOUT  PS12 ------------------------------
	float scaleValue = static_cast<float>(size) / static_cast<float>(texture.width());
	Image newBrush = scaleLin(texture, scaleValue);

    vector<Image> rotatedBrushes = rotateBrushes(newBrush, nAngles);

    int acceptedStrokes = 0;
    while (acceptedStrokes < N) {
        int randX = rand() % im.width();
        int randY = rand() % im.height();
	
        float importanceValue = importance(randX, randY, 0);
        float randomSample = static_cast<float>(rand()) / RAND_MAX;
        if (randomSample > importanceValue) {
            continue;
        }

        float Txx = tensor(randX, randY, 0);
        float Txy = tensor(randX, randY, 1);
        float Tyy = tensor(randX, randY, 2);

        Matrix T = Matrix::Zero(2, 2);
		T(0, 0) = Txx;
		T(0, 1) = Txy;
		T(1, 0) = Txy;
		T(1, 1) = Tyy;

        Eigen::EigenSolver<Matrix> solver(T);
		Vec2f eigenvalues = solver.eigenvalues().real();
		Matrix eigenvectors = solver.eigenvectors().real();
        Vec2f smallestEigenvector = eigenvectors.col((eigenvalues[0] < eigenvalues[1]) ? 0 : 1);

        float angle = atan2(smallestEigenvector[1], smallestEigenvector[0]);
        angle = fmod(angle + 2 * M_PI, 2 * M_PI);
        int angleFactor = static_cast<int>((angle / (2 * M_PI)) * nAngles) % nAngles;
        Image currBrush = rotatedBrushes[angleFactor];

        // float randomFactor = 1.0f - noise / 2.0f + noise * (static_cast<float>(rand()) / RAND_MAX);
        float colorR = im(randX, randY, 0) * (1.0f - noise / 2.0f + noise * (static_cast<float>(rand()) / RAND_MAX));
        float colorG = im(randX, randY, 1) * (1.0f - noise / 2.0f + noise * (static_cast<float>(rand()) / RAND_MAX));
        float colorB = im(randX, randY, 2) * (1.0f - noise / 2.0f + noise * (static_cast<float>(rand()) / RAND_MAX));
        vector<float> color = {colorR, colorG, colorB};

        brush(out, randX, randY, color, currBrush);
        acceptedStrokes++;
    }
	return;
}

void orientedPaint(const Image &im, Image &out, const Image &texture, int N, int size, float noise) {
	// Similar to painterly() but strokes are oriented along the directions of maximal structure
	// // --------- HANDOUT  PS12 ------------------------------
	Image layer1 = Image(im.width(), im.height(), im.channels());

	Image tensor = computeTensor(im);
	Image importance = Image(im.width(), im.height(), im.channels());
	for (int i = 0; i < importance.number_of_elements(); i++) {
		importance(i) = 0.01f;
	}
	singleScaleOrientedPaint(im, importance, layer1, tensor, texture, 50, 10000, noise);
	
	Image sharpness = sharpnessMap(im);
	Image layer2 = Image(im.width(), im.height(), im.channels());
	singleScaleOrientedPaint(im, sharpness, layer2, tensor, texture, size, N, noise);
	out = layer2;

	for (int x = 0; x < layer2.width(); x++) {
		for (int y = 0; y < layer2.height(); y++) {
			layer1(x, y, 0) = (layer1(x, y, 0) > layer2(x, y, 0)) ? layer1(x, y, 0) : layer2(x, y, 0);
			layer1(x, y, 1) = (layer1(x, y, 1) > layer2(x, y, 1)) ? layer1(x, y, 1) : layer2(x, y, 1);
			layer1(x, y, 2) = (layer1(x, y, 2) > layer2(x, y, 2)) ? layer1(x, y, 2) : layer2(x, y, 2);
		}
	}
	out = layer1;
	return;
}



