#include "Endscene.h"
#include <Windows.h>
#include <chrono>

#include "Memory.h"
#include "Hooks.h"

typedef std::chrono::high_resolution_clock Clock;

namespace Memory
{
	CEndscene Endscene;

	// Pointer to code cave.
	/*DWORD functionAddress		= NULL;

	// Pointer to return of function.
	DWORD functionReturn		= NULL;

	// Code cave location.
	DWORD functionSpace			= NULL;

	DWORD lastExecutionTime		= NULL;
	DWORD address				= NULL;
	DWORD waitTime				= 1000;*/


	// *** Statics
	DWORD CEndscene::GetPtrAddress()
	{
		DWORD pEnd = NULL;
		DWORD pScene = NULL;
		DWORD pDevice = NULL;
		DWORD pEndScene = NULL;

		rpm(Offsets::pDevicePtr_1, &pDevice, sizeof(pDevice));
		rpm(pDevice + Offsets::pDevicePtr_2, &pEnd, sizeof(pEnd));

		rpm((pEnd), &pScene, sizeof(pScene));

		if (pScene == NULL) return NULL;
		return pScene + Offsets::oEndScene;
	}

	// Returns Endscene address.
	DWORD CEndscene::Get()
	{
		DWORD pEndScene = NULL;

		rpm(CEndscene::GetPtrAddress(), &pEndScene, sizeof(pEndScene));

		return pEndScene;
	}

	bool CEndscene::IsHooked()
	{
		if (!Memory::cached) Memory::CacheModules();
		return !Memory::AddressInD3D9(CEndscene::Get());
	}

	// *** End of statics

	bool CEndscene::IsHandlerStarted()
	{
		//vlog("address: 0x%x, space: 0x%x, return: 0x%x", functionAddress, functionSpace, functionReturn);
		return this->functionAddress != NULL && this->functionReturn != NULL && this->functionSpace != NULL;
	}

	bool CEndscene::CreateFunction()
	{
		if (this->IsHandlerStarted()) return TRUE;

		this->functionAddress = (DWORD)VirtualAllocEx(WoW::handle, NULL, sizeof(DWORD), MEM_COMMIT, PAGE_EXECUTE_READWRITE);
		this->functionReturn = (DWORD)VirtualAllocEx(WoW::handle, NULL, sizeof(DWORD), MEM_COMMIT, PAGE_EXECUTE_READWRITE);
		this->functionSpace = (DWORD)VirtualAllocEx(WoW::handle, NULL, FUNCTIONSPACE_MAX, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

		if (this->functionAddress == NULL)
		{
			velog("functionAddress allocation error: %d", GetLastError());
			return false;
		}

		if (this->functionReturn == NULL)
		{
			velog("functionReturn allocation error: %d", GetLastError());
			return false;
		}

		if (this->functionSpace == NULL)
		{
			velog("functionSpace allocation error: %d", GetLastError());
			return false;
		}

		return true;
	}

	bool CEndscene::CleanupHandler()
	{
		if (this->functionAddress != NULL)
			VirtualFreeEx(WoW::handle, (LPVOID)this->functionAddress, sizeof(DWORD), MEM_RELEASE);
	
		if (this->functionReturn != NULL)
			VirtualFreeEx(WoW::handle, (LPVOID)this->functionReturn, sizeof(DWORD), MEM_RELEASE);

		if (this->functionSpace != NULL)
			VirtualFreeEx(WoW::handle, (LPVOID)this->functionSpace, 1024, MEM_RELEASE);

		this->functionAddress = NULL;
		this->functionReturn = NULL;
		this->functionSpace = NULL;
		return true;
	}

	bool CEndscene::CanExecute()
	{
		return WoW::isRunning() && this->IsHandlerStarted() && this->IsHooked() && this->ReadFunctionAddress() == NULL;
	}

	bool CEndscene::ClearFunctionSpace()
	{
		BYTE buffer[FUNCTIONSPACE_MAX];

		if (this->functionSpace == NULL) return false;

		memset(&buffer, 0, sizeof(buffer));
		return wpm(this->functionSpace, &buffer, sizeof(buffer));
	}

	bool CEndscene::Fix()
	{
		DWORD endscene = NULL;
		HMODULE d3d9 = NULL;

		if (!this->IsHooked())
		{
			vlog("Endscene seems to be running fine...");
			return true;
		}

		d3d9 = LoadLibraryEx("D3D9.dll", NULL, DONT_RESOLVE_DLL_REFERENCES);

		if (!d3d9) return false;
		endscene = (DWORD)GetProcAddress(d3d9, "Direct3DCreate9Ex") + 0x13BDE;

		if (!endscene) return false;
		vlog("Address: 0x%x", endscene);

		FreeLibrary(d3d9);
		return wpm(this->GetPtrAddress(), &endscene, sizeof(endscene));
	}

	DWORD CEndscene::getFunctionAddress()
	{
		return this->functionReturn;
	}
		
	DWORD CEndscene::getFunctionSpace()
	{
		return this->functionSpace;
	}

	DWORD CEndscene::getFunctionReturn()
	{
		return this->functionReturn;
	}

	DWORD CEndscene::getLastRuntime()
	{
		return this->lastRunTime;
	}

	DWORD CEndscene::getAddress()
	{
		return this->address;
	}

	DWORD CEndscene::ReadFunctionReturn()
	{
		DWORD buffer = NULL;

		if (this->functionReturn == NULL) return NULL;

		rpm(this->functionReturn, &buffer, sizeof(DWORD));
		return buffer;
	}

	DWORD CEndscene::ReadFunctionAddress()
	{
		DWORD buffer = NULL;

		if (this->functionAddress == NULL) return NULL;

		rpm(this->functionAddress, &buffer, sizeof(DWORD));
		return buffer;
	}

	// Returns time it took to execute the stub.
	// Wait until we can execute again.
	// Timeout by default is 1000 miliseconds.
	DWORD CEndscene::Wait()
	{
		using namespace std::chrono;

		Clock timer;
		auto start = timer.now();

		if (this->waitTime == 0) return 1;

		while (!this->CanExecute() && WoW::isRunning())
		{
			if (duration_cast<milliseconds>(timer.now() - start).count() > this->waitTime)
			{
				log("EndScene callback timed out");
				LogFile("WaitOnEndScene: EndScene callback timed out");
				break;
			}

			Sleep(1);
		}

		this->lastRunTime = duration_cast<milliseconds>(timer.now() - start).count();
		return (lastRunTime) ? lastRunTime : 1;
	}

	// EndsceneAction target(0xDA33);
	// puts this action into queue
	// Next time the main thread is free, next action will execute
	// Need to figure out a queue handler (another thread? in WoW process using asm?).
	// Probably mutex on global queue.
	void CEndscene::Queue(Stub &stub)
	{
		stub.setAddress(this->functionSpace);
		this->_stubQueue.push(stub);
	}

	// Need to figure out how to have only one instance running.
	// Mutex, lock, single thread? wow info loop?
	DWORD CEndscene::ExecuteQueue()
	{
		DWORD time = 0;

		while (this->_stubQueue.size() > 0)
		{
			Stub &current = this->_stubQueue.front();
			current.inject();

			time += this->Execute(current);
			this->_stubQueue.pop();
		}
	}

	DWORD CEndscene::Execute(Stub &stub)
	{
		return this->Execute(stub.getAddress());
	}

	DWORD CEndscene::Execute(DWORD stubAddress)
	{
		DWORD buffer = NULL;

		// If there are conditions that might crash WoW or cause undefined behaviour, then return false.
		if (!this->CanExecute()) return CEndscene::EXECUTE_ERROR;
		if (!WoW::isRunning()) return CEndscene::EXECUTE_ERROR;
		if (!Memory::CanRead(stubAddress)) 
		{
			velog("Endscene::Execute Unable to read stub memory");
			return CEndscene::EXECUTE_ERROR;
		}

		SuspendWoW();
		
		// Change empty function in Endscene to current stub.
		if (!wpm(this->functionReturn, &buffer, sizeof(buffer)))
		{
			velog("Endscene::Execute WPM functionReturn");
			return CEndscene::EXECUTE_ERROR;
		}

		if (!wpm(this->functionAddress, &stubAddress, sizeof(stubAddress)))
		{
			velog("Endscene::Execute WPM functionAddress");
			return CEndscene::EXECUTE_ERROR;
		}

		ResumeWoW();

		// Wait for our EndScene overhead to execute stub.
		return this->Wait();
	}

	void CEndscene::setWait(DWORD mili)
	{
		this->waitTime = mili;
	}

	bool CEndscene::StartMainThreadHandler()
	{
		DWORD oldProtect = NULL;
		DWORD jumpDistance = NULL;
		DWORD pointerAddress = NULL;

		BYTE check;

		if (this->IsHooked()) return FALSE;
		if (!this->IsHandlerStarted()) return FALSE;

		/* EndScene Stub:
			if (g_dwEndScenefunctionToBeCalled != NULL)
			{
				call g_dwEndScenefunctionToBeCalled;
				g_dwEndScenefunctionToBeCalled = NULL;
				mov [g_dwEndScenefunctionReturn], eax
			}
			else
			{
				OriginalCode();
				Jmp(EndScene());
			}
		*/

		// Our function delegator.
		BYTE stub[] = {
			0x60,														// pushad
			0x9C,														// pushfd
			0x83, 0x3D, 0x00, 0x00, 0x00, 0x00, 0x00,					// cmp dword ptr [g_dwEndScenefunctionToBeCalled], 00
			0x74, 0x17,													// je &popfd
			0xA1, 0x00, 0x00, 0x00, 0x00,								// mov eax, [g_dwEndScenefunctionToBeCalled]
			0xFF, 0xD0,													// call eax
			0x89, 0x05, 0x00, 0x00, 0x00, 0x00,							// mov [g_dwEndScenefunctionReturn], eax
			0xC7, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// mov [g_dwEndScenefunctionToBeCalled], 00000000
			0x9D,														// popfd
			0x61,														// popad

			0xE9, 0x90, 0x90, 0x90, 0x90								// jmp oEndscene
		};
	
		memcpy(&stub[4],  &this->functionAddress, sizeof(this->functionAddress));
		memcpy(&stub[12], &this->functionAddress, sizeof(this->functionAddress));
		memcpy(&stub[20], &this->functionReturn,  sizeof(this->functionReturn));
		memcpy(&stub[26], &this->functionAddress, sizeof(this->functionAddress));

		vflog("functionAddress: 0x%x", this->functionAddress);
		vflog("functionReturn: 0x%x", this->functionReturn);

		// Allocate 1KB of new memory.
		wEndScene.Cave = (DWORD)VirtualAllocEx(WoW::handle, NULL, 1024, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

		if (wEndScene.Cave == NULL)
		{
			velog("StartMainThreadHandler: Error alocating cave");
			return false;
		}

		// Set old endscene hook.
		wEndScene.HookedFunction = this->getAddress();
		wEndScene.CaveSize = sizeof(stub);
	
		// Calculate jump distance to endscene from the code cave.
		jumpDistance = wEndScene.HookedFunction - (wEndScene.Cave + sizeof(stub));
		memcpy(&stub[37],  &jumpDistance, sizeof(jumpDistance));

		// Write the stub to the code cave.
		if (!wpm(wEndScene.Cave, &stub, sizeof(stub)))
		{
			velog("StartMainThreadHandler: WPM");
			return false;
		}

		// Read newly stubbed cave.
		if (!rpm(wEndScene.Cave, &check, sizeof(check)))
		{
			velog("StartMainThreadHandler: RPM");
			return false;
		}

		// If the data does not correspond to what we wrote, there was an error 
		// writing the stub. Cleanup and retry.
		if (check != 0x60)
		{
			velog("StartMainThreadHandler: Invalid cave stub. Byte check: 0x%x", check);

			// Free invalid cave.
			VirtualFreeEx(WoW::handle, &wEndScene.Cave, 1024, MEM_RELEASE);
			return false;
		}

		// Get WoW's pointer to endscene.
		pointerAddress = this->GetPtrAddress();
	
		// Ensure we will be able to write to this address.
		if (!VirtualProtectEx(WoW::handle, (LPVOID)pointerAddress, sizeof(DWORD), PAGE_READWRITE, &oldProtect) || pointerAddress == NULL)
		{
			velog("VirtualProtect error");
			return false;
		}

		SuspendWoW();

		// Write the VTable hook to redirect any endscene calls to our cave.
		if (!wpm(pointerAddress, &wEndScene.Cave, sizeof(wEndScene.Cave)))
		{
			velog("WPM error");
			ResumeWoW();

			return false;
		}

		// Update new endscene address.
		this->update();

		// Old jmp patch hook.
		//HookEndScene();
		//ChangeHookFunction(g_dwEndScene, wEndScene.Cave, stub, sizeof(stub));

		ResumeWoW();

		// Restore the old protection.
		if (!VirtualProtectEx(WoW::handle, (LPVOID)pointerAddress, sizeof(DWORD), oldProtect, &oldProtect))
		{
			velog("VirtualProtect error");
			return false;
		}

		vflog("Cave: 0x%x", wEndScene.Cave);
		vflog("GetEndScene(): 0x%x", this->getAddress());
		vlog("Endscene hooked: 0x%x (new) 0x%x (old)", this->getAddress(), wEndScene.HookedFunction);
		return TRUE;
	}

	bool CEndscene::Start()
	{
		//return true;
		if (this->IsHooked()) return true;
		if (!this->CreateFunction()) return false;

		return this->StartMainThreadHandler();
	}

	void CEndscene::update()
	{
		this->address = this->Get();
	}

	void CEndscene::reset(DWORD wait)
	{
		this->address = this->Get();
		this->waitTime = wait;
	}

	CEndscene::CEndscene(DWORD wait)
	{
		this->functionAddress = NULL;
		this->functionReturn = NULL;
		this->functionSpace = NULL;
		this->lastRunTime = NULL;
		this->reset(wait);
	}

	CEndscene::CEndscene()
	{
		this->functionAddress = NULL;
		this->functionReturn = NULL;
		this->functionSpace = NULL;
		this->lastRunTime = NULL;
		this->reset(1000);
	}

	CEndscene::~CEndscene()
	{}
};