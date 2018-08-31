//Version 2.0
//Date 2017.1.12
//Changed by Abraham
//It is the version that we used at last in 2016.12.18.
//Problem01 The motion initial is not proper Action::GetInstance()->Start(14) need to change.
//Problem02 The process of motion()->swing() is too slow we can change it in motion.cpp and change its FBstep 
//and its not so proper for static RLturn.We need to change its RLturn by the degree we need to adjust
//Problem03 The process of Moveback is not straight.
//Problem04 Maybe we need two way to design the process of move.Stable and Speed is two side of move that we need to consider.
//Problem05 The LostDispose doesn't process so well.

//Version 3.1  Edited by Forever
//This version is stable for getting the ball!!!
//Thanks to Abraham
/*
Version 4.0 edited by steer
based on the version 3.1
*/
#include "fira_shot.h"

Shot::Shot()
{
   imgRes = new HistogramResult();
   imgProc = new Histogram();
   Histogram *tmp_proc = dynamic_cast<Histogram *>(imgProc);
  
/***********General Part***********/  
   
   m_execute = true;
   m_process_state = MOVEBACK;
        
   debug_print = 1;
   m_percent_CenterDiff = 0.000;
   m_CenterDiff_unit = 0.001;
   
/***********Image Part***********/
   
   m_ball_center.x = 0;
   m_ball_center.y = 0;
   m_ball_center_target.x = 138.5;
   m_ball_center_target.y = 153.5; //162.5
   m_ball_center_diff.x = 15;
   m_ball_center_diff.y = 12;
   
   m_basket_center.x = 0;
   m_basket_center.y = 0;
   m_basket_center_target.x = 189.0;
   m_basket_center_target.y = 151;
   m_basket_center_diff.x = 15;
   m_basket_center_diff.y = 15;
   
   x_direction_down = false;
   y_direction_down = false;
   
   m_basket_up = 0;
   m_basket_down = 0;
   m_basket_diff = 0;

   find_ball_times = 0;
   no_ball_times = 0;
   no_basket_times = 0;
   find_basket_times = 0;
/***********Motion Part************/
   
   m_Xoffset = 3;
   m_Xoffset_back = 4.0;
   m_Xoffset_unit_back = 1.5;
   m_Xoffset_basket = -1.0;
   
   m_FBstep_back = -10;
   m_FBstep_forward = 10;
   m_FBstep_unit_back = 1.0;   

   m_FBstep = 0;
   m_FBstep_goal = 10;
   m_FBstep_unit = 0.6;
   
   m_RLTurn = 0;
   m_RLTurn_goal = 0;
   m_RLTurn_unit = 1.5;
   
   m_ball_diff = 0;
   pan_unit = -7.0;
   
   cnt1 = 0;
   cnt2 = 0;
}
Shot::~Shot()
{
}
void Shot::ThreadMotion()
{
   Init();
   Action::GetInstance()->m_Joint.SetEnableBodyWithoutHead(true,true);
   usleep(50 * 8000);
   Action::GetInstance()->Start(88);//展开手
   usleep(700 * 8000);
   Walking::GetInstance()->m_Joint.SetEnableBodyWithoutHead(true, true);   //Lock arm again
   Walking::GetInstance()->m_Joint.SetEnableLeftArmOnly(false,false);
   Walking::GetInstance()->m_Joint.SetEnableRightArmOnly(false,false);
   Head::GetInstance()->m_Joint.SetEnableHeadOnly(true, true);
   while(m_execute)
   {
       GetImageResult();
       if (m_process_state != MOVEBACK && m_process_state != MOVEBACK2) 
            m_percent_CenterDiff = 0;
       if(m_process_state == MOVEBACK)
          MoveBack();
       else if(m_process_state == FINDBALL)
          FindBall();
       else if(m_process_state == MOVEBALL)
          MoveBall();
       else if(m_process_state == GETBALL)
          GetBall();
       //else if(m_process_state == CHECKBALL)
         // CheckBall();
       else if(m_process_state == MOVEBACK2)
          MoveBack2();
       else if(m_process_state == FINDBASKET)
          FindBasket();
       else if(m_process_state == TURNBASKET)
          TurnBasket();
       else if(m_process_state == MOVEBASKET)
          MoveBasket();
       else if(m_process_state == SHOTBALL)
          ShotBall();
   }
}
int Shot::GetImageResult()
{
    while( is_new_img == false ) usleep(8000);
    imgProc->imageProcess(frame,imgRes);
    HistogramResult *tmp_result = dynamic_cast<HistogramResult *>(imgRes);
    m_ball_center = tmp_result->ball_center;
    m_basket_center = tmp_result->basket_center;
    m_basket_up = tmp_result->basket_center_up;
    m_basket_down = tmp_result->basket_center_down;
    m_ball_valid = tmp_result->ball_valid;
    m_basket_valid = tmp_result->basket_valid;
    is_new_img = false;
    printf("m_basket_up:%f\n",m_basket_up);
    printf("m_basket_down:%f\n",m_basket_down);
    return 0;
}
void Shot::Init()
{
    motion = new Motion();
    motion->poseInit();
    Action::GetInstance()->m_Joint.SetEnableBody(true,true);
    while(Action::GetInstance()->IsRunning() == 1) usleep(8000); //?
    Walking::GetInstance()->m_Joint.SetEnableBodyWithoutHead(true, true);//After the initial motion
    Walking::GetInstance()->m_Joint.SetEnableLeftArmOnly(false,false);  //Lock its arms
    Walking::GetInstance()->m_Joint.SetEnableRightArmOnly(false,false);
    Head::GetInstance()->m_Joint.SetEnableHeadOnly(true, true);
    Head::GetInstance()->MoveByAngle(0, 20);
   
    Walking::GetInstance()->X_OFFSET = -5;
    Walking::GetInstance()->Z_MOVE_AMPLITUDE = 30;
    Walking::GetInstance()->Start();
}

void Shot::MoveBack()
{
    //Walking::GetInstance()->Stop();
    
    printf("MoveBack\n");
    Head::GetInstance()->MoveByAngle(0,20);
    Walking::GetInstance()->m_Joint.SetEnableLeftArmOnly(false,false);//Lock its arms
    Walking::GetInstance()->m_Joint.SetEnableRightArmOnly(false,false);
    if(m_Xoffset < m_Xoffset_back)
        m_Xoffset += m_Xoffset_unit_back;
    Walking::GetInstance()->X_OFFSET = m_Xoffset;
    
    m_percent_CenterDiff += m_CenterDiff_unit;
   
    if(m_FBstep > m_FBstep_back)
        m_FBstep -= m_FBstep_unit_back;
    motion->walk(m_FBstep,0,0);
    if (m_percent_CenterDiff >= 0.018)//0.025
    {
        Walking::GetInstance() -> Stop();
        usleep(100 * 8000);
        pan_unit = 7.0;
        m_process_state = FINDBALL;
    }
}

void Shot::FindBall()
{
    printf("FindBall\n");
    const int find_ball_max = 1;
    const int no_ball_max = 2;
    usleep(50 * 8000);
    GetImageResult();
    Walking::GetInstance()->Stop();
    double tilt = Head::GetInstance()->GetTiltAngle();
    //static double tilt_unit = 10;
    //static double tilt_down_limit = -20;

    
    if(m_ball_valid)
    {
        find_ball_times++;
        no_ball_times = 0;
        if(find_ball_times >= find_ball_max)
        {
            m_process_state = MOVEBALL;
            printf("Findball change to Moveball");
            find_ball_times = 0;
            no_ball_times = 0;
            x_direction_down = false;
            y_direction_down = false;
        }
    }
    else
    {
        no_ball_times++;
        if(no_ball_times >= no_ball_max)
        {
            double tilt = Head::GetInstance()->GetTiltAngle();
            Head::GetInstance() -> MoveByAngle(0, tilt - 5);
            m_process_state = MOVEBALL;
            find_ball_times = 0;
            no_ball_times = 0;
            printf("Findball change to Moveball,but not find");
        }
     
    }
}

void Shot::MoveBall()
{
    printf("Moveball\n");
    const int no_ball_max = 5;
    static int cntx = 0;
    static int cnty = 0;
    static double tilt_unit = 5.0;
    //static double tilt_down_limit = -20;
    Walking::GetInstance()->Start();
    Walking::GetInstance()->Z_MOVE_AMPLITUDE = 10;
    
    double tilt = Head::GetInstance()->GetTiltAngle();
    if (tilt > -10)
    {
	tilt = tilt-tilt_unit;
	printf("tilt:%f\n",tilt);
    }
    else if (tilt <= -10)
       tilt_unit = tilt_unit; 
    Head::GetInstance() -> MoveByAngle(0, tilt);
    usleep(2 * 8000);
    Walking::GetInstance() -> X_OFFSET = m_Xoffset;
    if(!m_ball_valid)
    {
        no_ball_times++;
        if(no_ball_times > no_ball_max)
        {
            m_process_state = FINDBALL;
            find_ball_times = 0;
            no_ball_times = 0;
            printf("Findball change to Moveball");
	}
        return;
    }
    Walking::GetInstance() -> X_OFFSET = m_Xoffset;
    /*if(m_ball_center.y >= 200 && tilt <= -10)
    {
        m_process_state = MOVEBACK;
        printf("Back to MOVEBACK\n");
        find_ball_times = 0;
        no_ball_times = 0;
        return;
    }*/
    if (tilt<=-10 && m_ball_center.x >= m_ball_center_target.x - m_ball_center_diff.x &&
        m_ball_center.x <= m_ball_center_target.x + m_ball_center_diff.x)
    {
        cntx++;
        if(cntx == 3)
        {
            x_direction_down = true;
            cntx = 0;
        }
    }
    if (tilt<=-10 && m_ball_center.y >= m_ball_center_target.y - m_ball_center_diff.y &&
	m_ball_center.y <= m_ball_center_target.y + m_ball_center_diff.y)
    {
        cnty++;
        if(cnty == 3)
        {
            y_direction_down = true;
        }
    }
    m_ball_diff = m_ball_center_target.x - m_ball_center.x;
    m_RLTurn_goal = 0.4 * m_ball_diff;
    if (m_RLTurn > 0 && m_RLTurn_goal < 0 || m_RLTurn < 0 && m_RLTurn_goal > 0)
        m_RLTurn = 0;
    else if (m_RLTurn < m_RLTurn_goal)
        m_RLTurn += m_RLTurn_unit;
    else if (m_RLTurn > m_RLTurn_goal)
        m_RLTurn -= m_RLTurn_unit;
    printf("m_RLTurn_goal:%f\n", m_RLTurn_goal);
    printf("m_RLturn:%lf\n",m_RLTurn);
    if(!x_direction_down)
    {
        if(m_RLTurn_goal < 0)
            motion->walk(2.0,0,0.5 * m_RLTurn);
        else
            motion->walk(2.0,0, 0.7 * m_RLTurn);
    }
    else if(!y_direction_down)
    {
        motion->walk(1.0,0,0.3 * m_RLTurn);
         if (m_ball_center.x < m_ball_center_target.x - m_ball_center_diff.x ||
            m_ball_center.x > m_ball_center_target.x + m_ball_center_diff.x)
            x_direction_down = false;
    }
    else
        m_process_state = GETBALL;
}

void Shot::GetBall()
{
    printf("Getball\n");
    Walking::GetInstance()->m_Joint.SetEnableLeftArmOnly(false,false);
    Walking::GetInstance()->m_Joint.SetEnableRightArmOnly(false,false);
    Walking::GetInstance()->Stop();
    
    Action::GetInstance()->m_Joint.SetEnableBody(true, true);

    Action::GetInstance()->Start(124); //get the  ball
    usleep(2200 * 8000);

    Walking::GetInstance()->m_Joint.SetEnableBody(true, true);   //Lock arm again
    Walking::GetInstance()->m_Joint.SetEnableLeftArmOnly(false,false);
    Walking::GetInstance()->m_Joint.SetEnableRightArmOnly(false,false);
    m_process_state = TURNBASKET;
}
void Shot::TurnBasket()
{
    printf("Turn to basket\n");
    Head::GetInstance()->m_Joint.SetEnableHeadOnly(true, true);
    Walking::GetInstance() -> X_OFFSET = m_Xoffset_basket;
    //Head::GetInstance() -> MoveByAngle(0, 50);
    const int find_basket_turn_max = 2;
    const int no_basket_turn_max = 40;
    int flag = 0;
    //static double pan_unit = 7.0;
    double pan = Head::GetInstance() -> GetPanAngle();
   
    const double pan_left_limit = Head::GetInstance() -> GetLeftLimitAngle();
    const double pan_right_limit = Head::GetInstance() -> GetRightLimitAngle();
    
    Walking::GetInstance() -> X_OFFSET = 10;
    if(m_basket_valid)
    {
        find_basket_times++;
        no_basket_times = 0;
        if(find_basket_times >= find_basket_turn_max)
           flag = 1;
    }
    else
    {
        no_basket_times++;
        find_basket_times = 0;
        if(no_basket_times >= no_basket_turn_max)
        {  
            //Head::GetInstance() -> MoveByAngle(pan, 50);
            return;
        }
    }
    /*pan -= pan_unit;
    if (pan > pan_left_limit)
         pan_unit = -pan_unit;
    else if (pan < pan_right_limit)
         pan_unit = -pan_unit;*/ 
    usleep(50 * 8000);
    Head::GetInstance() -> MoveByAngle(0, 50);
    if(flag == 1)
    {
        Head::GetInstance() -> MoveByAngle(0, 50);
        /*if (pan < 0)
            motion -> swing(2, 2.0, 0, -10.0);
        else
            motion -> swing(2, 2.0, 0, 10.0);*/
        find_basket_times = 0;
        no_basket_times = 0;
        m_process_state = FINDBASKET;
    }
}
void Shot::FindBasket()
{
    printf("findbasket\n");
    const int find_basket_max = 2;
    const int no_basket_max = 2;
    Walking::GetInstance() -> X_OFFSET = 10;
    Head::GetInstance()->MoveByAngle(0,50);
    if(m_basket_valid)
    {
        find_basket_times++;
        if (find_basket_times > find_basket_max)
        {
            m_process_state = MOVEBACK2;
	    x_direction_down = false;
            y_direction_down = false;
            find_basket_times = 0;
            no_basket_times = 0;
        }
    }
    else
    {
        no_basket_times++;
        if (no_basket_times > no_basket_max)
            m_process_state = TURNBASKET;
    }
}

void Shot::MoveBack2()
{
    printf("MoveBack2\n");
    Walking::GetInstance()->m_Joint.SetEnableLeftArmOnly(false,false);
    Walking::GetInstance()->m_Joint.SetEnableRightArmOnly(false,false);
    Head::GetInstance()->m_Joint.SetEnableHeadOnly(true, true);
    Head::GetInstance() -> MoveByAngle(0, 50);
    m_percent_CenterDiff += m_CenterDiff_unit;
    
    Walking::GetInstance() -> X_OFFSET =10;
    if(m_FBstep > m_FBstep_back)
        m_FBstep -= m_FBstep_unit_back;
    motion -> walk(m_FBstep, 0, 0);

    if (m_percent_CenterDiff >= 0.020)
    {
        Walking::GetInstance() -> Stop();
        usleep(100 * 8000);
        m_process_state = MOVEBASKET;
	m_percent_CenterDiff = 0;
    }
}
void Shot::MoveBasket()
{
    printf("MoveBasket\n");
    const int no_basket_max = 2;
    
    Walking::GetInstance() -> X_OFFSET = 25;
    Head::GetInstance() -> MoveByAngle(0, 54);
    Walking::GetInstance()->Start();
    Walking::GetInstance() -> Z_MOVE_AMPLITUDE = 13;
    m_FBstep = 5.0;
    if(!m_basket_valid)
    {
        no_basket_times++;
        if(no_basket_times >= no_basket_max)
        {
            m_process_state = FINDBASKET;
            no_basket_times = 0;
            return;
        }
    }
    else 
       no_basket_times = 0;
    if (m_basket_center.x >= m_basket_center_target.x - m_basket_center_diff.x &&
	m_basket_center.x <= m_basket_center_target.x + m_basket_center_diff.x)
    {
        cnt1++;
        if(cnt1 == 3)
        {
            x_direction_down = true;
            cnt1 = 0; 
        }
    }
    else
        cnt1 = 0;
    printf("!!!!!x_direction_down:%d\n",x_direction_down);
    printf("!!!!!y_direction_down:%d\n",y_direction_down);
    printf("m_basket_down:%lf\n",m_basket_down);
    printf("m_basket_up:%lf\n",m_basket_up);
    if(m_basket_down - m_basket_up > 141 /*|| m_basket_up < 40*/) //67?
    {
        cnt2++;
        if(cnt2 == 3)
        {
            y_direction_down = true;
            cnt2 = 0;
            
        }
    }
    else cnt2 = 0;
    printf(" !!!!!cnt2:%d\n", cnt2);
    m_RLTurn_goal = 0.30 * (m_basket_center_target.x - m_basket_center.x); 
    printf("m_diff:%f\n", m_basket_center_target.x - m_basket_center.x);
    printf("m_RLTurn:%f\n", m_RLTurn);
    printf("m_RLTurn_goal:%f\n", m_RLTurn_goal);
    if (m_RLTurn > 0 && m_RLTurn_goal < 0 || m_RLTurn < 0 && m_RLTurn_goal > 0)
        m_RLTurn = 0;
    else if (m_RLTurn < m_RLTurn_goal)
        m_RLTurn += m_RLTurn_unit;
    else if (m_RLTurn > m_RLTurn_goal)
        m_RLTurn -= m_RLTurn_unit;
    m_percent_CenterDiff += m_CenterDiff_unit;

    if(!x_direction_down)
    {
        /*if(m_percent_CenterDiff >= 0.0)
        {
            if (m_RLTurn_goal < 0)
	         motion -> swing(1,3.5, 0, m_RLTurn - 4.7);
	    else
        	 motion -> swing(1,3.5, 0, m_RLTurn);
        }
        else*/
        //{
            if (m_RLTurn_goal < 0)
		 motion -> walk(3.5, 0,( m_RLTurn - 4.7)*0.5);
	    else
        	 motion -> walk(3.5, 0, 0.5*m_RLTurn);
        //}
    }
    else if(!y_direction_down)
    {
        motion->walk(4.0,0,m_RLTurn);
        if (m_basket_center.x < m_basket_center_target.x - m_basket_center_diff.x ||
            m_basket_center.x > m_basket_center_target.x + m_basket_center_diff.x)
            x_direction_down = false;
    }
    else
    {
        printf("down\n");
        m_process_state = SHOTBALL;
    }
    if(y_direction_down)
       printf("fininsh Y\n");
    else
       printf("not fininsh Y\n");
    
    if(x_direction_down)
       printf("fininsh X\n");
    else
       printf("not fininsh X\n");
}
void Shot::ShotBall()
{
    printf("ShotBall\n");
    Head::GetInstance()->MoveByAngle(0,50);
    Walking::GetInstance()->Stop();
    usleep(20 * 8000);
    Action::GetInstance()->m_Joint.SetEnableBody(true, true);
    Action::GetInstance()->Start(215); 
    usleep(1500 * 8000);
    Action::GetInstance() -> Start(79);
    usleep(1000 * 8000);  
}




