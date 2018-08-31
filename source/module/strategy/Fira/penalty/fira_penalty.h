#ifndef FIRA_penalty_H
#define FIRA_penalty_H

#include <strategy.h>

enum State
{
    //FINDFOOTBALL,
    APPROACHBALL,
    ADJUSTDIR,
    //ADJUSTRL,
    ADJUSTFB,
    KICK,
    LOST,
    NUMBEROFSTATE
};
class Fira_penalty:public Strategist
{
public:
     Fira_penalty();
     ~Fira_penalty();
private:
     enum State
     {
    	FINDFOOTBALL,
    	APPROACHBALL,
    	ADJUSTDIR,
    	ADJUSTRL,
    	ADJUSTFB,
        MOVEBACK,
    	KICK,
    	LOST,
    	NUMBEROFSTATE
     };
     void Init();
     void FindBall();
     void ApproachBall();
     //void AdjustRL();
     void AdjustFB();
     void AdjustDir();
     void MoveBack();
     void Kick();
     void Lost();

/***********Image part***************/

     PenaltyImgResult *images;
     PenaltyImg *imgpro;
     
     cv::Point2f m_ball_center;
     cv::Point2f m_goal_center;
     cv::Point2f m_AddressBallCenter,m_SwingBallCenter;
     
     bool m_ball_valid;
     bool m_goal_valid;
     
     double m_RLdiff;
     double m_FBdiff;
     
     double m_findthre;
     double m_SwingDiff_X;
     double m_SwingDiff_Y;
     double m_goaldiff;
     double m_direction;
     int flag;
     int ifset;
     
     bool x_down;
     bool y_down;

     bool m_right_valid;
     
/***********Motion part**************/

     double m_FBstep;
     double m_FBstep_goal;
     double m_unit_FBstep;
     double m_FBstepcof;
     double m_FBstep_back;

     double m_RLstep;
     double m_RLstep_goal;
     double m_unit_RLstep;
     double m_RLstepcof;
     
     double m_RLturn;
     double m_RLturn_goal;
     double m_unit_RLturn;
     double m_RLturncof;
     
     double m_timescontrol;
     double m_unit_time;
     int RL_lostball;
     int AP_find;
     double m_kickdir;
     int right;
     int left;
     double m_kickflag;

/***********Other part**************/

     struct timespec now,start;
     bool m_execute;
   
     State m_process_state;
     State m_pre_state;

protected:
     virtual void ThreadMotion();
     virtual int GetImageResult();

};
#endif
