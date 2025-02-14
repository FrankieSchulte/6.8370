#ifndef A9_H_PHUDVTKB
#define A9_H_PHUDVTKB

// Write your declarations here, or extend the Makefile if you add source
// files
#include <iostream>
#include "a9.h"
#include "Image.h"
#include "filtering.h"
#include "matrix.h"

using namespace std;

// Write your implementations here, or extend the Makefile if you add source
// files

// Gradient Descent
Image applyKernel(const Image &im, Filter &kernel);
Image computeResidual(Filter &kernel, const Image &x, const Image &y);
Image applyConjugatedKernel(const Image &im, Filter &kernel);
float computeStepSize(const Image &r, Filter &kernel);
Image deconvGradDescent(const Image &im_blur, Filter &kernel, int niter);

// Conjugate Gradient
float computeGradientStepSize(const Image &r, const Image &d, Filter &kernel);
float computeConjugateDirectionStepSize(const Image &oldR, const Image &newR);
Image deconvCG(const Image &im_blur, Filter &kernel, int niter = 10); // niter = 10

// Noise Regularization
vector<float> laplacianKernel();
Image applyLaplacian(const Image &im);
Image applyAMatrix(const Image &im, Filter &kernel);
Image applyRegularizedOperator(const Image &im, Filter &kernel, float lamb);
float computeGradientStepSize_reg(const Image &r, const Image &p, Filter &kernel, float lamb);
Image deconvCG_reg(const Image &im_blur, Filter &kernel, float lamb = 0.05, int niter = 10); // lamb = 0.05, niter = 10

// Naive Compositing
Image naiveComposite(const Image &bg, const Image &fg, const Image &mask, int y, int x);

// Poisson Image Editing
Image poisson(const Image &bg, const Image &fg, const Image &mask, int niter = 200); // niter = 200
Image poissonCG(const Image &bg, const Image &fg, const Image &mask, int niter = 200); // niter = 200

// PSF File Loading
Filter loadPSF(const std::string &filename);

#endif /* A9_H_PHUDVTKB */

