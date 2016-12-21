/**
 * @file syncAPP.c
 * @author Renê Alves
 * @date 26 may 2016
 * @brief Videocolaboration QRCode Sync
 */

#include <stdio.h> //C I/O
#include <stdlib.h> //Memory Manipulation
#include <qrencode.h>
#include <pthread.h>
#include "common.h"

#include <errno.h>
#include <unistd.h>

#include <cv.h>
#include <highgui.h>

QRcode * Qcode;

#define size_screen 420

const int EDGE_WID = 20;
int fps;

void *checkFPS(){
	while(1){
		adaptativeSleep(1);
		printf("%d\n", fps);
		fps = 0; 
	}

}

void qrRander(QRcode * qrcode, int width){
	IplImage * img;
	CvSize imgsize = cvSize(EDGE_WID * 2 + qrcode->width * width, EDGE_WID * 2 + qrcode->width * width);
	img = cvCreateImage(imgsize, 8, 1);

	unsigned char * data = (unsigned char *)img->imageData;

	int x,y;

	for (y = 0; y < EDGE_WID; y++){
		for (x = 0; x < imgsize.width; x++){
			data[img->widthStep * y + x] = 255;
		}
	}
	for (y = imgsize.height - EDGE_WID; y < imgsize.height; y++){
		for (x = 0; x < imgsize.width; x++){
			data[img->widthStep * y + x] = 255;
		}
	}
	for (y = EDGE_WID; y < imgsize.height - EDGE_WID; y++){
		for (x = 0; x < EDGE_WID; x++){ 
			data[img->widthStep * y + x] = 255;
		}
		for (x = imgsize.width - EDGE_WID; x < imgsize.width; x++){
			data[img->widthStep * y + x] = 255;
		}
	}
	int qrwith = qrcode->width * width;
	for (y = 0; y < qrwith; y++){
		for (x = 0; x < qrwith; x++){
			if (qrcode->data[qrcode->width * (y/width) + (x/width)] & 0x01){
				data[img->widthStep * (y + EDGE_WID) + x + EDGE_WID] = 0;
			}else{
				data[img->widthStep * (y + EDGE_WID) + x + EDGE_WID] = 255;
			}
		}
	}
	cvShowImage("FogoSync", img); 
	fps++;
	cvReleaseImage(&img);
}

void createQRCodeWithTime(int size, IplImage* cvFrame, int quadrant, int frame){
 	char * strTime = malloc(sizeof(char)*40);

 	long double timeNow = getSystemTime(NULL);

 	if(frame == -1){
 		sprintf(strTime,"q%d|t%Lf",quadrant,timeNow);
 	}else{
 		sprintf(strTime,"q%d|f%d|t%Lf",quadrant,frame,timeNow);
 	}
 	
	QRcode*			pQRC;

 	if (pQRC = QRcode_encodeString(strTime, 0, QR_ECLEVEL_L , QR_MODE_8)){
		qrRander(pQRC, size);
	}
}

int parseArguments(int argc, char ** argv, int *quadrant, int *frame_count, int *console_log_fps){
	int option = 0;
	while((option = getopt(argc, argv, "q:f:s")) != -1){
		switch(option){
			case 'q':
				*quadrant = atoi(optarg);
				break;
			case 'f':
				*frame_count = 1;
				break;
			case 's':
				*console_log_fps = 1;
				break;
		}
	}
}


 int main(int argc, char** argv){
 	int quadrant, frame_status = 0;

 	int frame = -1, console_log_fps = 0;

 	IplImage* img;

 	parseArguments(argc,argv,&quadrant,&frame_status, &console_log_fps);

 	float outputFramerate = 25.0;

 	long double nowTime = 0.0;
	long double lastTime = 0.0;
	long double difTime = 0.0;

 	IplImage* cvFrame = NULL;
 	cvFrame = cvCreateImage(cvSize(60,60),8,1);

 	cvNamedWindow( "FogoSync", CV_WINDOW_OPENGL );
 	cvMoveWindow( "FogoSync", 0, 0);
 	cvResizeWindow("FogoSync", size_screen,size_screen);

 	pthread_t PTfps;
 	if(console_log_fps)
		pthread_create(&PTfps, NULL, checkFPS, NULL);

 	lastTime = getSystemTime (NULL);
 	
 	while(1){

 		if(frame_status){
 			frame++;
 		}
 		createQRCodeWithTime(40, cvFrame,quadrant, frame);
		cvWaitKey(1);
		nowTime = getSystemTime (NULL);
			adaptativeSleep ( 1/outputFramerate - (nowTime - lastTime));
		lastTime = getSystemTime (NULL);
 	}
	 cvDestroyWindow( "FogoSync");
 }