[BaseContainerProps(), SCR_BaseContainerCustomTitleUIInfo("m_UIInfo")]
class ATH_ToggleInvisibilityAttribute : SCR_BaseEditorAttribute
{
	override SCR_BaseEditorAttributeVar ReadVariable(Managed item, SCR_AttributesManagerEditorComponent manager)
	{
		SCR_EditableEntityComponent editableEntity = SCR_EditableEntityComponent.Cast(item);
		if (!editableEntity)
			return null;
			
		IEntity entity = editableEntity.GetOwner();
		if (!entity)
			return null;
			
		// True if INVISIBLE (i.e. VISIBLE flag is not set)
		bool isInvisible = (entity.GetFlags() & EntityFlags.VISIBLE) == 0;
		return SCR_BaseEditorAttributeVar.CreateBool(isInvisible);
	}
	
	override void WriteVariable(Managed item, SCR_BaseEditorAttributeVar var, SCR_AttributesManagerEditorComponent manager, int playerID)
	{
		if (!var)
			return;
			
		SCR_EditableEntityComponent editableEntity = SCR_EditableEntityComponent.Cast(item);
		if (!editableEntity)
			return;
			
		IEntity entity = editableEntity.GetOwner();
		if (!entity)
			return;
			
		bool makeInvisible = var.GetBool();
		
		if (makeInvisible)
		{
			entity.ClearFlags(EntityFlags.VISIBLE, true);
		}
		else
		{
			entity.SetFlags(EntityFlags.VISIBLE, true);
		}
		
		CharacterIdentityComponent identity = CharacterIdentityComponent.Cast(entity.FindComponent(CharacterIdentityComponent));
		if (identity)
		{
			// SetVisibleAll ensures the character's identity meshes are handled correctly
			identity.SetVisibleAll(!makeInvisible);
		}
		
		PerceptionComponent perception = PerceptionComponent.Cast(entity.FindComponent(PerceptionComponent));
		if (perception)
		{
			if (makeInvisible)
				perception.SetPerceptionFactor(0.0);
			else
				perception.SetPerceptionFactor(1.0);
		}
	}
}
