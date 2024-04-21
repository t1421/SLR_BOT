#include <iostream>
#include "incl/Broker.h"
#ifdef MIS_DEBUG
#include "incl/DEBUG.h"
#include <future>
#endif // MIS_DEBUG
#include "incl/Util.h"
#include "incl/LOAD.h"
#include "incl/CardBaseSMJ.h"



bool run_FireBot(broker* Bro,  unsigned short port, std::string sName);


int main(int argc, char** argv)
{

	//Create Stuff
	broker* Bro = new broker;

#ifdef MIS_DEBUG
	DEBUG* B = new DEBUG("MIS_BOT_log", true, true, false);
	B->teachB();
	B->bFilter = true;	
	
	std::future<bool> fBOT;
#endif 

	LOAD* L = new LOAD();
	L->teachL();

	Util* U = new Util();
	U->teachU();

	CardBaseSMJ* J = new CardBaseSMJ();
	J->teachJ();

	//INIT stuff
	switch (L->ProcessArg(argc, argv))
	{
	case -1: return -1;
	case 1: B->ReOpenLog(L->Name);
		break;

	}	
	L->StartUp();

#ifdef MIS_Online
	if(L->SMJOnline)J->DownloadSMJ();
#endif
#ifdef MIS_Stream 
	L->Load_Stream();
	L->StartsPlus();
#endif

	if (!J->readJSON())
	{
		MISERROR("No JSON - i cant work like this :-(");
		//B->StatusE("E", "Main", "No JSON - i cant work like this :-(");
		//printf("No JSON - i cant work like this :-(\n");
		return -1;
	}

	L->EchoSettings();

#ifndef MIS_DEBUG
	MISERROR("sync");
	run_FireBot(Bro, L->Port, L->Name);
#else 
	MISERROR("async");
	fBOT = std::async(&run_FireBot, Bro, L->Port, L->Name);

	char buf[1024] = { '0' };
	while (Bro->sComand != "X")
	{
		std::cin >> buf;
		Bro->sComand = buf;
	}
	
#endif // !MIS_DEBUG

	

	return 0;
}
