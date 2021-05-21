#include "Color.hpp"

Color::Color()
{
	setType("Color");
	setColor(Scalar(0,0,0));
}

Color::Color(string name){

	setType(name);

    if(name=="red")
    {
		setHSVmin(Scalar(141,0,0));
		setHSVmax(Scalar(184,255,255));
		//BGR value for Red:
		setColor(Scalar(0,0,255));
	}
	if(name=="green")
    {
		setHSVmin(Scalar(34,50,50));
		setHSVmax(Scalar(80,220,225));
		//BGR value for Red:
		setColor(Scalar(0,255,0));
	}
	if(name=="blue")
    {
		setHSVmin(Scalar(75, 25, 45));
		setHSVmax(Scalar(130, 150, 200));
		//BGR value for Red:
		setColor(Scalar(255,0,0));
	}
}


void Color::setHSVmin(Scalar min)
{Color::HSVmin = min;}

void Color::setHSVmax(Scalar max)
{Color::HSVmax = max;}

Scalar Color::getHSVmin()
{return Color::HSVmin;}

Scalar Color::getHSVmax()
{return Color::HSVmax;}

Color::~Color() ///Destructor
{}

