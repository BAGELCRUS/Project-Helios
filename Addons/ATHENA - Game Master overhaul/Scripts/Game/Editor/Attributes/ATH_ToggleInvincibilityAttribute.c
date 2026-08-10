[BaseContainerProps(), SCR_BaseContainerCustomTitleUIInfo("m_UIInfo")]
class ATH_ToggleInvincibilityAttribute : SCR_BaseEditorAttribute
{
	override SCR_BaseEditorAttributeVar ReadVariable(Managed item, SCR_AttributesManagerEditorComponent manager)
	{
		SCR_EditableEntityComponent editableEntity = SCR_EditableEntityComponent.Cast(item);
		if (!editableEntity)
			return null;
			
		IEntity entity = editableEntity.GetOwner();
		if (!entity)
			return null;
			
		SCR_DamageManagerComponent dmgMgr = SCR_DamageManagerComponent.Cast(entity.FindComponent(SCR_DamageManagerComponent));
		if (!dmgMgr)
			return null;
			
		bool isInvincible = !dmgMgr.IsDamageHandlingEnabled();
		return SCR_BaseEditorAttributeVar.CreateBool(isInvincible);
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
			
		SCR_DamageManagerComponent dmgMgr = SCR_DamageManagerComponent.Cast(entity.FindComponent(SCR_DamageManagerComponent));
		if (!dmgMgr)
			return;
			
		bool makeInvincible = var.GetBool();
		dmgMgr.EnableDamageHandling(!makeInvincible);
	}
}
