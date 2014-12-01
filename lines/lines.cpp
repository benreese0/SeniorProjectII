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

//interrupt libraries
#include <signal.h>
#include <unistd.h>


using namespace cv;
using namespace std;
 
Mat og, src, src_gray, img;
int thresh = 150;
int max_thresh = 255;

int newsockfd = 0;
int sockfd = 0;

void handler_catch (int sig) {
    cout << "Received signal:" <<sig << '\t';
    if  (newsockfd != 0) {
        cout << "closing newsockfd:" << close(newsockfd) <<'\t';
        newsockfd = 0;
    }
    if  (sockfd != 0) {
        cout << "closing sockfd:" << close(sockfd) << endl;
        sockfd = 0;
    }
		exit(0);
}



 
/** @function main */
int main(void)
{
	struct sigaction sigIntHandler;
	int portno;
	int nread =0;
	int n;
	char * imgBuff;
	time_t t_val;
	vector <char> imgVec;
        
        //Interrupts
        sigIntHandler.sa_handler = handler_catch;
        sigemptyset(&sigIntHandler.sa_mask);
        sigIntHandler.sa_flags = 0;
        sigaction(SIGINT, &sigIntHandler, NULL);
	
    struct sockaddr_in serv_addr;
    // sockaddr_in = struct containing int addr  
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd <0)
    {
      perror("ERROR opening socket");
      exit(1);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = 7780;
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
				t_val = time(0);

        read(newsockfd,&n,sizeof(n));
        if (n < 1)
        {
          perror("ERROR reading from socket");
          exit(1);
        }

        // allocate buffer size
        imgBuff = new char [n]; 
				nread = 0;	
				while(nread < n){
       	 nread += read(newsockfd, (imgBuff+nread), n-nread);
			 }
				//cout << "Network time:" << ((float)(clock() - now))/CLOCKS_PER_SEC<<endl;
				//now = clock();

        /* ******** start deleting this ********* */
        // need to start porting the video into this 
				imgVec.assign(imgBuff, imgBuff+n);
						
        imdecode(imgVec, CV_LOAD_IMAGE_GRAYSCALE, &og);  // decoding image from buffer
	      // can try not including the image color part or making it grayscale
        // de-allocate buffer
        delete[] imgBuff;
				if(og.empty()){
					cout << "OG EMPTY!!" << endl;}

 				imshow("og", og);
				waitKey(5);

		    double xcrop, ycrop;
			  xcrop = 3.5/8.0;
			  ycrop = 3.5/5.0;
		      
        Rect ROI(og.cols*xcrop, og.rows*ycrop, og.cols*(1-xcrop), og.rows*(0.9-ycrop));
        src = og(ROI);
				if(src.empty()){
					cout << "SRC EMPTY!!" << endl;}
        //cvtColor(src, src_gray, CV_RGB2GRAY);
				src.copyTo(src_gray);
 
        imshow ("src", src);
 
        GaussianBlur(src_gray, src_gray, Size(9,9),0,0);
 
			  Mat canny_output;
        vector<vector<Point> > contours;
        vector<Vec4i> hierarchy;
        vector<float> lines;
        std::vector<cv::Point2f> corners, left, right, final;
        
        /// Detect edges using canny
        Canny( src_gray, canny_output, thresh, thresh*2, 3 );
				if(canny_output.empty()){
					cout << "canny EMPTY!!" << endl;}
        /// Find contours
				//imshow("Cany_output",canny_output);
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
 
                     double theta = atan((lines[1])/(lines[0])) * 57.2957795;
										cout << "theta og:" << theta << endl;
									 theta = theta - 45;	
										 // final factor converts from radians to degress and shifts for perspective
					 
					 double m, b, xsmall, Xsmall, d;
					 m = lines[1]/lines[2];
					 b = lines[3] - m*lines[2];

					 xsmall = (src.rows-b)/m;
					 Xsmall = xsmall+xcrop*og.cols;

					 d = Xsmall - og.cols/2;

          char result[10];
					for (unsigned i =0; i < 10; ++i) result[i] = 0;

						 cout << "theta:" << theta << '\t' << "Dist: " << d << endl;
					 sprintf(result,"%f",theta);
           write(newsockfd,result, 10);
                     //imshow("Contours", drawing);
 
                     //waitKey(20);
                     //imshow("vertices",img1);
         }
				else{
								cout << "No Contours Found" << endl;
				}
 
       //imshow ("img", src);
       //imshow("original", og);
       //waitKey(20);
       cout << "end:\t"<< time(0)-t_val << endl;
       }
 
       close(newsockfd);
       close(sockfd);
}
