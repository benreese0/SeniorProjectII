// LINE DETECTION WITH SOCKET COMMUNICATION -- LINUX
 
// OpenCV libraries
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/legacy/legacy.hpp>
#include <opencv2/video/tracking.hpp>

// Regular Libraries
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <vector>
#include <math.h>

#include <fstream>

// socket libraries
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace cv;
using namespace std;
 
Mat og, src, src_gray, img;
int thresh = 150;
int max_thresh = 255;

 
/** @function main */
int main(void)
{
    int sockfd, newsockfd, portno;
		int nread =0;
		int n;
    char * imgBuff;
    vector <char> imgVec;
	
    struct sockaddr_in serv_addr;
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
    cout << "Listening\n" << endl;
    /* Accept connection from any client */
    newsockfd = accept(sockfd, NULL, NULL);
    if (newsockfd < 0) 
    {
        perror("ERROR on accept");
        exit(1);
    }
    cout << "Accepted connection" << endl;
    //write(newsockfd, "testing", 8);
    /* If connection is established then start communicating */
    while (1) {
				//read in size of next image
        read(newsockfd,&n,sizeof(n));
        cout << "n:" << n << endl;
        if (n < 1)
        {
          perror("ERROR reading from socket");
          exit(1);
        }

        // allocate buffer size
        imgBuff = new char [n]; 
				
				while(nread < n){
       	 nread += read(newsockfd, (imgBuff+nread), n-nread);
			 }
        /* ******** start deleting this ********* */
        // need to start porting the video into this 
				imgVec.assign(imgBuff, imgBuff+n);
						
        imdecode(imgVec, CV_LOAD_IMAGE_GRAYSCALE, &og);  // decoding image from buffer
	      // can try not including the image color part or making it grayscale
        // de-allocate buffer
        delete[] imgBuff;
				if(og.empty()){
					cout << "IT's EMPTY!!" << endl;}
        //time_t now = clock();
 
		    double xcrop, ycrop;
			  xcrop = 6.5/8;
			  ycrop = 3.0/5;
		      
        Rect ROI(og.cols*xcrop, og.rows*ycrop, og.cols*(1-xcrop), og.rows*(1-ycrop));
        src = og(ROI);
        //cvtColor(src, src_gray, CV_RGB2GRAY);
				src_gray.copyTo(src);
        cout << "Am i here?" << endl;
 
          //imshow ("src", src);
 
        GaussianBlur(src_gray, src_gray, Size(9,9),0,0);
 
			  Mat canny_output;
              vector<vector<Point> > contours;
              vector<Vec4i> hierarchy;
              vector<float> lines;
              std::vector<cv::Point2f> corners, left, right, final;
              
              /// Detect edges using canny
              Canny( src_gray, canny_output, thresh, thresh*2, 3 );
              /// Find contours
              findContours( canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
 
              /// Draw contours
              Mat img1 = Mat::zeros( canny_output.size(), CV_8UC3 );
              Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
 
              //cout << "Size Contours: " << contours.size() << endl;
 
              if (contours.size() > 0) {
 
                     for( unsigned i = 0; i< contours.size(); i++ )
                           {
                           //Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
                           drawContours( drawing, contours, i, Scalar(128,255,255), 2, 8, hierarchy, 0, Point() );
                           }
 
                     //fitLine (contours[0], lines, CV_DIST_HUBER, 0, 0.01, 0.01);
                     fitLine (contours[0], lines, CV_DIST_HUBER, 0, 0.01, 0.01);
                     //cout << "lines: " << lines[0] << " "<< lines[1] << " "<< lines[2] << " " << lines[3] << endl;
                     int lefty = (-lines[2]*lines[1]/lines[0])+lines[3];
                     int righty = ((src_gray.cols-lines[2])*lines[1]/lines[0])+lines[3];
                     line(src,Point(src_gray.cols-1,righty),Point(0,lefty),Scalar(0,0,255),2);
 
                     double theta = atan((lines[1])/(lines[0])) * 57.2957795 - 31; // final factor converts from radians to degress and shifts for perspective
					 
					 double m, b, xsmall, Xsmall, d;
					 m = lines[1]/lines[2];
					 b = lines[3] - m*lines[2];

					 xsmall = (src.rows-b)/m;
					 Xsmall = xsmall+xcrop*og.cols;

					 d = Xsmall - og.cols/2;

          char result[10];
					 if (theta >= 0){
						 cout << "L" << theta << endl;
						 cout << "Dist: " << d << endl;
             sprintf(result,"L%f\n", theta);
             write(newsockfd,result,10);
					 }
					 else {
						 cout << "R" << theta << endl;
						 cout << "Dist: " << d << endl;
             sprintf(result,"R%f\n", (-1.0)*theta);
             write(newsockfd,result,10);
					 }
                     //imshow("Contours", drawing);
 
                     //waitKey(20);
                     //imshow("vertices",img1);
              }
 
       //imshow ("img", src);
       //imshow("original", og);
       //waitKey(20);
      
        
       }
 
       waitKey(20);
       close(newsockfd);
       close(sockfd);
}
