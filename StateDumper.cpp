#include "incl/Broker.h"
#ifdef MIS_DEBUG
#include "incl/DEBUG.h"
#endif // MIS_DEBUG
#include "incl/LOAD.h"

bool run_DumpBot(broker* Bro, unsigned short port, std::string sName);

int main(int argc, char** argv)
{
	broker* Bro = new broker;

#ifdef MIS_DEBUG
	DEBUG* B = new DEBUG("MIS_DUMP_log", true, true, false);
	B->teachB();
	B->bFilter = true;
#endif 

	LOAD* L = new LOAD();
	L->teachL();

	//INIT stuff
	switch (L->ProcessArg(argc, argv))
	{
	case -1: return -1;
#ifdef MIS_DEBUG
	case 1: B->ReOpenLog(L->Name);
#endif 
		break;
	}
	L->StartUp();

	run_DumpBot(Bro, L->Port, L->Name);

	return 0;
}
