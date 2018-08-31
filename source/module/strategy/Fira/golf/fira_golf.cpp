#include "fira_golf.h"
#include <time.h>

Golf::Golf()
{
	m_imgRes = new GolfVisionResult();
	m_imgPro = new GolfVision();

	m_execute = true;
	m_process_state = FINDBALL;
	m_pre_action = 0;

	m_ball_valid = 0;
	m_hole_valid = 0;

	m_ball_center.x = 0; 
	m_ball_center.y = 0;
	m_ball_radius = 0.0;
	m_hole_center.x = 0; 
	m_hole_center.y = 0;

	m_ApproachBallTop = 120;
	m_ApproachHoleTop = 105;
	m_AddressHoleCenter = 85;
	m_AddressDiff = 20;
	m_AddressBallCenter.x = 160;

	m_SwingBallCenter.x = 20;
	m_SwingBallCenter.y = 190;
	m_SwingDiff_X = 5;
	m_SwingDiff_Y = 20;
	m_diff_ball_hole = 0;
	m_diff_ball_hole_standard = 20;
	m_pre_state = ADDRESSBALL1;

	m_RLdiff = 0;
	m_FBdiff = 0;

	m_pan = 0;
	m_pan_unit = -7.0;
	m_pan_left_limit = Head::GetInstance() -> GetLeftLimitAngle();
	m_pan_right_limit = Head::GetInstance() -> GetRightLimitAngle();
	m_tilt = 0;
	m_tilt_unit = 10.0;

	m_FBStep = 0;
	m_FBStep_goal = 0;
	m_unit_FBStep = 0.3;
	m_MAX_FBStep = 25; 

	m_RLTurn = 0;
	m_RLTurn_goal = 0;
	m_RLTurn_unit = 3.0;
	m_RLTurn_max = 10;

	m_RLStep_goal = 0;
	m_RLStep = 0;
	m_unit_RLStep = 0.3;
	m_MAX_RLStep = 20;

	swing_count = 0;
	debug_print = 1;
	experience_data = 0;

	flag = 0;
}

Golf::~Golf()
{
}

void Golf::ThreadMotion()
{
	Init();

	while (0)
	{
		GetImageResult();
		printf("%f %f\n", m_hole_center.x, m_hole_center.y);
		//while (MotionStatus::BUTTON != 2);
		//Action::GetInstance()->m_Joint.SetEnableBody(true, true);
		//Action::GetInstance()->Start(81);
		//printf("%f %f\n", m_ball_center.x, m_ball_center.y);
		//Action::GetInstance()->m_Joint.SetEnableBody(true, true);
		if (MotionStatus::BUTTON == 1)
			Action::GetInstance()->Start(80);
	}

	while (0)
	{
		double hole_center;
		double ball_center;

		Walking::GetInstance() -> Stop();
		Head::GetInstance() -> MoveByAngle(0, -20);
		usleep(100 * 8000);
		
		ball_center = m_ball_center.x;
		usleep(100 * 8000);

		Head::GetInstance() -> MoveByAngle(0, 20);
		usleep(100 * 8000);
		
		usleep(100 * 8000);
		hole_center = m_hole_center.x;

		m_diff_ball_hole = ball_center - hole_center;

		printf("\n");
		printf("ball_center:%f\n", ball_center);
		printf("hole_center:%f\n", hole_center);
		printf("m_diff:%f\n", m_diff_ball_hole);
	}    	

	while(m_execute)
	{	
		printf("\n");
		printf("m_process_state:%d\n", m_process_state);
		//printf("m_FBStep:%f\n", m_FBStep);
		//printf("m_RLStep:%f\n", m_RLStep);
		//printf("m_RLTurn:%f\n", m_RLTurn);
		motion->CheckStatus();

		GetImageResult();

		if (m_process_state == FINDBALL)
			FindBall();
		else if (m_process_state == ADDRESSBALL1)
			AddressBall1();
		else if (m_process_state == APPROACHBALL1)
			ApproachBall1();
		else if (m_process_state == FINDHOLE)
			FindHole();
		else if (m_process_state == ADJUSTHOLE)
			AdjustHole();
		else if (m_process_state == ADJUSTRL1)
			AdjustRL1();
		else if (m_process_state == ADJUSTFB1)
			AdjustFB1();
		else if (m_process_state == LOST1)
			Lost();
		else if (m_process_state == FINDBALL2)
			FindBall2();
	}
}

int Golf::GetImageResult()
{
	m_imgPro->imageProcess(frame,m_imgRes);
	GolfVisionResult *tmp_result;
	tmp_result = dynamic_cast<GolfVisionResult *>(m_imgRes);

	while ( is_new_img == false ) usleep(8000);

	m_ball_valid = tmp_result -> ball_valid;
	m_hole_valid = tmp_result -> hole_valid;

	m_ball_center = tmp_result -> ball_center;
	m_hole_center = tmp_result -> hole_center;

	is_new_img = false;
	return 0;
}

void Golf::Init()
{
	motion = new Motion();
	GolfVision *tmp_proc = dynamic_cast<GolfVision *>(m_imgPro);
	motion -> poseInit();  	
	Head::GetInstance()->MoveByAngle(0,20);
	clock_gettime(CLOCK_MONOTONIC,&start);
	Walking::GetInstance() -> Z_MOVE_AMPLITUDE = 15;
}

void Golf::FindBall()
{
	static int ball_valid_cnt = 0;
	printf("m_ball_valid:%d\n", m_ball_valid);
	if (m_ball_valid)
	{
		Head::GetInstance() -> MoveByAngle(0, 20);
		if (m_pan < 0)
			motion -> swing(1, 0, 0, -8.0);
		else if (m_pan > 0)
			motion -> swing(1, 0, 0, 8.0);	
		usleep(50 * 8000);
		GetImageResult();
		if (m_ball_valid)
		{
			ball_valid_cnt++;
			if (ball_valid_cnt >= 1)
			{
				Head::GetInstance() -> MoveByAngle(0, 20);
				usleep(100 * 8000);
				m_process_state = ADDRESSBALL1;
			}
		}
	}
	else
	{
		ball_valid_cnt = 0;
		m_pan = Head::GetInstance() -> GetPanAngle();
		m_tilt = Head::GetInstance() -> GetTiltAngle();

		m_pan += m_pan_unit;
		if (m_pan > m_pan_left_limit)
			m_pan_unit = -m_pan_unit;
		else if (m_pan < m_pan_right_limit)
			m_pan_unit = -m_pan_unit;   

		Head::GetInstance() -> MoveByAngle(m_pan, m_tilt);
		usleep(50 * 8000);
	}
}

void Golf::AddressBall1()
{
	Walking::GetInstance() -> X_OFFSET = -5;
	double ball_diff = m_ball_center.x - m_AddressBallCenter.x;

	if ( fabs(ball_diff) >  m_AddressDiff)
	{
		m_RLTurn_goal = -1 * 1.3 * m_RLTurn_max * ball_diff / IMG_WIDTH;
		if (m_RLTurn > 0 && m_RLTurn_goal < 0 || m_RLTurn < 0 && m_RLTurn_goal > 0)
			m_RLTurn = 0;
		if(m_RLTurn < m_RLTurn_goal)
			m_RLTurn += m_RLTurn_unit;
		else
			m_RLTurn -= m_RLTurn_unit;
	}
	else
	{
		m_process_state = APPROACHBALL1;
		m_FBStep = 5.0; 
		m_RLStep = 0.0; 
		m_RLTurn = 0.0;
	}    	

	motion -> walk(m_FBStep, m_RLStep, m_RLTurn);	
}

void Golf::ApproachBall1()
{
	if (m_ball_valid)
	{
		double diff = m_ball_center.x - 160;
		m_RLTurn_goal = -1 * 4.0 * m_RLTurn_max * diff / IMG_WIDTH;
		if ( m_RLTurn < m_RLTurn_goal )
			m_RLTurn += m_RLTurn_unit;
		else 
			m_RLTurn -= m_RLTurn_unit;
		m_RLStep = 0;
		motion->walk(m_FBStep,m_RLStep,m_RLTurn);

		printf("m_ball_center.y:%f\n", m_ball_center.y);
		printf("m_RLTurn:%f\n", m_RLTurn);
		if (m_ball_center.y > 80 && m_tilt > -20)
		{
			m_tilt = Head::GetInstance() -> GetTiltAngle();
			m_tilt -= m_tilt_unit;
			printf("m_tilt:%f\n", m_tilt);
			Head::GetInstance() -> MoveByAngle(0, m_tilt);
			usleep(100 * 8000);
		}
		else if (m_ball_center.y > 100 && m_tilt == -20)
		{
			m_process_state = FINDHOLE;
			motion -> walk(0, 0, 0);
			Walking::GetInstance() -> Stop();
			Head::GetInstance() -> MoveByAngle(0, 20);
		}
	}
	else 
		motion -> walk(3, 0, 0);
}

void Golf::FindHole()
{
	static int hole_valid_cnt = 0;

	if (m_ball_valid)
	{
		motion -> swing(1, 5, 0, 0);
		return ;
	}

	if (m_hole_valid)
	{
		Head::GetInstance() -> MoveByAngle(0, 20);
		if (m_pan < 0)
			motion -> swing(1, -5.0, 0, -8.0);
		else if (m_pan > 0)
			motion -> swing(1, -5.0, 0, 8.0);
		usleep(100 * 8000);
		GetImageResult();
		if (m_hole_valid)
		{
			hole_valid_cnt++;
			if (hole_valid_cnt >= 2)
			{
				Head::GetInstance() -> MoveByAngle(0, 20);
				if (!flag)
				{
					motion->walk(0,0,0);
					usleep(100*8000);
					Action::GetInstance()->m_Joint.SetEnableBody(true, true);
					Action::GetInstance()->Start(81);
					usleep(500 * 8000);
					Walking::GetInstance()->m_Joint.SetEnableBodyWithoutHead(true, true);
					Walking::GetInstance()->m_Joint.SetEnableLeftArmOnly(false,false);
					Head::GetInstance()->m_Joint.SetEnableHeadOnly(true, true);
				}
				Head::GetInstance() -> MoveByAngle(0, -20);
				m_process_state = ADJUSTRL1;
			}	
		}
	}
	else
	{
		hole_valid_cnt = 0;
		m_pan = Head::GetInstance() -> GetPanAngle();
		m_tilt = Head::GetInstance() -> GetTiltAngle();

		m_pan += m_pan_unit;
		if (m_pan > m_pan_left_limit)
			m_pan_unit = -m_pan_unit;
		else if (m_pan < m_pan_right_limit)
			m_pan_unit = -m_pan_unit;   

		Head::GetInstance() -> MoveByAngle(m_pan, m_tilt);
		usleep(50 * 8000);

	}
}

void Golf::FindBall2()
{
	static int ball_valid_cnt2 = 0;
	printf("m_ball_valid:%d\n", m_ball_valid);
	if (m_ball_valid)
	{
		Head::GetInstance() -> MoveByAngle(0, -20);
		if (m_pan < 0)
			motion -> swing(1, 0, 0, -8.0);
		else if (m_pan > 0)
			motion -> swing(1, 0, 0, 8.0);	
		usleep(50 * 8000);
		GetImageResult();
		if (m_ball_valid)
		{
			ball_valid_cnt2++;
			if (ball_valid_cnt2 >= 1)
			{
				Head::GetInstance() -> MoveByAngle(0, 20);
				usleep(100 * 8000);
				m_process_state = ADJUSTRL1;
			}
		}
	}
	else
	{
		ball_valid_cnt2 = 0;
		m_pan = Head::GetInstance() -> GetPanAngle();
		m_tilt = Head::GetInstance() -> GetTiltAngle();

		m_pan += m_pan_unit;
		if (m_pan > m_pan_left_limit)
			m_pan_unit = -m_pan_unit;
		else if (m_pan < m_pan_right_limit)
			m_pan_unit = -m_pan_unit;   

		Head::GetInstance() -> MoveByAngle(m_pan, -20);
		usleep(50 * 8000);
	}

}

void Golf::AdjustRL1()
{	
	static int lost_RL = 0;
	GetImageResult();
	if (!m_ball_valid)
	{
		lost_RL++;
		if (lost_RL >= 5)
		{
			m_pre_state = ADJUSTRL1;
			m_process_state = FINDBALL2;
			return;
		}
	}
	else
		lost_RL = 0;

	Walking::GetInstance() -> Stop();
	Head::GetInstance() -> MoveByAngle(0, -20);

	m_RLdiff = m_ball_center.x - m_SwingBallCenter.x;
	printf("m_RLdiff:%f\n", m_RLdiff);
	Action::GetInstance()->m_Joint.SetEnableBody(true, true);
	if(fabs(m_RLdiff) < m_SwingDiff_X)
	{
		m_pre_state = ADJUSTRL1;
		m_process_state = ADJUSTFB1;
			Walking::GetInstance()->m_Joint.SetEnableBodyWithoutHead(true, true);
			Walking::GetInstance()->m_Joint.SetEnableLeftArmOnly(false,false);
			Head::GetInstance()->m_Joint.SetEnableHeadOnly(true, true);
	}
	else
	{
		if (m_RLdiff > 0)
		{
			Action::GetInstance()->Start(95);
			usleep(50 * 8000);

		}
		else
		{
			Action::GetInstance() -> Start(94);
			usleep(50 * 8000);
		}
	}
}

void Golf::AdjustFB1()
{
	static int lost_FB = 0;
	GetImageResult();
	if (!m_ball_valid)
	{
		lost_FB++;
		if (lost_FB >= 2)
		{
			m_pre_state = ADJUSTFB1;
			m_process_state = LOST1;
			return;
		}
	}
	else	
		lost_FB = 0;

	Head::GetInstance() -> MoveByAngle(0, -20);
	m_FBdiff = m_ball_center.y - m_SwingBallCenter.y;
	Walking::GetInstance() -> Z_MOVE_AMPLITUDE = 10;
	m_RLStep_goal = -1 * 4.0 * m_MAX_RLStep * m_RLdiff / IMG_WIDTH;
	m_FBStep_goal = -1 * 1.0 * m_MAX_FBStep * m_FBdiff / IMG_HEIGHT;

	if(fabs(m_FBdiff) >= m_SwingDiff_Y)
	{
		if (m_FBStep < 0 && m_FBStep_goal > 0 || m_FBStep >0 && m_FBStep_goal < 0)
			m_FBStep = 0;
		if (m_FBStep < m_FBStep_goal)
			m_FBStep += 0.1*m_unit_FBStep;
		else
			m_FBStep -= m_unit_FBStep;

		m_RLTurn = 3.0;
		m_RLStep = 0.0;
		motion->walk(m_FBStep,m_RLStep, m_RLTurn);
		Walking::GetInstance() -> Start();
		m_pre_state = ADJUSTFB1;
	}
	else
	{
		if (m_pre_state == ADJUSTRL1)
		{
			m_process_state = ADJUSTHOLE;
			m_pre_state = ADJUSTFB1;
		}
		else
			m_process_state = ADJUSTRL1;
	}
}

void Golf::Lost()
{
	GetImageResult();
	Head::GetInstance() -> MoveByAngle(0, -20);       
	if (m_ball_valid)
		m_process_state = m_pre_state;
	Walking::GetInstance()->m_Joint.SetEnableBodyWithoutHead(true, true);
	Walking::GetInstance()->m_Joint.SetEnableLeftArmOnly(false,false);
	Head::GetInstance()->m_Joint.SetEnableHeadOnly(true, true);
	Walking::GetInstance() -> Start();
	Walking::GetInstance() -> Z_MOVE_AMPLITUDE = 10;
	motion -> walk(-5.0, 0, 2.0);
}

void Golf::AdjustHole()
{    

	Head::GetInstance() -> MoveByAngle(0, 20);
	Walking::GetInstance() -> Z_MOVE_AMPLITUDE = 10;

	usleep(100 * 8000);
	GetImageResult();

	double hole_diff = m_hole_center.x - 90;

	printf("m_hole_center.x:%f\n", m_hole_center.x);
	printf("hole_diff:%f\n", hole_diff);

	if (m_hole_center.x == -1)
	{
		flag = 1; 
		m_process_state = FINDHOLE;
		return;
	}

	if (fabs(hole_diff) < 15)
	{
		if (m_pre_state == ADJUSTFB1)
		{
			Walking::GetInstance() -> Stop();
			usleep(500 * 8000);
			Action::GetInstance()->m_Joint.SetEnableBody(true, true);
			Action::GetInstance()->Start(80);
			usleep(1000 * 8000);
			Walking::GetInstance()->m_Joint.SetEnableBodyWithoutHead(true, true);
			Walking::GetInstance()->m_Joint.SetEnableLeftArmOnly(false,false);
			Head::GetInstance()->m_Joint.SetEnableHeadOnly(true, true);

		}
		else
			m_process_state = ADJUSTRL1;
	}
	else
	{
		if (hole_diff > 0)
			motion -> swing(1, -5.0, 0, -15.0);
		else	
			motion -> swing(1, -5.0, 0, 15.0);
		m_pre_state = ADJUSTHOLE;
	}
}
