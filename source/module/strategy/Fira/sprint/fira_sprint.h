#ifndef _FiraSprint_H_
#define _FiraSprint_H_

#include <strategy.h>

#define IMG_WIDTH 320
#define IMG_HEIGHT 240
enum FiraSprintState
{
        MOVEFORWARD,
        MOVESLOW,
        MOVEBACK,
	MOVEBACK0,
        NOTFIND
};
class FiraSprint:public Strategist
{
public:
    FiraSprint();
    ~FiraSprint();

private:
    

    Motion *motion;

    bool debug_print;
    bool m_excute;
private://image part
    int m_point_found;
    Point2D m_point_center_2D;
    double m_point_center_up;
    double m_point_center_down;
    double m_RLTurn_degree;

    int m_tmp_img_result;

private://motion part
    void motionInit();
    void checkStatus();
    void moveForward();
    void moveSlow();
    void moveBack();
	void moveBack0();
    void moveForwardOrBack();
    void lostTarget();
    void speedUp();
    void FiraSprintFB();
    void FiraSprintRL(double RLTurn, double unit_RLTurn);
	void init();

    double m_period_time;
    bool m_is_checkstatus;
    
    int m_target_direction;

    double m_percent_CenterDiff;
    double m_unit_percent_CenterDiff;

    double m_Xoffset;
    double m_forward_target_Xoffset;
    double m_forward_unit_Xoffset;
	double m_back_target_Xoffset;
	double m_back_unit_Xoffset;
    //
    double m_FBStep;
    double m_RLTurn;
    double m_goal_FBStep;
    double m_goal_RLTurn;
   
    double m_max_FBStep;
    double m_max_RLTurn;
    double m_unit_RLTurn;

    FiraSprintState m_process_state;
    
    bool if_find;
    double m_RLStep;
    //
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

#endif // _FiraSprint_H_
