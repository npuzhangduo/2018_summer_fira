#ifndef _SHOT_H_
#define _SHOT_H_

#include <strategy.h>


class Shot:public Strategist
{
public:
     Shot();
     ~Shot();
private:
     enum ShotState
     {
        MOVEBACK,
        FINDBALL,
        MOVEBALL,
        GETBALL,
          CHECKBALL,
        MOVEBACK2,
        FINDBASKET,
        TURNBASKET,
        MOVEBASKET,
        SHOTBALL,
        NUMBEROFSTATE,
};
     void Init();
     void MoveBack();
     void FindBall();
     void MoveBall();
     void GetBall();
     void CheckBall();
     void MoveBack2();
     void FindBasket();
     void TurnBasket();
     void MoveBasket();
     void ShotBall();

/******** General Part*******/
     
     bool m_execute;
     int debug_print;
     ShotState m_process_state;
     double m_percent_CenterDiff;
     double m_CenterDiff_unit;

/****** Image Part *********/
     
     HistogramResult* imgRes;
     Histogram* imgProc;
     cv::Point2f m_ball_center;
     cv::Point2f m_ball_center_target;
     cv::Point2f m_ball_center_diff;
     
     cv::Point2f m_basket_center;
     cv::Point2f m_basket_center_target;
     cv::Point2f m_basket_center_diff;
     
     double m_basket_up;
     double m_basket_down;
     double m_basket_diff;
     
     bool m_ball_valid;
     bool m_basket_valid;
     double pan_unit;
    
     int find_ball_times;
     int no_ball_times;
     int find_basket_times;
     int no_basket_times;
     
     bool x_direction_down;
     bool y_direction_down;
/******* Motion Part ********/
     double m_Xoffset;
     double m_Xoffset_back;
     double m_Xoffset_unit_back;
     double m_Xoffset_basket;

     double m_FBstep_back;
     double m_FBstep_forward;
     double m_FBstep_unit_back;
     
     double m_FBstep;
     double m_FBstep_goal;
     double m_FBstep_unit;
     
     double m_RLTurn;
     double m_RLTurn_goal;
     double m_RLTurn_unit;
     
     double m_ball_diff;

     int cnt1;
     int cnt2;
protected:

     virtual void ThreadMotion();
     virtual int GetImageResult();
     
};

#endif // _SHOT_H_
