#include <iostream>
#include "incl/Broker.h"
#include "incl/DEBUG.h"
#include "incl/Util.h"
#include "incl/LOAD.h"
#include "incl/CardBaseSMJ.h"


void run_FireBot(broker* Bro,  unsigned short port);

int main()
{
	broker* Bro = new broker;

	DEBUG* B = new DEBUG("BOT_LOG", true, true, false);
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


	B->StatusNew("", "Boot BOT with this Settings");
	L->EchoSettings();

	run_FireBot(Bro,6370);

	return 0;
}
