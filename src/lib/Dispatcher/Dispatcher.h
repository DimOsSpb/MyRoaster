#pragma once
#include <Nextion.h>
#include <Roaster.h>




class Dispatcher{
    
    public:
        Dispatcher();
        void init();
        void refreshStates();
        void listEvents();
    private:
        Nextion _nextion;
        Roaster _roaster;
        char _buf[100];
     
};

