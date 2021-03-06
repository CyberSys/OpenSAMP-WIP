#include "../main.h"

extern "C" RakServerInterface* PluginGetRakServer()
{
	if (__NetGame != NULL)
		return __NetGame->rakServerInterface;
	else
		return NULL;
}

extern "C" CNetGame* PluginGetNetGame()
{
	return __NetGame;
}

extern "C" CConsole* PluginGetConsole()
{
	return __Console;
}

extern "C" bool PluginUnloadFilterScript(char* pFileName)
{
	if (__NetGame != NULL)
		return __NetGame->filterscriptsManager->UnloadFilterScript(pFileName);
	else
		return false;
}

extern "C" bool PluginLoadFilterScriptFromMemory(char* pFileName, char* pFileData)
{
	if (__NetGame != NULL)
		return 1;//__NetGame->filterscriptsManager->LoadFilterScriptFromMemory(pFileName, pFileData);
	else
		return false;
}

extern "C" int PluginCallPublicFS(char *szFunctionName)
{
	if (__NetGame != NULL)
		return __NetGame->filterscriptsManager->CallPublic(szFunctionName);
	else
		return 0;
}

extern "C" int PluginCallPublicGM(char *szFunctionName)
{
	if (__NetGame != NULL && __NetGame->gamemodeManager)
		return __NetGame->gamemodeManager->CallPublic(szFunctionName);
	else
		return 0;
}

CPlugins::CPlugins()
{
#if defined AMX_ALIGN || defined AMX_INIT
	amxExports[PLUGIN_AMX_EXPORT_Align16] = amx_Align16;
	amxExports[PLUGIN_AMX_EXPORT_Align32] = amx_Align32;
#if defined _I64_MAX || defined HAVE_I64
	amxExports[PLUGIN_AMX_EXPORT_Align64] = amx_Align64;
#endif
#endif
	amxExports[PLUGIN_AMX_EXPORT_Allot] = amx_Allot;
	amxExports[PLUGIN_AMX_EXPORT_Callback] = amx_Callback;
	amxExports[PLUGIN_AMX_EXPORT_Cleanup] = amx_Cleanup;
	amxExports[PLUGIN_AMX_EXPORT_Clone] = amx_Clone;
	amxExports[PLUGIN_AMX_EXPORT_Exec] = amx_Exec;
	amxExports[PLUGIN_AMX_EXPORT_FindNative] = amx_FindNative;
	amxExports[PLUGIN_AMX_EXPORT_FindPublic] = amx_FindPublic;
	amxExports[PLUGIN_AMX_EXPORT_FindPubVar] = amx_FindPubVar;
	amxExports[PLUGIN_AMX_EXPORT_FindTagId] = amx_FindTagId;
	amxExports[PLUGIN_AMX_EXPORT_Flags] = amx_Flags;
	amxExports[PLUGIN_AMX_EXPORT_GetAddr] = amx_GetAddr;
	amxExports[PLUGIN_AMX_EXPORT_GetNative] = amx_GetNative;
	amxExports[PLUGIN_AMX_EXPORT_GetPublic] = amx_GetPublic;
	amxExports[PLUGIN_AMX_EXPORT_GetPubVar] = amx_GetPubVar;
	amxExports[PLUGIN_AMX_EXPORT_GetString] = amx_GetString;
	amxExports[PLUGIN_AMX_EXPORT_GetTag] = amx_GetTag;
	amxExports[PLUGIN_AMX_EXPORT_GetUserData] = amx_GetUserData;
	amxExports[PLUGIN_AMX_EXPORT_Init] = amx_Init;
	amxExports[PLUGIN_AMX_EXPORT_InitJIT] = amx_InitJIT;
	amxExports[PLUGIN_AMX_EXPORT_MemInfo] = amx_MemInfo;
	amxExports[PLUGIN_AMX_EXPORT_NameLength] = amx_NameLength;
	amxExports[PLUGIN_AMX_EXPORT_NativeInfo] = amx_NativeInfo;
	amxExports[PLUGIN_AMX_EXPORT_NumNatives] = amx_NumNatives;
	amxExports[PLUGIN_AMX_EXPORT_NumPublics] = amx_NumPublics;
	amxExports[PLUGIN_AMX_EXPORT_NumPubVars] = amx_NumPubVars;
	amxExports[PLUGIN_AMX_EXPORT_NumTags] = amx_NumTags;
	amxExports[PLUGIN_AMX_EXPORT_Push] = amx_Push;
	amxExports[PLUGIN_AMX_EXPORT_PushArray] = amx_PushArray;
	amxExports[PLUGIN_AMX_EXPORT_PushString] = amx_PushString;
	amxExports[PLUGIN_AMX_EXPORT_RaiseError] = amx_RaiseError;
	amxExports[PLUGIN_AMX_EXPORT_Register] = amx_Register;
	amxExports[PLUGIN_AMX_EXPORT_Release] = amx_Release;
	amxExports[PLUGIN_AMX_EXPORT_SetCallback] = amx_SetCallback;
	amxExports[PLUGIN_AMX_EXPORT_SetDebugHook] = amx_SetDebugHook;
	amxExports[PLUGIN_AMX_EXPORT_SetString] = amx_SetString;
	amxExports[PLUGIN_AMX_EXPORT_SetUserData] = amx_SetUserData;
	amxExports[PLUGIN_AMX_EXPORT_StrLen] = amx_StrLen;

	pPluginData[PLUGIN_DATA_LOGPRINTF] = logprintf;
	
	pPluginData[PLUGIN_DATA_AMX_EXPORTS] = amxExports;
	pPluginData[PLUGIN_DATA_CALLPUBLIC_FS] = PluginCallPublicFS;
	pPluginData[PLUGIN_DATA_CALLPUBLIC_GM] = PluginCallPublicGM;

	pPluginData[PLUGIN_DATA_NETGAME] = PluginGetNetGame;
	pPluginData[PLUGIN_DATA_CONSOLE] = PluginGetConsole;
	pPluginData[PLUGIN_DATA_RAKSERVER] = PluginGetRakServer;
	pPluginData[PLUGIN_DATA_LOADFSCRIPT] = PluginLoadFilterScriptFromMemory;
	pPluginData[PLUGIN_DATA_UNLOADFSCRIPT] = PluginUnloadFilterScript;
}

CPlugins::~CPlugins()
{
	std::vector<pluginData*>::iterator itor;

	for(itor = pluginsVector.begin(); itor != pluginsVector.end(); itor++) 
	{
		pluginData* pPluginData = *itor;

		if (pPluginData->Unload)
			pPluginData->Unload();

		PLUGIN_UNLOAD(pPluginData->hModule);
		delete pPluginData;
	}
}

BOOL CPlugins::LoadPlugin(char* szPluginPath) 
{
	pluginData* pPluginData;
	pPluginData = new pluginData();
	
	pPluginData->hModule = PLUGIN_LOAD(szPluginPath);
	if (pPluginData->hModule == NULL) 
	{
		delete pPluginData;
		return FALSE;
	}

	pPluginData->Load = (ServerPluginLoad_t)PLUGIN_GETFUNCTION(pPluginData->hModule, "Load");
	pPluginData->Unload = (ServerPluginUnload_t)PLUGIN_GETFUNCTION(pPluginData->hModule, "Unload");
	pPluginData->Supports = (ServerPluginSupports_t)PLUGIN_GETFUNCTION(pPluginData->hModule, "Supports");

	if (pPluginData->Load == NULL || pPluginData->Supports == NULL) 
	{
		logprintf("  Plugin does not conform to architecture.");
		PLUGIN_UNLOAD(pPluginData->hModule);
		delete pPluginData;
		return FALSE;
	}

	pPluginData->dwSupportFlags = (SUPPORTS_FLAGS)pPluginData->Supports();

	if ((pPluginData->dwSupportFlags & SUPPORTS_VERSION_MASK) > SUPPORTS_VERSION) 
	{
		// Unsupported version, sorry!
		logprintf("  Unsupported version - This plugin requires version %x.", (pPluginData->dwSupportFlags & SUPPORTS_VERSION_MASK));
		PLUGIN_UNLOAD(pPluginData->hModule);
		delete pPluginData;
		return FALSE;
	}

	if ((pPluginData->dwSupportFlags & SUPPORTS_AMX_NATIVES) != 0) 
	{
		pPluginData->AmxLoad = (ServerPluginAmxLoad_t)PLUGIN_GETFUNCTION(pPluginData->hModule, "AmxLoad");
		pPluginData->AmxUnload = (ServerPluginAmxUnload_t)PLUGIN_GETFUNCTION(pPluginData->hModule, "AmxUnload");
	} 
	else 
	{
		pPluginData->AmxLoad = NULL;
		pPluginData->AmxUnload = NULL;
	}

	if ((pPluginData->dwSupportFlags & SUPPORTS_PROCESS_TICK) != 0)
	{
		pPluginData->ProcessTick = (ServerPluginProcessTick_t)PLUGIN_GETFUNCTION(pPluginData->hModule, "ProcessTick");
	}
	else
	{
		pPluginData->ProcessTick = NULL;
	}

	if (!pPluginData->Load(this->pPluginData)) 
	{
		PLUGIN_UNLOAD(pPluginData->hModule);
		delete pPluginData;
		return FALSE;
	}

	pluginsVector.push_back(pPluginData);

	return TRUE;
}

void CPlugins::LoadPlugins(char* szSearchPath)
{
	char szPath[MAX_PATH];
	char szFullPath[MAX_PATH];
	char* szDlerror = NULL;
	strcpy(szPath, szSearchPath);

#ifdef LINUX
	if (szPath[strlen(szPath)-1] != '/') 
		strcat(szPath, "/");
#else
	if (szPath[strlen(szPath)-1] != '\\') 
		strcat(szPath, "\\");
#endif

	logprintf("");
	logprintf("Server Plugins");
	logprintf("--------------");
	int iPluginCount = 0;
	char* szFilename = strtok(__Console->GetStringVar("plugins"), " ");
	while (szFilename)
	{
		logprintf(" Loading plugin: %s", szFilename);

		strcpy(szFullPath, szPath);
		strcat(szFullPath, szFilename);

		if (this->LoadPlugin(szFullPath))
            logprintf("  Loaded.");
		else 
		{
#ifdef LINUX
			szDlerror = PLUGIN_GETERROR();
			if (szDlerror) 
				logprintf("  Failed (%s)", szDlerror);
			else 
				logprintf("  Failed.");
#else
			logprintf("  Failed.");
#endif
		}

		szFilename = strtok(NULL, " ");
	}
	logprintf(" Loaded %d plugins.\n", this->GetPluginCount());
}

void CPlugins::DoProcessTick()
{

	std::vector<pluginData*>::iterator itor;

	for(itor = pluginsVector.begin(); itor != pluginsVector.end(); itor++) {
		pluginData* pPluginData = *itor;

		if ((pPluginData->dwSupportFlags & SUPPORTS_PROCESS_TICK) != 0)
			if (pPluginData->ProcessTick != NULL)
				pPluginData->ProcessTick();
	}

}

void CPlugins::DoAmxLoad(AMX *amx) 
{

	std::vector<pluginData*>::iterator itor;

	for(itor = pluginsVector.begin(); itor != pluginsVector.end(); itor++) {
		pluginData* pPluginData = *itor;

		if ((pPluginData->dwSupportFlags & SUPPORTS_AMX_NATIVES) != 0)
			if (pPluginData->AmxLoad != NULL)
				pPluginData->AmxLoad(amx);
	}

}

void CPlugins::DoAmxUnload(AMX *amx)
{

	std::vector<pluginData*>::iterator itor;

	for(itor = pluginsVector.begin(); itor != pluginsVector.end(); itor++) 
	{
		pluginData* pPluginData = *itor;

		if ((pPluginData->dwSupportFlags & SUPPORTS_AMX_NATIVES) != 0)
			if (pPluginData->AmxUnload != NULL)
				pPluginData->AmxUnload(amx);
	}

}
