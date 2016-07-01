#include <thread>
#include <array>
#include <functional>
#include <vector>
#include <iostream>

#include "Core.hh"
#include "ASystem.hh"
#include "Event.hh"


class DummySystem : public Obake::ASystem {
public:
    void printA();
    void printB();
    void printC();
    void printD();
    void printE();
    void throwEvent();
    DummySystem();
};
DummySystem::DummySystem() : ASystem() {
	//auto x = std::bind(&std::remove_pointer<decltype(this)>::type::printA, this);
	OBAKE_ADD(printA);
	OBAKE_ADD(printB);
	OBAKE_ADD(printC);
	OBAKE_ADD(printD);
	OBAKE_ADD(printE);
	OBAKE_ADD(printE);
}
void DummySystem::printA() { std::cout << "A" << std::endl; }
void DummySystem::printB() { std::cout << "B" << std::endl; }
void DummySystem::printC() { std::cout << "C" << std::endl; }
void DummySystem::printD() { std::cout << "D" << std::endl; }
void DummySystem::printE() { std::cout << "E" << std::endl; }
void DummySystem::throwEvent() { }

int main(int, const char*[]) {
    Obake::Core c;
    DummySystem d;

    //Obake::Event<&DummySystem, decltype(&DummySystem::printA)> e;
    //e.bind(&DummySystem::printA, &d);
    c.registerSystem(&d);

    int runRet = c.run();
    //system("pause");
    return runRet;
}
