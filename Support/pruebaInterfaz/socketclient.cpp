#include "../../classes/socket/socketclient.h"

int main(int argc, char const* argv[])
{
	socketclient client;

	client.init();
	client.clear();
	client.paintLine(100, 25, 176, 255, 100, 100, 900, 700);
	client.paintLine(255, 0, 0, 255, 100, 100, 100, 900);
	client.paintDot(200, 0, 15, 200, 500, 600, 15);
	client.paintDot(182, 182, 182, 255, 412, 517, 15);
	client.paintDot(255, 0, 0, 255, 25, 25, 15);
	client.closeConnection();
	return 0;
}
