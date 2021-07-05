#include "pch.h"
#include "FlowWork.h"
#include "AutoTrack.h"

FlowWork::FlowWork()
{
}

FlowWork::~FlowWork()
{
}

void FlowWork::append(AutoTrack* at, bool(AutoTrack::*funPtr)(void), int errCode)
{
	AT = at;
	funPtrList.push_back(funPtr);
	funErrCodeList.push_back(errCode);
}

bool FlowWork::run()
{
	if (funPtrList.size() > 0)
	{
		for (int i = 0; i < funPtrList.size(); i++)
		{
			if ((AT->*funPtrList[i])() == false)
			{
				//err.push(0);
				err = funErrCodeList[i];
				return false;
			}
		}
		return true;
	}
	else
	{
		err = 14;
		return false;
	}

}
