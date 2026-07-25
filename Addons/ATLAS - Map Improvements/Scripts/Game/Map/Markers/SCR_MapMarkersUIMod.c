//------------------------------------------------------------------------------------------------
//! ATLAS - Map Improvements
//! Modded SCR_MapMarkersUI: doubles the character limit on the custom marker name input field.
//! Vanilla enforces the limit via EditBoxFilterComponent on the EditBox widget inside the
//! MapMarkerEditBox layout. We retrieve that filter after dialog creation and double the limit.
modded class SCR_MapMarkersUI
{
	//------------------------------------------------------------------------------------------------
	//! Called after the standard edit dialog is built. We grab the EditBoxFilterComponent from the
	//! marker name EditBox and double the character ceiling.
	override protected void CreateMarkerEditDialog(bool isEditing = false, int tabID = 0, int selectedIconEntry = -1, int selectedColorEntry = -1)
	{
		super.CreateMarkerEditDialog(isEditing, tabID, selectedIconEntry, selectedColorEntry);

		ApplyCharacterLimit(USERID_EDITBOX);
	}

	//------------------------------------------------------------------------------------------------
	//! Same override for the military symbol marker dialog, which has its own EditBox.
	override protected void CreateMilitaryMarkerEditDialog(bool isEditing = false, int selectedFactionEntry = -1, int selectedDimensionEntry = -1)
	{
		super.CreateMilitaryMarkerEditDialog(isEditing, selectedFactionEntry, selectedDimensionEntry);

		ApplyCharacterLimit(USERID_EDITBOX_MIL);
	}

	//------------------------------------------------------------------------------------------------
	//! Finds the EditBox with the given UserID in the current edit dialog root,
	//! reads the current vanilla character limit, then doubles it.
	protected void ApplyCharacterLimit(int userID)
	{
		if (!m_MarkerEditRoot)
			return;

		// Walk the widget tree to find the EditBox by UserID
		Widget editBoxWidget = FindWidgetByUserID(m_MarkerEditRoot, userID);
		if (!editBoxWidget)
			return;

		EditBoxFilterComponent filter = EditBoxFilterComponent.Cast(editBoxWidget.FindHandler(EditBoxFilterComponent));
		if (!filter)
			return;

		// Read the vanilla limit and double it - stays correct even if BI changes the base value
		int currentLimit = filter.GetNumLimitOfCharacters();
		if (currentLimit > 0)
			filter.SetCharacterLimit(currentLimit * 2);
	}

	//------------------------------------------------------------------------------------------------
	//! Recursive widget search by UserID.
	protected Widget FindWidgetByUserID(Widget parent, int userID)
	{
		if (!parent)
			return null;

		if (parent.GetUserID() == userID)
			return parent;

		Widget child = parent.GetChildren();
		while (child)
		{
			Widget found = FindWidgetByUserID(child, userID);
			if (found)
				return found;

			child = child.GetSibling();
		}

		return null;
	}
}
