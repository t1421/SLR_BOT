#include "incl/Broker.h"
#include "incl/DEBUG.h"

void run_FireBot(broker* Bro,  unsigned short port);

int main()
{
	broker* Bro = new broker;

	DEBUG* B = new DEBUG("BOT_LOG", true, true, false);
	B->teachB();

	B->StatusNew("", "Hello");

	run_FireBot(Bro,6370);

	return 0;
}
