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

bool writeInit = false;
bool isWrite = false;
cv::Mat3b leftFrame;
cv::Mat3b rightFrame;

void get_local_time(char *localTime){
	time_t t;

	t = time(NULL);
	struct tm *t1;
	t1 = localtime(&t);
	strftime(localTime, 64, "_%Y%m%d_%H%M%S", t1);
	printf("local time is %s\n", localTime);

}

void set_outfile_name(char* name){
	char time[20] = {0};
	get_local_time(time);
	strncpy(name, time, strlen(time));
	strcat(name, ".jpg");
}

void write_ctrl(gpointer frame){
	while(true){
		sleep(30);
		writeInit = true;
	}
}

void* write_videoR(gpointer frame){
	char file_name[30] = {0};
	file_name[0] = 'R';
	set_outfile_name(file_name+1);
	printf("file_name is %s\n", file_name);

}

void* buttonL_clicked(GtkWidget *widget, gpointer data){
    GtkWidget *window_left;
	g_print("ButtonL event:\n");

	cv::VideoCapture camera(0);
	double width = 1280;
	double height = 480;
	camera.set(CV_CAP_PROP_FRAME_HEIGHT, height);
	camera.set(CV_CAP_PROP_FRAME_WIDTH, width);

	GThread *write_l = g_thread_new("write left", (GThreadFunc)write_ctrl, NULL);

	while(camera.isOpened()) {
		cv::Mat3b frame;
		camera >> frame;


		int col = frame.cols;
		leftFrame = frame(Range::all(),Range(1,col/2));

		if(writeInit){
			writeInit = false;
			char file_name[25] = {0};
			file_name[0] = 'L';
			set_outfile_name(file_name+1);
			printf("file_name is %s\n", file_name);

			cv::imwrite(file_name, leftFrame);

		}

		cv::imshow("left", leftFrame);
		window_left = (GtkWidget *)cvGetWindowHandle("left");
		gtk_widget_show(window_left);

		int c = cvWaitKey(40);

		if(27 == char(c)){
			break;
		}

	}
	camera.release();
	gtk_widget_destroy(window_left);

}

void buttonR_clicked(GtkWidget *widget, gpointer data){
	g_print("ButtonR event: \n");
	GtkWidget *window_right;

	cv::VideoCapture camera(0);
	double width = 1280;
	double height = 480;
	camera.set(CV_CAP_PROP_FRAME_HEIGHT, height);
	camera.set(CV_CAP_PROP_FRAME_WIDTH, width);

	GThread *write_r = g_thread_new("write right", (GThreadFunc)write_ctrl, NULL);
	cv::VideoWriter writer;

	while(camera.isOpened()) {
		IplImage *image;
		cv::Mat3b frame;
		camera >> frame;

		int col = frame.cols;
		rightFrame = frame(Range::all(),Range(col/2,col));

		if(writeInit){
			writeInit = false;
			char file_name[25] = {0};
			file_name[0] = 'R';
			set_outfile_name(file_name+1);
			printf("file_name is %s\n", file_name);

			cv::imwrite(file_name, leftFrame);

		}

		cv::imshow("right", rightFrame);
		window_right = (GtkWidget *)cvGetWindowHandle("right");

		gtk_widget_show(window_right);

		int c = cvWaitKey(40);

		if(27 == char(c)){
			break;
		}

	}
	camera.release();
	gtk_widget_destroy(window_right);
}

GtkWidget *create_main_window(){
	GtkWidget *window;
	GtkWidget *fixed;

	GtkWidget *buttonL;
	GtkWidget *buttonR;
	GtkWidget *buttonQ;

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window),"Main Window");
	gtk_window_set_default_size(GTK_WINDOW(window), 640, 480);

	fixed = gtk_fixed_new();
	buttonL = gtk_button_new_with_label("Left");
	buttonR = gtk_button_new_with_label("Right");
	buttonQ = gtk_button_new_with_label("Exit");

	gtk_widget_set_size_request(buttonL, 200, 100);
	gtk_widget_set_size_request(buttonR, 200, 100);
	gtk_widget_set_size_request(buttonQ, 200, 100);

	gtk_fixed_put(GTK_FIXED(fixed), buttonL, 220, 45);
	gtk_fixed_put(GTK_FIXED(fixed), buttonR, 220, 190);
	gtk_fixed_put(GTK_FIXED(fixed), buttonQ, 220, 335);

	gtk_container_add(GTK_CONTAINER(window),fixed);

	g_signal_connect(window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
	g_signal_connect(G_OBJECT(buttonQ), "clicked", G_CALLBACK(gtk_main_quit), NULL);

	g_signal_connect(G_OBJECT(buttonL), "clicked", G_CALLBACK(buttonL_clicked), NULL);
	g_signal_connect(G_OBJECT(buttonR), "clicked", G_CALLBACK(buttonR_clicked), NULL);

	return window;
}


int main(int argc, char** argv){

	if(!g_thread_supported())
		g_thread_init(NULL);
	gdk_threads_init();

	GtkWidget *window_main = NULL;
	gtk_init(&argc, &argv);

	window_main = create_main_window();

	gtk_widget_show_all(window_main);
	gtk_main();
    gdk_threads_leave();

	return 0;
}
