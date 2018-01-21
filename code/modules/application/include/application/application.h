//---------------------------------------------------------------------------

#pragma once

#ifndef APPLICATION_H
#define APPLICATION_H

//---------------------------------------------------------------------------

#ifdef _MSC_VER
#include <windows.h>
#endif

#include <core/addition/singleton.h>
#include <container/array_list.h>
#include <function/method.h>
#include <core/String.h>

#include "thread_loop.h"

//---------------------------------------------------------------------------

api(export)
int wmain(int argc, wchar_t * argv[]);

namespace asd
{
    class application;
    
    struct entrance;
    
    using application_arguments_t = data<wchar_t *>;
    using entrance_function_t = int (*)();
    using exit_function_t = void (*)();
    
    template
    struct api(application) singleton<application>;
    
    class application : public singleton<application>
    {
        friend int::wmain(int argc, wchar_t * argv[]);
        
        friend struct singleton<application>;
        friend struct entrance;
    
    public:
        #ifdef WIN32
        static api(application) HINSTANCE getWindowsInstance();
        #endif
        
        static api(application) const wstring & root_path();
        static const array_list<wstring> api(application) & arguments();
        
        static api(application) int show_command();
    
    private:
        static api(application) int main(int argc, wchar_t ** argv);
        
        application() {}
        
        void api(application) pause();
        
        static api(application) int load();

#ifdef WIN32
        static api(application) wstring getExecutionPath(HINSTANCE hInstance);

        HINSTANCE hInstance = nullptr;
#endif
        
        wstring _root_path;
        array_list<wstring> args;
        
        function<int()> entrance = nullptr;
        int _show_command;
    };
    
    struct entrance
    {
        entrance(entrance_function_t func) {
            if (application::instance().entrance != nullptr) {
                throw Exception("Can't set multiple entrances!");
            }
            
            application::instance().entrance = func;
        }
        
        entrance(exit_function_t func) {
            if (application::instance().entrance != nullptr) {
                throw Exception("Can't set multiple entrances!");
            }
            
            application::instance().entrance = [func]() {
                func();
                
                #ifdef ASD_CONSOLE
                application::instance().pause();
                #endif
                return 0;
            };
        }
    };
}

//---------------------------------------------------------------------------
#endif
