#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

#include "a9.h"
#include "Image.h"
#include "filtering.h"
#include "matrix.h"

using namespace std;

// Gradient Descent

float dotIm(const Image &im1, const Image &im2) {
    float result = 0.0;
    for (int x = 0; x < im1.width(); x++)
        for (int y = 0; y < im1.height(); y++)
            for (int z = 0; z < im1.channels(); z++)
                result += im1(x, y, z) * im2(x, y, z);
    return result;
}

Image applyKernel(const Image &im, Filter &kernel) {
    return kernel.convolve(im, true);
}

Image computeResidual(Filter &kernel, const Image &x, const Image &y) {
    Image convolveX = kernel.convolve(x, true);
    Image result = y - convolveX;
    return result;
}

Image applyConjugatedKernel(const Image &im, Filter &kernel) {
    Filter kernelT(kernel.getWidth(), kernel.getHeight());

    for (int x = 0; x < kernel.getWidth(); x++) {
        for (int y = 0; y < kernel.getHeight(); y++) {
            kernelT(x, y) = kernel(kernel.getWidth() - 1 - x, kernel.getHeight() - 1 - y);
        }
    }

    Image result = kernelT.convolve(im, true);
    return result;
}

float computeStepSize(const Image &r, Filter &kernel) {
    Image Mr = applyKernel(r, kernel);
    Image MTr = applyConjugatedKernel(Mr, kernel);

    float dot1 = dotIm(r, MTr);
    float dot2 = dotIm(MTr, MTr);

    return dot1 / dot2;
}

Image deconvGradDescent(const Image &im_blur, Filter &kernel, int niter) {
    Image result = im_blur;

    for (int i = 0; i < niter; i++) {
        Image r = computeResidual(kernel, result, im_blur);
        Image MTr = applyConjugatedKernel(r, kernel);
        float alpha = computeStepSize(r, kernel);

        result = result + alpha * MTr;

    }
    return result;
}

// Conjugate Gradient

float computeGradientStepSize(const Image &r, const Image &d, Filter &kernel) {
    Image Md = applyKernel(d, kernel);
    Image conjMd = applyConjugatedKernel(Md, kernel);

    float dot1 = dotIm(r, d);
    float dot2 = dotIm(d, conjMd);

    return dot1 / dot2;
}

float computeConjugateDirectionStepSize(const Image &oldR, const Image &newR) {
    float dot1 = dotIm(newR, newR);
    float dot2 = dotIm(oldR, oldR);

    return dot1 / dot2;
}

Image deconvCG(const Image &im_blur, Filter &kernel, int niter) {
    Image newImage = im_blur;

    Image Mt_y = applyConjugatedKernel(im_blur, kernel);
    Image Mx = applyKernel(newImage, kernel);
    Image r = Mt_y - applyConjugatedKernel(Mx, kernel);
    Image d = r;
    Image oldR = r;

    for (int i = 0; i < niter; i++) {
        Image Md = applyKernel(d, kernel);
        Image MtMd = applyConjugatedKernel(Md, kernel);
        float alpha = computeGradientStepSize(r, d, kernel);
        newImage = newImage + alpha * d;

        r = r - alpha * MtMd;
        float beta = computeConjugateDirectionStepSize(oldR, r);
        d = r + beta * d;

        oldR = r;
    }

    return newImage;
}

// Noise Regularization

vector<float> laplacianKernel() {
    return {0, -1, 0,
            -1, 4, -1,
            0, -1, 0};
}

Image applyLaplacian(const Image &im) {
    Filter laplacianFilter(laplacianKernel(), 3, 3);
    return laplacianFilter.convolve(im, true);
}

Image applyAMatrix(const Image &im, Filter &kernel) {
    Image Mx = applyKernel(im, kernel);
    Image Mt_Mx = applyConjugatedKernel(Mx, kernel);

    Image laplacian = applyLaplacian(im);

    Image result = Mt_Mx + 0.05 * laplacian;
    return result;
}

Image applyRegularizedOperator(const Image &im, Filter &kernel, float lamb) {
    Image Mx = applyKernel(im, kernel);
    Image Mt_Mx = applyConjugatedKernel(Mx, kernel);

    Image laplacian = applyLaplacian(im);
    Image regularization = lamb * laplacian;

    Image result = Mt_Mx + regularization;
    return result;
}

float computeGradientStepSize_reg(const Image &r, const Image &p, Filter &kernel, float lamb) {
    Image A_p = applyRegularizedOperator(p, kernel, lamb);
    float dot1 = dotIm(r, p);
    float dot2 = dotIm(p, A_p);

    return dot1 / dot2;
}

Image deconvCG_reg(const Image &im_blur, Filter &kernel, float lamb, int niter) {
    Image newImage = im_blur;
    Image Mt_y = applyConjugatedKernel(im_blur, kernel);
    Image r = Mt_y - applyRegularizedOperator(newImage, kernel, lamb);
    Image p = r;

    for (int i = 0; i < niter; i++) {
        Image Ap = applyRegularizedOperator(p, kernel, lamb);
        float alpha = computeGradientStepSize_reg(r, p, kernel, lamb);

        newImage = newImage + alpha * p;
        Image new_r = r - alpha * Ap;
        float beta = computeConjugateDirectionStepSize(r, new_r);
        p = new_r + beta * p;

        r = new_r;
    }

    return newImage;
}

// Naive Compositing

Image naiveComposite(const Image &bg, const Image &fg, const Image &mask, int y_coord, int x_coord) {
    Image newImage = bg;

    for (int x = 0; x < fg.width(); x++) {
        for (int y = 0; y < fg.height(); y++) {
            int bgx = x + x_coord;
            int bgy = y + y_coord;

            if (bgx >= 0 && bgx < bg.width() && bgy >= 0 && bgy < bg.height()) {
                for (int z = 0; z < bg.channels(); z++) {
                    if (mask(x, y, z) > 0) {
                        newImage(bgx, bgy, z) = fg(x, y, z);
                    }
                }
            }
        }
    }
    return newImage;
}

// Poisson Image Editing

Image poisson(const Image &bg, const Image &fg, const Image &mask, int niter) {
    Image newImage = fg * mask + bg * (1.0f - mask);

    Image b = Filter(laplacianKernel(), 3, 3).convolve(fg, true);

    for (int i = 0; i < niter; i++) {
        Image r = b - applyLaplacian(newImage);
        r = r * mask;
        Image Ar = applyLaplacian(r);
        float alpha = dotIm(r, r) / (dotIm(r, Ar) + 1e-6);

        newImage = newImage + alpha * r;
    }

    return newImage;
}

Image poissonCG(const Image &bg, const Image &fg, const Image &mask, int niter) {
    Image newImage = fg * mask + bg * (1.0f - mask);

    Image b = Filter(laplacianKernel(), 3, 3).convolve(fg, true);
    Image r = b - applyLaplacian(newImage);
    r = r * mask;
    Image d = r;

    for (int i = 0; i < niter; i++) {
        Image Ad = applyLaplacian(d);
        float alpha = dotIm(r, r) / (dotIm(d, Ad) + 1e-6);
        newImage = newImage + alpha * d;

        Image r1 = r - alpha * Ad;
        r1 = r1 * mask;
        float beta = dotIm(r1, r1) / (dotIm(r, r) + 1e-6);
        d = r1 + beta * d;

        r = r1;
    }

    return newImage;
}


// PSF FILE PARSING
Filter loadPSF(const std::string &filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Error: Could not open PSF file!");
    }

    std::vector<float> psfData;  // Vector to store PSF values
    int width = 0;
    int height = 0;

    std::string line;
    while (getline(file, line)) {
        std::stringstream ss(line);
        float value;
        int rowWidth = 0;

        while (ss >> value) {
            psfData.push_back(value);
            rowWidth++;
            if (ss.peek() == ',') ss.ignore();
        }

        if (width == 0) width = rowWidth;
        else if (rowWidth != width) {
            throw std::runtime_error("Error: PSF file rows have inconsistent widths!");
        }

        height++;
    }
    file.close();

    // Normalize PSF
    float sum = 0.0f;
    for (float val : psfData) sum += val;

    if (sum > 1e-10) { // Avoid division by zero
        for (float &val : psfData) val /= sum;
    } else {
        throw std::runtime_error("Error: PSF sum is too close to zero!");
    }

    return Filter(psfData, width, height);
}