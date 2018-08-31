/**          weightlifting5.0              */
/**          10.1               **/

#include <MotionStatus.h>
#include "fira_weightlifting.h"

weightlifting::weightlifting()
{
    imgRes = new WeightliftResult;
    imgProc = new Weightlift;

    debug_print = true;

    m_point_found = false;
    m_angle = 0.0;
    m_point_center_2D.X = 0.0;
    m_point_center_2D.Y = 0.0;
    m_point_center_up = 0.0;
    m_point_center_down = 0.0;
    m_RLTurn_degree = 0.0;
    m_tmp_img_result = 0;

    m_period_time = 800;
    //m_period_time = 500;
    m_is_checkstatus = true;
    m_process_state = FORWARD;
    m_target_direction = 0;

    m_Xoffset = -20.0;
    m_forward_target_Xoffset = -14.0;
    m_forward_unit_Xoffset = 0.5;

    m_FBStep = 0.5;
    m_RLTurn = 0.0;
    m_goal_FBStep = 0.0;
    m_goal_RLTurn = 0.0;

    m_forward_target_FBStep = 27.0;//25.0
    m_forward_unit_FBStep = 2.0;

    m_forward_left_RLTurn = 10;
    m_forward_right_RLTurn = -13;

    m_forward_left_unit_RLTurn = 0.8;
    m_forward_right_unit_RLTurn = 0.8;

    m_rate1 = 1.3;
    m_rate2 = 0.5;
	
    m_percent_CenterDiff = 0.0000;
    m_unit_percent_CenterDiff = 0.0001;

    motion = NULL;
}

weightlifting::~weightlifting()
{}

void weightlifting::ThreadMotion()
{
    Weightlift *tmp_proc = dynamic_cast<Weightlift *>(imgProc);

    int tmp = 0;

    while(true)
    {
        tmp++;

        init();
        motionInit();

        while (true)
        {
          
            checkStatus();

            m_tmp_img_result = GetImageResult();

            if(m_process_state == FORWARD)
		Forward();
	    else if (m_process_state == MOVEFORWARD)
                walkForward();
            else if(m_process_state == LIFT)
                lift();
	
        }
    }
}

int weightlifting::GetImageResult()
{
	const int threshold_for_X = 5;

    while(is_new_img == false)
	{
        usleep(8000);
	}
    imgProc->imageProcess(frame,imgRes);
    WeightliftResult *tmp_result = dynamic_cast<WeightliftResult *>(imgRes);

    m_point_found = tmp_result -> obj_valid;
    printf("m_point_found:::%d\n", m_point_found);

    if (!m_point_found)
        return  0;
    else{
        m_point_center_2D.X = tmp_result -> center.x;
        m_point_center_2D.Y = tmp_result -> center.y;
        m_point_center_up = tmp_result -> obj_up;
        m_point_center_down = tmp_result -> obj_down;
        m_angle = tmp_result -> obj_angle;
        m_RLTurn_degree = fabs((m_point_center_2D.X - IMG_WIDTH / 2.0) / IMG_WIDTH);

        if (m_point_center_2D.X > IMG_WIDTH / 2.0 + threshold_for_X)
            m_target_direction = -1;
        else if (m_point_center_2D.X < IMG_WIDTH / 2.0 - threshold_for_X)
            m_target_direction = 1;
        else
            m_target_direction = 0;
    }
    is_new_img = false;

	printf("m_point_center_2D.Y:::%lf\n",m_point_center_2D.Y);
    return 1;
}

void weightlifting::motionInit()
{
    if (motion == NULL)
        motion = new Motion();

    motion -> poseInit();

    Head::GetInstance() -> MoveByAngle(0, -20);
    usleep(300 * 8000);

    Walking::GetInstance() -> Start();
    Walking::GetInstance() -> X_OFFSET = m_Xoffset;
    Walking::GetInstance() -> PERIOD_TIME = m_period_time;
    Walking::GetInstance() -> Z_MOVE_AMPLITUDE = 30;//steph
}

void weightlifting::checkStatus()
{
    if (m_is_checkstatus)
    {
        motion -> CheckStatus();
    }
}

void weightlifting::Forward()
{
    
	double tilt = Head::GetInstance() -> GetTiltAngle();
    double pan = Head::GetInstance() -> GetPanAngle();
    double RLdiff = m_point_center_2D.X - 160;
    double m_RLturn = -1 * 0.2 * RLdiff;
    //static double tilt_unit = 5.0;
    //static double tilt_down_limit = -20;
    static int find_times;

	motion -> walk(0.5, 0, m_RLturn);
	
	//tilt -= tilt_unit;
    //if(debug_print) printf("tilt:::%lf\n", tilt);

	printf("forward\n");

       //Head::GetInstance() -> MoveByAngle(pan, -20);

	
      if(m_point_center_2D.Y > 80)
      {
          find_times++;
          if(find_times >= 2)
          {
    	      m_process_state = MOVEFORWARD;
	      //Walking::GetInstance()->Stop();
              //usleep(3000 * 8000);
          } 			
      }
}

void weightlifting::walkForward()
{
	Walking::GetInstance() -> Z_MOVE_AMPLITUDE = 20;
    if(m_point_center_2D.Y < 145)
    {
        if (m_target_direction == 0)
            motion -> walk(0, 0, m_RLTurn);
        else if (m_target_direction == 1)
            weightliftingRL(m_forward_left_RLTurn , m_forward_left_unit_RLTurn);
        else if (m_target_direction == -1)
            weightliftingRL(m_forward_right_RLTurn, m_forward_right_unit_RLTurn);
    }
    else
    {
    	if(m_angle > 10)
    	{
			m_process_state = MOVEFORWARD;
			printf("left swing\n");
    		m_RLTurn = m_angle * m_rate1;
    		motion -> swing(1, -1, 0, m_RLTurn);
			
			if(m_point_center_2D.Y > 220)
			{
				motion -> swing(4, -8, 0, 5);
				printf("back!!!\n");
			}
	
    	}
    	else if(m_angle < -10)
    	{
			m_process_state = MOVEFORWARD;
			printf("right swing\n");
    		m_RLTurn = m_angle * m_rate1;
    		motion ->swing(1, -1, 0, m_RLTurn);

			if(m_point_center_2D.Y > 220)
			{
				motion -> swing(4, -8, 0, 5);
				printf("back!!!\n");
			}	

    	}
    	else
    	{
    		motion -> walk(m_FBStep - 0.2, 0, 0);
    		if(m_point_center_2D.Y > 180)
    		{
    			Walking::GetInstance()->Stop();
    			m_process_state = LIFT;
				
    		}
    	}
    }
    if(m_point_center_2D.Y > 190)
    {
    	Walking::GetInstance()->Stop();
    	m_process_state = LIFT;
				
    }
}



void weightlifting::lift()
{
	Action::GetInstance()->m_Joint.SetEnableBody(true, true);
    Action::GetInstance()->Start(121);
    usleep(3000 * 8000);
    Action::GetInstance()->Start(126);
    usleep(2400 * 8000);
  //  Action::GetInstance()->Start(213);
  // usleep(1000 * 8000);
  //  Action::GetInstance()->Start(83);
  //  usleep(100 * 8000);
  //  Action::GetInstance()->Start(84);
  //  usleep(1000*8000);
}

void weightlifting::weightliftingRL(double RLTurn, double unit_RLTurn)
{
    m_goal_RLTurn = fabs(m_RLTurn_degree) * RLTurn;


	m_RLTurn = m_goal_RLTurn - 2;


    motion -> walk(m_FBStep, 0, m_RLTurn);
}

void weightlifting::init()
{
    debug_print = true;

    m_point_found = false;
    m_point_center_2D.X = 0.0;
    m_point_center_2D.Y = 0.0;
    m_point_center_up = 0.0;
    m_point_center_down = 0.0;
    m_RLTurn_degree = 0.0;
    m_tmp_img_result = 0;

    //m_period_time = 600;
    m_period_time = 600;
    m_is_checkstatus = true;
   
    m_target_direction = 0;

    m_Xoffset = -20.0;
    m_forward_target_Xoffset = -14.0;
    m_forward_unit_Xoffset = 0.5;

    m_FBStep = 1.0;
    m_RLTurn = 0.0;
    m_goal_FBStep = 0.0;
    m_goal_RLTurn = 0.0;

    m_forward_target_FBStep = 30.0;
    m_forward_unit_FBStep = 2.0;

    m_forward_left_RLTurn = 25.0;
    m_forward_right_RLTurn = -20.0;

    m_forward_left_unit_RLTurn = 1.0;
    m_forward_right_unit_RLTurn = 1.0;

    m_percent_CenterDiff = 0.0000;
    m_unit_percent_CenterDiff = 0.0001;
}


