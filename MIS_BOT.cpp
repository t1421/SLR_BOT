#include <iostream>
#include "incl/Broker.h"
#include "incl/DEBUG.h"
#include "incl/CardBaseSMJ.h"


void run_FireBot(broker* Bro,  unsigned short port);

int main()
{
	broker* Bro = new broker;

	DEBUG* B = new DEBUG("BOT_LOG", true, true, false);
	B->teachB();
	B->StatusNew("", "Hello there :-)");

	CardBaseSMJ* J = new CardBaseSMJ();
	J->teachJ();
	J->DownloadSMJ();
	if (!J->readJSON())
	{
		B->StatusE("E", "Main", "No JSON - i cant work like this :-(");
		return -1;
	}

	for (const auto& item : J->mainJSON["data"])
	{
		if (item["_id"] == "09")
		{
			for (auto it = item.begin(); it != item.end(); ++it) {
				if (it.key() != "_id") {
					std::cout << "Schlüssel: " << it.key() << ", Wert: " << it.value() << std::endl;
				}
			}
		}
	}

	run_FireBot(Bro,6370);

	return 0;
}
