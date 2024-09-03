#include "AdEventTesting.h"
#include "AdLog.h"

#define BIND_EVENT_FN(fn)           [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }
#define REG_EVENT(type) mObserver->OnEvent<type>([](const type &event){ LOG_T(event.ToString()); });

static void TestStaticFunc(const ade::AdMouseButtonPressEvent &event){
   // LOG_T("event.ToString()");
    
}

void AdEventTesting::TestMemberFunc(const ade::AdMouseButtonReleaseEvent &event) {
   // LOG_T(event.ToString());
}

AdEventTesting::AdEventTesting() {
    mObserver = std::make_shared<ade::AdEventObserver>();
    //mObserver->OnEvent<ade::AdFrameBufferResizeEvent>([](const ade::AdFrameBufferResizeEvent &event){ LOG_T(event.ToString()); });
    mObserver->OnEvent<ade::AdMouseButtonPressEvent>(TestStaticFunc);
    mObserver->OnEvent<ade::AdMouseButtonReleaseEvent>(BIND_EVENT_FN(TestMemberFunc));
    //REG_EVENT(ade::AdWindowFocusEvent);
    //REG_EVENT(ade::AdWindowLostFocusEvent);
    //REG_EVENT(ade::AdWindowMovedEvent);
    //REG_EVENT(ade::AdWindowCloseEvent);
    //REG_EVENT(ade::AdKeyPressEvent);
    //REG_EVENT(ade::AdKeyReleaseEvent);
    //REG_EVENT(ade::AdMouseMovedEvent);
    //REG_EVENT(ade::AdMouseScrollEvent);
}

AdEventTesting::~AdEventTesting() {
    mObserver.reset();
}
