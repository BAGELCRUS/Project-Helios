modded class SCR_GetInUserAction : SCR_CompartmentUserAction
{
	override bool CanBePerformedScript(IEntity user)
	{
		// Let vanilla logic run first
		if (!super.CanBePerformedScript(user))
			return false;
			
		// Add custom Game Master lock check
		BaseCompartmentSlot compartment = GetCompartmentSlot();
		if (!compartment) return true;
		
		IEntity owner = compartment.GetOwner();
		Vehicle vehicle = Vehicle.Cast(SCR_EntityHelper.GetMainParent(owner, true));
		if (!vehicle) return true;
		
		VehicleControllerComponent controller = VehicleControllerComponent.Cast(vehicle.FindComponent(VehicleControllerComponent));
		if (controller && controller.ATH_IsVehicleLocked())
		{
			SetCannotPerformReason("Locked by Game Master");
			return false;
		}
		
		return true;
	}
}
