#ifndef AD_EVENT_TESTING_H
#define AD_EVENT_TESTING_H

#include "Event/AdEventObserver.h"

class AdEventTesting{
public:
    AdEventTesting();
    ~AdEventTesting();
private:
    void TestMemberFunc(const ade::AdMouseButtonReleaseEvent &event);

    std::shared_ptr<ade::AdEventObserver> mObserver;
};

#endif