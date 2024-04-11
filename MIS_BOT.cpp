#include <iostream>
#include "incl/Broker.h"
#ifdef MIS_DEBUG
#include "incl/DEBUG.h"
#include <future>
#endif // MIS_DEBUG
#include "incl/Util.h"
#include "incl/LOAD.h"
#include "incl/CardBaseSMJ.h"


bool run_FireBot(broker* Bro,  unsigned short port);


int main(int argc, char** argv)
{
	broker* Bro = new broker;

#ifdef MIS_DEBUG
	DEBUG* B = new DEBUG("MIS_BOT_log", true, true, false);
	B->teachB();
	B->bFilter = true;
	B->StatusNew("", "Init");
	
	std::future<bool> fBOT;
#endif // MIS_DEBUG	

	LOAD* L = new LOAD();
	L->teachL();

	Util* U = new Util();
	U->teachU();

	CardBaseSMJ* J = new CardBaseSMJ();
	J->teachJ();

	//B->StatusNew("", "Startup");
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

	if (argc >= 2)L->Port = atoi(argv[1]);
	if (L->Port == 0)L->Port = 6370;

	printf("BOT Settings\n");
	L->EchoSettings();

#ifndef MIS_DEBUG
	MISERROR("sync");
	run_FireBot(Bro, L->Port);
#else 
	MISERROR("async");
	fBOT = std::async(&run_FireBot, Bro, L->Port);

	char buf[1024] = { '0' };
	while (Bro->sComand != "X")
	{
		std::cin >> buf;
		Bro->sComand = buf;
	}
	
#endif // !MIS_DEBUG

	

	return 0;
}
