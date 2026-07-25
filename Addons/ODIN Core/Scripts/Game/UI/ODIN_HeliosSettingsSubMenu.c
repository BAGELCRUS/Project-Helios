//------------------------------------------------------------------------------------------------
//! ODIN Core - Helios Settings SubMenu
//! This script attaches to the root of the new HELIOS tab layout.
//! It fetches the registered mods from ODIN_SettingsManager and populates a dropdown.
class ODIN_HeliosSettingsSubMenu : SCR_SettingsSubMenuBase
{
	protected Widget m_wModSelectorFrame;
	protected SCR_ComboBoxComponent m_ModSelector;
	protected Widget m_wCurrentSettingsRoot;
	
	//------------------------------------------------------------------------------------------------
	override void OnTabCreate(Widget menuRoot, ResourceName buttonsLayout, int index)
	{
		super.OnTabCreate(menuRoot, buttonsLayout, index);
		
		Widget root = GetRootWidget();
		if (!root)
			return;
			
		// The container where the child mod's specific settings layout will be loaded
		m_wModSelectorFrame = root.FindAnyWidget("SettingsContainer");
		
		// The dropdown to select which mod's settings to view
		Widget comboWidget = root.FindAnyWidget("ModSelectorDropdown");
		if (comboWidget)
			m_ModSelector = SCR_ComboBoxComponent.Cast(comboWidget.FindHandler(SCR_ComboBoxComponent));
			
		if (m_ModSelector)
		{
			m_ModSelector.m_OnChanged.Insert(OnModSelected);
			PopulateModList();
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void PopulateModList()
	{
		if (!m_ModSelector)
			return;
		
		m_ModSelector.ClearAll();
		
		map<string, ref ODIN_ModSettingsModule> mods = ODIN_SettingsManager.GetInstance().GetRegisteredMods();
		
		for (int i = 0; i < mods.Count(); i++)
		{
			ODIN_ModSettingsModule mod = mods.GetElement(i);
			m_ModSelector.AddItem(mod.m_sDisplayName, false, mod);
		}
		
		if (mods.Count() > 0)
		{
			m_ModSelector.SetCurrentItem(0);
			OnModSelected(m_ModSelector, 0, true);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnModSelected(SCR_ComboBoxComponent comp, int item, bool programmatic)
	{
		ODIN_ModSettingsModule mod = ODIN_ModSettingsModule.Cast(comp.GetItemData(item));
		if (!mod || !m_wModSelectorFrame) 
			return;
		
		// Remove previous settings layout from the frame
		if (m_wCurrentSettingsRoot)
		{
			m_wCurrentSettingsRoot.RemoveFromHierarchy();
			m_wCurrentSettingsRoot = null;
		}
		
		// Load the newly selected mod's layout into the frame
		if (mod.m_sLayoutPath != string.Empty)
		{
			m_wCurrentSettingsRoot = GetGame().GetWorkspace().CreateWidgets(mod.m_sLayoutPath, m_wModSelectorFrame);
		}
	}
}
