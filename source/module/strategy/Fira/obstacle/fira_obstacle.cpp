#include "fira_obstacle.h"
#define TILT (20)

Obstacle::Obstacle()
{
    	imgRes = new FiraObstacleVisionResult();
    	imgProc = new FiraObstacleVision();

    	m_pixel_num = 0;
    	m_PixelJudgeNum[0] = 12000;
    	m_PixelJudgeNum[1] = 4000;
    	m_PixelJudgeNum[2] = 100;//abandon
    	m_obstacle_center.x =0;
    	m_obstacle_center.y =0;
    	m_ObstacleDiff = 10;
    	m_ObstacleCenterNeed[0] = 290;
    	m_ObstacleCenterNeed[1] = 100;
    	m_ObstacleCenterNeed[2] = 262;
    	m_TurnPan[0] = 67;
    	m_TurnPan[1] = 65;
    	m_TurnPan[2] = 65;

    	m_execute = 1;
    	m_pre_action = 0;
    	m_pre_state = LAST;
    	m_process_state = BEFOREODD;

    	TURN_count = 0;

    	//TODO go straight first    
    	pan = 0;
    	tilt = 10;
    	unit_pan = 0.5;

    	pan_range = Head::GetInstance()->GetLeftLimitAngle(); 
    	tilt_min = Head::GetInstance()->GetBottomLimitAngle();
    	tilt_range = Head::GetInstance()->GetTopLimitAngle() - tilt_min;
    	unit_tilt = 0.5;

    	m_FBstep = 10;
    	m_FBstep_straight = 10;
    	m_FBstep_goal = 10;
    	m_FBstep_ApproachMin = 5;
    	m_unit_FBstep = 0.3;
    	m_MAX_FBstep = 30; 
    	m_RLturn = 0;
    	m_RLturn_goal = 0;
    	m_RLturn_straight = 0;
    	m_unit_RLturn = 5.0;
    	m_MAX_RLturn = 35;
    	m_RLstep_straight = 0;
    	m_RLstep_goal = 0;
    	m_RLstep = 0;
    	m_unit_RLstep = 0.3;
	m_MAX_RLstep = 30;

    	tmp_img_result = 0;
    	tmp_rlfix_result = 0;
    	tmp_turn_result = 0;
    	turn_flag = 0;

    debug_print = 0;
}

Obstacle::~Obstacle()
{
}

void Obstacle::ThreadMotion()
{
	while (0)
    	{ 
       	if ( debug_print ) 
       		fprintf(stderr,"state %d. FB:%lf.  RLstep:%lf  RLturn%lfcolor:%d\n",m_process_state,Walking::GetInstance()->X_MOVE_AMPLITUDE,Walking::GetInstance()->Y_MOVE_AMPLITUDE,Walking::GetInstance()->A_MOVE_AMPLITUDE,TURN_count);
        	int last_count=0;
        	Head::GetInstance()->MoveByAngle(0,TILT);
        	if (last_count==1)
            		motion->walk(m_FBstep_straight,m_RLstep_straight,m_RLturn_straight);
        	else
        	{
            		motion->walk(1,0,-10);
            		motion->walk(1,0,-10);
            		int  tmp = 0;
            		while(1)
            		{
                		printf("%d\n",tmp++);
                		usleep(620*8000);
            		}
            		last_count =1;
        	}
    	}

    	while ( m_execute )
    	{
        	motion->CheckStatus();
        	if ( m_process_state == BEFOREODD)
        		BeforeOdd();
        	else if ( m_process_state == BEFOREEVEN)
        		BeforeEven();
        	else if ( m_process_state == LAST)
        		Last();
        	else if ( m_process_state == TURN )
        		Turn();
        	else if ( m_process_state == TURNNEXT)
        		TurnNext();
    	}
}

void Obstacle::Init()
{
    	motion = new Motion();
    	motion->walk(m_FBstep_straight, m_RLstep_straight, m_RLturn_straight);
    	motion->poseInit();
    	Walking::GetInstance()->Start();
		Walking::GetInstance() -> X_OFFSET = 0;
}

int Obstacle::GetImageResult()
{
    while ( is_new_img == false ) usleep(8000);
    imgProc->imageProcess(frame,imgRes);
    FiraObstacleVisionResult *tmp_result = dynamic_cast<FiraObstacleVisionResult *>(imgRes);

 /*   m_obstacle_center[1] = tmp -> color1.center;
    m_obstacle_center[2] = tmp -> color2.center;
    m_obstacle_center[3] = tmp -> color3.center;

    m_obstacle_valid[1] = tmp -> color1.valid;
    m_obstacle_valid[2] = tmp -> color2.valid;
    m_obstacle_valid[3] = tmp -> color3.valid;

    m_pixel_num[1] = tmp -> color1.area;
    m_pixel_num[2] = tmp -> color2.area;
    m_pixel_num[3] = tmp -> color3.area;
 */
    is_new_img = false;
    return 0;
}

void Obstacle::BeforeOdd()
{
      	Head::GetInstance()->MoveByAngle(0,TILT);
	tmp_img_result = GetImageResult();
      	if ( tmp_img_result == -1 )
             LostDispose();
      else 
      {
             turn_flag = 1;
             tmp_turn_result = TurnAdjust(TURN_count); 
             if ( tmp_turn_result == 0 )
             {
                    m_FBstep = m_FBstep_straight;
                    m_RLturn = m_RLturn_straight;
                    turn_flag = 0;
                    m_pre_state = BEFOREODD;
                    m_process_state = TURN;
             }
             else 
                    motion->walk(m_FBstep,m_RLstep,m_RLturn);
      }
}

void Obstacle::BeforeEven()
{
	Head::GetInstance()->MoveByAngle(0,TILT);
       tmp_img_result = GetImageResult();
       if ( tmp_img_result == -1 )
       {
      		if ( debug_print ) 
                	fprintf(stderr,"Can't find the Obstacle!\n");
      	}
      else 
      {
             //if ( m_pixel_num > m_PixelJudgeNum[TURN_count] || turn_flag ){
      		turn_flag = 1;
             tmp_turn_result = TurnAdjust(TURN_count); 
             if ( tmp_turn_result == 0 )
             {
                    m_FBstep = m_FBstep_straight;
                    m_RLturn = m_RLturn_straight;
                    turn_flag = 0;
                    m_pre_state = BEFOREEVEN;
                    m_process_state = TURN;
             }
             motion->walk(m_FBstep,m_RLstep,m_RLturn);
             //}
      } 
}

void Obstacle::Last()
{
	static int last_count=0;
      	Head::GetInstance()->MoveByAngle(0,TILT);
      	if (last_count==1)
             	motion->walk(m_FBstep_straight,m_RLstep_straight,m_RLturn_straight);
      	else
      	{
             	motion->walk(1,0,-10);
             	motion->walk(1,0,-10);
             	last_count =1;
             	usleep(620*8000);
      }
}

void Obstacle::Turn()
{
      	static bool pixelnum_flag = false;
      	static int not_found_count = 0;
      	tmp_img_result = GetImageResult();
      	if ( !pixelnum_flag) 
      	{
             if ( m_pixel_num > m_PixelJudgeNum[TURN_count] ) 
                    pixelnum_flag = true;
      	}
      	static const int TurnMaxCount = 50000; //TODO
      	if ( tmp_img_result == -1 )
      	{
             	if ( debug_print ) fprintf(stderr,"Can't find the Obstacle!\n");
             	not_found_count++;
             	if ( m_pre_state == BEFOREODD )
                   	Head::GetInstance()->MoveByAngleOffset(-0.1*unit_pan,0);
             	else 
                    Head::GetInstance()->MoveByAngleOffset(0.1*unit_pan,0);
      	}
      else 
      {
             	not_found_count = 0;
             	HeadTracker(m_obstacle_center);
             	pan = MotionStatus::m_CurrentJoints.GetAngle(JointData::ID_HEAD_PAN);
             if ( fabs(pan) > m_TurnPan[TURN_count] && pixelnum_flag)
             { 
                    pixelnum_flag = false;
                    TURN_count++;
                    if (TURN_count == 3 )
                        m_process_state = LAST;
                    else 
                        m_process_state = TURNNEXT;
             }   
             else 
                   not_found_count++;
             tmp_rlfix_result = RLFixed();
             if( tmp_rlfix_result == 0 )
                    m_FBstep = m_FBstep_straight; m_RLstep = m_RLstep_straight; m_RLturn = m_RLturn_straight; 
      }
      motion->walk(m_FBstep, m_RLstep, m_RLturn);
      if ( not_found_count > TurnMaxCount )
      {
             TURN_count++;
             //ChangeObstacleColor(TURN_count);
             m_process_state = TURNNEXT;
      	}
}

void Obstacle::TurnNext()
{
      	double LR = 1.0;// +L -R
      	static bool track_flag = false;
      	tmp_img_result = GetImageResult();

      	pan = MotionStatus::m_CurrentJoints.GetAngle(JointData::ID_HEAD_PAN);

      	if ( pan > 0)
      		LR *= -1;

      	if ( fabs(pan) < 10 )
      	{
             	if (tmp_img_result==0)
             	{
                   	if ( m_pre_state == BEFOREODD )
                   	{
                        	if (TURN_count >1)
                            m_process_state = LAST;
                        	else
                            m_process_state = BEFOREEVEN;
                    }
                    else 
                        m_process_state = BEFOREODD;
             	}
             	else 
             	{
                    printf("tracker!!!\n");
                    track_flag = true;
             }
      	}
      	else 
             Head::GetInstance()->MoveByAngleOffset(LR*3*unit_pan,0);

       if ( tmp_img_result == 0 )
       {
             track_flag = false;
             if ( debug_print ) 
             		fprintf(stderr,"Find the next Obstacle!\n");
             m_RLturn_goal = pan*m_MAX_RLturn/pan_range;
             if ( debug_print ) 
             		fprintf(stderr,"RLturn_goal: %lf !\n",m_RLturn_goal);
             	if(m_RLturn < m_RLturn_goal)
                    m_RLturn += 0.5*m_unit_RLturn;
             else
                    m_RLturn -= 0.5*m_unit_RLturn;
             m_FBstep = -1.0;
             motion->walk(m_FBstep,m_RLstep,m_RLturn);
       }
       else 
       {
             	if (track_flag==true)
                    tracker.Search(0);
             motion->walk(0,0,0);
      	}
}

int Obstacle::RLFixed()
{
    int tmp_return = 0;
    double diff = 0.0;
    static int LeftLimit = 170, RightLimit =120;
    if(TURN_count % 2 == 0 ){
        if ( m_obstacle_center.x <LeftLimit ){
            tmp_return = -1; 
            diff = LeftLimit - m_obstacle_center.x;
            m_RLturn_goal = m_MAX_RLturn *diff/IMG_WIDTH;
        }
        else if  ( m_obstacle_center.x > (LeftLimit+65) ){
            tmp_return = -1; 
            diff = LeftLimit+65 - m_obstacle_center.x;
            m_RLturn_goal = m_MAX_RLturn *diff/IMG_WIDTH;
        }
    }
    	else
	{
      		if ( m_obstacle_center.x > RightLimit ){
            	tmp_return = -1; 
            	diff = (-1)*(m_obstacle_center.x - RightLimit);
            	m_RLturn_goal = m_MAX_RLturn *diff/IMG_WIDTH;
       }
       else if ( m_obstacle_center.x  < (RightLimit - 70) ){
            tmp_return = -1; 
            diff = m_obstacle_center.x - (RightLimit - 70);
            m_RLturn_goal = m_MAX_RLturn *diff/IMG_WIDTH;
        }
    }
    if (tmp_return != 0 ){
        if(m_RLturn < 5*m_RLturn_goal){
            m_RLturn  += 1.2*m_unit_RLturn;
        }
        else{
            m_RLturn -= 0.7*m_unit_RLturn;
        }
        m_FBstep = 10.0;
    }
    return tmp_return;
}

int Obstacle::TurnAdjust(int direction) // OK return 0; else -1
{
    	int tmp_return = 0;
    	double diff = m_obstacle_center.x - m_ObstacleCenterNeed[direction];
    	if ( fabs(diff) > m_ObstacleDiff )
    	{
        	tmp_return = -1;
        	m_RLturn_goal = -1 * m_MAX_RLturn * diff /(IMG_WIDTH);
        	if ( diff > 0 )
            		m_pre_action = -1;
        	else 
            		m_pre_action = 1;
        	
        	if(m_RLturn < m_RLturn_goal)
            		m_RLturn += m_unit_RLturn;
        	else
            		m_RLturn -= m_unit_RLturn;
 
        	m_FBstep = -1.0;
        	if ( debug_print ) 
        		fprintf(stderr,"RLfix:turn: %lf\n", m_RLturn);
    	}
    	return tmp_return;
}

int Obstacle::LostDispose()
{

}

int Obstacle::HeadTracker(cv::Point2f pos)
{
    	static int NoFindCount =0;
    	static const int NoFindMaxCount = 4;
    	if(pos.x < 0 || pos.y < 0)
    	{
        	if(NoFindCount < NoFindMaxCount)
        	{
            		Head::GetInstance()->MoveTracking();
            		NoFindCount++;
        	}
        	else
            		Head::GetInstance()->InitTracking();
    	}
    	else
    	{
       	NoFindCount = 0;
        	Point2D offset;
        	offset.X = pos.x - IMG_WIDTH/2;
        	offset.Y = pos.y - IMG_HEIGHT/2;
        	offset *= -1; // Inverse X-axis, Y-axis 
        	offset.X *= 0.7*(VIEW_H_ANGLE / IMG_WIDTH); // pixel per angle 
        	offset.Y *= 0.5*(VIEW_V_ANGLE / IMG_HEIGHT);
        	//offset.Y =0;
        Head::GetInstance()->MoveTracking(offset);
    	}
}

