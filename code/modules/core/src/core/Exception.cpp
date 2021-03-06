//---------------------------------------------------------------------------

#include <core/Exception.h>

#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <iostream>

//---------------------------------------------------------------------------

#define EXC_BUFFER_SIZE 256

#ifndef _MSC_VER
	char * strerror_s(char * s, size_t size, error_t e) {
		return strerror_r(e, s, size);
	}
#endif

namespace asd
{
	void viewException(const exception & ex)
	{
		using namespace std;
		//MessageBoxA(0, ex.what(), "Exception", MB_ICONERROR);
		cout << "EXCEPTION: " << ex.what() << endl;
	}

	String stdError()
	{
		char buffer[EXC_BUFFER_SIZE];
		::strerror_s(buffer, EXC_BUFFER_SIZE, errno);

		return buffer;
	}

	String stdError(errno_t err)
	{
		char buffer[EXC_BUFFER_SIZE];
		::strerror_s(buffer, EXC_BUFFER_SIZE, err);

		return buffer;
	}
}
