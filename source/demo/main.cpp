#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include <opencv/highgui.h>
#include <opencv/cv.h>
#include <BlobResult.h>
#include <camera.h>
#include <imgproc.h>
#include <motion.h>
#include <strategy.h>
#include <communication.h>

#include <fira_golf.h>
#include <fira_ladder.h>
#include <fira_marathon.h>
#include <fira_obstacle.h>
#include <fira_penalty.h>
#include <fira_shot.h>
#include <fira_sprint.h>
#include <fira_weightlifting.h>
#include <fira_2v2.h>
#include <fira_defender.h>

#define IMG_WIDTH 320
#define IMG_HEIGHT 240

// 以下宏定义对应于键盘上的0~9.
#define TEST			0
#define OBSTACLE		1
#define MARATHON		2
#define ROBCUP_STRIKER 		3
#define ROBCUP_FULLBACK		4
#define ROBCUP_GOALKEEPER	5
#define ROBCUP_PENALTY		6
#define SPRINT			7
#define LADDER			8
#define GOLF			9
#define SHOT			10
#define FIRA_PENALTY	        11
#define ARCHERY                 12
#define WEIGHTLIFTING           13
#define TWOVSTWO                     14
#define DEFENDER         15

void runObstacle();
void runMarathon();
void runSprint();
void runLadder();
void runGolf();
void runShot();
void runFira_penalty();
void runWeightlifting();
void runtwovstwo();
void runTest();
void runDefender();

int main(int argc, char *argv[])
{	
    if(argc == 1)
        return 1;
    switch(((int)argv[1][0] - 48) * 10 + ((int)argv[1][1] - 48)) {
        case OBSTACLE:
            runObstacle();
            break;
        case MARATHON:
            runMarathon();
            break;
        case SPRINT:
            runSprint();
            break;
        case LADDER:
            runLadder();
            break;
        case GOLF:
            runGolf();
            break;
        case SHOT:
            runShot();
            break;
        case FIRA_PENALTY:
            runFira_penalty();
            break;
        case WEIGHTLIFTING:
            runWeightlifting();
	    case TWOVSTWO:
		    runtwovstwo();
        case DEFENDER:
            runDefender();

        default:
            printf("Please input the choice(e.g:sudo ./demo 1):\n");
            printf("%2d:Obstacle,         %2d:Marathon,           %2d:Robcup_striker\n", OBSTACLE, MARATHON,ROBCUP_STRIKER);
            printf("%2d:Robcup_Fullback,  %2d:Robcup_goalkeeper,  %2d:runRobcup_penalty\n", ROBCUP_FULLBACK, ROBCUP_GOALKEEPER, ROBCUP_PENALTY);
            printf("%2d:Sprint,           %2d:Ladder,             %2d:Golf\n", SPRINT, LADDER, GOLF);
            printf("%2d:Shot,             %2d:Fira_penalty:       %2d:Archery  \n", SHOT, FIRA_PENALTY,ARCHERY);
            printf("%2d:Weightlifting\n", WEIGHTLIFTING);
            break;
    }

    return 0;
}

void runObstacle()
{
    Obstacle *obstacle = new Obstacle();
    obstacle->run();
}

void runMarathon()
{
    Fira_Marathon * marathon = new Fira_Marathon();
    marathon -> run();
}

void runSprint()
{
    FiraSprint *sprint = new FiraSprint();
    sprint->run();
}

void runLadder()
{
    Ladder *ladder = new Ladder();
    ladder->run();
}

void runGolf()
{
    Golf *golf = new Golf();
    golf->run();
}

void runShot()
{
    Shot *shot = new Shot();
    shot->run();
}

void runFira_penalty()
{
    Fira_penalty *fira_penalty = new Fira_penalty();
    fira_penalty->run();
}

void runWeightlifting()
{
    weightlifting *wweightlifting = new weightlifting();
    wweightlifting -> run();
}

void runtwovstwo()
{
    Fira_2v2 *fira_2v22 = new Fira_2v2();
    fira_2v22 -> run();
}

void runDefender()
{
    FiraDefender *firadefender = new FiraDefender();
    firadefender -> run();
}
