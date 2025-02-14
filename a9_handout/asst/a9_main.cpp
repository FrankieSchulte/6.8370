#include <iostream>

#include "a9.h"
#include "filtering.h"
#include "Image.h"

using namespace std;

Filter testGauss2D() {
  cout << "running testGauss2D" << endl;
  vector<float> gaussKernel = gauss2DFilterValues(1.0, 2.0);
  Filter kernel(gaussKernel, 5, 5);
  return kernel;
};

Image testApplyKernel(const Image &im, Filter &kernel) {
  cout << "running testApplyKernel" << endl;
  Image blurredImage = applyKernel(im, kernel);
  return blurredImage;
};

void testDeconvGradDescent(const Image &blurredImage, Filter &kernel) {
  cout << "running testDeconvGradDescent" << endl;
  Image deblurredImage = deconvGradDescent(blurredImage, kernel, 10);
  deblurredImage.write("Output/psf_deblurred.png");
};

void testDeconvCG(const Image &blurredImage, Filter &kernel) {
  cout << "running testDeconvCG" << endl;
  Image deblurredCG = deconvCG(blurredImage, kernel, 30);
  deblurredCG.write("Output/psf_deblurred_CG.png");
};

void testDeconvCG_reg(const Image &blurredImage, Filter &kernel) {
  cout << "running testDeconvCG_reg" << endl;
    float lamb = 0.001;
    Image deblurredImage = deconvCG_reg(blurredImage, kernel, lamb, 30);
    deblurredImage.write("Output/psf_deblurred_CG_reg.png");
}

void testNaiveComposite() {
  cout << "running testNaiveComposite" << endl;
    Image bg("Input/waterpool.png");
    Image fg("Input/bear.png");
    Image mask("Input/mask.png");
    Image naiveCompositeImage = naiveComposite(bg, fg, mask, 30, 30);
    naiveCompositeImage.write("Output/naive_composite.png");
}

void testPoisson() {
  cout << "running testPoisson" << endl;
  Image mask("Input/mask2.png");
  Image fg("Input/bear2.png");
  Image bg("Input/waterpool.png");
  Image poissonResult = poisson(bg, fg, mask, 1000);
  poissonResult.write("Output/poisson.png");
};

void testElenaPoisson() {
  cout << "running testElenaPoisson" << endl;
  Image mask("Input/elenamask3.png");
  Image fg("Input/elena3.png");
  Image bg("Input/pineapple3.png");
  Image poissonResult = poisson(bg, fg, mask, 1000);
  poissonResult.write("Output/elena_poisson.png");
};

void testPoissonCG() {
  cout << "running testPoissonCG" << endl;
  Image mask("Input/mask2.png");
  Image fg("Input/bear2.png");
  Image bg("Input/waterpool.png");
  Image poissonCGResult = poissonCG(bg, fg, mask, 1000);
  poissonCGResult.write("Output/poissonCG.png");
};

void testElenaPoissonCG() {
  cout << "running testElenaPoissonCG" << endl;
  Image mask("Input/elenamask3.png");
  Image fg("Input/elena3.png");
  Image bg("Input/pineapple3.png");
  Image poissonCGResult = poissonCG(bg, fg, mask, 1000);
  poissonCGResult.write("Output/elena_poissonCG.png");
};

int main()
{
    Image pruImage("Input/pru.png");
    Filter kernel = testGauss2D();
    Filter psfKernel = loadPSF("Input/psf");

    Image blurredImage = testApplyKernel(pruImage, kernel);
    blurredImage.write("Output/blurred.png");
    Image psfBlurredImage = testApplyKernel(pruImage, psfKernel);
    psfBlurredImage.write("Output/psf_blurred.png");
    
    testDeconvGradDescent(psfBlurredImage, psfKernel);
    testDeconvCG(psfBlurredImage, psfKernel);
    testDeconvCG_reg(psfBlurredImage, psfKernel);

    testNaiveComposite();
    testPoisson();
    testElenaPoisson();
    testPoissonCG();
    testElenaPoissonCG();

    return EXIT_SUCCESS;
}
