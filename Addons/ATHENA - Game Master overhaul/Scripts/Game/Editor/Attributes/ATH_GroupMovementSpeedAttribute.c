[BaseContainerProps(), SCR_BaseContainerCustomTitleUIInfo("m_UIInfo")]
class ATH_GroupMovementSpeedAttribute : SCR_BaseValueListEditorAttribute
{
	protected static const ref array<string> SPEED_NAMES = {
		"Walk",
		"Run",
		"Sprint"
	};
	
	protected static const ref array<EMovementType> SPEED_VALUES = {
		EMovementType.WALK,
		EMovementType.RUN,
		EMovementType.SPRINT
	};

	override SCR_BaseEditorAttributeVar ReadVariable(Managed item, SCR_AttributesManagerEditorComponent manager)
	{
		// Default to Run (index 1) if we can't read the exact speed setting reliably
		return SCR_BaseEditorAttributeVar.CreateInt(1);
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
		
		int selectedIndex = var.GetInt();
		if (selectedIndex < 0 || selectedIndex >= SPEED_VALUES.Count()) return;
		
		EMovementType speed = SPEED_VALUES[selectedIndex];
		
		SCR_AIGroupUtilityComponent utility = group.GetGroupUtilityComponent();
		if (utility)
		{
			SCR_AIGroupCharactersMovementSpeedSetting speedSetting = SCR_AIGroupCharactersMovementSpeedSetting.Create(SCR_EAISettingOrigin.EDITOR, speed);
			
			// Group Settings Component is typically attached to the same entity
			SCR_AIGroupSettingsComponent settingsComp = SCR_AIGroupSettingsComponent.Cast(group.FindComponent(SCR_AIGroupSettingsComponent));
			if (settingsComp)
			{
				settingsComp.AddSetting(speedSetting, true, true);
			}
		}
	}
	
	override int GetEntries(notnull array<ref SCR_BaseEditorAttributeEntry> outEntries)
	{
		for (int i = 0; i < SPEED_NAMES.Count(); i++)
		{
			SCR_BaseEditorAttributeEntryText entry = new SCR_BaseEditorAttributeEntryText(SPEED_NAMES[i]);
			outEntries.Insert(entry);
		}
		
		return outEntries.Count();
	}
}
