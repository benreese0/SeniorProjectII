//#include "stdafx.h"
 
// don't need all of these hpp files, can isolate later
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
 
#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
 
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/legacy/legacy.hpp>
#include <opencv2/video/tracking.hpp>

// socket libraries
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace cv;
using namespace std;
 
Mat src, src_gray, img;
int thresh = 150;
int max_thresh = 255;
 
/// Function header
//void thresh_callback(int, void* );
 
/** @function main */
int main(int argc, char **argv)
{
    int sockfd, newsockfd, portno, clilen, n;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    // sockaddr_in = struct containing int addr  
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd <0)
    {
      perror("ERROR opening socket");
      exit(1);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = 12345;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

     /* Now bind the host address using bind() call.*/
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
         perror("ERROR on binding");
         exit(1);
    }
     /* Now start listening for the clients, here process will
    * go in sleep mode and will wait for the incoming connection
    */
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    
    /* Accept actual connection from the client */
    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, (unsigned int *)&clilen);
    if (newsockfd < 0) 
    {
        perror("ERROR on accept");
        exit(1);
    }
    /* If connection is established then start communicating */
    bzero(buffer,256);
    n = read( newsockfd,buffer,255 );
    if (n < 0)
    {
        perror("ERROR reading from socket");
        exit(1);
    }
    printf("Here is the message: %s\n",buffer);

    /* Write a response to the client */
    n = write(newsockfd,"I got your message",18);
    if (n < 0)
    {
        perror("ERROR writing to socket");
        exit(1);
    }

    /* ******** start deleting this ********* */
    // need to start porting the video into this 

    // store the video to a string
	  //string filename = "cameraTestwithTime.mp4";
 
    //   VideoCapture cap(filename);
 
	   // create a matrix
       Mat camera;
       Mat greyImg;
 
 	   // if video file is not found
     //  if( !cap.isOpened()) {
     //         cout << "Error when reading route3-awb-vs.H264" << endl;
     //  }
 
       //double t = (double) getTickCount();
 
       cap >> camera;
 
       Mat og;
 
       while (1) {
 
              //time_t now = clock();
 
              cap >> og;
              Rect ROI(og.cols*6.5/8, og.rows*3/5, og.cols*1.5/8, og.rows*2/5);
              src = og(ROI);
              cvtColor(src, src_gray, CV_RGB2GRAY);
 
              //imshow ("src", src);
 
              GaussianBlur(src_gray, src_gray, Size(9,9),0,0);
 
        // createTrackbar( " Canny thresh:", "Source", &thresh, max_thresh, thresh_callback );
         //thresh_callback( 0, 0 );
         Mat canny_output;
              vector<vector<Point> > contours;
              vector<Vec4i> hierarchy;
              vector<float> lines;
              std::vector<cv::Point2f> corners, left, right, final;
              //int mx, rxtop, rytop, lxtop, lytop, rxbot, rybot, lxbot, lybot;
 
              /// Detect edges using canny
              Canny( src_gray, canny_output, thresh, thresh*2, 3 );
              /// Find contours
              findContours( canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
 
              /// Draw contours
              Mat img1 = Mat::zeros( canny_output.size(), CV_8UC3 );
              Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
 
              cout << "Size Contours: " << contours.size() << endl;
 
              if (contours.size() > 0) {
 
                     for( int i = 0; i< contours.size(); i++ )
                           {
                           //Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
                           drawContours( drawing, contours, i, Scalar(128,255,255), 2, 8, hierarchy, 0, Point() );
                           }
 
                     //fitLine (contours[0], lines, CV_DIST_HUBER, 0, 0.01, 0.01);
                     fitLine (contours[0], lines, CV_DIST_HUBER, 0, 0.01, 0.01);
                     cout << "lines: " << lines[0] << " "<< lines[1] << " "<< lines[2] << " " << lines[3] << endl;
                     int lefty = (-lines[2]*lines[1]/lines[0])+lines[3];
                     int righty = ((src_gray.cols-lines[2])*lines[1]/lines[0])+lines[3];
                     line(src,Point(src_gray.cols-1,righty),Point(0,lefty),Scalar(0,0,255),2);
 
                     double theta = atan((lines[1])/(lines[0])) * 57.2957795; // final factor converts from radians to degress
 
                     cout << "theta: " << theta << endl;
 
                     imshow("Contours", drawing);
 
                     //waitKey(20);
                     //imshow("vertices",img1);
              }
 
       imshow ("img", src);
       imshow("original", og);
       waitKey(20);
       }
 
       waitKey(20);
       close(newsockfd);
       close(sockfd);
}
 
             
 
