//============================================================================
// Name        : main.cpp
// Author      : hq
// Version     :
// Copyright   : Your copyright notice
// Description : open camera in opencv, create window in gtk
//============================================================================

#include "calib.h"
#include "Const.h"
#include <time.h>
#include <string.h>
#include <gtk/gtk.h>

using namespace std;
using namespace cv;
using std::string;

bool writeInit = false;
bool isCalib = false;
string *filenameL = new string[n_boards];
string *filenameR = new string[n_boards];

//class TwoEyesCamera{
//	public:
//		CvCapture *cap = NULL;
//		void open();
//		void close();
//};
//
typedef struct {
	CvCapture *capture;
}CallbackData;
//
//void TwoEyesCamera::open(){
//	cout << "aaa" << endl;
////
////	cap.open(0);
////	cap.set(CV_CAP_PROP_FRAME_HEIGHT, height);
////	cap.set(CV_CAP_PROP_FRAME_WIDTH, width*2);
////
////	if(!cap.isOpened()){
////		printf("open failed\n");
////		return;
////	}
//
//	cap = cvCreateCameraCapture(-1);
//	assert(cap);
//	cvSetCaptureProperty(cap, CV_CAP_PROP_FRAME_HEIGHT, height);
//	cvSetCaptureProperty(cap, CV_CAP_PROP_FRAME_WIDTH, width*2);
//
//	if(!cap){
//		printf("open failed\n");
//	}
//}
//
//void TwoEyesCamera::close(){
//	cvReleaseCapture(&cap);
////	cap.release();
//	printf("close two-eyes camera\n");
//}

//void getLocalTime(char *localTime){
//	time_t t = time(NULL);
//	struct tm *t1 = localtime(&t);
//	strftime(localTime, 64, "_%Y%m%d_%H%M%S", t1);
//	printf("local time is %s\n", localTime);
//}
//
//void setOutfileName(char* name){
//	char time[20] = {0};
//	getLocalTime(time);
//	strncpy(name, time, strlen(time));
//	strcat(name, ".jpg");
//}
//
//void writeCtrl(gpointer frame){
//	while(true){
//		sleep(outTime);
//		writeInit = true;
//	}
//}

void openTwoEyes(CvCapture *cap){

	cap = cvCreateCameraCapture(-1);
	assert(cap);
	cvSetCaptureProperty(cap, CV_CAP_PROP_FRAME_HEIGHT, height);
	cvSetCaptureProperty(cap, CV_CAP_PROP_FRAME_WIDTH, width*2);

	if(!cap){
		printf("open failed\n");
	}
//	if(!camera->cap.isOpened()){
//		printf("capture open failed!\n");
//		return;
//	}
//	GThread *write = g_thread_new("write", (GThreadFunc)writeCtrl, NULL);

	while(!isCalib){
		IplImage *img = cvQueryFrame(cap);

		Mat Image = cvarrToMat(img, true);
		Mat ImgTmpL = Image(Range::all(), Range(1,Image.cols/2));

		Mat ImgTmpR = Image(Range::all(), Range(Image.cols/2,Image.cols));

		cv::imshow(lWindowName, ImgTmpL);
		cv::imshow(rWindowName, ImgTmpR);

		int c = cvWaitKey(100);

		if(27 == char(c)){
			break;
		}
	}

	if(isCalib){
		startCalib(cap);
	}
//	if()
//	while(camera->cap.isOpened()) {
//		printf("open success\n");
//		cv::Mat frame;
//		camera->cap >> frame;
//		cv::Mat leftFrame;
//		cv::Mat rightFrame;
//
//		int col = frame.cols;
//		leftFrame = frame(Range::all(),Range(1,col/2));
//		rightFrame = frame(Range::all(),Range(col/2,col));

//		if(writeInit){
//			writeInit = false;
//			char fileNameL[25] = {0};
//			char fileNameR[25] = {0};
//			fileNameL[0] = 'L';
//			fileNameR[0] = 'R';
//			setOutfileName(fileNameL+1);
//			setOutfileName(fileNameR+1);
//			printf("file_nameL is %s\n", fileNameL);
//			printf("file_nameR is %s\n", fileNameR);
//
//			cv::imwrite(fileNameL, leftFrame);
//			cv::imwrite(fileNameR, rightFrame);
//		}


//		cv::imshow(lWindowName, leftFrame);
//		cv::imshow(rWindowName, rightFrame);
//
//		int c = cvWaitKey(40);
//
//		if(27 == char(c)){
//			break;
//		}
//	}
}

void toggle_open(GtkWidget *widget, gpointer data){
	CallbackData *cData = (CallbackData *)data;
	printf("open camera\n");
	openTwoEyes(cData->capture);
}

void calibTwo(GtkWidget *widget, gpointer data){
//	CallbackData *cData = (CallbackData *)data;
//	if((cData->capture)){
//		printf("start calib");
//		startCalib(cData->capture);
//	}
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))){
		gtk_button_set_label(GTK_BUTTON(widget), "标定ing");
		isCalib = true;
	}
}

void quit(GtkWidget *widget, gpointer data){
	CallbackData *cData = (CallbackData *)data;
	cvReleaseCapture(&cData->capture);
	printf("destroy\n");
	gtk_main_quit();
}

GtkWidget * main_window(CvCapture *capture){
	GtkWidget *window;
	GtkWidget *lWindow;
	GtkWidget *rWindow;
	GtkWidget *lCWindow;
	GtkWidget *rCWindow;
	GtkWidget *box;
	GtkWidget *mBox;
	GtkWidget *bBox;
	GtkWidget *openButton;
	GtkWidget *calibButtonL;
	GtkWidget *calibButtonR;
	GtkWidget *calibButton;
	GtkWidget *hBox;
	GtkWidget *lBox;
	GtkWidget *rBox;
	GtkWidget *lLabel;
	GtkWidget *rLabel;
	GtkWidget *hCBox;
	GtkWidget *lCBox;
	GtkWidget *rCBox;
	GtkWidget *lCLabel;
	GtkWidget *rCLabel;

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_size_request(window, 1500, 1030);
	gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
	gtk_window_set_title(GTK_WINDOW(window), "Camera");
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_container_set_border_width(GTK_CONTAINER(window), 20);

	box = gtk_hbox_new(FALSE, 10);
	gtk_container_add(GTK_CONTAINER(window), box);

	mBox = gtk_vbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box), mBox, TRUE, TRUE, 0);

	bBox = gtk_vbox_new(TRUE, 10);
	gtk_box_pack_start(GTK_BOX(box), bBox, TRUE, TRUE, 0);

	calibButton = gtk_check_button_new_with_label("标定");
	gtk_box_pack_start(GTK_BOX(bBox), calibButton, TRUE, TRUE, 0);

	openButton = gtk_button_new_with_label("打开摄像头");
	//gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(openButton), FALSE);
	gtk_box_pack_start(GTK_BOX(bBox), openButton, TRUE, TRUE, 0);

	hBox = gtk_hbox_new(TRUE, 10);
	gtk_box_pack_start(GTK_BOX(mBox), hBox, TRUE, TRUE, 0);

	namedWindow(lWindowName);
	lLabel = gtk_label_new("左视图");
	lBox = gtk_vbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hBox), lBox, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(lBox), lLabel, TRUE, TRUE, 0);

	lWindow = (GtkWidget *)cvGetWindowHandle(lWindowName.c_str());
	GdkWindow *parent = gtk_widget_get_parent_window(lWindow);
	gtk_widget_set_size_request(lWindow, width, height);
	gtk_widget_reparent(lWindow, lBox);
	gdk_window_destroy(parent);

	namedWindow(rWindowName);
	rLabel = gtk_label_new("右视图");
	rBox = gtk_vbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hBox), rBox, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(rBox), rLabel, TRUE, TRUE, 0);

	rWindow = (GtkWidget *)cvGetWindowHandle(rWindowName.c_str());
	parent = gtk_widget_get_parent_window(rWindow);
	gtk_widget_set_size_request(rWindow, width, height);
	gtk_widget_reparent(rWindow, rBox);
	gdk_window_destroy(parent);

	hCBox = gtk_hbox_new(TRUE, 10);
	gtk_box_pack_start(GTK_BOX(mBox), hCBox, TRUE, TRUE, 0);

	namedWindow(lShowWindowName);
	lCLabel = gtk_label_new("左展示图");
	lCBox = gtk_vbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hCBox), lCBox, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(lCBox), lCLabel, TRUE, TRUE, 0);

	lCWindow = (GtkWidget *)cvGetWindowHandle(lShowWindowName.c_str());
	parent = gtk_widget_get_parent_window(lCWindow);
	gtk_widget_set_size_request(lCWindow, width, height);
	gtk_widget_reparent(lCWindow, lCBox);
	gdk_window_destroy(parent);

	namedWindow(rShowWindowName);
	rCLabel = gtk_label_new("右展示图");
	rCBox = gtk_vbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hCBox), rCBox, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(rCBox), rCLabel, TRUE, TRUE, 0);

	rCWindow = (GtkWidget *)cvGetWindowHandle(rShowWindowName.c_str());
	parent = gtk_widget_get_parent_window(rCWindow);
	gtk_widget_set_size_request(rCWindow, width, height);
	gtk_widget_reparent(rCWindow, rCBox);
	gdk_window_destroy(parent);


	CallbackData data;
	data.capture = capture;

	g_signal_connect(GTK_OBJECT(openButton), "clicked", G_CALLBACK(toggle_open), &data);
	g_signal_connect(GTK_OBJECT(calibButton), "clicked", G_CALLBACK(calibTwo), &data);
	g_signal_connect(GTK_OBJECT(window), "destroy", G_CALLBACK(quit), &data);

	return window;
}

int main(int argc, char *argv[]){
	GtkWidget *window;
//	TwoEyesCamera camera;
	CvCapture *cap = NULL;
	gtk_init(&argc, &argv);

	window = main_window(cap);
	gtk_widget_show_all(window);

	gtk_main();
	return 0;
}
