#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "comm.hpp"
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
using namespace cv;
using namespace std;

int lmap(int x, int in_min, int in_max, int out_min, int out_max)
{
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

int main( int argc, char** argv )
{
        if(argc != 2){
                printf("Usage: ./colortrack <serial port>\n");
                return 3;
        }
        VideoCapture cap(0); //capture the video from webcam
        cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);
        cap.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
        cap.set(CV_CAP_PROP_FPS, 60);
        if ( !cap.isOpened() )  // if not success, exit program
        {
                cout << "Cannot open camera" << endl;
                return -1;
        }
        comm_start(argv[1]);
        namedWindow("Control", CV_WINDOW_NORMAL); //create a window called "Control"
        int iLowH = 170;
        int iHighH = 179;

        int iLowS = 150; 
        int iHighS = 255;

        int iLowV = 60;
        int iHighV = 255;
        int dSlider = 0;
        int minSz = 50;
        unsigned char steer=127, power=127;

        //Create trackbars in "Control" window
        createTrackbar("LowH", "Control", &iLowH, 179); //Hue (0 - 179)
        createTrackbar("HighH", "Control", &iHighH, 179);

        createTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)
        createTrackbar("HighS", "Control", &iHighS, 255);

        createTrackbar("LowV", "Control", &iLowV, 255);//Value (0 - 255)
        createTrackbar("HighV", "Control", &iHighV, 255);


        createTrackbar("Minimum obj. size", "Control", &minSz, 200);
        createTrackbar("Car Speed", "Control", &dSlider, 127);

        //Capture a temporary image from the camera
        Mat imgTmp;
        Mat imgSample = Mat::zeros(100, 150, CV_8UC3);
        cap.read(imgTmp); 

        //Create a black image with the size as the camera output
        Mat imgLines = Mat::zeros( imgTmp.size(), CV_8UC3 );


        while (true)
        {
                Mat imgOriginal;

                bool bSuccess = cap.read(imgOriginal); // read a new frame from video



                if (!bSuccess) //if not success, break loop
                {
                        cout << "Cannot read a frame from video stream" << endl;
                        break;
                }
                comm_check();
                Mat imgHSV;

                cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV

                Mat imgThresholded;

                inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image

                //morphological opening (removes small objects from the foreground)
                erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
                dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 

                //Calculate the moments of the thresholded image
                Moments oMoments = moments(imgThresholded);

                double dM01 = oMoments.m01;
                double dM10 = oMoments.m10;
                double dArea = oMoments.m00;
                Point center;
                int width;
                center = Point(imgTmp.size());
                width = center.x;
                center.x *= 0.5;
                center.y *= 0.5;
                // if the area <= 50000 px, its likely just noise

                imgLines = Mat::zeros( imgTmp.size(), CV_8UC3 );
                if (dArea > ((double)minSz*1000))
                {
                        //calculate the position of the ball
                        int posX = dM10 / dArea;
                        int posY = dM01 / dArea;        

                        if (posX >= 0 && posY >= 0)
                        {
                                //Draw a line from the previous point to the current point
                                line(imgLines, Point(posX, posY), center, Scalar(0,0,255), 2);
                        }
                        if(dSlider > 0){
                                steer=constrain(lmap(posX,0,width,-20,275),0,255);
                                power=constrain((127-dSlider)+(abs(steer-127)/4),0,120);
                        } else {
                                power=255;
                                steer=127;
                        }
                } else {
                        power=255;
                        steer=127;
                }

                pthread_mutex_lock( &lck );
                s_power = power;
                s_steer = steer;
                pthread_mutex_unlock( &lck );

                imshow("Thresholded Image", imgThresholded); //show the thresholded image

                imgOriginal = imgOriginal + imgLines;
                imshow("Original", imgOriginal); //show the original image
                rectangle(imgSample,Point(0,0),Point(75,100),Scalar(iLowH,iLowS,iLowV),CV_FILLED,8,0);
                rectangle(imgSample,Point(75,0),Point(150,100),Scalar(iHighH,iHighS,iHighV),CV_FILLED,8,0);
                cvtColor(imgSample,imgSample,COLOR_HSV2BGR);
                imshow("sample",imgSample);
                if (waitKey(20) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
                {
                        cout << "esc key pressed, exiting" << endl;
                        break; 
                }

        }
        comm_stop();
        return 0;
}
