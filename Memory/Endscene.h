#pragma once

#include <Windows.h>
#include <queue>

#include <Main\Structures.h>
#include <Memory\Memory.h>

// The max size of the function space.
#define FUNCTIONSPACE_MAX 10000

namespace Memory
{	
	class CEndscene
	{
	private:
		DWORD functionAddress;
		DWORD functionReturn;
		DWORD functionSpace;

		DWORD lastRunTime;
		DWORD waitTime;
		DWORD address;

		std::queue<Stub> _stubQueue;

	public:
		enum eReturn
		{
			EXECUTE_ERROR = -1,
		};

		// Functions
		DWORD Execute(DWORD stubAddress);
		DWORD Execute(Stub &stub);
		DWORD ExecuteQueue();
		DWORD Wait();

		void setWait(DWORD mili);

		bool Fix();
		void Queue(Stub &stub);

		// Initialize.
		bool CanExecute();
		bool CreateFunction();
		bool CleanupHandler();
		bool ClearFunctionSpace();

		bool Start();
		bool StartMainThreadHandler();

		// Hook/Handler state.
		bool IsHandlerStarted();

		// Address information.
		DWORD ReadFunctionAddress();
		DWORD ReadFunctionReturn();

		void reset(DWORD wait);
		void update();

		DWORD getFunctionAddress();
		DWORD getFunctionReturn();
		DWORD getFunctionSpace();
		DWORD getLastRuntime();
		DWORD getAddress();

		// Statics
		static DWORD GetPtrAddress();
		static DWORD Get();

		static bool IsHooked();

		CEndscene(DWORD wait);
		CEndscene();
		~CEndscene();
	};

	extern CEndscene Endscene;
};

