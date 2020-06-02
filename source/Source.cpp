/**
  @file videocapture_basic.cpp
  @brief A very basic sample for using VideoCapture and VideoWriter
  @author PkLab.net
  @date Aug 24, 2016
*/
#define GREEN Scalar(0,255,0)
#define PINK Scalar(255,11,255)
#define BLUE Scalar(240,240,255)
#define WHITE Scalar(255,255,255)
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include "opencv2/videoio.hpp"
#include <map>
#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;

Point centerhand(vector<Point> handSet) {
    Point center=Point(0,0);
    for (int i = 0; i < handSet.size(); i++) {
    
        center+=handSet[i];
    }
    center=center/(double)(handSet.size());
    return center;
}

void condefects(vector<Vec4i> convexityDefectsSet, vector<Point> mycontour, Mat& original)
{
    multimap<double, int,greater<double>> pair;

    int max_depth = -1;
    int max_depth_idx = -1;
    for (int cDefIt = 0; cDefIt < convexityDefectsSet.size(); cDefIt++) {

        int startIdx = convexityDefectsSet[cDefIt].val[0]; Point ptStart(mycontour[startIdx]);

        int endIdx = convexityDefectsSet[cDefIt].val[1]; Point ptEnd(mycontour[endIdx]);

        int farIdx = convexityDefectsSet[cDefIt].val[2]; Point ptFar(mycontour[farIdx]);

        double depth = static_cast<double>(convexityDefectsSet[cDefIt].val[3]) / 256;


        if (depth > max_depth) max_depth_idx = cDefIt;

        pair.insert(make_pair(depth, cDefIt));
        //cout << "depth" << depth << endl;
        //display start points
        circle(original, ptStart, 5, CV_RGB(255, 0, 0), 2, 8);
        //display all end points
        circle(original, ptEnd, 5, CV_RGB(255, 255, 0), 2, 8);
        //display all far points
        circle(original, ptFar, 5, CV_RGB(0, 255, 0), 2, 8);
//        rectangle(original, ptFar, 5, CV_RGB(0, 255, 0), 2, 8);

    }

    //cout << "깊이 : "<< max_depth_idx <<" "<<convexityDefectsSet[max_depth_idx].val[3] / 256;

    //cout << "ㅁㅇ :" << pair.begin()->second;
    vector<Point> center;
    int i = 0;
    for (auto it = pair.begin(); it != pair.end(); it++,i++) {
        if (i >= 4) break;
        int farIdx = convexityDefectsSet[it->second].val[2];
        Point ptFar(mycontour[farIdx]);
        center.push_back(ptFar);
        circle(original, ptFar, 5, CV_RGB(255, 255, 255), 2, 8);
    }
    circle(original, centerhand(center), 5, CV_RGB(27, 150, 255), 2, 8);
  
}
int findMaxContourArea(vector<vector<Point>> contours) {
    int max_contour = -1;
    int max_area = -1;

    for (int i = 0; i < contours.size(); i++)
    {
        int area = contourArea(contours[i]);

        if (area > max_area) {
            max_area = area;
            max_contour = i;
        }
    }
    return max_contour;
}

Mat findHandContour(Mat picture) {
    Mat srcimg = picture; // 불러온 picture 하나
    Mat HSVImage; // InRange용
    Mat img_mask = srcimg; //Contour용
    
                           
    //손 검출 지정영역
    Scalar low = Scalar(0, 30, 80);
    Scalar high = Scalar(20, 255, 255);


    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    
    cvtColor(srcimg, HSVImage, COLOR_BGR2HSV); // 1파라미터를 2파라미터에 HSV로 변환
    

    inRange(HSVImage, low, high, HSVImage);


    findContours(HSVImage, contours, hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE);

    int max_contour=findMaxContourArea(contours);
   
        vector<vector<Point>> contours2;
        contours2.push_back(contours[max_contour]);
        vector<vector<Point>> hull(contours2.size());
        vector<vector<int>>inthull(contours2.size()); //contoursize 꼭 해줘야함

        vector<vector<Vec4i>> defects(contours2.size());


        //for (int i = 0; i < contours2.size(); i++) {}
        convexHull(contours2[0], hull[0], false);
        convexHull(contours2[0], inthull[0], false);
        try {
            convexityDefects(contours2[0], inthull[0], defects[0]);
        }
        catch (Exception & e) {
            return picture;
        }

        drawContours(img_mask, contours2, 0, BLUE, 1, 8);
        drawContours(img_mask, hull, 0, WHITE, 2, 8);
        condefects(defects[0], contours2[0], img_mask);

        return img_mask;


}
int main(int, char**)
{
    int select;
    cin >> select;
    Mat srcimg;
    switch (select) {
    case 0:

        srcimg = imread("C:\\Users\\seyoung97\\Desktop\\sample4.jpg");
        imshow("hel", findHandContour(srcimg));
        waitKey(0);
        break;
    case 1:
        VideoCapture cap1(0);
        if (!cap1.isOpened())
            cout << "첫번째 카메라를 열 수 없습니다." << endl;
        
        namedWindow("testing", 1);
        while (1) {

            cap1 >> srcimg;

            //Mat srcimg = imread("C:\\Users\\seyoung97\\Desktop\\sample4.jpg"); // 뒤에 flag가 0이면 grayscale로 불러옴
            if (srcimg.empty()) {
                return -1;
            }


            imshow("hel", findHandContour(srcimg));

            // waitKey(0);
            if (waitKey(25) >= 0) break; //esc 키 입력시 종료
        }
    }
     return 0;
}
