#include <iostream>
#include "incl/Broker.h"
#include "incl/DEBUG.h"
#include "incl/Util.h"
#include "incl/LOAD.h"
#include "incl/CardBaseSMJ.h"


void run_FireBot(broker* Bro,  unsigned short port);

int main(int argc, char** argv)
{
	broker* Bro = new broker;

	DEBUG* B = new DEBUG("MIS_BOT_log", true, true, false);
	B->teachB();
	B->StatusNew("", "Init");

	LOAD* L = new LOAD();
	L->teachL();

	Util* U = new Util();
	U->teachU();

	CardBaseSMJ* J = new CardBaseSMJ();
	J->teachJ();

	B->StatusNew("", "Startup");
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
		B->StatusE("E", "Main", "No JSON - i cant work like this :-(");
		return -1;
	}

	if (argc >= 2)L->Port = atoi(argv[1]);
	if (L->Port == 0)L->Port = 6370;

	B->StatusNew("", "Boot BOT with this Settings");
	L->EchoSettings();

	run_FireBot(Bro,6370);

	return 0;
}
