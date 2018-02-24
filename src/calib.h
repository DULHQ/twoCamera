/*
 * calib.h
 *
 *  Created on: 2018年1月31日
 *      Author: hq
 */

#ifndef CALIB_H_
#define CALIB_H_

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <cv.h>
#include <highgui.h>
#include <cxcore.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio/videoio_c.h>
//#include <videoio.hpp>
//#include <videostab.hpp>

using namespace cv;
using namespace std;

IplImage *getImage(Mat *img, bool isLeft);
void startCalibTwo();
//void calib(VideoCapture *cap);
void startCalib(CvCapture *cap);

#endif /* CALIB_H_ */
