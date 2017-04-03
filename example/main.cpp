
#include "Beebox.h"
#include "BeeboxServer.h"
#include "Utils.h"

int main(int argc, char* argv[])
{
	CBeebox beebox;
	beebox.run();
	beebox.shot();

	CUtils::sleep(1000);

	beebox.shot();

	beebox.beWaiting();

	CBeeboxServer beeboxServer;
	beeboxServer.run();

	printf("exit...\n");
	return 0;
}