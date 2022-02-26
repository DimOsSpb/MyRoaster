#include <parser.h>

Parser::Parser(String *source){
    this->source = source;
}
int Parser::getInt(String fieldName){
        int _indx1, _len, _res = 0;
        String _tmp;
        
        if ((*source).length() > 0)
        {
            _len = fieldName.length();
            _indx1 = (*source).indexOf("\""+fieldName+"\":");

            if( _indx1 >= 0 ){
                _tmp = this->getValue(_indx1+_len+3); 
                return _tmp.toInt();   
            }
        }
        return _res;
};
String Parser::getString(String fieldName){
        int _indx1, _len;
        String _res = "";
        String _tmp;
        
        if ((*source).length() > 0)
        {
            _len = fieldName.length();
            _indx1 = (*source).indexOf("\""+fieldName+"\":\"");

            if( _indx1 >= 0 ){
                _tmp = this->getValue(_indx1+_len+4); 
                return _tmp;   
            }
        }
        return _res;
};
String Parser::getValue(uint16_t startPos){
    String _res = "", _tmp;
    const String stopChar = "{},\"";
    for(uint32_t i=startPos; i < (*source).length() - startPos; i++){
        _tmp = (*source).charAt(i);
        if(stopChar.indexOf(_tmp) >=0 )
            break;
        _res += _tmp;    
    }
    return _res;
};


