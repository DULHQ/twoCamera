#include <iostream>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <opencv2/opencv.hpp>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <gtk/gtk.h>

using namespace std;
using namespace cv;

const String lWindowName = "leftCamera";
const String rWindowName = "rightCamera";
const double width = 640;
const double height = 480;

//bool writeInit = false;
//
//void write_ctrl(gpointer data);
//void set_outfile_name(char* name);
//void get_local_time(char *localTime);

void openTwoEyes();
gboolean startCamera(gpointer data){
	openTwoEyes();
	return 1;

}

GtkWidget * main_window();
void onChanged(GtkWidget *widget, gpointer data);

int main(int argc, char *argv[]){
	GtkWidget *window = NULL;

	if(!g_thread_supported())
		g_thread_init(NULL);
	gdk_threads_init();

	gtk_init(&argc, &argv);

	window = main_window();

	gtk_widget_show_all(window);

	gtk_timeout_add (1000,
	                 startCamera,
	                 	NULL);

	gtk_main();
    gdk_threads_leave();

	return 0;
}



void openTwoEyes(){
	cv::VideoCapture camera(-1);
	camera.set(CV_CAP_PROP_FRAME_HEIGHT, height);
	camera.set(CV_CAP_PROP_FRAME_WIDTH, width*2);

//	控制writeInit，每隔30s为true
//	GThread *write_l = g_thread_new("write", (GThreadFunc)write_ctrl, NULL);

	printf("two-eyes camera opened\n");

	while(camera.isOpened()) {
		cv::Mat3b frame;
		camera >> frame;
		cv::Mat3b leftFrame;
		cv::Mat3b rightFrame;
		int col = frame.cols;
		leftFrame = frame(Range::all(),Range(1,col/2));
		rightFrame = frame(Range::all(),Range(col/2,col));

//		当writeInit为true时，保存并用当前时间命名帧
//			if(writeInit){
//				writeInit = false;
//
//				char file_nameL[25] = {0};
//				file_nameL[0] = 'L';
//				set_outfile_name(file_nameL+1);
//				printf("file_nameL is %s\n", file_nameL);
//				cv::imwrite(file_nameL, leftFrame);
//
//				char file_nameR[25] = {0};
//				file_nameR[0] = 'R';
//				set_outfile_name(file_nameR+1);
//				printf("file_nameR is %s\n", file_nameR);
//				cv::imwrite(file_nameR, rightFrame);
//			}

		cv::imshow(lWindowName, leftFrame);
		cv::imshow(rWindowName, rightFrame);

		//cvWaitKey(0);


//		int c = cvWaitKey(10);

//		if(27 == char(c)){
//			break;
//		}

	}
	camera.release();
}

void onChanged(GtkWidget *widget, gpointer data){
//	CallbackData *cData = (CallbackData *)data;
//	GtkWidget *window = cData->window;
//	GtkWidget *combo = cData->combo;
	char * id = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(widget));

	if(strcmp(id, "two-eyes Camera") == 0){
		printf("selecte %s\n", id);
//		GThread *open = g_thread_new("open", (GThreadFunc)openTwoEyes, NULL);
		openTwoEyes();
	}
}

//void get_local_time(char *localTime){
//	time_t t;
//	t = time(NULL);
//	struct tm *t1;
//	t1 = localtime(&t);
//	strftime(localTime, 64, "_%Y%m%d_%H%M%S", t1);
//}
//
//void set_outfile_name(char* name){
//	char time[20] = {0};
//	get_local_time(time);
//	strncpy(name, time, strlen(time));
//	strcat(name, ".jpg");
//}
//
//void write_ctrl(gpointer data){
//	while(true){
//		sleep(30);
//		writeInit = true;
//	}
//}

GtkWidget * main_window(){
	GtkWidget *window = NULL;
	GtkWidget *leftWindow = NULL;
	GtkWidget *rightWindow = NULL;
	GtkWidget *mBox;
	GtkWidget *box;
	GtkWidget *lBox;
	GtkWidget *rBox;
	GtkWidget *lLabel;
	GtkWidget *rLabel;
	GtkWidget *comboBox;

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_size_request(window, 1300, 600);
	gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
	gtk_window_set_title(GTK_WINDOW(window), "Camera");
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_container_set_border_width(GTK_CONTAINER(window), 20);

	//mBox = gtk_vbox_new(FALSE, 10);
	//gtk_container_add(GTK_CONTAINER(window), mBox);

	/*comboBox = gtk_combo_box_text_new();
	gtk_box_pack_start(GTK_BOX(mBox), comboBox, TRUE, TRUE, 0);
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboBox), "two-eyes Camera");*/

	box = gtk_hbox_new(TRUE, 10);
//	gtk_box_pack_start(GTK_BOX(mBox), box, TRUE, TRUE, 0);
	gtk_container_add(GTK_CONTAINER(window), box);

	lLabel = gtk_label_new("左视图");
	lBox = gtk_vbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box), lBox, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(lBox), lLabel, TRUE, TRUE, 0);

	namedWindow(lWindowName);
	leftWindow = (GtkWidget *)cvGetWindowHandle(lWindowName.c_str());
	GdkWindow *parent = gtk_widget_get_parent_window(leftWindow);
	gtk_widget_set_size_request(leftWindow, width, height);
	gtk_widget_reparent(leftWindow, lBox);
	gdk_window_destroy(parent);

	rLabel = gtk_label_new("右视图");
	rBox = gtk_vbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box), rBox, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(rBox), rLabel, TRUE, TRUE, 0);

	namedWindow(rWindowName);
	rightWindow = (GtkWidget *)cvGetWindowHandle(rWindowName.c_str());
	parent = gtk_widget_get_parent_window(rightWindow);
	gtk_widget_set_size_request(rightWindow, width, height);
	gtk_widget_reparent(rightWindow, rBox);
	gdk_window_destroy(parent);

	//g_signal_connect(G_OBJECT(comboBox), "changed", G_CALLBACK(onChanged), NULL);
	g_signal_connect(GTK_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

	return window;
}
