[BaseContainerProps(), SCR_BaseContainerCustomTitleUIInfo("m_UIInfo")]
class ATH_ToggleBudgetsEditorAttribute : SCR_BaseEditorAttribute
{	
	override SCR_BaseEditorAttributeVar ReadVariable(Managed item, SCR_AttributesManagerEditorComponent manager)
	{
		// If opened in global attributes (Game Settings)
		if (!IsGameMode(item)) 
			return null;
		
		return SCR_BaseEditorAttributeVar.CreateBool(SCR_BaseGameMode.s_bBudgetsEnforced);
	}
	
	override void WriteVariable(Managed item, SCR_BaseEditorAttributeVar var, SCR_AttributesManagerEditorComponent manager, int playerID)
	{
		if (!var) 
			return;
		
		SCR_BaseGameMode gamemode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		if (!gamemode)
			return;
		
		gamemode.ATH_SetBudgetsEnforced(var.GetBool());
	}
}
