#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;

int main( int argc, char** argv )
{
    VideoCapture cap(0); //standard capturing of video from webcam, dont forget to select webcam!!!!

    if ( !cap.isOpened() )  // if not success, exit program
    {
        cout << "Cannot open the web cam" << endl;
        return -1;
    }

    namedWindow("Control", CV_WINDOW_AUTOSIZE); //create a window called "Control"

    int lowerBoundH = 0;
    int upperBoundH = 255;

    int lowerBoundS = 0;
    int upperBoundS = 255;

    int lowerBoundV = 0;
    int upperBoundV = 255;

    //Create trackbars in "Control" window
    createTrackbar("LowH", "Control", &lowerBoundH, 255); //Hue (0 - 255)
    createTrackbar("HighH", "Control", &upperBoundH, 255);

    createTrackbar("LowS", "Control", &lowerBoundS, 255); //Saturation (0 - 255)
    createTrackbar("HighS", "Control", &upperBoundS, 255);

    createTrackbar("LowV", "Control", &lowerBoundV, 255);//Value (0 - 255)
    createTrackbar("HighV", "Control", &upperBoundV, 255);

    int prevX = -1;
    int prevY = -1;

    //Capture a temporary image from the camera
    Mat tempImg;
    cap.read(tempImg);

    //Create a black image with the size as the camera output
    Mat picLines = Mat::zeros( tempImg.size(), CV_8UC3 );;


    while (true)
    {
        Mat OrgImg;

        bool readFrame = cap.read(OrgImg); // read a new frame from video



        if (!readFrame) //if not success, break loop
        {
            cout << "Cannot read a frame from video stream" << endl;
            break;
        }

        Mat convBGRHSV;

        cvtColor(OrgImg, convBGRHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV

        Mat thresholdImg;

        inRange(convBGRHSV, Scalar(lowerBoundH, lowerBoundS, lowerBoundV), Scalar(upperBoundH, upperBoundS, upperBoundV), thresholdImg); //Threshold the image

        //morphological operations: either dilute (convoluting an image A with kernel B [a maximizing operation] ) or erode (computes local min over area of kernel)
        erode(thresholdImg, thresholdImg, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );//takes in input image, output image, and kernel (default is a 3x3 matrix
        //can change to MORPH_RECT or MORPH_CROSS)
        dilate( thresholdImg, thresholdImg, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); //takes same parameters as erode()

        //morphological closing (removes small holes from the foreground)
        dilate( thresholdImg, thresholdImg, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
        erode(thresholdImg, thresholdImg, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );


        //Calculate the moments of the thresholded image
        Moments oMoments = moments(thresholdImg);
        //defining spatial moments
        double dM01 = oMoments.m01;
        double dM10 = oMoments.m10;
        double area = oMoments.m00;

        // if the area <= 10000, I consider that the there are no object in the image and it's because of the noise, the area is not zero
        if (area > 10000)
        {
            //calculate centroid of the object, from which the lines to track it shall be drawn
            int abcissa = dM10 / area;
            int ordinate = dM01 / area;
            //pretty much if it moved and if it has a centroid
            if (prevX >= 0 && prevY >= 0 && abcissa >= 0 && ordinate >= 0)
            {
                //Draw a red line from the previous point to the current point
                line(picLines, Point(abcissa, ordinate), Point(prevX, prevY), Scalar(0,0,255), 2);
            }

            prevX = abcissa;
            prevY = ordinate;
        }
        //show the thresholded image
        imshow("Thresholded Image", thresholdImg);
        //reassign the value of the original image to be the value of what it was, but now it has the lines
        OrgImg = OrgImg + picLines;

        imshow("Original", OrgImg); //show the original image

        if (waitKey(30) == 27) //standard procedure: wait for 'esc' key press for 30ms. If pressed, break loop
        {
            cout << "esc key is pressed by user" << endl;
            break;
        }
    }

    return 0;
}