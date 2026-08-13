[BaseContainerProps(), SCR_BaseContainerCustomTitleUIInfo("m_UIInfo")]
class ATH_AIStanceAttribute : SCR_BaseValueListEditorAttribute
{
	protected static const ref array<string> STANCE_NAMES = {
		"Stand",
		"Crouch",
		"Prone"
	};
	
	protected static const ref array<ECharacterStance> STANCE_VALUES = {
		ECharacterStance.STAND,
		ECharacterStance.CROUCH,
		ECharacterStance.PRONE
	};

	override SCR_BaseEditorAttributeVar ReadVariable(Managed item, SCR_AttributesManagerEditorComponent manager)
	{
		// We'll default to Stand (index 0) if reading isn't precise
		return SCR_BaseEditorAttributeVar.CreateInt(0);
	}
	
	override void WriteVariable(Managed item, SCR_BaseEditorAttributeVar var, SCR_AttributesManagerEditorComponent manager, int playerID)
	{
		if (!var) return;
		
		SCR_EditableEntityComponent editableEntity = SCR_EditableEntityComponent.Cast(item);
		if (!editableEntity) return;
		
		IEntity entity = editableEntity.GetOwner();
		if (!entity) return;
		
		int selectedIndex = var.GetInt();
		if (selectedIndex < 0 || selectedIndex >= STANCE_VALUES.Count()) return;
		
		ECharacterStance stance = STANCE_VALUES[selectedIndex];
		
		// If it's a group, apply to all members
		SCR_AIGroup group = SCR_AIGroup.Cast(entity);
		if (group)
		{
			array<AIAgent> agents = {};
			group.GetAgents(agents);
			foreach (AIAgent agent : agents)
			{
				ApplyStanceToCharacter(agent, stance);
			}
		}
		else
		{
			// Try applying to the entity itself
			ApplyStanceToCharacter(entity, stance);
		}
	}
	
	protected void ApplyStanceToCharacter(IEntity entity, ECharacterStance stance)
	{
		if (!entity) return;
		
		// Ensure this only applies to AI
		if (EntityUtils.IsPlayer(entity)) return;
		
		// Check if it's an AIAgent wrapping a character, or a character directly
		IEntity characterEntity = entity;
		AIAgent agent = AIAgent.Cast(entity);
		if (agent)
		{
			characterEntity = agent.GetControlledEntity();
		}
		
		if (!characterEntity) return;
		
		// Apply physical stance change
		CharacterControllerComponent controller = CharacterControllerComponent.Cast(characterEntity.FindComponent(CharacterControllerComponent));
		if (controller)
		{
			controller.SetStanceChange(stance);
		}
		
		// Add setting to utility component so behavior tree respects it (for Groups only since Character AddSetting is protected)
		if (agent)
		{
			SCR_AIUtilityComponent utility = SCR_AIUtilityComponent.Cast(agent.FindComponent(SCR_AIUtilityComponent));
			if (utility)
			{
				SCR_AIGroupSettingsComponent groupSettingsComp = SCR_AIGroupSettingsComponent.Cast(agent.FindComponent(SCR_AIGroupSettingsComponent));
				if (groupSettingsComp)
				{
					// TODO: SCR_AICharacterStanceSetting stanceSetting = SCR_AICharacterStanceSetting.Create(SCR_EAISettingOrigin.EDITOR, SCR_EAIBehaviorCause.UNKNOWN, stance);
					// groupSettingsComp.AddSetting(stanceSetting, true, true);
				}
			}
		}
	}
	
	override int GetEntries(notnull array<ref SCR_BaseEditorAttributeEntry> outEntries)
	{
		for (int i = 0; i < STANCE_NAMES.Count(); i++)
		{
			SCR_BaseEditorAttributeEntryText entry = new SCR_BaseEditorAttributeEntryText(STANCE_NAMES[i]);
			outEntries.Insert(entry);
		}
		
		return outEntries.Count();
	}
}
