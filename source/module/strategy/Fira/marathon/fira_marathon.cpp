
#include "fira_marathon.h"

Fira_Marathon::Fira_Marathon()
{

    m_imgRes = new MarathonResult();
    m_imgProc = new Marathon();
	//*******add: Histogram *tmp_proc = dynamic_cast<Histogram *>(imgProc);


	//GENERAL
    m_execute = true;
    m_process_state = LINE;
	//m_pre_state = RIGHT;
	//m_pre_slope = 0;
	debug_print = 1;
	m_percent_CenterDiff = 0.00;
	m_percent_CenterDiff_unit = 0.001;
	max_speed_count = 0;
	
    
	//IMAGE
	m_noline_count = 0;
	m_noline_max = 4;
	m_line_angle = 0.00;
	pan_unit = 2.0;
	pan = -10.0;
	tilt = -10.0;
	m_line_found = 1;

	tmp_img_result = 0;

	//*******add:image paraments
	m_line_center.x = 0;
	m_line_center.y = 0;
	m_line_center_diff.x = 0;
	m_line_center_diff.y = 0;
	m_MAX_RLTurn = 35;

	//MOTION
	 m_RLTurn = 0 ;
	 m_RLTurn_unit = 2.0;
	 m_RLTurn_goal = 0;
	 m_xoffset = -4.0;
	 m_xoffdet_unit = 1.5;
	 m_FBStep = 10.0;
	 m_FBStep_unit = 0.1;
	 m_RLStep = 0.0;
	 m_unit_RLTurn = 2.0;


   
}

Fira_Marathon::~Fira_Marathon()
{
}

void Fira_Marathon::ThreadMotion(){
    motion = new Motion();

    motion->poseInit();
    Head::GetInstance()->MoveByAngle(0,-15);//-10
    //motion->walk( m_FBStep, m_RLStep, m_RLTurn);
	Walking::GetInstance() -> Z_MOVE_AMPLITUDE = 20;
    Walking::GetInstance()->PERIOD_TIME = 600;//TODO400
    while( m_execute )
	{
		printf("\n");
            motion->CheckStatus();
           
			//if ( debug_print )
				//fprintf(stderr,"state %d\n\n",m_process_state);

			tmp_img_result = GetImageResult();

			if (m_process_state == LINE)
				Line();
			else if(m_process_state == NOLINE)
				NoLine();
			else if(m_process_state == JUDGE)
             			Judge();
			else if(m_process_state == LOST)
             			Lost();
        }
        
}

int Fira_Marathon::GetImageResult()
{
	int cnt = 0;
    while ( is_new_img == false ) usleep(8000);

    m_imgProc->imageProcess(frame,m_imgRes);
     MarathonResult *tmp_result = dynamic_cast< MarathonResult *>(m_imgRes);//TODO

	 if(m_process_state == LINE||m_process_state == NOLINE||m_process_state == LOST)
	 {
	 	if ( tmp_result->line_valid == 1)
	 	{
			 m_line_found = true;
			 
			 
			 m_line_center.x= tmp_result->line_center.x;
			 m_line_center.y = tmp_result->line_center.y;
			 m_pre_slope =(m_line_center.x-160.0)/(m_line_center.y-240);
			 m_line_angle = (m_pre_slope) * 180 / 3.141592;
			 //if (debug_print) //fprintf(stderr, "angle:%lf  line center: %lf %lf \n", m_line_angle, m_line_center.x, m_line_center.y);
			printf("slope:%lf\n",m_pre_slope);
			m_process_state=LINE;
		 }
		 else
		 {
			m_process_state = JUDGE;
		 }
	}
	if(m_process_state == JUDGE)
	{
		if ( tmp_result->line_valid == 1)
	 	{
			 m_line_found = true;
			
			 m_line_center.x = tmp_result->line_center.x;
			 m_line_center.y = tmp_result->line_center.y;
			m_pre_slope =(m_line_center.x-160.0)/(m_line_center.y-240);
			 m_line_angle = (m_pre_slope) * 180 / 3.141592;
			 m_noline_count ++;
			 if(m_noline_count > m_noline_max)
				m_process_state = LOST;
			 //if (debug_print) 
				//fprintf(stderr, "angle:%lf  line center: %lf %lf \n", m_line_angle, m_line_center.x, m_line_center.y);
			//printf("slope:%lf\n",m_pre_slope);
		 }
		 else
		 {
			m_process_state = LOST;
		 }
	}
    is_new_img = false;
}

void Fira_Marathon::Line()
{
	 Head::GetInstance()->MoveByAngle(0,-15);//-10
	usleep(10*8000);
	//printf("LINE\n");
	int t;
	//m_FBStep += m_FBStep_unit;
	
	//m_xoffset -= m_xoffdet_unit;
	//if(m_FBStep>=10)
	//	m_FBStep = m_FBStep;
	//if(m_xoffset<-10)
	//	m_xoffset = m_xoffset;
	if (m_line_found == true&&m_pre_slope>=0.5)
	{	t=1;
		if (m_line_angle < 0)
		{
			m_pre_state = RIGHT;
			m_RLTurn_goal = 0.3* m_line_angle  / 90 * m_MAX_RLTurn;//-1 *0.3* (m_line_angle + 90) / 90 * m_MAX_RLTurn;
			RLTurn(m_RLTurn_goal,t);
		}
		if (m_line_angle > 0)
		{
			m_pre_state = LEFT;
			m_RLTurn_goal = 0.3*m_line_angle/ 90 * m_MAX_RLTurn;// 0.3*(90 - m_line_angle) / 90 * m_MAX_RLTurn
			RLTurn(m_RLTurn_goal,t);
		}
		
	}
	else if (m_line_found == true&&m_pre_slope<-0.5)
	{
		t=2;
		if (m_line_angle < 0)
		{
			m_pre_state = RIGHT;
			m_RLTurn_goal = 0.3* m_line_angle / 90 * m_MAX_RLTurn;
			RLTurn(m_RLTurn_goal,t);
		}
		if (m_line_angle > 0)
		{
			m_pre_state = LEFT;
			m_RLTurn_goal = 0.3* m_line_angle / 90 * m_MAX_RLTurn;
			RLTurn(m_RLTurn_goal,t);
		}
		
	}
	else if (m_line_found == true&&m_pre_slope>-0.5&&m_pre_slope<0.5)
	{
		t=3;
		if (m_line_angle < 0)
		{
			m_pre_state = RIGHT;
			m_RLTurn_goal = 0.3* m_line_angle  / 90 * m_MAX_RLTurn;
			RLTurn(m_RLTurn_goal,t);
		}
		if (m_line_angle > 0)
		{
			m_pre_state = LEFT;
			m_RLTurn_goal = 0.3* m_line_angle/ 90 * m_MAX_RLTurn;
			RLTurn(m_RLTurn_goal,t);
		}	
	}
	else if (m_line_found == false)
	{
		m_process_state = NOLINE;
	}
	printf("line_angle:%f\n", m_line_angle);
	printf("m_RLTurn_goal:%f\n", m_RLTurn_goal);
	motion->walk( m_FBStep, m_RLStep, m_RLTurn);
	Walking::GetInstance()->X_OFFSET = m_xoffset;
}
void Fira_Marathon::NoLine()
{
	printf("NOLINE\n");
        int ti=3;
	Head::GetInstance()->MoveByAngle(0,-15);//-15
	motion->walk(8.00, m_RLStep, m_RLTurn);
	Walking::GetInstance()->X_OFFSET = -4.0;
	if (m_line_found == true)
	{
		if (m_line_angle < 0)
		{
			m_pre_state = RIGHT;
			m_RLTurn_goal = -1 * m_line_angle  / 90 * m_MAX_RLTurn;
			RLTurn(m_RLTurn_goal,ti);
		}
		if (m_line_angle > 0)
		{
			m_pre_state = LEFT;
			m_RLTurn_goal =  m_line_angle / 90 * m_MAX_RLTurn;
			RLTurn(m_RLTurn_goal,ti);
		}
		
	}
	else if (m_line_found == false)
	{
		m_process_state = NOLINE;
	}
	motion->walk( m_FBStep, m_RLStep, m_RLTurn);
	Walking::GetInstance()->X_OFFSET = m_xoffset;
}
void  Fira_Marathon::Judge()
{
	pan =-10;
	printf("JUDGE\n");
	 motion->walk(0,0,0);
	 Walking::GetInstance()->Stop();
	 usleep(200*8000);

	 Head::GetInstance()->MoveByAngle(pan,0);
	 usleep(200*8000);

}
void  Fira_Marathon::Lost()
{
	//Head::GetInstance() -> MoveByAngle(-10,0 );
	motion->walk( m_FBStep, m_RLStep, m_RLTurn);
	usleep(50*8000);
	printf("LOST\n");
	motion->walk( m_FBStep, m_RLStep, 0);
	usleep(10*8000);
	 motion->walk(0,0,0);
	 Walking::GetInstance()->Stop();
 	const int pan_left_limit = 20;
    const int pan_right_limit= -20;
	if(m_line_angle>0)
	 { pan += pan_unit;
    if (pan > pan_left_limit)
        pan_unit = -pan_unit;
    else if (pan < pan_right_limit)
        pan_unit = -pan_unit;   
	}
	else if(m_line_angle<0)
	{
		pan -= pan_unit;
    if (pan > pan_left_limit)
        pan_unit = -pan_unit;
    else if (pan < pan_right_limit)
        pan_unit = -pan_unit;  
	}

	Head::GetInstance() -> MoveByAngle(pan, 0);
	usleep(50 * 8000);
}
void Fira_Marathon::RLTurn(double RLTurn,int turn)
{
	if (m_process_state == LINE&&turn==3)
	{
		if (m_RLTurn < m_RLTurn_goal) 
		{
			m_RLTurn += 1.0*m_unit_RLTurn;//1.8
		}
		else
		{
			m_RLTurn -= 1.0*m_unit_RLTurn;
		}
		printf("m_RLTurn%lf\n",m_RLTurn);
		//printf("ZHIXING 111111\n");
		Walking::GetInstance()->A_MOVE_AMPLITUDE = m_RLTurn;
		motion->walk( m_FBStep, m_RLStep, m_RLTurn);
	}
	else if (m_process_state == LINE&&turn==2)
	{
		if (m_RLTurn < m_RLTurn_goal) 
		{
			m_RLTurn += 1.0*m_unit_RLTurn;//1.8
		}
		else
		{
			m_RLTurn -= 1.2*m_unit_RLTurn;//1.0  0.8 1.2
		}
		printf("m_RLTurn%lf\n",m_RLTurn);
		//printf("XIANGYOU2222222\n");
		Walking::GetInstance()->A_MOVE_AMPLITUDE = m_RLTurn;
		motion->walk( m_FBStep, m_RLStep, m_RLTurn);
	}
	else if (m_process_state == LINE&&turn==1)
	{
		if (m_RLTurn < m_RLTurn_goal) 
		{
			m_RLTurn += 2.5*m_unit_RLTurn;//1.8
		}
		else
		{
			m_RLTurn -= 1.0*m_unit_RLTurn;
		}
		printf("m_RLTurn%lf\n",m_RLTurn);
		//printf("XIANg ZUO33333333333\n");
		Walking::GetInstance()->A_MOVE_AMPLITUDE = m_RLTurn;
		motion->walk( m_FBStep, m_RLStep, m_RLTurn);
	}
	
	else if (m_process_state == NOLINE )
	{
		if (m_RLTurn < m_RLTurn_goal)
		{
			m_RLTurn += 0.8*m_unit_RLTurn;
		}
		else
		{
			m_RLTurn -= 0.5*m_unit_RLTurn;
		}
	}
		Walking::GetInstance()->A_MOVE_AMPLITUDE = m_RLTurn;
		motion->walk(8.00, m_RLStep, m_RLTurn);
	
}
