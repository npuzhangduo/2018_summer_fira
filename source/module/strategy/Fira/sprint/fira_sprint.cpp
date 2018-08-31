#include <MotionStatus.h>
#include "fira_sprint.h"
#include <iostream>
FiraSprint::FiraSprint()
{
    imgRes = new wyResult;
    imgProc = new wy;
    //
    m_is_checkstatus = true;
    m_excute = true;
    m_Xoffset = -10;
    m_process_state = MOVEFORWARD;

    m_period_time = 550;

    m_FBStep = 0;
    m_forward_unit_FBStep = 0.8;
    m_goal_FBStep = 0;
    
    if_find = true;
    m_RLTurn = 0;
    m_goal_RLTurn = 0;
    m_unit_RLTurn = 5.0;

    m_RLStep = 0;
    //
    debug_print = true;
    
}

FiraSprint::~FiraSprint()
{
}

void FiraSprint::ThreadMotion()
{
        motionInit();
        
	wy *tmp_proc = dynamic_cast<wy *>(imgProc);
        while(0)
        {
             if(m_process_state == 0) cout <<"movedoward"<<endl;
             else cout <<"NOTFIND"<<endl;
             checkStatus();
             if_find = GetImageResult();
             
             if(m_process_state == MOVEFORWARD)
                 moveForward();
             else if(m_process_state == NOTFIND)
                 lostTarget();
        }
        while(1)
        {
            moveForward();
        }
    
        
}
void FiraSprint::moveForward()
{
     motion->walk(2,0,0);
 
     //Action::GetInstance()->m_Joint.SetEnableBody(true, true);
     //Action::GetInstance()->Start(21);
     //usleep(1000*80000);
     /*if(!if_find) m_process_state = NOTFIND;
     m_goal_FBStep = 25;
     m_RLStep = 0;
     m_goal_RLTurn = 50 * m_target_direction * m_RLTurn_degree;
     if(m_target_direction == 1) m_goal_RLTurn *= 2;
     
     //前进
     if(m_FBStep < m_goal_FBStep)
	m_FBStep += m_forward_unit_FBStep;
     else if(m_FBStep > m_goal_FBStep)
	m_FBStep = m_goal_FBStep;
     //转弯
     if(m_RLTurn < m_goal_RLTurn)
	m_RLTurn += m_unit_RLTurn;
     else if(m_RLTurn > m_goal_RLTurn)
	m_RLTurn -= m_unit_RLTurn;
     //Walking::GetInstance()->m_Joint.SetEnableBodyWithoutHead(true, true);
     cout << m_goal_RLTurn <<endl;
     cout <<m_goal_FBStep <<endl;
     cout <<m_FBStep<<endl;
     motion -> walk(0, 0, 0); */
}
void FiraSprint::lostTarget()
{
   if(if_find) m_process_state = MOVEFORWARD;
     m_goal_FBStep = 0;
     m_RLStep = 0;
     m_goal_RLTurn = 0;
     
     if(m_target_direction == -1)
     //前进
     if(m_FBStep < m_goal_FBStep)
	m_FBStep += m_forward_unit_FBStep;
     else if(m_FBStep > m_goal_FBStep)
	m_FBStep = m_goal_FBStep;
     //转弯
     if(m_RLTurn < m_goal_FBStep)
	m_RLTurn += m_unit_RLTurn;
     else if(m_RLTurn > m_goal_FBStep)
	m_RLTurn -= m_unit_RLTurn;
     //Walking::GetInstance()->m_Joint.SetEnableBodyWithoutHead(true, true);
     motion -> walk(0, m_RLStep, m_RLTurn); 
}

int FiraSprint::GetImageResult()
{

    while (is_new_img == false)  usleep(8000);

    imgProc->imageProcess(frame,imgRes);
    wyResult *tmp_result = dynamic_cast<wyResult *>(imgRes);
        
	m_point_found = tmp_result -> obj_valid;
	//printf("m_point_found:%d\n", m_point_found);
    if (!m_point_found)
        return  0;
    else 
    {
        m_point_center_2D.X = tmp_result -> center.x;
        m_point_center_2D.Y = tmp_result -> center.y;
        m_RLTurn_degree = fabs((m_point_center_2D.X - IMG_WIDTH / 2.0) / IMG_WIDTH);
    
        if (m_point_center_2D.X > IMG_WIDTH / 2.0)
        {
               m_target_direction = -1;
               cout <<"right"<<endl;
        }
        else if (m_point_center_2D.X < IMG_WIDTH / 2.0)
        {
               m_target_direction = 1;
               cout <<"left"<<endl;
        }
        else
            m_target_direction = 0;
    }

    is_new_img = false;
	return 1;
}

void FiraSprint::motionInit()
{
    cout <<"begin init"<<endl;
    motion = new Motion();
    cout <<"finish initialize motion and began to poseInit"<<endl;
    motion -> poseInit();
    cout <<"finish poseInit and set head"<<endl;
    Head::GetInstance() -> MoveByAngle(0, 50);
    usleep(100 * 8000);
    cout <<"begin to set walking"<<endl;
    Walking::GetInstance() -> Start();
    Walking::GetInstance() -> X_OFFSET = m_Xoffset;
    Walking::GetInstance() -> PERIOD_TIME = m_period_time;
    Walking::GetInstance() -> Z_MOVE_AMPLITUDE = 30;
    cout <<"finish moinInit"<<endl; 
}

void FiraSprint::checkStatus()
{
    if (m_is_checkstatus)
    {
        motion -> CheckStatus();
    }
}

/*void FiraSprint::FiraSprintRL(double RLTurn, double unit_RLTurn)
{
    if (m_process_state == MOVEFORWARD && m_target_direction == -1)
        m_goal_RLTurn = fabs(m_RLTurn_degree) * 2.5 * RLTurn;
	else if (m_process_state == MOVEFORWARD && m_target_direction == 1)
        m_goal_RLTurn = fabs(m_RLTurn_degree) * 2.0 * RLTurn;
    else if (m_process_state == MOVEBACK && m_target_direction == 1)
        m_goal_RLTurn = fabs(m_RLTurn_degree) * 6.0 * RLTurn;
	else if (m_process_state == MOVEBACK && m_target_direction == -1)
        m_goal_RLTurn = fabs(m_RLTurn_degree) * 5.0 * RLTurn;

    if (m_RLTurn < 0 && m_goal_RLTurn > 0 || m_RLTurn > 0 && m_goal_RLTurn < 0)
		unit_RLTurn *= 2.5;

    if (m_RLTurn < m_goal_RLTurn /*&& m_RLTurn + unit_RLTurn < m_goal_RLTurn)
        m_RLTurn += unit_RLTurn;
    else if (m_RLTurn > m_goal_RLTurn /*&& m_RLTurn - unit_RLTurn > m_goal_RLTurn)
        m_RLTurn -= unit_RLTurn;

	FiraSprintFB();
}
*/




