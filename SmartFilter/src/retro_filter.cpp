#include "retro_filter.h"

using namespace cv;

Mat RetroFilter::filter(const Mat &input) {
	Mat output;
	cvtColor(input, output, CV_BGR2GRAY);
	blur(output, output, Size(3, 3));
	Mat noise = Mat::zeros(output.size(), output.type());
	randn(noise, 30, 30);
	output += noise;
	return output;
}
