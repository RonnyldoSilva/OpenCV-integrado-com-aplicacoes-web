#include "cartoon_filter.h"

using namespace cv;

Mat CartoonFilter::filter(const Mat &input) {
	Mat output = input.clone();
	Mat edges;
	cvtColor(output, edges, CV_BGR2GRAY);
	Canny(edges, edges, 10, 100, 3);
	cvtColor(edges, edges, CV_GRAY2BGR);
	for (int i = 0; i < output.rows; i++) {
		uchar *data = output.ptr<uchar>(i);
		for (int j = 0; j < output.cols * output.channels(); j++) {
			data[j] = data[j] / NUM_COLORS * NUM_COLORS + NUM_COLORS / 2;
		}
	}
	output -= edges;	
	return output;
};
