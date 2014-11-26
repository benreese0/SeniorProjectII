// SIGN DETECTION WITH SOCKET COMMUNICATION -- LINUX

// OpenCV libraries
// socket libraries
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/legacy/legacy.hpp>
#include "opencv2/video/tracking.hpp"


#include <time.h>
#include <math.h>

using namespace cv;
using namespace std;

//int lowThreshold = 50; // 75
int lowThreshold = 70;
bool match = false;
double H;
Mat og, src, src_gray, img;
string intToString(int number) {

    std::stringstream ss;
    ss << number;
    return ss.str();
}

class Symbol {

    public:
        Mat img;
        string name;

};



string yield(std::vector<vector<Point> >&contours) {

    vector<Point> approxTri;

    //cout << contours.size() << endl;

    for (size_t i = 0; i < contours.size(); i++) {
        approxPolyDP(contours[i], approxTri, arcLength(Mat(contours[i]), true) * 0.05, true);
        if (approxTri.size() == 3) {
            float area = contourArea(contours[i]);

            if (area > 85) {
                std::vector<cv::Point2f> corners;

                vector<Point>::iterator vertex;
                vertex = approxTri.begin();
                corners.push_back(*vertex);
                vertex++;
                corners.push_back(*vertex);
                vertex++;
                corners.push_back(*vertex);

                Moments mu;
                mu = moments(contours[i], false);
                Point2f center(mu.m10 / mu.m00, mu.m01 / mu.m00);

                std::vector<cv::Point2f> top, bot; // should bot only be pointf for 1 point? NO point2f (0,0) therefore fine!

                //cout << "Size: " << corners.size() << endl;


                for (unsigned int i = 0; i < corners.size(); i++) {
                    if (corners[i].y < center.y)
                    {
                        top.push_back(corners[i]);
                        //cout << "Pushed " << i << " into top" << endl;
                    }
                    else
                    {
                        bot.push_back(corners[i]);
                        //cout << "Pushed " << i << " into bottom" << endl;
                    }
                }

                if (top.size() != 2) {
                }
                else {

                    cv::Point2f tl = top[0].x > top[1].x ? top[1] : top[0]; // condition ? result if true: result if false
                    cv::Point2f tr = top[0].x > top[1].x ? top[0] : top[1];
                    cv::Point2f b = bot[0]; // may need to be bot[1];

                    corners.clear();
                    corners.push_back(tl);
                    corners.push_back(tr);
                    corners.push_back(b);

                    //cout << "Corners  " << corners << endl;

                    double d1, d2, d3;
                    d1 = sqrt(pow(tl.y-tr.y,2)+pow(tl.x-tr.x,2)); // top left to top right corner
                    d2 = sqrt(pow(tl.y-b.y,2)+pow(tl.x-b.x,2)); // top left to bottom corner
                    d3 = sqrt(pow(b.y-tr.y,2)+pow(b.x-tr.x,2)); // top right to bottom corner

                    cv::Point2f m;
                    m.x = (tl.x+tr.x)/2;
                    m.y = (tl.y+tr.y)/2;
                    //cout << "m  " << m << endl;

                    H = sqrt(pow(b.y-m.y,2)+pow(b.x-m.x,2));

                    if ((d1*1.1 > d2) && (d2 > d1/1.1) && (d1*1.1 > d3) && (d3 > d1/1.1)) {// was 1.05
                        match = true;
                    }

                }

            }

        }

    }

    if (match == true){
        string str ="Y";
        return str;
    }
    else {
        return "0";
    }
}

string stop(std::vector<vector<Point> >&contours) {
    vector<Point> approxOct;

    for (size_t i = 0; i < contours.size(); i++) {
        approxPolyDP(contours[i], approxOct, arcLength(Mat(contours[i]), true) * 0.04, true);
        if (approxOct.size() == 8) {
            float area = contourArea(contours[i]);

            if (area > 200) {
                std::vector<cv::Point2f> corners;

                vector<Point>::iterator vertex;
                vertex = approxOct.begin();
                corners.push_back(*vertex);
                vertex++;
                corners.push_back(*vertex);
                vertex++;
                corners.push_back(*vertex);
                vertex++;
                corners.push_back(*vertex);
                vertex++;
                corners.push_back(*vertex);
                vertex++;
                corners.push_back(*vertex);
                vertex++;
                corners.push_back(*vertex);
                vertex++;
                corners.push_back(*vertex);

                Moments mu;
                mu = moments(contours[i], false);
                Point2f center(mu.m10 / mu.m00, mu.m01 / mu.m00);

                std::vector<cv::Point2f> Q1, Q2, Q3, Q4, Q5, Q6, Q7, Q8; // should bot only be pointf for 1 point? NO point2f (0,0) therefore fine!

                for (unsigned int i = 0; i < corners.size(); i++) {
                    if (corners[i].x < center.x && corners[i].y < center.y)
                        Q1.push_back(corners[i]);
                    else if (corners[i].x > center.x && corners[i].y < center.y)
                        Q2.push_back(corners[i]);
                    else if (corners[i].x > center.x && corners[i].y > center.y)
                        Q3.push_back(corners[i]);
                    else
                        Q4.push_back(corners[i]);
                }

                if (Q1.size() != 2 || Q2.size() != 2 || Q3.size() != 2 || Q4.size() != 2){
                    //cout << "Q.size() != 2" << endl;
                    match = false; // will get rid of this later
                }
                else {

                    bool error = false;

                    /*cout << "Q1 " << Q1 << endl;
                      cout << "Q2 " << Q2 << endl;
                      cout << "Q3 " << Q3 << endl;
                      cout << "Q4 " << Q4 << endl;
                      cout << "Center " << center << endl;*/

                    cv::Point2f p1, p2, p3, p4, p5, p6, p7, p8;

                    if (Q1[0].y > Q1[1].y && Q1[0].x < Q1[1].x){
                        p1 = Q1[0];
                        p2 = Q1[1];
                    }
                    else if (Q1[1].y > Q1[0].y && Q1[1].x < Q1[0].x){
                        p1 = Q1[1];
                        p2 = Q1[0];
                    }
                    else {
                        error = true;
                        //cout << "Error: Q1 does not meet reqs." << endl;
                    }


                    if (Q2[0].y < Q2[1].y && Q2[0].x < Q2[1].x && error == false){
                        p3 = Q2[0];
                        p4 = Q2[1];
                    }
                    else if (Q2[1].y < Q2[0].y && Q2[1].x < Q2[0].x && error == false){
                        p3 = Q2[1];
                        p4 = Q2[0];
                    }
                    else {
                        error = true;
                        //cout << "Error: Q2 does not meet reqs." << endl;
                    }

                    if (Q3[0].y < Q3[1].y && Q3[0].x > Q3[1].x && error == false){
                        p5 = Q3[0];
                        p6 = Q3[1];
                    }
                    else if (Q3[1].y < Q3[0].y && Q3[1].x > Q3[0].x && error == false){
                        p5 = Q3[1];
                        p6 = Q3[0];
                    }
                    else {
                        error = true;
                        //cout << "Error: Q3 does not meet reqs." << endl;
                    }

                    if (Q4[0].y < Q4[1].y && Q4[0].x < Q4[1].x && error == false){
                        p7 = Q4[0];
                        p8 = Q4[1];
                    }
                    else if (Q4[1].y < Q4[0].y && Q4[1].x < Q4[0].x && error == false){
                        p7 = Q4[1];
                        p8 = Q4[0];
                    }
                    else {
                        error = true;
                        //cout << "Error: Q4 does not meet reqs." << endl;
                    }

                    if (error == false){
                        match = true;
                        //cout << "MATTCCHHH!!!!!" << endl;
                        /*corners.clear();
                          corners.push_back(p1);
                          corners.push_back(p2);
                          corners.push_back(p3);
                          corners.push_back(p4);
                          corners.push_back(p5);
                          corners.push_back(p6);
                          corners.push_back(p7);
                          corners.push_back(p8);*/

                        H = sqrt(pow(p7.y-p2.y,2)+pow(p7.x-p2.x,2));
                        //cout << "pixel height" << H << endl;
                    }
                    //  MAY NEED MORE GEOMETRY!!!
                }
            }
        }
    }

    if (match == true){
        string str ="S";
        return str;
    }
    else {
        return "0";
    }
}



string speed(std::vector<vector<Point> >&contours, Symbol *symbols,Mat &greyImg) { //Point2f (needed for moments)

    vector<Point> approxRect;

    string str;

    for (size_t i = 0; i < contours.size(); i++) {
        approxPolyDP(contours[i], approxRect,
                arcLength(Mat(contours[i]), true) * 0.05, true);
        if (approxRect.size() == 4) {
            float area = contourArea(contours[i]);

            if (area > 200) {
                std::vector<cv::Point2f> corners;

                vector<Point>::iterator vertex;
                vertex = approxRect.begin();
                corners.push_back(*vertex);
                vertex++;
                corners.push_back(*vertex);
                vertex++;
                corners.push_back(*vertex);
                vertex++;
                corners.push_back(*vertex);

                Moments mu;
                mu = moments(contours[i], false);
                Point2f center(mu.m10 / mu.m00, mu.m01 / mu.m00);

                std::vector<cv::Point2f> top, bot;

                for (unsigned int i = 0; i < corners.size(); i++) {
                    if (corners[i].y < center.y)
                        top.push_back(corners[i]);
                    else
                        bot.push_back(corners[i]);
                }

                if (top.size() != 2 || bot.size() != 2){
                    //cout << "top.size() or bot.size() != 2" << endl;
                }
                else {

                    cv::Point2f tl = top[0].x > top[1].x ? top[1] : top[0];
                    cv::Point2f tr = top[0].x > top[1].x ? top[0] : top[1];
                    cv::Point2f bl = bot[0].x > bot[1].x ? bot[1] : bot[0];
                    cv::Point2f br = bot[0].x > bot[1].x ? bot[0] : bot[1];

                    corners.clear();
                    corners.push_back(tl);
                    corners.push_back(tr);
                    corners.push_back(br);
                    corners.push_back(bl);

                    double H, W;

                    H = sqrt(pow(tl.x-bl.x,2) + pow(tl.y-bl.y,2));

                    //cout << "H inside: " << H << endl;

                    W = sqrt(pow(tl.x-tr.x,2) + pow(tl.y-tr.y,2));

                    //cout << "W/H: " << W/H << endl;

                    if (0.97 > W/H && W/H > 0.57 ){ // actual should be 0.77 therefore +- 0.18

                        // Define the destination image
                        Mat correctedImg = ::Mat::zeros(symbols[0].img.rows*2, symbols[0].img.cols, CV_8UC3); // fine [0] since same size as [1]

                        // Corners of the destination image
                        std::vector<cv::Point2f> quad_pts;
                        quad_pts.push_back(Point2f(0, 0));
                        quad_pts.push_back(Point2f(correctedImg.cols, 0));
                        quad_pts.push_back(Point2f(correctedImg.cols, correctedImg.rows));
                        quad_pts.push_back(Point2f(0, correctedImg.rows));


                        // Get transformation matrix
                        Mat transmtx = getPerspectiveTransform(corners, quad_pts);

                        // Apply perspective transformation
                        warpPerspective(greyImg, correctedImg, transmtx, correctedImg.size());

                        Mat correctedImgBin;

                        //cvtColor(correctedImg, correctedImgBin, CV_RGB2GRAY);
                        correctedImg.copyTo(correctedImgBin);

                        imshow("correctedimgbin", correctedImgBin);

                        threshold(correctedImgBin, correctedImgBin, 140, 255, 0);

                        int r,c;
                        r = correctedImgBin.rows;
                        c = correctedImgBin.cols;
                        Rect ROI(0,r/2,c,r/2);

                        Mat new_image;

                        correctedImgBin = correctedImgBin(ROI);
                        correctedImgBin.copyTo(new_image);

                        Mat diffImg;

                        int minDiff, diff;

                        minDiff = 35000;


                        for (int i = 0; i < 2; i++) {

                            bitwise_xor(new_image, symbols[i].img, diffImg, noArray());
                            diff = countNonZero(diffImg);

                            if (i ==1){
                                cout << "diff: " << diff << endl;
                            }
                            /*if (diff < minDiff  && i == 0) {
                            //minDiff = diff;
                            match = true;
                            str = "V1";
                            }*/
                            if (diff < minDiff  && i == 1) {
                                cout << "Hi" << endl;
                                match = true;
                                str = "V2";
                            }
                            imshow("diff", diffImg);
                            imshow("act", new_image);
                            waitKey(0);
                        }
                    }
                }
            }
        }
    }

    if (match == true) {
        return str;
    }
    else {
        return "0";
    }
}

int readRefImages(Symbol *symbols) {

    symbols[0].img = imread("SpeedLimitOne.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    if (!symbols[0].img.data){
        return -1;}
    threshold(symbols[0].img, symbols[0].img, 100, 255, 0);
    symbols[0].name = "Speed Limit 1";

    int r, c, margin;
    margin = 35;
    r = symbols[0].img.rows;
    c = symbols[0].img.cols;
    Rect ROI(margin,r/2,c-2*margin,r/2-margin);

    symbols[0].img = symbols[0].img(ROI);

    symbols[1].img = imread("SpeedLimitTwo.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    if (!symbols[1].img.data){
        return -1;}
    threshold(symbols[1].img, symbols[1].img, 100, 255, 0);
    symbols[1].name = "Speed Limit 2";

    symbols[1].img = symbols[1].img(ROI);

    return 0;

}


void CannyThreshold(int, void*) {
}

int main(int argc, char** argv) {
    // for OpenCV general detection/matching framework details

    //VideoCapture cap;
    //cap.open(0);
    int sockfd, newsockfd, portno;
    int nread = 0;
    int n;
    char * imgBuff;
    time_t t_val;
    vector <char> imgVec;
    struct sockaddr_in serv_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd<0)
    {
        perror("ERROR opening socket");
        exit(1);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = 7779;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Error on binding");
        exit(1);
    }
    listen(sockfd,5);
    cout << "Listening \n " << endl;
    newsockfd = accept(sockfd, NULL,NULL);
    if (newsockfd<0)
    {
        perror("ERROR on accept");
        exit(1);
    }
    cout << "Accepted connection" << endl;
    Mat camera;
    Mat greyImg;

    Symbol symbols[10];
    if (readRefImages(symbols) == -1) {
        printf("Error reading reference symbols\n");
        return -1;
    }

    createTrackbar("Min Threshold:", "A", &lowThreshold, 100, CannyThreshold);

    double t = (double) getTickCount();

    //cap >> camera;

    while (1) {

        time_t now = clock();
        read(newsockfd,&n,sizeof(n));
        if (n<1)
        {
            perror("ERROR reading from socket");
            exit(1);
        }
        imgBuff = new char [n];
        nread = 0;
        while (nread<n){
            nread += read(newsockfd, (imgBuff+nread),n-nread);
        }
        imgVec.assign(imgBuff,imgBuff+n);
        imdecode(imgVec,CV_LOAD_IMAGE_GRAYSCALE, &og);
        delete[] imgBuff;
        if(og.empty()){
            cout << "OG EMPTY!!" << endl;}
        imshow("og",og);
        waitKey(50);



        //cap >> camera;

        //camera = imread("flopped-isotest-AWBauto-ISO400-shutter1000.jpeg", CV_LOAD_IMAGE_GRAYSCALE);
        //    camera = imread("flopped-isotest-AWBhorizon-ISO400-shutter1000.jpeg", CV_LOAD_IMAGE_GRAYSCALE);

        // resize(camera,greyImg, Size(camera.cols/3, camera.rows/3), 0, 0);

        //cvtColor(camera, greyImg, CV_RGB2GRAY);

        Mat canny_output;
        vector<vector<Point> > contours;
        vector<Vec4i> hierarchy;
        og.copyTo(greyImg);
        GaussianBlur(greyImg, greyImg, Size(9, 9), 0, 0);

        /// Detect edges using canny
        Canny(greyImg, canny_output, lowThreshold, lowThreshold * 3, 3);

        imshow("canny_output",canny_output); // can comment out when calibrated

        /// Find contours
        findContours(canny_output, contours, hierarchy, CV_RETR_TREE,
                CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

        string result;

        //result = yield(contours);

        if (match != true){
            result = stop(contours);

            if (match != true){
                result = speed(contours, symbols, greyImg);
            }
        }

        double seconds = clock()-now;

        //imshow("B", correctedImg);

        if (match == true) {
            //putText(camera, result, Point(320, 30), 1,
            //2, Scalar(0, 0, 255), 2);
            float h = H*10;
            //cout << "h: " << h << endl;
            double d = 0.1;
            //cout << "H: " << H << endl;
            if (int(h+0.5)>=(int(h)+1)) {
                //cout << "int(h+0.5) " << int(h+0.5) << endl;
                //cout << "int(h)+1 " << int(h)+1 << endl;
                d = (int(h)+1)/10.0;
                //cout << "H" << d << endl;
            }
            else {
                d = (int(h))/10.0;
                //cout << "H " << d << endl;
            }
            char finalResult[10];
            cout << result << d << endl;
            sprintf(finalResult,"%s%f\n",result,d);
            write(newsockfd,finalResult,6);
            printf("%f time\n",seconds/CLOCKS_PER_SEC);
            match = false;
            //}

            //imshow("A", camera);

            //system("pause");

    }

    close(newsockfd);
    close(sockfd);
    waitKey(0);
}
}
