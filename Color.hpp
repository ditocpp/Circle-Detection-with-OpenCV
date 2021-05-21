#pragma once /// better include guard than ifndef
#include <string>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\opencv.hpp>
using namespace std;
using namespace cv;

class Color{
    private:
        string type;
        Scalar HSVmin, HSVmax;
        Scalar Colored;

    public:
        Color(); //constructor
        Color(string name);

        Scalar getHSVmin();
        Scalar getHSVmax();

        void setHSVmin(Scalar min);
        void setHSVmax(Scalar max);

        string getType(){return type;}
        void setType(string t){type = t;}

        Scalar getColor()
        {return Colored;}
        void setColor(Scalar z)
        {Colored = z;}

        ~Color(void); //destructr
};
