//------------------------------------------------------------------------------------------------
//! ATLAS - Mod Initialization
//! Registers ATLAS settings to the ODIN Core centralized settings manager.
[BaseContainerProps()]
class ATLAS_InitConfig
{
	// Called by a GameMode component or a custom init script.
	// For simplicity, we can hook this into GameMode or just run it via an init class.
}

modded class SCR_BaseGameMode
{
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		
		// Wait a frame to ensure ODIN_SettingsManager is ready
		GetGame().GetCallqueue().CallLater(RegisterAtlasSettings, 1000, false);
	}
	
	protected void RegisterAtlasSettings()
	{
		// Make sure ODIN is loaded (check if manager exists)
		if (ODIN_SettingsManager.GetInstance())
		{
			// The ResourceName should point to the ATLAS settings layout. 
			// We use a dummy path for now; the user will update this in Workbench.
			ODIN_SettingsManager.GetInstance().RegisterMod("ATLAS", "ATLAS - Map Improvements", "{A494571E7E2CF53D}UI/layouts/Map/AtlasSettings.layout");
		}
	}
}
