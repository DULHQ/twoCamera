/*
 * Const.h
 *
 *  Created on: 2018年1月31日
 *      Author: hq
 */

#ifndef CONST_H_
#define CONST_H_

#include <string>
#include <opencv2/core/types.hpp>

using std::string;
using namespace cv;

//flags
const int findCBCornerFlags = CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE;
const int calibFlags = 0;
const int stereoFlags = CALIB_FIX_INTRINSIC;
//const float aspectRatio = 1.0f;

//calib
const int n_boards = 15;
const int board_dt = 20;
const int board_w = 11;//9;
const int board_h = 11;//6;
const float squareSize = 25.0; //mm
const Size boardSize = Size(board_w, board_h);

//file
const string matrixXMLNameL = "IntrinsicsLeft.xml";
const string distXMLNameL = "DistortionLeft.xml";
const string matrixXMLNameR = "IntrinsicsRight.xml";
const string distXMLNameR = "DistortionRight.xml";

//window
const string lWindowName = "leftCamera";
const string rWindowName = "righttCamera";
const string lShowWindowName = "leftShowCamera";
const string rShowWindowName = "rightShowtCamera";
const double width = 640;
const double height = 480;
const Size imgSize = Size(width, height);
const int outTime = 30;

#endif /* CONST_H_ */
