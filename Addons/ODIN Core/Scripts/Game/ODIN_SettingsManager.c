//------------------------------------------------------------------------------------------------
//! ODIN Core - Centralized Settings Manager
//! Allows child mods (ATLAS, APOLLO, IRIS) to register their settings UI dynamically.
class ODIN_SettingsManager
{
	protected static ref ODIN_SettingsManager s_Instance;
	
	protected ref map<string, ref ODIN_ModSettingsModule> m_mRegisteredMods = new map<string, ref ODIN_ModSettingsModule>();
	
	//------------------------------------------------------------------------------------------------
	static ODIN_SettingsManager GetInstance()
	{
		if (!s_Instance)
			s_Instance = new ODIN_SettingsManager();
			
		return s_Instance;
	}
	
	//------------------------------------------------------------------------------------------------
	void RegisterMod(string modId, string displayName, ResourceName layoutPath)
	{
		if (m_mRegisteredMods.Contains(modId))
			return;
			
		ODIN_ModSettingsModule module = new ODIN_ModSettingsModule(modId, displayName, layoutPath);
		m_mRegisteredMods.Insert(modId, module);
	}
	
	//------------------------------------------------------------------------------------------------
	map<string, ref ODIN_ModSettingsModule> GetRegisteredMods()
	{
		return m_mRegisteredMods;
	}
}

class ODIN_ModSettingsModule
{
	string m_sModId;
	string m_sDisplayName;
	ResourceName m_sLayoutPath;
	
	void ODIN_ModSettingsModule(string id, string name, ResourceName path)
	{
		m_sModId = id;
		m_sDisplayName = name;
		m_sLayoutPath = path;
	}
}
