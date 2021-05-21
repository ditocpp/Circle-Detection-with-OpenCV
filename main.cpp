///-----------------------------------------------------------------------------------------------
/// PROJECT: CIRCLE OF ORDER
/// Algorithm & Data Structures Final Semester Project 2019
/// Program to detect, track, and identify circles of RGB color taken from webcam
///-----------------------------------------------------------------------------------------------
/// Credits to : Kyle Hounslow (2013) [HSV 8-bit color Object Tracking]
///-----------------------------------------------------------------------------------------------
/// Group Members :
/// Eka Kurnia Wibowo - H1A018040
/// Rafli Nur Faizi - H1A018046
/// Bima Adityo Kurniawan - H1A018048
/// Firany Dini Safitri - H1A018056
/// Rizky Suka H - H1A08058
/// Haris Prasetyo - H1A01864
///-----------------------------------------------------------------------------------------------
#include "Color.hpp"
#include <iostream>
#include <vector>
#include <opencv2\highgui\highgui.hpp> /// opencv built-in graphics user interface
#include <opencv2\opencv.hpp> /// opencv main header
#include <opencv2\imgcodecs\imgcodecs.hpp>

using namespace std;
using namespace cv;
///-----------------------------------------------------------------------------------------------
/// GLOBAL VARIABLES
///-----------------------------------------------------------------------------------------------
Mat camera_feed;
Mat filter_feed;
Mat HSV_filter;
Mat treshold;
///-----------------------------------------------------------------------------------------------
/// METHODS WORKSPACE
///-----------------------------------------------------------------------------------------------
string intToString(int number) /// Converts integer input as a string type
{
	stringstream s;
	s << number;
    return s.str();
}

void Circle_Detector(Color input_color, Mat input, Mat &camerafeed) /// Detects Circles in RGB
{
        vector<Vec3f> circles; /// Vector to store x, y, r of circles
        HoughCircles(input, circles, HOUGH_GRADIENT,
        2,   /// accumulator resolution (size of the image / 2)
        input.rows/4,  /// minimum distance between two circles
        100, /// internal Canny high threshold
        100, /// minimum number of votes for center detection
        10, 100); /// min and max radius
        for( size_t i = 0; i < circles.size(); i++ ) /// Circle object counter
        {
            Vec3i a = circles[i];
            Point center = Point(a[0], a[1]);
            /// draw circle center & outline---------------------------------------------------------------------
            circle( camerafeed, center, 1, input_color.getColor(), 3, LINE_AA);
            int radius = a[2];
            circle( camerafeed, center, radius, input_color.getColor(), 3, LINE_AA);
            /// radius, Area, and Perimeter of the circles in vector scalar units--------------------------------
            putText(camerafeed,format("%d",i+1),Point(a[0],a[1]-10),2,1,input_color.getColor()); ///count
            putText(camerafeed,format("r: %d",radius),Point(a[0]+9,a[1]+10),2,0.5,input_color.getColor(),1); ///radius
            putText(camerafeed,format("L: %d",(22/7)*radius*radius),Point(a[0]+9,a[1]+25),2,0.5,input_color.getColor(),1); ///area
            putText(camerafeed,format("K: %d",(22/7)*2*radius),Point(a[0]+9,a[1]+35),2,0.5,input_color.getColor(),1); ///perimeter
        }
        /// Placing RGB circle counters----------------------------------------------------------------
        if(input_color.getType()=="red")
        {putText(camerafeed,intToString(circles.size()),Point(220,15),2,0.5,input_color.getColor(),1);}
        if(input_color.getType()=="green")
        {putText(camerafeed,intToString(circles.size()),Point(380,15),2,0.5,input_color.getColor(),1);}
        if(input_color.getType()=="blue")
        {putText(camerafeed,intToString(circles.size()),Point(530,15),2,0.5,input_color.getColor(),1);}
        putText(camerafeed,"Red Circle  :",Point(100,15),2,0.5,Scalar(0,0,255),1);
        putText(camerafeed,"Green Circle:",Point(260,15),2,0.5,Scalar(0,255,0),1);
        putText(camerafeed,"Blue Circle :",Point(420,15),2,0.5,Scalar(255,0,0),1);
}

void Noise_Reductor(Mat &input_filter) /// Filters the frame image for smoother circle detection
{
    //morphological opening (removes small objects from the foreground)
    erode(input_filter, input_filter, getStructuringElement(MORPH_ELLIPSE, Size(3, 3)) );
    dilate( input_filter, input_filter, getStructuringElement(MORPH_ELLIPSE, Size(8, 8)) );
    //morphological closing (removes small holes from the foreground)
    dilate( input_filter, input_filter, getStructuringElement(MORPH_ELLIPSE, Size(3, 3)) );
    erode(input_filter, input_filter, getStructuringElement(MORPH_ELLIPSE, Size(8, 8)) );
    GaussianBlur(input_filter, input_filter, Size(9, 9), 2, 2 );
    //medianBlur(input_filter, input_filter, 5);
    //Canny(input_filter, input_filter, 17, 17*3, 3);
}

void drawlines(vector<Color> theObjects,Mat &frame, Mat &temp, vector< vector<Point> > contours, vector<Vec4i> hierarchy)
{
    /// draws contour lines taken from circle_tracker method, lines will follow default RGB color from HSV setting
	for(int i =0; i<theObjects.size(); i++)
    {drawContours(frame,contours,i,theObjects.at(i).getColor(),1,LINE_4,hierarchy);}
}

void Circle_Tracker(Color colored,Mat threshold, Mat &camerafeed) /// tracks detected circles and store them in matrix
{
	vector <Color> colors;
	Mat temp; ///temporary matrix storage
	threshold.copyTo(temp);
    vector< vector<Point> > contours; /// store x,y coordinate
	vector<Vec4i> hierarchy; /// contour count
	findContours(temp,contours,hierarchy,RETR_LIST,CHAIN_APPROX_SIMPLE );///find contours of filtered image
	///use moments method to find our filtered object
	bool objectFound = false;
	if (hierarchy.size() > 0) {
		int numObjects = hierarchy.size();
		if(numObjects<100) /// if number of objects greater than 100 we have a noisy filter
        {   for (int index = 0; index >= 0; index = hierarchy[index][0])
            {   Moments moment = moments((cv::Mat)contours[index]);
				double area = moment.m00;
				if(area>400) ///if the area is less than 20 px by 20px (400) then it is probably just noise
				{   Color object;
					object.setType(colored.getType());
					object.setColor(colored.getColor());
					colors.push_back(object);
					objectFound = true;
				}else objectFound = false;
			}
			if(objectFound ==true)///let user know you found an object
            {drawlines(colors,camerafeed,temp,contours,hierarchy);}///draw object location on screen
            }else
            putText(camerafeed,"TOO MUCH NOISE, ADJUST BRIGHTNESS",Point(0,50),1,1,Scalar(0,0,255),2);
	}
}

void Executor(Mat camera1, Mat camera2, Mat frame1, Mat frame2, Color colour)
{
    cvtColor(camera1,frame1,COLOR_BGR2HSV);///CONVERT RGB to HSV
    inRange(frame1, colour.getHSVmin(),colour.getHSVmax(), frame2);///set HSV color
    Noise_Reductor(frame2); /// filter the camera feed
    Circle_Detector(colour,frame2,camera1); /// detects circles and show them in actual footage
    Circle_Detector(colour,frame2,camera2); /// detects circles and show them in filtered footage
    Circle_Tracker(colour,frame2,camera2); /// tracks the edges and draws contours
}

///-----------------------------------------------------------------------------------------------
/// MAIN EXECUTOR
///-----------------------------------------------------------------------------------------------
int main()
{
    VideoCapture webcam(0);
    if(!webcam.isOpened())
    {
        cout << "ERROR 69 : Cannot open webcam.3gp" << endl;
        return -1;
    }
    int frameCounter = 0;
    int tick = 0;
    int fps;
	time_t timeBegin=time(0);
	waitKey(1000);
    while(1)
    {
        ///FRAME PER SECOND CALCULATIONS--------------------------------------------------------------------------
        frameCounter++;
        time_t timeNow=time(0)-timeBegin;
        if (timeNow - tick >= 1)
        {
            tick++;
            fps = frameCounter;
            frameCounter = 0;
        }
        bool Zucc = webcam.read(camera_feed);
        if(!Zucc)
        {
            cout << "Laptop anda kentang ya?" << endl;
            break;
        }
        webcam.read(filter_feed);
        ///BLACK BOX UPPER SCREEN CREATION--------------------------------------------------------------------------
        rectangle(camera_feed,Point(0,20),Point(900,0),Scalar(0,0,0),-1,8);
        rectangle(filter_feed,Point(0,900),Point(900,0),Scalar(0,0,0),-1,8);
        rectangle(camera_feed,Point(0,20),Point(900,0),Scalar(255,255,255),1,8);
        rectangle(filter_feed,Point(0,20),Point(900,0),Scalar(255,255,255),1,8);
        putText(camera_feed,format("FPS : %d", fps ),Point(5,15),5,0.5,Scalar(255,255,255),1);
        putText(filter_feed,format("FPS : %d", fps ),Point(5,15),5,0.5,Scalar(255,255,255),1);
        ///RED GREEN BLUE RGB-HSV COLORS OBJECT DECLARATION---------------------------------------------------------
        Color red("red"), green("green"), blue("blue");
        ///PROCESSING FRAMES----------------------------------------------------------------------------------------
        Executor(camera_feed, filter_feed, HSV_filter, treshold, red);
        Executor(camera_feed, filter_feed, HSV_filter, treshold, green);
        Executor(camera_feed, filter_feed, HSV_filter, treshold, blue);
        ///OPEN WINDOW TO SHOW THE FEED------------------------------------------------------------------------------
        imshow("Filtered Feed", filter_feed);
        imshow("Actual Feed", camera_feed);
        ///EXIT CONDITIONS-------------------------------------------------------------------------------------------
        if (waitKey(30) == 27) ///wait for 'esc' key press for 30ms. If 'esc' key is pressed, end loop
        {
            cout << "Exit .w." << endl;
            break;
        }
    }
    return 0;
}
