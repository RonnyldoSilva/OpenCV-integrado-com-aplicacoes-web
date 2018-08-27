#include "edge_filter.h"

using namespace cv;

Mat EdgeFilter::filter(const Mat &input) {
	Mat output;
	cvtColor(input, output, CV_BGR2GRAY);
	Canny(output, output, 10, 100, 3);
	return output;
}
