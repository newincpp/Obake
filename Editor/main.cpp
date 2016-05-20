#include "Core.hh"
#include "TestEventSystem1.hh"
#include "TestEventSystem2.hh"

int main()
{
	Obake::Core c;
	System::TestEventSystem1 system1(&c);
	System::TestEventSystem2 system2(&c);
	
	//c.registerSystem(&system1);
	//c.registerSystem(&system2);


	int runRet = c.run();
	system("Pause");
	return runRet;
}
