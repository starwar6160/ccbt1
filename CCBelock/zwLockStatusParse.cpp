#include "stdafx.h"
using boost::ifind_all;
using boost::iterator_range;
using boost::split;
using boost::is_any_of;

typedef struct jcLockStatus_t{
	string ActiveStatus;
	string EnableStatus;
	string LockStatus;
	string DoorStatus;
	string BatteryStatus;
	string ShockAlert;
	string ShockValue;
	string TempAlert;
	string nodeTemp;
	string PswTryAlert;
	string LockOverTime;
}JCLOCKSTATUS;

void zwParseLockStatus(const char *LockStatus)
{
	string LockStatusStr=LockStatus;
	vector<string>StatusVec;
	split(StatusVec, LockStatusStr, is_any_of(",.:-+")); //使用标点分割
	for(BOOST_AUTO(pos, StatusVec.begin()); pos != StatusVec.end(); ++pos)
	{
		cout<<" "<<*pos<<" ";
	}
	cout<<endl;
	cout<<"**********************"<<endl;
	JCLOCKSTATUS lst;
	lst.ActiveStatus=StatusVec[0];
	lst.EnableStatus=StatusVec[1];
	lst.LockStatus=StatusVec[2];
	lst.DoorStatus=StatusVec[3];
	lst.BatteryStatus=StatusVec[4];
	lst.ShockAlert=StatusVec[5];
	lst.ShockValue=StatusVec[6];
	lst.TempAlert=StatusVec[7];
	lst.nodeTemp=StatusVec[8];
	lst.PswTryAlert=StatusVec[9];
	lst.LockOverTime=StatusVec[10];

}