// CVtest1.cpp : Read Image from File and Display
//
#include "stdafx.h"

// don't need all of these hpp files, can isolate later
#include "opencv2/core/core.hpp"
#include "opencv2/flann/miniflann.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/photo/photo.hpp"
#include "opencv2/video/video.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/ml/ml.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/contrib/contrib.hpp"
#include "opencv2/core/core_c.h"
#include "opencv2/highgui/highgui_c.h" // need for sure
#include "opencv2/imgproc/imgproc_c.h" // need for sure

#include <iostream>
#include <math.h>
#include <time.h>


using namespace cv;
using namespace std;

/// Global Variables
int DELAY_CAPTION = 1500; // Gaussian Blur
int DELAY_BLUR = 100;
int MAX_KERNEL_LENGTH = 31;

int edgeThresh = 1; // Canny
int lowThreshold;
int const max_lowThreshold = 150;
int ratio = 3;
int kernel_size = 3;
int levels = 3;
char* window = "Edge Map";

Mat src; Mat dst; Mat gau; Mat og;// Gauss
Mat can, detected_edges; //Canny

char window_name[] = "Filter Demo 1";

vector<vector<Point> > contours;
vector<Vec4i> hierarchy;



/**
 * @function CannyThreshold
 * @brief Trackbar callback - Canny thresholds input with a ratio 1:3
 */
void CannyThreshold(int, void*)
{
  /// Reduce noise with a kernel 3x3
  blur( gau, detected_edges, Size(3,3) );

  lowThreshold = 140;

  /// Canny detector
  Canny( detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size );

  /// Using Canny's output as a mask, we display our result
  can = Scalar::all(0);

  gau.copyTo( can, detected_edges);
  imshow( window, can );
 }

static void on_trackbar(int, void*)
 {
     Mat cnt_img = Mat::zeros(can.cols, can.rows, CV_8UC3);
     int _levels = levels - 3;
     drawContours( cnt_img, contours, _levels <= 0 ? 3 : -1, Scalar(128,255,255),
                   3, CV_AA, hierarchy, std::abs(_levels) );
 
     imshow("contours", cnt_img);
 }



/**
 * function main
 */
 int main( int argc, char** argv )
 {

	time_t now = clock();
   /// Load the source image
   og = imread( "fovtestyield50in.jpg", CV_LOAD_IMAGE_GRAYSCALE );

   resize(og, src, Size(og.cols/3, og.rows/3),0,0, CV_INTER_AREA);

   imshow("Original Grayscale",src);

    //GaussianBlur( src, gau, Size( 15, 15 ), 0, 0 );

    //imshow("Gaussian Blur", gau);

   gau = src;

	/// Create a matrix of the same type and size as gau (for can)
    can.create( src.size(), src.type() );

    /// Create a window

    namedWindow( window, CV_WINDOW_AUTOSIZE );

    /// Create a Trackbar for user to enter threshold
    createTrackbar( "Min Threshold:", window, &lowThreshold, max_lowThreshold, CannyThreshold );

    /// Show the image
    CannyThreshold(0, 0);

    vector<vector<Point> > contours0;
    findContours( can, contours0, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
 
    contours.resize(contours0.size());
    for( size_t k = 0; k < contours0.size(); k++ )
        approxPolyDP(Mat(contours0[k]), contours[k], 8, true);

   namedWindow( "contours", 1 );
   createTrackbar( "levels+3", "contours", &levels, 7, on_trackbar );
 
   on_trackbar(0,0);

   double seconds = clock()-now;

	printf("%f time",seconds/CLOCKS_PER_SEC);

     waitKey(0);
     return 0;
	 
 }

