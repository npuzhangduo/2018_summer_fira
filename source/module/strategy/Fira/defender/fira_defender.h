#ifndef _fira_DEFENDER_H_
#define _fira_DEFENDER_H_

#include <strategy.h>

class FiraDefender:public Strategist
{
public:
	FiraDefender();
	~FiraDefender();

private:
	void Init();

protected:
	virtual void ThreadMotion();
	virtual int GetImageResult();
};

#endif // _FiraDefender_H_
