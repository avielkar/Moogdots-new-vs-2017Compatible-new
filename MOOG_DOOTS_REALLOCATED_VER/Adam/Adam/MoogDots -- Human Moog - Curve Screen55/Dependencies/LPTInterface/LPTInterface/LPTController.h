#ifndef LPTCONTROLLER
#define LPTCONTROLLER
#include <stdio.h>
#include <conio.h>
#include <windows.h>


namespace LPTInterface
{
	class LPTCOntroller
	{
	private:
		typedef short(__stdcall*inpfuncPtr)(short portaddr);
		typedef void(__stdcall*oupfuncPtr)(short portaddr, short datum);

		HINSTANCE m_hLib;
		inpfuncPtr m_inp32;
		oupfuncPtr m_oup32;
	public:

		LPTCOntroller()
		{
		}

		int Connect()
		{
			/* Load the library */
			m_hLib = LoadLibrary("inpout32.dll");

			if (m_hLib == NULL) {
				printf("LoadLibrary Failed.\n");
				return -1;
			}

			/* get the address of the function */

			m_inp32 = (inpfuncPtr)GetProcAddress(m_hLib, "Inp32");

			if (m_inp32 == NULL) {
				printf("GetProcAddress for Inp32 Failed.\n");
				return -1;
			}


			m_oup32 = (oupfuncPtr)GetProcAddress(m_hLib, "Out32");

			if (m_oup32 == NULL) {
				printf("GetProcAddress for Oup32 Failed.\n");
				return -1;
			}

			return 0;
		}

		void Write(short portAdress , short data)
		{
			m_oup32(portAdress , data);
		}

		short Read(short portAdress)
		{
			return m_inp32(portAdress);
		}
	};
}
#endif