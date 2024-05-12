#include "incl/Broker.h"
#include "incl/CardBaseSMJ.h"

int main(int argc, char** argv)
{
	broker* Bro = new broker;
	CardBaseSMJ* J = new CardBaseSMJ();
	J->teachJ();
	J->DownloadSMJ();
	return 0;
}
