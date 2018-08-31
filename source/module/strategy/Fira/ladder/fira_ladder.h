#ifndef _FIRA_LADDER_H_
#define _FIRA_LADDER_H_

#include <strategy.h>

#define IMG_WIDTH 320
#define IMG_HEIGHT 240

class Ladder:public Strategist
{
public:
    Ladder();
    ~Ladder();

private:
    enum LadderState
    {
		WALKFORWARD1,
        MOVELADDER,
		UPLADDER1,
		UPLADDER2,
		UPLADDER3,
		WALKFORWARD2,
    };

    Motion *motion;

    bool debug_print;

private://image part
    int m_point_found;
    Point2D m_point_center_2D;
 //   double m_point_center_up;
    double m_point_center_down;
	double m_point_center_x;
	double m_point_center_y;
    double m_RLTurn_degree;

    int m_tmp_img_result;

private://motion part
    void motionInit();
    void checkStatus();
	void walkForward1();
	void moveLadder();
	void upLadder1();
void upLadder2();
void upLadder3();
    void walkForward2();
    void ladderFB();
    void ladderRL(double RLTurn, double unit_RLTurn);
	void init();

    double m_period_time;
    bool m_is_checkstatus;
    int m_process_state;
    int m_target_direction;

    double m_percent_CenterDiff;
    double m_unit_percent_CenterDiff;

    double m_Xoffset;
    double m_forward_target_Xoffset;
    double m_forward_unit_Xoffset;
	double m_back_target_Xoffset;
	double m_back_unit_Xoffset;

    double m_FBStep;
    double m_RLTurn;
    double m_goal_FBStep;
    double m_goal_RLTurn;

    double m_forward_target_FBStep;
    double m_forward_unit_FBStep;
	double m_back_target_FBStep;
	double m_back_unit_FBStep;

    double m_forward_left_RLTurn;
    double m_forward_right_RLTurn;
    double m_back_left_RLTurn;
    double m_back_right_RLTurn;

    double m_forward_left_unit_RLTurn;
    double m_forward_right_unit_RLTurn;
    double m_back_left_unit_RLTurn;
    double m_back_right_unit_RLTurn;

    int back_count;
    const static int back_count_max = 3;

protected:
    virtual void ThreadMotion();
    virtual int GetImageResult();
};

#endif // _LADDER_H_
