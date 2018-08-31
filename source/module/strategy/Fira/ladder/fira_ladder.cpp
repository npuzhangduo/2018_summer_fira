/*
//Version 3.3.1  2017.4.16 Edited by Forever
  This version is stable for 38s
  Thanks to Abraham
*/

#include <MotionStatus.h>
#include "fira_ladder.h"

Ladder::Ladder()
{
    imgRes = new LadderrunResult;
    imgProc = new Ladderrun;

    debug_print = true;

    m_point_found = false;
    m_point_center_2D.X = 0.0;
    m_point_center_2D.Y = 0.0;
 //   m_point_center_up = 0.0;
    m_point_center_down = 0.0;
    m_RLTurn_degree = 0.0;
    m_tmp_img_result = 0;

    m_period_time = 600;
    //m_period_time = 500;
    m_is_checkstatus = true;
    m_process_state = WALKFORWARD1;

    m_target_direction = 0;

    m_Xoffset = -10.0;
    m_forward_target_Xoffset = -14.0;
    m_forward_unit_Xoffset = 0.5;
	m_back_target_Xoffset = -14.0;
	m_back_unit_Xoffset = 2.0;

    m_FBStep = 0.0;
    m_RLTurn = 0.0;
    m_goal_FBStep = 0.0;
    m_goal_RLTurn = 0.0;

    m_forward_target_FBStep = 28.0;//25.0
    m_forward_unit_FBStep = 2.0;
	m_back_target_FBStep = -17.0;//-15.0
	m_back_unit_FBStep = 1.0;//2.0

    m_forward_left_RLTurn = 20.0;
    m_forward_right_RLTurn = -20.0;
    m_back_left_RLTurn = 20.0;
    m_back_right_RLTurn = -20.0;

    m_forward_left_unit_RLTurn = 3.0;
    m_forward_right_unit_RLTurn = 3.0;
    m_back_left_unit_RLTurn = 3.0;
    m_back_right_unit_RLTurn = 3.0;

    m_percent_CenterDiff = 0.0000;
    m_unit_percent_CenterDiff = 0.0001;

    back_count = 0;
	motion = NULL;
}

Ladder::~Ladder()
{
}

void Ladder::ThreadMotion()
{    
	Ladderrun *tmp_proc = dynamic_cast<Ladderrun *>(imgProc);
	printf("1111111\n");
	int tmp = 0;

	while (true)
	{    
		tmp++;

		//init();
		motionInit();

		while (true)
		{
		    if(MotionStatus::BUTTON == 2)
		    {
		        MotionManager::GetInstance()-> Reinitialize();
		        MotionManager::GetInstance()-> SetEnable(true);

				motion -> walk(0, 0, 0);
				Action::GetInstance() -> Stop();
            	while(Action::GetInstance()->Start(15) == false) usleep(8000);
            	while(Action::GetInstance()->IsRunning() == true) usleep(8000);

				break;
		    }
			//Action::GetInstance()->m_Joint.SetEnableBody(true, true);
		
	
		//Action::GetInstance()->Start(94);
			//usleep(2100*8000);
		
		    checkStatus();
			m_tmp_img_result = GetImageResult();
			
		    if (m_process_state == WALKFORWARD1)
		       {
			 walkForward1();
			
			}
		    else if (m_process_state == UPLADDER2)
		       {
		//	printf("222222\n");
			 upLadder2();
			
			} 
		else if (m_process_state == UPLADDER3)
		       {
		//	printf("222222\n");
			 upLadder3();
			
			}
  		else if (m_process_state == MOVELADDER)
		       {
		//	printf("222222\n");
			 moveLadder();
			
			}
		    else if (m_process_state == WALKFORWARD2)
		        {
			walkForward2();
	//		printf("333333\n");
			}
			else if(m_process_state == UPLADDER1)
				{
				upLadder1();
		//		printf("4444444444\n");
			}
			
	//		fprintf(stderr, "m_RLTurn: %f\n", m_RLTurn);
	//		fprintf(stderr, "m_RLTurn_goal: %f\n", m_goal_RLTurn);
			
		}
	}
}

int Ladder::GetImageResult()
{

    while (is_new_img == false) 
       { 
//		printf("false");
		usleep(8000);
		
	}
	
    imgProc->imageProcess(frame,imgRes);
    LadderrunResult *tmp_result = dynamic_cast<LadderrunResult *>(imgRes);
       if(debug_print)printf("x: %f y: %f down: %f m_target_direction %d\n",tmp_result -> center.x,tmp_result -> center.y,tmp_result -> down,m_target_direction); 
	m_point_found = tmp_result -> valid;
	//printf("m_point_found:%d\n", m_point_found);
    if (!m_point_found)
        return  0;
    else 
    {
        m_point_center_2D.X = tmp_result -> center.x;
        m_point_center_2D.Y = tmp_result -> center.y;
	
   //     m_point_center_up = tmp_result -> obj_up;
        m_point_center_down = tmp_result -> down;
        m_RLTurn_degree = fabs((m_point_center_2D.X - IMG_WIDTH / 2.0) / IMG_WIDTH);
    if(m_process_state==WALKFORWARD1)
	{
        if (m_point_center_2D.X > (IMG_WIDTH / 2.0+IMG_WIDTH/10.0))
            m_target_direction = -1;//r
        else if (m_point_center_2D.X < (IMG_WIDTH / 2.0+IMG_WIDTH/10.0))
            m_target_direction = 1;//l
        else
            m_target_direction = 0;
	}
  if(m_process_state==MOVELADDER)
{
//		printf("tuuuuuuuxianggggmmmmmmmmove");
        if (m_point_center_2D.X > (IMG_WIDTH / 2.0+IMG_WIDTH/30.0))
            m_target_direction = -1;//r
        else if (m_point_center_2D.X < (IMG_WIDTH / 2.0+IMG_WIDTH/30.0))
            m_target_direction = 1;//l
        else
            m_target_direction = 0;
}
    }

    is_new_img = false;
	return 1;
}

void Ladder::motionInit()
{
	if (motion == NULL)
		motion = new Motion();

    motion -> poseInit();

    Head::GetInstance() -> MoveByAngle(0, 20);
    usleep(100 * 8000);

    Walking::GetInstance() -> Start();
    Walking::GetInstance() -> X_OFFSET = m_Xoffset;
    Walking::GetInstance() -> PERIOD_TIME = m_period_time;
    Walking::GetInstance() -> Z_MOVE_AMPLITUDE = 30;//steph
}

void Ladder::checkStatus()
{
    if (m_is_checkstatus)
    {
        motion -> CheckStatus();
    }
}

void Ladder::walkForward1()
{       
    const double RL_lessforBack = 1.0;
    const double m_percent_less = 0.0500;
	double tilt = 25;
	double pan = Head::GetInstance() -> GetPanAngle();
	static double tilt_unit = 1.0;
	static double tilt_down_limit = -20;

	motion -> walk(10, 0, 0);
	usleep(20*8000);
	Walking::GetInstance() -> X_OFFSET = m_Xoffset;
	printf("walkforwardinto slope\n");

    if (m_target_direction == 0)
    {
        ladderFB();
    }
    else if (m_target_direction == 1)
    {
            ladderRL(m_forward_left_RLTurn , m_forward_left_unit_RLTurn);
      
    }
    else if (m_target_direction == -1)
    {
            ladderRL(m_forward_right_RLTurn, m_forward_right_unit_RLTurn);
       
    }
	else{
	//	printf("error error error error error\n");
	}
     

	
   if (tilt > 0.00)
     {
	 tilt = tilt-tilt_unit;
	// printf("tilt:%f\n",tilt);
	}
 
   else if (tilt <= 0.00)
       tilt_unit = tilt_unit;  
		 

    Head::GetInstance() -> MoveByAngle(pan, tilt);
    usleep(20 * 8000);
  m_percent_CenterDiff += m_unit_percent_CenterDiff;
	//printf("m_percent_CenterDiff:%lf\n",m_percent_CenterDiff);

	if(m_percent_CenterDiff>0.0030&&m_point_center_down>=90.0)//170
	{
		back_count++;
		if(back_count>back_count_max)
		{
			m_process_state = MOVELADDER;
			printf("5");
			m_percent_CenterDiff=0.000;
		}
	//	printf("6");

	}

}

void Ladder::moveLadder()
{
	static int m=0;
	
	Head::GetInstance() -> MoveByAngle(0, 0);

    Walking::GetInstance() -> PERIOD_TIME = 550;
	
	Walking::GetInstance() -> Start();
	Walking::GetInstance() -> X_OFFSET = m_Xoffset;
//	printf("moveladder into slope\n");
	
	
  //  m_RLTurn = 0;
   // m_goal_RLTurn = 0;



    if (m_Xoffset < 0.0)//orthocenter backleft
	{
        m_Xoffset += 1.0;	
		Walking::GetInstance() -> X_OFFSET = m_Xoffset;
	}

	 Walking::GetInstance() -> Z_MOVE_AMPLITUDE = 8;//steph
	 m_FBStep = 4.0;//steph	
		//motion->walk(10,0,20);
		//ladderFB();
		if (m_target_direction == 0)
		    {
			ladderFB();
		    }
		    else if (m_target_direction == 1)
		    {
			    ladderRL(m_forward_left_RLTurn , m_forward_left_unit_RLTurn);
		      
		    }
		    else if (m_target_direction == -1)
		    {
			    ladderRL(m_forward_right_RLTurn, m_forward_right_unit_RLTurn);
		       
		    }
			else{
//				printf("error error error error error\n");
			}
		
		//m_percent_CenterDiff += m_unit_percent_CenterDiff;
//		printf("m_percent_CenterDiff:%lf\n",m_percent_CenterDiff);
		//if(m_percent_CenterDiff>=0.001)
			//{
				//Walking::GetInstance() -> Stop();
				//usleep(100 * 8000);
				if(m_point_center_down>=224)//224
			{
				m++;//?????
				if(m==3)//?????////
				{
				m_process_state = UPLADDER1;
				Walking::GetInstance()->Stop();
				m_percent_CenterDiff = 0;
				}
				
			}
			//	else m_percent_CenterDiff = 0;
				
				
			//}	
	
	
		
	//printf("moveladder");
	 
}

void Ladder::upLadder1()
{
	int cntup=3;
	int cntdown=3;
	int m=5;
	double tilt = Head::GetInstance() -> GetTiltAngle();
	double pan = Head::GetInstance() -> GetPanAngle();
 //  if(debug_print)
	//	printf("UP LADDER\n");
	while(m--)
	{
	Walking::GetInstance() -> Start();
	Walking::GetInstance() -> X_OFFSET = m_Xoffset;
	 Walking::GetInstance() -> Z_MOVE_AMPLITUDE = 8;//steph
	 m_FBStep = 3.0;//steph
	
	}

	//Head::GetInstance() -> MoveByAngle(0, -25);
		Action::GetInstance()->m_Joint.SetEnableBody(true, true);
		
	
			Action::GetInstance()->Start(87);
			usleep(1900*8000);
		//	printf("1\n");		
			
			Action::GetInstance()->Start(99);
			usleep(1280*8000);
		//	printf("1111IIIIIPPPPPPP\n");
			
			Action::GetInstance()->Start(87);
			usleep(1900*8000);
		//	printf("2\n");		
			
			Action::GetInstance()->Start(99);
			usleep(1280*8000);
		//	printf("1yyyyyyyyy2222222222222\n");
		//	
			Action::GetInstance()->Start(87);
			usleep(1900*8000);
		//	printf("3\n");	




		/*while(m--)
		{
			Action::GetInstance()->Start(99);
			usleep(1280*8000);
		}	*/
		
	
	usleep(10*8000);
	m_process_state = UPLADDER2;
}
void Ladder::upLadder2()
{
	int cntup=3;
	int cntdown=3;
	int m=5;
	int t=0;
	double tilt = Head::GetInstance() -> GetTiltAngle();
	double pan = Head::GetInstance() -> GetPanAngle();
   if(debug_print)
		printf("UP LADDER\n");
		
		Walking::GetInstance()->m_Joint.SetEnableBodyWithoutHead(true, true);
		 Head::GetInstance()->m_Joint.SetEnableHeadOnly(true, true);
 	   	Walking::GetInstance()->Start();
		Walking::GetInstance() -> Z_MOVE_AMPLITUDE =5;//steph
	 m_FBStep = 1.0;//steph	
		 //tilt = -25;
		Head::GetInstance() -> MoveByAngle(0, -25);
		usleep(100*8000);
		m_tmp_img_result = GetImageResult();
		if (m_target_direction == 0)
		    {
			ladderFB();
		    }
		    else if (m_target_direction == 1)
		    {
			    ladderRL(m_forward_left_RLTurn , m_forward_left_unit_RLTurn);
		      
		    }
		    else if (m_target_direction == -1)
		    {
			    ladderRL(m_forward_right_RLTurn, m_forward_right_unit_RLTurn);
		       
		    }
			else{
//				printf("error error error error error\n");
			}
		printf("m_FBStep:%f\n", m_FBStep);
		motion -> walk(m_FBStep, 0, 0);
		while(m_point_center_down>=218)//204
			{
				t++;
				if(t==5)
				{
				Walking::GetInstance()->Stop();
				usleep(20*8000);
				m_process_state = UPLADDER3;
				break;
				}
			
			}
			
		
		
	
	//usleep(100*8000);
	//m_process_state = UPLADDER3;
}
void Ladder::upLadder3()
{
	int cntup=3;
	int cntdown=3;
	int m=5;
	double tilt = Head::GetInstance() -> GetTiltAngle();
	double pan = Head::GetInstance() -> GetPanAngle();
	
  // if(debug_print)
		//printf("UP LADDER\n");
		Action::GetInstance()->m_Joint.SetEnableBody(true, true);
  
		Action::GetInstance()->Start(102);
		usleep(2500*8000);
	
		Action::GetInstance()->Start(102);
		usleep(2500*8000);

		Action::GetInstance()->Start(102);
		usleep(2500*8000);
   		/* if(cntdown != 0)
		{
			Action::GetInstance()->Start(902);
			usleep(100*8000);
		}*/
		
		
	
	usleep(20*8000);
	m_process_state = WALKFORWARD2;
}

void Ladder::walkForward2()
{
	const double RL_lessforBack = 1.0;
    const double m_percent_less = 0.0500;
	Walking::GetInstance() -> X_OFFSET = m_Xoffset;
	printf("walkforward2");
	Walking::GetInstance() -> Z_MOVE_AMPLITUDE =30;//steph
	 m_FBStep = 5.0;//steph	
	Walking::GetInstance()->m_Joint.SetEnableBodyWithoutHead(true, true);
	Head::GetInstance() -> MoveByAngle(0, 20);
	usleep(20*8000);
    Walking::GetInstance()->Start();
	m_FBStep = 8.0;
	printf("m_FBStep:%f\n", m_FBStep);
	motion -> walk(m_FBStep, 0, 0);

    if (m_target_direction == 0)
    {
        ladderFB();
    }
    else if (m_target_direction == 1)
    {
            ladderRL(m_forward_left_RLTurn , m_forward_left_unit_RLTurn);
      
    }
    else if (m_target_direction == -1)
    {
            ladderRL(m_forward_right_RLTurn, m_forward_right_unit_RLTurn);
       
    }

      //  m_percent_CenterDiff += m_unit_percent_CenterDiff;

	
}

void Ladder::ladderRL(double RLTurn, double unit_RLTurn)
{
    if (m_process_state == WALKFORWARD1 && m_target_direction == -1||m_process_state == WALKFORWARD2 && m_target_direction == -1)
        m_goal_RLTurn = fabs(m_RLTurn_degree) * 2.4 * RLTurn;
	else if (m_process_state == WALKFORWARD1 && m_target_direction == 1||m_process_state == WALKFORWARD2 && m_target_direction == -1)
        m_goal_RLTurn = fabs(m_RLTurn_degree) * 1.0 * RLTurn;//1.0
	
if (m_process_state == MOVELADDER&& m_target_direction == -1)
        m_goal_RLTurn = fabs(m_RLTurn_degree) * 2.4 * RLTurn;
	else if (m_process_state == MOVELADDER&& m_target_direction == 1)
        m_goal_RLTurn = fabs(m_RLTurn_degree) * 1.1 * RLTurn;//1.0

    if (m_RLTurn < m_goal_RLTurn /*&& m_RLTurn + unit_RLTurn < m_goal_RLTurn*/)
        m_RLTurn = m_goal_RLTurn;
    else if (m_RLTurn > m_goal_RLTurn /*&& m_RLTurn - unit_RLTurn > m_goal_RLTurn*/)
        m_RLTurn = m_goal_RLTurn;

	//printf("RLTURN:%f\n",m_RLTurn);
	ladderFB();
}


void Ladder::ladderFB()
{
	//printf("m_FBStep:%f\n", m_FBStep);
    //if (m_point_found == true)
    {	
		if ( m_target_direction == 0)
		{
			motion -> walk(m_FBStep, 0, m_RLTurn - 5.0);
		}
		else
			motion -> walk(m_FBStep, 0, m_RLTurn);
	}
}

void Ladder::init()
{
    debug_print = true;

    m_point_found = false;
    m_point_center_2D.X = 0.0;
    m_point_center_2D.Y = 0.0;
 //   m_point_center_up = 0.0;
   m_point_center_down = 0.0;
    m_RLTurn_degree = 0.0;
    m_tmp_img_result = 0;

    //m_period_time = 600;
    m_period_time = 550;
    m_is_checkstatus = true;
    m_process_state = UPLADDER2;
    m_target_direction = 0;

    m_Xoffset = -10.0;
    m_forward_target_Xoffset = -14.0;
    m_forward_unit_Xoffset = 0.5;
	m_back_target_Xoffset = -1.0;
	m_back_unit_Xoffset = 0.5;

    m_FBStep = 8.0;//
    m_RLTurn = 0.0;
    m_goal_FBStep = 0.0;//
    m_goal_RLTurn = 0.0;

    m_forward_target_FBStep = 30.0;
    m_forward_unit_FBStep = 2.0;
	m_back_target_FBStep = -15.0;
	m_back_unit_FBStep = 2.0;

    m_forward_left_RLTurn = 25.0;
    m_forward_right_RLTurn = -20.0;
    m_back_left_RLTurn = 15.0;
    m_back_right_RLTurn = -30.0;

    m_forward_left_unit_RLTurn = 3.0;
    m_forward_right_unit_RLTurn = 3.0;
    m_back_left_unit_RLTurn = 3.0;
    m_back_right_unit_RLTurn = 3.0;

    m_percent_CenterDiff = 0.0000;
    m_unit_percent_CenterDiff = 0.0001;

    back_count = 0;
}

