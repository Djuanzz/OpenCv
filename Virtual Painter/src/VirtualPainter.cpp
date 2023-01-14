#include "D:/OpenCv/opencv/build/include/opencv2/opencv.hpp"
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std;


Mat imgFrame;
vector<vector<int>> fixPoint;
//the idea is {{x, y, color(0)}, {x, y, color(1)}, dst} -> berupa vector

//VIRTUAL PAINTER WARNA OREN STABILO DAN PINK SSTABILO WITH CAMERA
vector<vector<int>> myColors {{146, 69, 175, 179, 220, 255}, //pink
                                {0, 161, 163, 43, 255, 255}}; //orange

vector<Scalar> myColValue {{127, 8, 255},
                            {0, 80, 255}};

Point getContours(Mat imgDil){
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;

    findContours(imgDil, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    // drawContours(imgFrame, contours, -1, Scalar(255, 255, 255), 2);

    vector<vector<Point>> conPoly(contours.size());
    vector<Rect> boundRect(contours.size());
    vector<Moments> M(contours.size());

    Point thePoint(0, 0);

    for(int i = 0; i < contours.size(); i++){

        int area = contourArea(contours[i]);
        cout << area << endl;

        if(area > 1000){

            //ini buat nyari ada berapa sisi
            float peri = arcLength(contours[i], true);
            approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);
            // cout << conPoly[i].size() << endl;
            // cout << "-----------------------" << endl;
            boundRect[i] = boundingRect(conPoly[i]);
            thePoint.x = boundRect[i].x + boundRect[i].width/2;
            thePoint.y = boundRect[i].y;

            drawContours(imgFrame, conPoly, i, Scalar(255, 255, 255), 2);
            //tl -> top left, br -> bottom right
            rectangle(imgFrame, boundRect[i].tl(), boundRect[i].br(), Scalar(0, 255, 0), 2);
        }
    }

    return thePoint;
}                           

vector<vector<int>> findColor(Mat img){
    Mat imgMask, imgHsv;
    Point myPoint;
    cvtColor(img, imgHsv, COLOR_BGR2HSV);

    for(int i = 0; i < myColors.size(); i++){
        Scalar lower(myColors[i][0], myColors[i][1], myColors[i][2]);
        Scalar higher(myColors[i][3], myColors[i][4], myColors[i][5]);
        inRange(imgHsv, lower, higher, imgMask);
        // imshow(to_string(i), imgMask);
        myPoint = getContours(imgMask);
        if(myPoint.x != 0 && myPoint.y != 0){
            fixPoint.push_back({myPoint.x, myPoint.y, i});
            // ini tu mksdnya menggabungkan ato menahmbahkan elemen ke 1 vector yg sama
            // dari situ brati isi vectornya ada x y dan warna yang udah diseleksi tadi (myColors)
        }
    }
    return fixPoint;
}

void drawing(vector<vector<int>> fixPoint, vector<Scalar> myColValue){
    for(int i = 0; i < fixPoint.size(); i++){
        circle(imgFrame, Point(fixPoint[i][0], fixPoint[i][1]), 5, myColValue[fixPoint[i][2]], FILLED); 
        //menggambar circle -> itu koordinatnya kok pake fixPoint gara2 kita udah dapet nilainya di fuction sebelumnya
        //trus itu scalar nya kok pake myColValue[fixPoint] juga karena di array i, 2 ada value dari warna yang udah di definisiin tadi
    }
}


int main(){
    VideoCapture camera(0);

    while(1){
        camera.read(imgFrame);
        fixPoint = findColor(imgFrame);
        drawing(fixPoint, myColValue);
        
        imshow("camera", imgFrame);
        waitKey(1);
    }
    return 0;
}