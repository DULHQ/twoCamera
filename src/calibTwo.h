/*
 * calibTwo.h
 *
 *  Created on: 2018年1月31日
 *      Author: hq
 */

#ifndef CALIBTWO_H_
#define CALIBTWO_H_

#include "calib.h"
#include "Const.h"

const CvMat *cameraMatrixL = (CvMat*)cvLoad(matrixXMLNameL.c_str());
const CvMat *distCoeffL = (CvMat*)cvLoad(distXMLNameL.c_str());
const CvMat *cameraMatrixR = (CvMat*)cvLoad(matrixXMLNameR.c_str());
const CvMat *distCoeffR = (CvMat*)cvLoad(distXMLNameR.c_str());

#endif /* CALIBTWO_H_ */
