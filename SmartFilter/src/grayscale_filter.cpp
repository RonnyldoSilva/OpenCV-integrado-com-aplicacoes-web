#include "grayscale_filter.h"

using namespace cv;

Mat GrayscaleFilter::filter(const Mat &input) {
	Mat output;
	cvtColor(input, output, CV_BGR2GRAY);
	return output;
}
