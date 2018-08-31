/**          weightliftinf4.0              */
/**          9.24               **/


#ifndef WEIGHTLIFTING_H
#define WEIGHTLIFTING_H

#include <strategy.h>

#define IMG_WIDTH 320
#define IMG_HEIGHT 240

class weightlifting:public Strategist
{
public:
    weightlifting();
    ~weightlifting();

private:
    enum WeightliftingState
    {
	FORWARD,
        MOVEFORWARD,
        LIFT
    };

    Motion *motion;

    bool debug_print;

private://image part
    int m_point_found;
    Point2D m_point_center_2D;
    double m_angle;
    double m_point_center_up;
    double m_point_center_down;
    double m_RLTurn_degree;

    int m_tmp_img_result;

private://motion part
    void motionInit();
    void checkStatus();
    void walkForward();
    void Forward();
    void lift();
    void weightliftingFB();
    void weightliftingRL(double RLTurn, double unit_RLTurn);
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


    double m_FBStep;
    double m_RLTurn;
    double m_goal_FBStep;
    double m_goal_RLTurn;

    double m_forward_target_FBStep;
    double m_forward_unit_FBStep;


    double m_forward_left_RLTurn;
    double m_forward_right_RLTurn;

    double m_rate1;
    double m_rate2;


    double m_forward_left_unit_RLTurn;
    double m_forward_right_unit_RLTurn;


protected:
    virtual void ThreadMotion();
    virtual int GetImageResult();


};

#endif // WEIGHTLIFTING_H
