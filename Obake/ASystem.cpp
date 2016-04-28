#include "ASystem.hh"

Obake::ASystem::ASystem(const EventList& eventListRef_) : _events(eventListRef_) {
    _executionQueue.reserve(32);
}