#ifndef _Fira_Marathon_H_
#define _Fira_Marathon_H_

#include<strategy.h>

class Fira_Marathon:public Strategist
{
public:
	Fira_Marathon();
	~Fira_Marathon();
private:
	enum Fira_MarathonState
	{
		NOLINE,
		LINE,
		JUDGE,
		LOST,
	};
	enum PreState
	{
		RIGHT,
		LEFT,
	};

	//

	bool m_execute;
	int debug_print;
	Fira_MarathonState m_process_state;
	PreState m_pre_state;
	
	double m_percent_CenterDiff;
	double m_percent_CenterDiff_unit;
	int max_speed_count;
	
	//IMAGE

	int m_noline_count;
	int m_noline_max;
	double m_line_angle;
	bool m_line_found;
	
	int tmp_img_result;

	cv::Point2f   m_line_center;
	cv::Point2f   m_line_center_diff;
	
	ImgProcResult *m_imgRes;
	ImgProc *m_imgProc;

	double pan_unit;
	double pan;
	double tilt;

	//MOTION
	int m_MAX_RLTurn;
	double m_RLTurn_unit;
	double m_RLTurn_goal;
	double m_pre_slope;
	double m_xoffset;
	double m_xoffdet_unit;
	double m_FBStep;
	double m_FBStep_unit;
	double m_RLStep;
	double m_unit_RLTurn;
	double m_RLTurn;
	

	void NoLine();
	void Line();
	void Judge();
	void Lost();
	void RLTurn(double RLTurn,int turn);

protected:
	virtual void ThreadMotion();
	virtual int GetImageResult();
};
#endif//Fira_Marathon_H_
