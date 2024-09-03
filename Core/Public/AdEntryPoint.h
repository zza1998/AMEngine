#ifndef AD_ENTRY_POINT_H
#define AD_ENTRY_POINT_H

#include "AdEngine.h"
#include "AdApplication.h"

extern ade::AdApplication *CreateApplicationEntryPoint();

#if AD_ENGINE_PLATFORM_WIN32 || AD_ENGINE_PLATFORM_MACOS || AD_ENGINE_PLATFORM_LINUX

int main(int argc, char *argv[]){

    std::cout<< "Adiosy Engine starting..." << std::endl;

    ade::AdApplication *app = CreateApplicationEntryPoint();
    // start
    app->Start(argc, argv);
    // main loop
    app->MainLoop();
    // stop
    app->Stop();
    // delete
    delete app;

    return EXIT_SUCCESS;
}

#endif

#endif