#include <fira_penalty.h>
#include <time.h>

Fira_penalty::Fira_penalty()
{
    images = new PenaltyImgResult();
    imgpro = new PenaltyImg();
    
    m_execute = true;
    m_process_state = FINDFOOTBALL;
    m_pre_state = APPROACHBALL;
 
    m_ball_valid = false;
    m_goal_valid = false;
    //m_right_valid = false;
    m_ball_center.x = 0;
    m_ball_center.y = 0;
    m_goal_center.x = 0;
    m_goal_center.y = 0;
   
    m_RLdiff = 0;
    m_FBdiff = 0;
    RL_lostball = 0;
    
    m_FBstep = 0;
    m_FBstep_goal = 0;
    m_unit_FBstep = 1;
    m_FBstepcof = 6;
    m_FBstep_back = -10;

    m_RLstep = 0;
    m_RLstep_goal = 0;
    m_unit_RLstep = 0.2;
    m_RLstepcof = 6;
    
    m_RLturn = 0;
    m_RLturn_goal = 0;
    m_unit_RLturn = 3;
    m_RLturncof = 6;
     
    m_unit_time = 0.01;
    m_timescontrol = 0;
   
    m_findthre = 30;
    m_AddressBallCenter.x = 160;
    m_AddressBallCenter.y = 93;
  
    m_SwingBallCenter.x = 180;
    m_SwingBallCenter.y = 180;
    m_SwingDiff_X = 30;
    m_SwingDiff_Y = 40;
    m_goaldiff = 0;
    m_direction = 0;
    flag = 1;
    m_kickflag = 0;
    
    x_down = false;
    y_down = false;
    //ifset = 1;
    AP_find = 0;
    m_kickdir = 0;
    right = 0;
    left = 0;
}

Fira_penalty::~Fira_penalty()
{
}

void Fira_penalty::ThreadMotion()
{
    Init();
    while(m_execute)
    {
        printf("\n");
       /* printf("m_process_state:%d\n", m_process_state);
        printf("m_FBStep:%f\n", m_FBstep);
        printf("m_RLStep:%f\n", m_RLstep);
        printf("m_RLTurn:%f\n", m_RLturn);*/
        motion->CheckStatus();
        
        GetImageResult();
        
        if(m_process_state == FINDFOOTBALL)
            FindBall();
        //else 
        if(m_process_state == APPROACHBALL)
            ApproachBall();
        //else if(m_process_state == ADJUSTRL)
          //  AdjustRL();
        else if(m_process_state == ADJUSTFB)
            AdjustFB();
        else if(m_process_state == ADJUSTDIR)
            AdjustDir();
        else if(m_process_state == LOST)
            Lost();
        else if(m_process_state == MOVEBACK)
            MoveBack();
        else if(m_process_state == KICK)
        {
            printf("ready to kick\n");
            Kick();
        }
    }
} 

int Fira_penalty::GetImageResult()
{
    printf("GetImageResult\n");
    imgpro->imageProcess(frame,images);
    PenaltyImgResult *tmp_result;
    tmp_result = dynamic_cast<PenaltyImgResult *>(images);
    
    while( is_new_img == false ) usleep(8000);

    m_ball_valid = tmp_result->ball_valid;
    m_goal_valid = tmp_result->hole_valid;
    //m_right_valid = tmp_result->hole_right_vaild;
    m_ball_center = tmp_result->ball_center;
    m_goal_center = tmp_result->hole_center;
    
    printf("ball_center:%f, %f\n", m_ball_center.x, m_ball_center.y);
    printf("hole_center:%f, %f\n", m_goal_center.x, m_goal_center.y); 
    
    is_new_img = false;
    return 0;   
}

void Fira_penalty::Init()
{
    motion = new Motion();
    //PenaltyImg *tmp_proc = dynamic_cast<PenaltyImg *>(m_imgPro);
    motion -> poseInit();
    Head::GetInstance()->MoveByAngle(0,10);
    Walking::GetInstance()->X_OFFSET = -10;
    Walking::GetInstance()->Z_MOVE_AMPLITUDE = 8;
    
}

void Fira_penalty::FindBall()
{
    m_process_state = APPROACHBALL;
    if(fabs(m_ball_center.x - 160) < 70)
        m_kickflag = 1;
    else
        m_kickflag = 2;
    usleep(200 * 8000);
    clock_gettime(CLOCK_MONOTONIC,&start);
}
void Fira_penalty::ApproachBall()
{    //static int dirflag;
     printf("Approachball\n");
     Head::GetInstance()->MoveByAngle(0,0);
     static int lostball = 0;
     if(!m_ball_valid)
     {
         lostball++;
         if(lostball >= 20 )
         {
              m_pre_state = APPROACHBALL;
              m_process_state = LOST;
              printf("ADjustRL change to lost");
              return;
         }
     }
     else//(m_ball_valid)
         lostball = 0;
     
     clock_gettime(CLOCK_MONOTONIC, &now);
     m_RLdiff = m_ball_center.x - m_SwingBallCenter.x;
     if(now.tv_sec - start.tv_sec < 0.2)
     {
         if(m_ball_center.x > 170)
             right++;
         else
             left++;
         if(right > 3)
         {
            m_direction = 1;//球在右边
            m_kickdir = 1;
         }
         else if(left > 3)
         {
            m_direction = 2;//球在左边
            m_kickdir = 2;
         }
         else
         {
             m_direction = 2;
             m_kickdir = 2;
         }

     }
     if(m_direction == 1) 
        printf("right\n");
     else
        printf("left\n");
     /*if(m_direction == 1) m_SwingBallCenter.x = 170;
     else m_SwingBallCenter.x = 150;*/
     m_SwingBallCenter.x = 160;
     m_RLdiff = m_ball_center.x - m_SwingBallCenter.x;
     m_RLturn_goal = -1 * 8 * m_RLturncof * m_RLdiff / IMG_WIDTH;
     if(m_RLturn * m_RLturn_goal < 0)
         m_RLturn = 0;
     if(m_RLturn < m_RLturn_goal)
         m_RLturn += m_unit_RLturn;
     else
         m_RLturn -= m_unit_RLturn;
      
     //if(m_RLturn_goal > 0) dirflag--;
     //else  dirflag++;
     //if(ifset == 1 && dirflag > 10)
     //{ 
       //  m_direction = 2;
        // ifset = 0;
     //}
     //if(ifset == 1 && dirflag < -10)
     //{
       //  m_direction = 1;
         //ifset = 2;
     //} 
     printf("m_RLturn_goal:%f\n",m_RLturn_goal);
     m_RLstep = 0.0;
     m_FBstep_goal = 3;
     if(m_FBstep * m_FBstep_goal < 0)
         m_FBstep = 0;
     if(m_FBstep < m_FBstep_goal)
         m_FBstep += m_unit_FBstep;
     else
         m_FBstep -= m_unit_FBstep;
     motion->walk(5,m_RLstep,m_RLturn);
     if(m_kickflag == 1)
     {
        if(m_ball_center.y > 180 )
        {
           AP_find++;
           if(AP_find >= 2)
           { 
              Walking::GetInstance() -> Stop();
              usleep(50 * 8000);
              m_process_state = ADJUSTDIR;
              printf("ApproachBall change to ADJUSTDIR\n");
             
             
           }
           //Walking::GetInstance() -> Stop();
        }
      }
      else
      {
          if(m_ball_center.y > 200 )
          {
           AP_find++;
           if(AP_find >= 2)
           { 
              Walking::GetInstance() -> Stop();
              usleep(50 * 8000);
              m_process_state = ADJUSTDIR;
              printf("ApproachBall change to ADJUSTDIR\n");
             
             
           }
         }
      } 
}
void Fira_penalty::AdjustDir()
{
    
    printf("AdjustDir\n");
    Head::GetInstance()->MoveByAngle(0,20);
    if(!m_goal_valid)
    {
        if(m_direction == 2)
        {
            motion->swing(1,2,0,13);
            m_kickdir = 1;
        }
        else if(m_direction == 1)
        {
            motion->swing(1,2,0,-17);
            m_kickdir = 2;
        }
        else
        {
            motion->swing(1,2,0,13);
            m_kickdir = 1;
        }
        
        usleep(150 * 8000);
        return;
    }
    if(m_goal_center.x > 160)
        m_goaldiff = m_goal_center.x - 240;
    else
        m_goaldiff = m_goal_center.x - 110;
    if(m_kickflag == 2 && m_direction == 1)
    {
	 m_goaldiff = m_goal_center.x - 240;
    }
    
    if(fabs(m_goaldiff) < 40)
    {
        m_process_state = MOVEBACK;
        
        Head::GetInstance()->MoveByAngle(0,-20);
        Walking::GetInstance()->Stop();
        usleep(150 * 8000);
        return;
    }
    else
    {
        m_RLturn_goal = -1 * 3 * m_RLturncof * m_goaldiff / IMG_WIDTH;
      
    }
    if(m_goaldiff < 0)
    {
        motion->swing(1,2,0,13);
        m_kickdir = 1;
    }
    else
    {
        motion->swing(1,2,0,-17);
        m_kickdir = 2;
    }
    usleep(100 * 800);
   
}
void Fira_penalty::MoveBack()
{
    printf("MoveBack\n");
    Walking::GetInstance()->Start();
    m_FBstep_goal = m_FBstep_back;
    if(m_FBstep * m_FBstep_goal < 0)
         m_FBstep = 0;
    if(m_FBstep < m_FBstep_goal)
         m_FBstep += m_unit_FBstep;
    else
         m_FBstep -= m_unit_FBstep;
    m_timescontrol += m_unit_time;
    motion->walk(m_FBstep,0,0);
    if(m_timescontrol > 0.18)
    {
        m_process_state = ADJUSTFB;
        m_timescontrol = 0;
        usleep(200 * 8000);
    }
}
void Fira_penalty::AdjustFB()
{
    /*m_SwingBallCenter.y 来控制何时完成前后调整，与头的俯仰角度有关，tilt一般为-20
    m_SwingDiff_Y(上下偏移阈值)
    */
    //if(m_right_valid)
     //   m_direction = 1;
    //else
      //  m_direction = 2;
    printf("AdjustFB\n");
    printf("m_kickflag:%f\n", m_kickflag);
    printf("m_kickdir:%f\n", m_kickdir);
    printf("m_direction:%f\n", m_direction);
    printf("m_SwingBallCenter.x:%f\n", m_SwingBallCenter.x);
    Head::GetInstance()->MoveByAngle(0,-20);
    static int lostFB = 0;
    static int cntx = 0;
    static int cnty = 0;
    printf("adjustFB");
    GetImageResult();
    if(!m_ball_valid)
    {
        lostFB++;
        if(lostFB >= 20)
        {
            m_pre_state = ADJUSTFB;
            m_process_state = LOST;
            printf("AdjustFB change to lost\n");
            return;
        }          
    }
    else
         lostFB = 0;
    if(m_direction == 2 && m_kickflag == 1)
    {
       m_SwingBallCenter.x = 180;
    }
    else if(m_direction == 1 && m_kickflag == 1)
    {
       m_SwingBallCenter.x = 140;
    }
    else if(m_direction == 1 && m_kickflag == 2)
    {
       m_SwingBallCenter.x = 180;
    }
    else if(m_direction == 2 && m_kickflag == 2)
    {
       m_SwingBallCenter.x = 140;
    }
   
    
    m_FBdiff = m_ball_center.y - m_SwingBallCenter.y;
    m_RLdiff = m_ball_center.x - m_SwingBallCenter.x;
   
    
    m_RLturn_goal = -1 * 10 * m_RLstepcof * m_RLdiff / IMG_WIDTH;
    if(m_RLturn * m_RLturn_goal < 0)
         m_RLturn = 0;
    if(m_RLturn < m_RLturn_goal)
         m_RLturn += m_unit_RLturn;
    else
         m_RLturn -= m_unit_RLturn;
    if(fabs(m_RLdiff) <= m_SwingDiff_X)
    {
        cntx++;
        if(cntx == 3)
        {
            x_down = true;
            cntx = 0;
        }
       
    }
    if(fabs(m_FBdiff) <= m_SwingDiff_Y)
    {
        cnty++;
        if(cnty == 3)
        {
            y_down = true;
            cnty = 0;
        }
    }
    if(!x_down)
    {
        if(m_RLturn_goal < 0)
            motion->walk(2.0,0,m_RLturn );
        else
            motion->walk(2.0,0,m_RLturn + 2.0);
    }
    else if(!y_down)
    {
        motion->walk(1.0,0,m_RLturn);
        if(fabs(m_RLdiff) > m_SwingDiff_X)
           x_down = false;
    }
    else
    {
        m_process_state = KICK;
        m_pre_state = ADJUSTFB;
        x_down = false;
        y_down = false;
        printf("AdjustFB change to KICK\n");
    }
    if( m_ball_center.y > 235)
    {
       
        m_process_state = KICK;
        m_pre_state = ADJUSTFB;
        x_down = false;
        y_down = false;
        usleep(100 * 8000);
        m_RLturn = 0;
        
        printf("AdjustFB change to MOVEBACK\n");
    }
    

}

void Fira_penalty::Kick()
{
    Head::GetInstance()->MoveByAngle(0,20);
    Walking::GetInstance()->Stop();
    printf("stop walking\n");
    if(m_direction == 2 && m_kickflag == 1)
    {
        motion->swing(2,2,0,10);
    }
    else if(m_direction == 1 && m_kickflag == 1)
    {
        motion->swing(2,2,0,-13);
    }
    static int lost_goal = 0;
    if(!m_goal_valid)
    {
        lost_goal++;
        if(lost_goal >= 10)
        {
            m_pre_state = KICK;
            m_process_state = LOST;
	    printf("KICK Change to Lost\n");
            return ;
        }
    }
    else
        lost_goal = 0;
    if(m_goal_center.x > 160)
        m_goaldiff = m_goal_center.x - 240;
    else
        m_goaldiff = m_goal_center.x - 100;
    
    printf("m_goal_center.x:%f\n", m_goal_center.x);
    printf("m_goal_diff:%f\n", m_goaldiff);
    //if(fabs(m_goaldiff) < 40)
    //{
         Head::GetInstance()->MoveByAngle(0,-20);
         usleep(100 * 8000);
         GetImageResult();
         Action::GetInstance()->m_Joint.SetEnableBody(true,true);
         if(m_ball_center.x < 160)
         {
            
              Action::GetInstance()->m_Joint.SetEnableBody(true,true);
              Action::GetInstance()->Start(22);
              usleep(500 * 8000);
              printf("left kick\n");
         }
         else if(m_ball_center.x > 170)
         {
            
              Action::GetInstance()->m_Joint.SetEnableBody(true,true);
              Action::GetInstance()->Start(21);
              usleep(500 * 8000);
              printf("you kick\n");
         }
         else
         {
              usleep(100 * 8000);
              Action::GetInstance()->Start(92);//zuo heng yi
              usleep(200 * 8000);
              printf("adjust zuo hen yi\n");
         }
    
}

void Fira_penalty::Lost()
{
    printf("LOST!!!!!!!!!!");
    if(m_pre_state == KICK)
    {
	Head::GetInstance()->MoveByAngle(0, 10);
	if(m_goal_valid)
       	m_process_state = m_pre_state;	

	Walking::GetInstance()->m_Joint.SetEnableBodyWithoutHead(true, true);
	Head::GetInstance()->m_Joint.SetEnableHeadOnly(true, true);
	Walking::GetInstance() -> Start();
	Walking::GetInstance() -> Z_MOVE_AMPLITUDE =  20;
	motion -> walk(-10.0, 0, 8);	
    }

    else
    {
	Head::GetInstance()->MoveByAngle(0, -10);
	if(m_ball_valid)
	    m_process_state = m_pre_state;

	Walking::GetInstance()->m_Joint.SetEnableBodyWithoutHead(true, true);
	Head::GetInstance()->m_Joint.SetEnableHeadOnly(true, true);
	Walking::GetInstance() -> Start();
	Walking::GetInstance() -> Z_MOVE_AMPLITUDE =  15;
	motion -> walk(-10.0, 0, 8);
    }
}





