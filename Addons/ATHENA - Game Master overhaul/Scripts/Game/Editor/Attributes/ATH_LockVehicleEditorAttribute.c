[BaseContainerProps(), SCR_BaseContainerCustomTitleUIInfo("m_UIInfo")]
class ATH_LockVehicleEditorAttribute : SCR_BaseEditorAttribute
{
	override SCR_BaseEditorAttributeVar ReadVariable(Managed item, SCR_AttributesManagerEditorComponent manager)
	{
		SCR_EditableEntityComponent editableEntity = SCR_EditableEntityComponent.Cast(item);
		if (!editableEntity) return null;
		
		IEntity entity = editableEntity.GetOwner();
		if (!entity) return null;
		
		Vehicle vehicle = Vehicle.Cast(entity);
		if (!vehicle) return null;
		
		BaseCompartmentManagerComponent compartmentMgr = BaseCompartmentManagerComponent.Cast(vehicle.FindComponent(BaseCompartmentManagerComponent));
		if (!compartmentMgr) return null;
		
		array<BaseCompartmentSlot> compartments = {};
		compartmentMgr.GetCompartments(compartments);
		
		bool isLocked = false;
		if (compartments.Count() > 0)
		{
			// If the first compartment is NOT accessible, we assume the vehicle is locked
			isLocked = !compartments[0].IsCompartmentAccessible();
		}
		
		return SCR_BaseEditorAttributeVar.CreateBool(isLocked);
	}
	
	override void WriteVariable(Managed item, SCR_BaseEditorAttributeVar var, SCR_AttributesManagerEditorComponent manager, int playerID)
	{
		if (!var) return;
		
		SCR_EditableEntityComponent editableEntity = SCR_EditableEntityComponent.Cast(item);
		if (!editableEntity) return;
		
		IEntity entity = editableEntity.GetOwner();
		if (!entity) return;
		
		Vehicle vehicle = Vehicle.Cast(entity);
		if (!vehicle) return;
		
		BaseCompartmentManagerComponent compartmentMgr = BaseCompartmentManagerComponent.Cast(vehicle.FindComponent(BaseCompartmentManagerComponent));
		if (!compartmentMgr) return;
		
		array<BaseCompartmentSlot> compartments = {};
		compartmentMgr.GetCompartments(compartments);
		
		bool shouldLock = var.GetBool();
		
		foreach (BaseCompartmentSlot slot : compartments)
		{
			slot.SetCompartmentAccessible(!shouldLock);
		}
	}
}
