#pragma once
#include <Nextion.h>
#include <Roaster.h>

class Dispatcher{
    
    public:
        Dispatcher();
        void init(Roaster *roaster);
        bool updateStates();
        //void showStates();
    private:
        Nextion _nextion;
        Roaster *_roaster;
        char _buf[100];
     
};

