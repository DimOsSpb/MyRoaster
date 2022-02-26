#pragma once
#include <Arduino.h>

class Parser{
    
    public:
        Parser(String *source);
        String getString(String fieldName);
        int getInt(String fieldName);

    private:

        String *source;
        String getValue(uint16_t startPos);

};