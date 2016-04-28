#include <thread>
#include <array>
#include <functional>
#include <vector>
#include <iostream>

#include "Core.hh"
#include "ASystem.hh"


class DummySystem : public Obake::ASystem {
public:
    void printA();
    void printB();
    void printC();
    void printD();
    void printE();
    void throwEvent();
    DummySystem(const Obake::EventList& evRef_);
};
DummySystem::DummySystem(const Obake::EventList& evRef_) : ASystem(evRef_) {
    ASystem::_executionQueue.push_back(std::bind(&DummySystem::printA, this));
    ASystem::_executionQueue.push_back(std::bind(&DummySystem::printB, this));
    ASystem::_executionQueue.push_back(std::bind(&DummySystem::printC, this));
    ASystem::_executionQueue.push_back(std::bind(&DummySystem::printD, this));
    ASystem::_executionQueue.push_back(std::bind(&DummySystem::printE, this));
}
void DummySystem::printA() { std::cout << "A" << std::endl; }
void DummySystem::printB() { std::cout << "B" << std::endl; }
void DummySystem::printC() { std::cout << "C" << std::endl; }
void DummySystem::printD() { std::cout << "D" << std::endl; }
void DummySystem::printE() { std::cout << "E" << std::endl; }
void DummySystem::throwEvent() { }

int main(int argc, const char * argv[]) {
    Obake::Core c;
    DummySystem d(c.getEventListRef());
    c.registerSystem(&d);
    
   return c.run();
}
