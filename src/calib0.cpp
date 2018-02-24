////============================================================================
//// Name        : calib.cpp
//// Author      : hq
//// Version     :
//// Copyright   : Your copyright notice
//// Description : camera calibration
////============================================================================
//
//#include "calib.h"
//#include "Const.h"
//
//using namespace std;
//using namespace cv;
//
//int board_n = board_w * board_h;
//
//IplImage *getImage(IplImage *img, bool isLeft){
//	Mat Image = cvarrToMat(img, true);
//	Mat ImgTmp = Image(Range::all(), Range(1,Image.cols/2));
//	if(!isLeft){
//		ImgTmp = Image(Range::all(), Range(Image.cols/2,Image.cols));
//	}
//	IplImage imgTmp = (IplImage)ImgTmp;
//	IplImage *image = cvCloneImage(&imgTmp);
//	return image;
//}
//
////分别各自标定
//void startCalibTwo(){
//	int countL = 0;
//	int countR = 0;
//	int successes = 0;
//	int frame = 0;
//
//	vector<string> imageNames[2];
//	vector<CvPoint2D32f> imgPoints[2];
//	vector<int> npoints;
//	vector<uchar> active[2];
//	vector<CvPoint3D32f> objPoints;
//	vector<CvPoint2D32f> cornersL(board_n);
//	vector<CvPoint2D32f> cornersR(board_n);
//
//	IplImage* gray_imageL = cvCreateImage(imgSize, 8, 1);
//	IplImage* gray_imageR = cvCreateImage(imgSize, 8, 1);
//
//	CvMat* M1 = (CvMat*)cvLoad(matrixXMLNameL.c_str());
//	CvMat* D1 = (CvMat*)cvLoad(distXMLNameL.c_str());
//	CvMat* M2 = (CvMat*)cvLoad(matrixXMLNameR.c_str());
//	CvMat* D2 = (CvMat*)cvLoad(distXMLNameR.c_str());
//
//	double R[3][3], T[3], E[3][3], F[3][3];
//	CvMat _R = cvMat(3, 3, CV_64F, R );
//	CvMat _T = cvMat(3, 1, CV_64F, T );
//	CvMat _E = cvMat(3, 3, CV_64F, E );
//	CvMat _F = cvMat(3, 3, CV_64F, F );
//
//	while(successes < n_boards){
//		char imgName[10];
//		vector<CvPoint2D32f>& pts1 = imgPoints[0];
//		vector<CvPoint2D32f>& pts2 = imgPoints[1];
//
//		sprintf(imgName, "L%d.jpg", successes+1);
//		IplImage *imageL = cvLoadImage(imgName);
//		imageNames[0].push_back(imgName);
//		sprintf(imgName, "R%d.jpg", successes+1);
//		IplImage *imageR = cvLoadImage(imgName);
//		imageNames[1].push_back(imgName);
//
//		int foundL = cvFindChessboardCorners(imageL, size, &cornersL[0], &countL,
//				CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_NORMALIZE_IMAGE);
//		int foundR = cvFindChessboardCorners(imageR, size, &cornersR[0], &countR,
//				CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_NORMALIZE_IMAGE);
//		cvDrawChessboardCorners(imageL, size, &cornersL[0], countL, foundL);
//		cvDrawChessboardCorners(imageR, size, &cornersR[0], countR, foundR);
//		cvShowImage(lWindowName.c_str(), imageL);
//		cvShowImage(rWindowName.c_str(), imageR);
//
//		int N1 = pts1.size();
//		int N2 = pts2.size();
//		pts1.resize(N1+board_n, cvPoint2D32f(0, 0));
//		pts2.resize(N2+board_n, cvPoint2D32f(0, 0));
//		active[0].push_back((uchar)foundL);
//		active[1].push_back((uchar)foundR);
//
//		cvCvtColor(imageL, gray_imageL, CV_BGR2GRAY);
//		cvCvtColor(imageR, gray_imageR, CV_BGR2GRAY);
//
//		if(foundL && foundR){
//			cvFindCornerSubPix(gray_imageL, &cornersL[0], countL, cvSize(11, 11), cvSize(-1, -1),
//						cvTermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1));
//			cvFindCornerSubPix(gray_imageR, &cornersR[0], countR, cvSize(11, 11), cvSize(-1, -1),
//							cvTermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1));
//			copy(cornersL.begin(), cornersL.end(), pts1.begin() + N1);
//			copy(cornersR.begin(), cornersR.end(), pts2.begin() + N2);
//			successes++;
//		}
//		cvReleaseImage(&imageL);
//		cvReleaseImage(&imageR);
//	}
//
//	//计算objPoints
//	objPoints.resize(successes*board_n);
//	for(int i=0; i<board_h; i++)
//		for(int j=0; j<board_w; j++)
//			objPoints[i*board_w + j] = CvPoint3D32f(i*squareSize, j*squareSize, 0);
//	for(int i=0; i<successes; i++)
//		copy(objPoints.begin(), objPoints.begin()+board_n, objPoints.begin()+i*board_n);
//
//	npoints.resize(successes, board_n);
//	CvMat _objPoints = cvMat(1, successes*board_n, CV_32FC3, &objPoints[0]);
//	CvMat _imgPoints1 = cvMat(1, successes*board_n, CV_32FC2, &imgPoints[0][0]);
//	CvMat _imgPoints2 = cvMat(1, successes*board_n, CV_32FC2, &imgPoints[1][0]);
//	CvMat _npoints = cvMat(1, npoints.size(), CV_32SC1, &npoints[0]);
//
//	bool stereoCalib = cvStereoCalibrate(&_objPoints, &_imgPoints1, &_imgPoints2,
//				&_npoints, M1, D1, M2, D2, imgSize,
//				&_R, &_T, &_E, &_F, CV_CALIB_USE_INTRINSIC_GUESS,
//				cvTermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, 100, 1e-5));
//
//	imgPoints[0].resize(successes*board_n);
//	imgPoints[1].resize(successes*board_n);
//	_imgPoints1 = cvMat(1, successes*board_n, CV_32FC2, &imgPoints[0][0]);
//	_imgPoints2 = cvMat(1, successes*board_n, CV_32FC2, &imgPoints[1][0]);
//
//	vector<CvPoint3D32f> lines[2];
//	lines[0].resize(successes*board_n);
//	lines[1].resize(successes*board_n);
//	CvMat _L1 = cvMat(1, successes*board_n, CV_32FC3, &lines[0][0]);
//	CvMat _L2 = cvMat(1, successes*board_n, CV_32FC3, &lines[1][0]);
//
//	cvUndistortPoints( &_imgPoints1, &_imgPoints1, M1, D1, 0, M1 );
//    cvUndistortPoints( &_imgPoints2, &_imgPoints2, M2, D2, 0, M2 );
//    cvComputeCorrespondEpilines( &_imgPoints1, 1, &_F, &_L1 );
//    cvComputeCorrespondEpilines( &_imgPoints2, 2, &_F, &_L2 );
//
//    double avgErr = 0;
//	for(int i = 0; i < successes*board_n; i++ )
//	{
//		double err = fabs(imgPoints[0][i].x*lines[1][i].x +
//			imgPoints[0][i].y*lines[1][i].y + lines[1][i].z)
//			+ fabs(imgPoints[1][i].x*lines[0][i].x +
//			imgPoints[1][i].y*lines[0][i].y + lines[0][i].z);
//		avgErr += err;
//	}
//	printf( "avg err = %g\n", avgErr/(successes*board_n) );
//
//	cvShowImage(rWindowName.c_str(), gray_imageR);
//	cvReleaseImage(&gray_imageL);
//	cvReleaseImage(&gray_imageR);
//
////		cvStereoRectify(cameraMatrixL, cameraMatrixR, distCoeffsL, distCoeffsR,
////				size, R, T, Rl, Rr, Pl, Pr, Q, CV_CALIB_ZERO_DISPARITY);
////
////		IplImage* mapLx = cvCreateImage(cvGetSize(imageL), IPL_DEPTH_32F, 1);
////		IplImage* mapLy = cvCreateImage(cvGetSize(imageL), IPL_DEPTH_32F, 1);
////
////		IplImage* mapRx = cvCreateImage(cvGetSize(imageR), IPL_DEPTH_32F, 1);
////		IplImage* mapRy = cvCreateImage(cvGetSize(imageR), IPL_DEPTH_32F, 1);
////
////		cvInitUndistortRectifyMap(cameraMatrixL, distCoeffsL, Rl, Pr, mapLx, mapLy);
////
////		cvInitUndistortRectifyMap(cameraMatrixR, distCoeffsR, Rr, Pr, mapRx, mapRy);
////
////		IplImage* rectifyImgL = cvCreateImage(cvGetSize(imageL), 8, 1);
////		IplImage* rectifyImgR = cvCreateImage(cvGetSize(imageL), 8, 1);
////
////		cvCvtColor(gray_imageL, rectifyImgL, CV_BGR2GRAY);
////		cvCvtColor(gray_imageR, rectifyImgR, CV_BGR2GRAY);
////
////		cvRemap(rectifyImgL, rectifyImgL, mapLx, mapLy, CV_INTER_LINEAR);
////		cvRemap(rectifyImgR, rectifyImgR, mapRx, mapRy, CV_INTER_LINEAR);
////
////		cvShowImage(lWindowName.c_str(), rectifyImgL);
////		cvShowImage(rWindowName.c_str(), rectifyImgR);
////
////		cvSave("R.xml", R);
////		cvSave("T.xml", T);
////		cvSave("Rl.xml", Rl);
////		cvSave("Rr.xml", Rr);
////		cvSave("Pl.xml", Pl);
////		cvSave("Pr.xml", Pr);
////		cvSave("Q.xml", Q);
//
//}
//
//void startCalib(CvCapture *cap){
//	printf("开始标定\n");
//
//	int countL = 0;
//	int countR = 0;
//	int successes = 0;
//	int step, frame = 0;
//
//	char imgNameL[10] = {0};
//	char imgNameR[10] = {0};
//
//	IplImage *image = cvQueryFrame(cap);
//	IplImage *imageL = getImage(image, true);
//	IplImage *imageR = getImage(image, false);
//
//	CvPoint2D32f* cornersL = new CvPoint2D32f[ board_n ];
//	IplImage* gray_imageL = cvCreateImage(cvGetSize(imageL), 8, 1);
//	CvMat* image_pointsL = cvCreateMat(n_boards*board_n, 2, CV_32FC1);
//	CvMat* object_points = cvCreateMat(n_boards*board_n, 3, CV_32FC1);
//	CvMat* point_counts = cvCreateMat(n_boards, 1, CV_32SC1);
//	CvMat* intrinsic_matrixL = cvCreateMat(3, 3, CV_32FC1);
//	CvMat* distortion_coeffsL = cvCreateMat(5, 1, CV_32FC1);
//
//	CvPoint2D32f* cornersR = new CvPoint2D32f[ board_n ];
//	IplImage* gray_imageR = cvCreateImage(cvGetSize(imageR), 8, 1);
//	CvMat* image_pointsR = cvCreateMat(n_boards*board_n, 2, CV_32FC1);
//	CvMat* intrinsic_matrixR = cvCreateMat(3, 3, CV_32FC1);
//	CvMat* distortion_coeffsR = cvCreateMat(5, 1, CV_32FC1);
//
//	while(successes < n_boards){
//		if(frame++ % board_dt == 0){
//			printf("get a image\n");
//			int foundL = cvFindChessboardCorners(imageL, size, cornersL, &countL,
//					CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);
//
//			int foundR = cvFindChessboardCorners(imageR, size, cornersR, &countR,
//					CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);
//
//			cvCvtColor(imageL, gray_imageL, CV_BGR2GRAY);
//			cvCvtColor(imageR, gray_imageR, CV_BGR2GRAY);
//
//			cvFindCornerSubPix(gray_imageL, cornersL, countL, cvSize(11, 11), cvSize(-1, -1),
//						cvTermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1));
//			cvFindCornerSubPix(gray_imageR, cornersR, countR, cvSize(11, 11), cvSize(-1, -1),
//							cvTermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1));
//
//			cvDrawChessboardCorners(imageL, size, cornersL, countL, foundL);
//			cvDrawChessboardCorners(imageR, size, cornersR, countR, foundR);
//
//			if( countL == board_n && countR == board_n){
//				cvShowImage(lShowWindowName.c_str(), imageL);
//				cvShowImage(rShowWindowName.c_str(), imageR);
//
//				sprintf(imgNameL, "L%d.jpg", successes);
//				sprintf(imgNameR, "R%d.jpg", successes);
//
//				cvSaveImage(imgNameL, imageL);
//				cvSaveImage(imgNameR, imageR);
//				cvSaveImage(imgNameL+1, image);
//
//				step = successes*board_n;
//				for( int i=step, j=0; j<board_n; ++i,++j ){
//					CV_MAT_ELEM(*image_pointsL, float, i, 0) = cornersL[j].x;
//					CV_MAT_ELEM(*image_pointsL, float, i, 1) = cornersL[j].y;
//
//					CV_MAT_ELEM(*image_pointsR, float, i, 0) = cornersR[j].x;
//					CV_MAT_ELEM(*image_pointsR, float, i, 1) = cornersR[j].y;
//
//					CV_MAT_ELEM(*object_points, float, i, 0) = j/board_w;
//					CV_MAT_ELEM(*object_points, float, i, 1) = j%board_w;
//					CV_MAT_ELEM(*object_points, float, i, 2) = 0.0f;
//				}
//
//				CV_MAT_ELEM(*point_counts, int, successes, 0) = board_n;
//
//				successes++;
//			}else{
//				printf("this image is not good!\n");
//			}
//		}
//
//		int c = cvWaitKey(15);
//		if(c == 'p'){
//			c = 0;
//			while(c != 'p' && c != 27){
//				c = cvWaitKey(250);
//			}
//		}
//
//		if(c == 27)
//			return;
//
//		image = cvQueryFrame(cap);
//		imageL = getImage(image, true);
//		imageR = getImage(image, false);
//		cvShowImage(lWindowName.c_str(), imageL);
//		cvShowImage(rWindowName.c_str(), imageR);
//	}
//
//		CvMat* image_pointsL2 = cvCreateMat(successes*board_n, 2, CV_32FC1);
//		CvMat* image_pointsR2 = cvCreateMat(successes*board_n, 2, CV_32FC1);
//
//		CvMat* point_counts2 = cvCreateMat(successes, 1, CV_32SC1);
//		CvMat* object_points2 = cvCreateMat(successes*board_n, 3, CV_32FC1);
//
//		for(int i=0; i<successes*board_n; ++i){
//			CV_MAT_ELEM(*image_pointsL2, float, i, 0) = CV_MAT_ELEM(*image_pointsL, float, i, 0);
//			CV_MAT_ELEM(*image_pointsL2, float, i, 1) = CV_MAT_ELEM(*image_pointsL, float, i, 1);
//
//			CV_MAT_ELEM(*image_pointsR2, float, i, 0) = CV_MAT_ELEM(*image_pointsR, float, i, 0);
//			CV_MAT_ELEM(*image_pointsR2, float, i, 1) = CV_MAT_ELEM(*image_pointsR, float, i, 1);
//
//			CV_MAT_ELEM(*object_points2, float, i, 0) = CV_MAT_ELEM(*object_points, float, i, 0);
//			CV_MAT_ELEM(*object_points2, float, i, 1) = CV_MAT_ELEM(*object_points, float, i, 1);
//			CV_MAT_ELEM(*object_points2, float, i, 2) = CV_MAT_ELEM(*object_points, float, i, 2);
//		}
//
//		for(int i=0; i<successes; ++i){
//			CV_MAT_ELEM(*point_counts2, int, i, 0) = CV_MAT_ELEM(*point_counts, int, i, 0);
//		}
//
//		cvReleaseMat(&object_points);
//		cvReleaseMat(&image_pointsL);
//		cvReleaseMat(&image_pointsR);
//
//		CV_MAT_ELEM(*intrinsic_matrixL, float, 0, 0) = 1.0f;
//		CV_MAT_ELEM(*intrinsic_matrixL, float, 0, 1) = 1.0f;
//
//		CV_MAT_ELEM(*intrinsic_matrixR, float, 0, 0) = 1.0f;
//		CV_MAT_ELEM(*intrinsic_matrixR, float, 0, 1) = 1.0f;
//
//		cvCalibrateCamera2(object_points2, image_pointsL2, point_counts2,
//				cvGetSize(imageL), intrinsic_matrixL, distortion_coeffsL,
//				NULL, NULL, 0);
//
//		cvCalibrateCamera2(object_points2, image_pointsR2, point_counts2,
//				cvGetSize(imageR), intrinsic_matrixR, distortion_coeffsR,
//				NULL, NULL, 0);
//
//		cvSave(matrixXMLNameL.c_str(), intrinsic_matrixL);
//		cvSave(distXMLNameL.c_str(), distortion_coeffsL);
//
//		cvSave(matrixXMLNameR.c_str(), intrinsic_matrixR);
//		cvSave(distXMLNameR.c_str(), distortion_coeffsR);
//
//		CvMat *cameraMatrixL = (CvMat*)cvLoad(matrixXMLNameL.c_str());
//		CvMat *distCoeffsL = (CvMat*)cvLoad(distXMLNameL.c_str());
//
//		CvMat *cameraMatrixR = (CvMat*)cvLoad(matrixXMLNameR.c_str());
//		CvMat *distCoeffsR = (CvMat*)cvLoad(distXMLNameR.c_str());
//
//		IplImage* mapLx = cvCreateImage( cvGetSize(imageL), IPL_DEPTH_32F, 1 );
//		IplImage* mapLy = cvCreateImage( cvGetSize(imageL), IPL_DEPTH_32F, 1 );
//
//		IplImage* mapRx = cvCreateImage( cvGetSize(imageR), IPL_DEPTH_32F, 1 );
//		IplImage* mapRy = cvCreateImage( cvGetSize(imageR), IPL_DEPTH_32F, 1 );
//
//		cvInitUndistortMap(cameraMatrixL, distCoeffsL, mapLx, mapLy);
//		cvInitUndistortMap(cameraMatrixR, distCoeffsR, mapRx, mapRy);
//
//		  // Just run the camera to the screen, now showing the raw and
//		  // the undistorted image.
//		  //
//		  while(image) {
//		    IplImage *tL = cvCloneImage(imageL);
//		    IplImage *tR = cvCloneImage(imageR);
//
//		    cvShowImage(lWindowName.c_str(), imageL ); // Show raw image
//			cvShowImage( rWindowName.c_str(), imageR ); // Show raw image
//
//		    cvRemap( tL, imageL, mapLx, mapLy );     // Undistort image
//			cvRemap( tR, imageR, mapRx, mapRy );     // Undistort image
//
//		    cvReleaseImage(&tL);
//			cvReleaseImage(&tR);
//
//		    cvShowImage(lShowWindowName.c_str(), imageL ); // Show corrected image
//			cvShowImage( rShowWindowName.c_str(), imageR ); // Show corrected image
//
//		    //Handle pause/unpause and ESC
//		    int c = cvWaitKey(15);
//		    if(c == 'p'){
//		       c = 0;
//		       while(c != 'p' && c != 27){
//		            c = cvWaitKey(250);
//		       }
//		    }
//		    if(c == 27)
//		        break;
//
//		    image = cvQueryFrame(cap);
//		    imageL = getImage(image, true);
//		    imageR = getImage(image, false);
//		  }
//}
//
