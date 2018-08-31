#ifndef _GOLF_H_
#define _GOLF_H_

#include <strategy.h>

enum GolfState
{
	FINDBALL,
	ADDRESSBALL1,
	APPROACHBALL1,
	FINDHOLE,
	FINDBALL2,
	ADJUSTHOLE,
	ADJUSTRL1,
	ADJUSTFB1,
	LOST1,
	NUMBEROFGolfState
};

class Golf:public Strategist
{
public:
	Golf();
	~Golf();
private:
	void Init();
	void FindBall();
	void AddressBall1();
	void ApproachBall1();
	void FindHole();
	void FindBall2();
	void AdjustHole();
	void AdjustRL1();
	void AdjustFB1();
	void Lost();

	//Image Part
	ImgProcResult *m_imgRes;
	ImgProc *m_imgPro;

	cv::Point2f m_ball_center;
	cv::Point2f m_hole_center;
	cv::Point2f m_AddressBallCenter,m_SwingBallCenter;

	bool m_ball_valid;
	bool m_hole_valid;

	double m_ball_radius;
	double m_ApproachBallTop;
	double m_ApproachHoleTop;
	double m_AddressHoleCenter;
	double m_AddressDiff;
	double m_SwingDiff_X;
	double m_SwingDiff_Y;
	double m_diff_ball_hole;	
	double m_diff_ball_hole_standard;
	GolfState m_pre_state;

	double m_RLdiff;
	double m_FBdiff;

	//Motion Part
	double m_pan;
	double m_pan_unit;
	double m_pan_left_limit;
	double m_pan_right_limit;
	double m_tilt;
	double m_tilt_unit;

	double m_FBStep_straight;
	double m_FBStep;
	double m_FBStep_ApproachMin;
	double m_FBStep_goal;
	double m_unit_FBStep;
	double m_MAX_FBStep;

	double m_RLTurn_straight;
	double m_RLTurn;
	double m_RLTurn_goal;
	double m_RLTurn_unit;
	double m_RLTurn_max;

	double m_RLStep_straight;
	double m_RLStep;
	double m_RLStep_goal;
	double m_unit_RLStep;
	double m_MAX_RLStep;
	int swing_count;
		
	int tmp_img_result;
	int tmp_img_result2;
	int tmp_return_value;

	//Other Part
	int flag;
	struct timespec now, start;
	double m_experience_data;
	double m_experience_date_unit;
	int debug_print;
	bool m_execute;
	GolfState m_process_state;
	double experience_data;
	int m_pre_action;// 1:move left  0:go straight  -1:move right 

protected:
	virtual void ThreadMotion();
	virtual int GetImageResult();
};

#endif 
