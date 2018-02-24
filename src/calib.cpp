/*
 * calib.cpp
 *
 *  Created on: 2018年2月7日
 *      Author: hq
 */

//============================================================================
// Name        : calib.cpp
// Author      : hq
// Version     :
// Copyright   : Your copyright notice
// Description : camera calibration
//============================================================================

#include "calib.h"
#include "Const.h"

using namespace std;
using namespace cv;

int board_n = board_w * board_h;

Mat getImage(IplImage *img, bool isLeft){
	Mat Image = cvarrToMat(img, true);
	Mat ImgTmp = Image(Range::all(), Range(1,Image.cols/2));
	if(!isLeft){
		ImgTmp = Image(Range::all(), Range(Image.cols/2,Image.cols));
	}
//	IplImage imgTmp = (IplImage)ImgTmp;
//	IplImage *image = cvCloneImage(&imgTmp);
	return ImgTmp;
}

static double calReprojectionErrors(
        const vector<vector<Point3f> >& objectPoints,
        const vector<vector<Point2f> >& imagePoints,
        const vector<Mat>& rvecs, const vector<Mat>& tvecs,
        const Mat& M, const Mat& D, vector<float>& perViewErrors ){

    vector<Point2f> imagePoints2;
    int i, totalPoints = 0;
    double totalErr = 0, err;
    perViewErrors.resize(objectPoints.size());

    for( i = 0; i < (int)objectPoints.size(); i++ ){
        projectPoints(Mat(objectPoints[i]), rvecs[i], tvecs[i],
                      M, D, imagePoints2);
        err = norm(Mat(imagePoints[i]), Mat(imagePoints2), NORM_L2);
        int n = (int)objectPoints[i].size();
        perViewErrors[i] = (float)std::sqrt(err*err/n);
        totalErr += err*err;
        totalPoints += n;
    }

    return std::sqrt(totalErr/totalPoints);
}
bool calCalibration(vector<vector<Point2f> > imagePoints,
		Size ImgSize, Size CBSize, int flags, Mat& M, Mat& D,
		vector<Mat>& rvecs, vector<Mat>& tvecs,
		vector<float>& reprojErrs, double& totalAvgErr){

	M = Mat::eye(3, 3, CV_64F);
	D = Mat::zeros(8, 1, CV_64F);

    vector<vector<Point3f> > objectPoints(1);

    for(int i=0; i<CBSize.height; i++)
    	for(int j=0; j<CBSize.width; j++)
    		objectPoints[0].push_back(Point3f(float(j*squareSize), float(i*squareSize), 0));
    objectPoints.resize(imagePoints.size(),objectPoints[0]);

	double rms = calibrateCamera(objectPoints, imagePoints, ImgSize, M,
			D, rvecs, tvecs, flags|CALIB_FIX_K4|CALIB_FIX_K5);
			///*|CALIB_FIX_K3*/|CALIB_FIX_K4|CALIB_FIX_K5);
	printf("RMS error reported by calibrateCamera: %g\n", rms);

	bool ok = checkRange(M) && checkRange(D);

	totalAvgErr = calReprojectionErrors(objectPoints, imagePoints,
		rvecs, tvecs, M, D, reprojErrs);

	return ok;
}

static void saveCameraParams( const string& filename,
		Size ImgSize, Size CBSize, float SQSize, int flags,
		const Mat& M, const Mat& D, const vector<Mat>& rvecs,
		const vector<Mat>& tvecs, const vector<float>& reprojErrs,
		const vector<vector<Point2f> >& imagePoints, double totalAvgErr){

	FileStorage fs( filename, FileStorage::WRITE );

    time_t tt;
    time( &tt );
    struct tm *t2 = localtime( &tt );
    char buf[1024];
    strftime( buf, sizeof(buf)-1, "%c", t2 );

    fs << "calibration_time" << buf;

    if( !rvecs.empty() || !reprojErrs.empty() )
        fs << "nframes" << (int)std::max(rvecs.size(), reprojErrs.size());
    fs << "image_width" << ImgSize.width;
    fs << "image_height" << ImgSize.height;
    fs << "board_width" << CBSize.width;
    fs << "board_height" << CBSize.height;
    fs << "square_size" << SQSize;

    if( flags != 0 ){
        sprintf( buf, "flags: %s%s%s%s",
            flags & CALIB_USE_INTRINSIC_GUESS ? "+use_intrinsic_guess" : "",
            flags & CALIB_FIX_ASPECT_RATIO ? "+fix_aspectRatio" : "",
            flags & CALIB_FIX_PRINCIPAL_POINT ? "+fix_principal_point" : "",
            flags & CALIB_ZERO_TANGENT_DIST ? "+zero_tangent_dist" : "" );
        //cvWriteComment( *fs, buf, 0 );
    }

    fs << "flags" << flags;

    fs << "camera_matrix" << M;
    fs << "distortion_coefficients" << D;

    fs << "avg_reprojection_error" << totalAvgErr;
    if( !reprojErrs.empty() )
        fs << "per_view_reprojection_errors" << Mat(reprojErrs);

    if( !rvecs.empty() && !tvecs.empty() ){
        CV_Assert(rvecs[0].type() == tvecs[0].type());
        Mat bigmat((int)rvecs.size(), 6, rvecs[0].type());
        for( int i = 0; i < (int)rvecs.size(); i++ ){
            Mat r = bigmat(Range(i, i+1), Range(0,3));
            Mat t = bigmat(Range(i, i+1), Range(3,6));

            CV_Assert(rvecs[i].rows == 3 && rvecs[i].cols == 1);
            CV_Assert(tvecs[i].rows == 3 && tvecs[i].cols == 1);
            //*.t() is MatExpr (not Mat) so we can use assignment operator
            r = rvecs[i].t();
            t = tvecs[i].t();
        }
        //cvWriteComment( *fs, "a set of 6-tuples (rotation vector + translation vector) for each view", 0 );
        fs << "extrinsic_parameters" << bigmat;
    }

    if( !imagePoints.empty() ){
        Mat imagePtMat((int)imagePoints.size(), (int)imagePoints[0].size(), CV_32FC2);
        for( int i = 0; i < (int)imagePoints.size(); i++ )
        {
            Mat r = imagePtMat.row(i).reshape(2, imagePtMat.cols);
            Mat imgpti(imagePoints[i]);
            imgpti.copyTo(r);
        }
        fs << "image_points" << imagePtMat;
    }
}


bool calAndSaveCalibData(const vector<vector<Point2f> >& imagePoints1,
        const vector<vector<Point2f> >& imagePoints2,
        Size ImgSize, Size CBSize, int flags,
		Mat& M1, Mat& M2, Mat& D1, Mat& D2){

	string filename1 = "./tmp/LcameraParams.yml";
	string filename2 = "./tmp/RcameraParams.yml";

	vector<Mat> rvecs[2],tvecs[2];
	vector<float> reprojErrs[2];
	double totalAvgErr1 = 0;
	double totalAvgErr2 = 0;

	bool ok1 = calCalibration(imagePoints1, ImgSize, CBSize,
			flags, M1, D1, rvecs[0], tvecs[0], reprojErrs[0], totalAvgErr1);
	bool ok2 = calCalibration(imagePoints2, ImgSize, CBSize,
			flags, M2, D2, rvecs[1], tvecs[1], reprojErrs[1], totalAvgErr2);

	if(ok1 && ok2){
		saveCameraParams(filename1, ImgSize, CBSize, squareSize, flags, M1, D1,
				rvecs[0], tvecs[0], reprojErrs[0], imagePoints1, totalAvgErr1);
		saveCameraParams(filename2, ImgSize, CBSize, squareSize, flags, M2, D2,
				rvecs[1], tvecs[1], reprojErrs[1], imagePoints2, totalAvgErr2);
	}
	return ok1 && ok2;
}

double runStereoCalibrate(vector<vector<Point3f> > objectPoints,
		vector<vector<Point2f> > imagePoints1, vector<vector<Point2f> > imagePoints2,
        Mat& cameraMatrix1, Mat& distCoeffs1,
        Mat& cameraMatrix2, Mat& distCoeffs2,
		vector<string> imgList, Size imageSize, int nimages){

	double err = 0;
	int npoints = 0;
	vector<Vec3f> lines[2];
	Mat R, T, E, F;
	Mat R1, R2, P1, P2, Q;
	Mat H1, H2;
	Mat rmap[2][2];
	Rect validRoi[2];

	double rms = stereoCalibrate(objectPoints, imagePoints1, imagePoints2,
			cameraMatrix1, distCoeffs1, cameraMatrix2, distCoeffs2,
			imgSize, R, T, E, F, stereoFlags,
			TermCriteria(TermCriteria::COUNT+TermCriteria::EPS, 100, 1e-5) );
	printf("stereoCalibrate RMS error is %f\n", rms);

	for(int i=0; i<nimages; i++){
		int npt = (int)imagePoints1[i].size();
		Mat imgpt1, imgpt2;

		imgpt1 = Mat(imagePoints1[i]);
		imgpt2 = Mat(imagePoints2[i]);
		undistortPoints(imgpt1, imgpt1, cameraMatrix1, distCoeffs1, Mat(), cameraMatrix1);
		computeCorrespondEpilines(imgpt1, 1, F, lines[0]);
		undistortPoints(imgpt2, imgpt2, cameraMatrix2, distCoeffs2, Mat(), cameraMatrix2);
		computeCorrespondEpilines(imgpt2, 2, F, lines[1]);

		 for(int j = 0; j < npt; j++ ) {
			double errij = fabs(imagePoints1[i][j].x*lines[1][j][0] +
								imagePoints1[i][j].y*lines[1][j][1] + lines[1][j][2]) +
						   fabs(imagePoints2[i][j].x*lines[0][j][0] +
								imagePoints2[i][j].y*lines[0][j][1] + lines[0][j][2]);
			err += errij;
		}
		npoints += npt;
	}
	printf("average epipolar error is %f\n", err/npoints);

	stereoRectify(cameraMatrix1, distCoeffs1, cameraMatrix2, distCoeffs2,
			      imageSize, R, T, R1, R2, P1, P2, Q, CALIB_ZERO_DISPARITY,
				  1, imageSize, &validRoi[0], &validRoi[1]);

	 FileStorage fs("./tmp/stereoParams.yml", FileStorage::WRITE);
	if( fs.isOpened() ){
		fs << "M1" << cameraMatrix1 << "D1" << distCoeffs1 <<
			"M2" << cameraMatrix2 << "D2" << distCoeffs2;
		fs << "R" << R << "T" << T << "R1" << R1 << "R2" << R2 <<
				"P1" << P1 << "P2" << P2 << "Q" << Q;
		fs.release();
	} else
		printf("Error: can not save the stereo calibrate parameters\n");

	bool isVerticalStereo = fabs(P2.at<double>(1, 3)) > fabs(P2.at<double>(0, 3));

	vector<Point2f> allimgpt[2];
	for(int i=0; i<nimages; i++){
		std::copy(imagePoints1[i].begin(), imagePoints1[i].end(), back_inserter(allimgpt[0]));
		std::copy(imagePoints2[i].begin(), imagePoints2[i].end(), back_inserter(allimgpt[1]));
	}

	F = findFundamentalMat(Mat(allimgpt[0]), Mat(allimgpt[1]), FM_RANSAC, 0, 0);
	stereoRectifyUncalibrated(Mat(allimgpt[0]), Mat(allimgpt[1]), F, imageSize, H1, H2, 3);
	R1 = cameraMatrix1.inv()*H1*cameraMatrix1;
	R2 = cameraMatrix2.inv()*H2*cameraMatrix2;
	P1 = cameraMatrix1;
	P2 = cameraMatrix2;

	initUndistortRectifyMap(cameraMatrix1, distCoeffs1, R1, P1, imageSize, CV_16SC2,
			rmap[0][0], rmap[0][1]);
	initUndistortRectifyMap(cameraMatrix2, distCoeffs2, R2, P2, imageSize, CV_16SC2,
			rmap[1][0], rmap[1][1]);

	Mat canvas;
	char name[10];
	double sf;
	int w, h;
	if( !isVerticalStereo ){
		sf = 600./MAX(imageSize.width, imageSize.height);
		w = cvRound(imageSize.width*sf);
		h = cvRound(imageSize.height*sf);
		canvas.create(h, w*2, CV_8UC3);
	}else{
		sf = 300./MAX(imageSize.width, imageSize.height);
		w = cvRound(imageSize.width*sf);
		h = cvRound(imageSize.height*sf);
		canvas.create(h*2, w, CV_8UC3);
	}

	namedWindow("rectified");
	for(int i = 0; i < nimages; i++ ){
		for(int k = 0; k < 2; k++ ){
			Mat img = imread(imgList[i*2+k], 0), rimg, cimg;
			remap(img, rimg, rmap[k][0], rmap[k][1], INTER_LINEAR);
			cvtColor(rimg, cimg, COLOR_GRAY2BGR);
			Mat canvasPart = !isVerticalStereo ? canvas(Rect(w*k, 0, w, h)) : canvas(Rect(0, h*k, w, h));
			resize(cimg, canvasPart, canvasPart.size(), 0, 0, INTER_AREA);
			Rect vroi(cvRound(validRoi[k].x*sf), cvRound(validRoi[k].y*sf),
					  cvRound(validRoi[k].width*sf), cvRound(validRoi[k].height*sf));
			rectangle(canvasPart, vroi, Scalar(0,0,255), 3, 8);
		}

		if( !isVerticalStereo )
			for(int j = 0; j < canvas.rows; j += 16 )
				line(canvas, Point(0, j), Point(canvas.cols, j), Scalar(0, 255, 0), 1, 8);
		else
			for(int j = 0; j < canvas.cols; j += 16 )
				line(canvas, Point(j, 0), Point(j, canvas.rows), Scalar(0, 255, 0), 1, 8);
		imshow("rectified", canvas);
		sprintf(name, "./tmp/C%d.jpg", i);

		imwrite(name, canvas);
		char c = (char)waitKey();
		if( c == 27 || c == 'q' || c == 'Q' )
			break;
	}

	destroyWindow("rectified");
	return rms;
}

void startCalib(CvCapture *cap){
	printf("开始标定\n");
	int successes = 0;
	int frame = 0;

	vector<vector<Point3f> > objPoints;
	vector<vector<Point2f> > imgPoints[2];
	vector<int> npoints;
	vector<string> imageNames;

	Mat M[2],D[2];
	Mat image1,image2;
	Mat grayImage1,grayImage2;

	imgPoints[0].resize(n_boards);
	imgPoints[1].resize(n_boards);
	objPoints.resize(n_boards);

	IplImage *image = cvQueryFrame(cap);
	getImage(image, true).copyTo(image1);
	getImage(image, false).copyTo(image2);

	while(successes < n_boards){
		char imgName1[10] = {0};
		char imgName2[10] = {0};
		char imgName[10] = {0};

		if(frame++ % board_dt == 0){
			printf("get a image\n");
			vector<Point2f>& corners1 = imgPoints[0][successes];
			vector<Point2f>& corners2 = imgPoints[1][successes];

			bool found1 = findChessboardCorners(image1, boardSize, corners1,
					findCBCornerFlags);
			bool found2 = findChessboardCorners(image2, boardSize, corners2,
					findCBCornerFlags);

			cvtColor(image1, grayImage1, CV_BGR2GRAY);
			cvtColor(image2, grayImage2, CV_BGR2GRAY);

			printf("set imgPoints data\n");
			if( found1 && found2){
				cornerSubPix(grayImage1, corners1, Size(11,11), Size(-1,-1),
						cvTermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1));
				cornerSubPix(grayImage2, corners2, Size(11,11), Size(-1,-1),
						cvTermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1));
				drawChessboardCorners(image1, boardSize, corners1, found1);
				drawChessboardCorners(image2, boardSize, corners2, found2);

				sprintf(imgName, "./tmp/%d.jpg", successes);
				sprintf(imgName1, "./tmp/L%d.jpg", successes);
				sprintf(imgName2, "./tmp/R%d.jpg", successes);
				imageNames.push_back(imgName1);
				imageNames.push_back(imgName2);
				imshow(lShowWindowName.c_str(), image1);
				imshow(rShowWindowName.c_str(), image2);
				imwrite(imgName1, image1);
				imwrite(imgName2, image2);
				cvSaveImage(imgName, image);

				successes++;
			}else{
				printf("this image is not good!\n");
			}
		}

		char c = (char)cvWaitKey(500);
		if(c == 27 || c == 'q' || c == 'Q')
			exit(-1);

		image = cvQueryFrame(cap);
		getImage(image, true).copyTo(image1);
		getImage(image, false).copyTo(image2);
		imshow(lWindowName.c_str(), image1);
		imshow(rWindowName.c_str(), image2);
	}

	for(int i=0; i<successes; i++){
		for(int j=0; j<boardSize.height; j++)
			for(int k=0; k<boardSize.width; k++)
				objPoints[i].push_back(Point3f(k*squareSize, j*squareSize, 0));
	}

	//M[0] = initCameraMatrix2D(objPoints, imgPoints[0], imgSize, 0);
	//M[1] = initCameraMatrix2D(objPoints, imgPoints[1], imgSize, 0);

	bool ok = calAndSaveCalibData(imgPoints[0], imgPoints[1], imgSize, boardSize,
			calibFlags, M[0], M[1], D[0], D[1]);

	printf("undistort map\n");
	while(image) {
		Mat tL = image1.clone();
		Mat tR = image2.clone();

		printf("show raw image\n");
		imshow(lWindowName, image1); // Show raw image
		imshow(rWindowName, image2 ); // Show raw image
		undistort( tL, image1, M[0], D[0] );     // Undistort image
		undistort( tR, image2, M[1], D[1] );     // Undistort image

		printf("show corrected image\n");
		imshow(lShowWindowName, image1 ); // Show corrected image
		imshow(rShowWindowName, image2 ); // Show corrected image

		//Handle pause/unpause and ESC
		int c = cvWaitKey(15);
		if(c == 27)
			break;

		image = cvQueryFrame(cap);
		getImage(image, true).copyTo(image1);
		getImage(image, false).copyTo(image2);
	}

//		if(ok){
//			double rmx = runStereoCalibrate(objPoints, imgPoints[0], imgPoints[1],
//					M[0], D[0], M[1], D[1], imageNames, imgSize, successes);
//		}

}




