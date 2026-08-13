[BaseContainerProps(), SCR_BaseContainerCustomTitleUIInfo("m_UIInfo")]
class ATH_GroupCombatModeAttribute : SCR_BaseValueListEditorAttribute
{
	protected static const ref array<string> MODE_NAMES = {
		"Green (Safe)",
		"Yellow (Vigilant)",
		"Red (Combat)",
		"Blue (Stealth)",
		"White (Careless)"
	};

	override SCR_BaseEditorAttributeVar ReadVariable(Managed item, SCR_AttributesManagerEditorComponent manager)
	{
		// Default to Red (Combat) which is the vanilla standard
		return SCR_BaseEditorAttributeVar.CreateInt(2);
	}
	
	override void WriteVariable(Managed item, SCR_BaseEditorAttributeVar var, SCR_AttributesManagerEditorComponent manager, int playerID)
	{
		if (!var) return;
		
		SCR_EditableEntityComponent editableEntity = SCR_EditableEntityComponent.Cast(item);
		if (!editableEntity) return;
		
		IEntity entity = editableEntity.GetOwner();
		if (!entity) return;
		
		SCR_AIGroup group = SCR_AIGroup.Cast(entity);
		if (!group) return;
		
		SCR_AIGroupUtilityComponent utility = group.GetGroupUtilityComponent();
		if (!utility) return;
		
		int selectedIndex = var.GetInt();
		if (selectedIndex < 0 || selectedIndex >= MODE_NAMES.Count()) return;
		
		// Map custom combat modes to Reforger's EAIGroupCombatMode
		switch (selectedIndex)
		{
			case 0: // Green (Safe) - AI is relaxed, ignore threats
			{
				// TODO: Map to actual EAIGroupCombatMode fields, e.g. utility.SetCombatMode(EAIGroupCombatMode.IGNORE)
				break;
			}
			case 1: // Yellow (Vigilant) - AI returns fire and suppresses
			{
				// TODO: utility.SetCombatMode(EAIGroupCombatMode.COMBAT)
				break;
			}
			case 2: // Red (Combat) - Maximum threat state
			{
				// TODO: utility.SetCombatMode(EAIGroupCombatMode.COMBAT)
				break;
			}
			case 3: // Blue (Stealth) - Move cover to cover, hold fire
			{
				// TODO: utility.SetCombatMode(EAIGroupCombatMode.STEALTH)
				break;
			}
			case 4: // White (Careless) - Ignore threats entirely and force move
			{
				// TODO: utility.SetCombatMode(EAIGroupCombatMode.IGNORE)
				break;
			}
		}
	}
	
	override int GetEntries(notnull array<ref SCR_BaseEditorAttributeEntry> outEntries)
	{
		for (int i = 0; i < MODE_NAMES.Count(); i++)
		{
			SCR_BaseEditorAttributeEntryText entry = new SCR_BaseEditorAttributeEntryText(MODE_NAMES[i]);
			outEntries.Insert(entry);
		}
		
		return outEntries.Count();
	}
}
