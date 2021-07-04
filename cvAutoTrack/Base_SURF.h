#pragma once

enum SurfMatchFlag
{
	defalut,
	rasan
};

class Base_SURF
{
	void setMatch(SurfMatchFlag flag);

	void setSource(Mat img);
	void setTarget(Mat img);

	void run();
	
	void getKeyPointMatch();
};

class SURF_Map
{
	Base_SURF surf;


};