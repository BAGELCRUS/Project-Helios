//------------------------------------------------------------------------------------------------
//! Extends SCR_InterfaceSettingsSubMenu to add per-category notification toggles.
//! Programmatically injects new spinbox widgets at runtime after an existing
//! anchor toggle. These sub-toggles are part of the master switch system.
modded class SCR_InterfaceSettingsSubMenu : SCR_SettingsSubMenuBase
{
	// Widget names for the notification category spinboxes
	protected const string NOTIF_PLAYER_JOIN_LEAVE_WIDGET = "NotifPlayerJoinLeave";
	protected const string NOTIF_KILLFEED_WIDGET = "NotifKillfeed";
	protected const string NOTIF_OBJECTIVES_WIDGET = "NotifObjectives";
	protected const string NOTIF_BASE_CAPTURE_WIDGET = "NotifBaseCapture";
	protected const string NOTIF_FACTION_CHANGES_WIDGET = "NotifFactionChanges";
	protected const string NOTIF_EDITOR_GM_WIDGET = "NotifEditorGM";
	protected const string NOTIF_WEATHER_WIDGET = "NotifWeather";
	
	// Track our injected category spinboxes separately for master switch integration
	protected ref array<ref SCR_SpinBoxComponent> m_aNotifCategorySpinBoxes = {};
	
	// Track injected root containers for cleanup
	protected ref array<Widget> m_aInjectedWidgets = {};
	
	// Resource name for the standard vanilla SpinBox layout
	protected const ResourceName SPINBOX_LAYOUT = "{C9DF0E6590F6C388}UI/layouts/WidgetLibrary/SpinBox/WLib_SpinBox.layout";
	
	//------------------------------------------------------------------------------------------------
	override void OnTabCreate(Widget menuRoot, ResourceName buttonsLayout, int index)
	{
		// Let vanilla set up all its spinboxes first
		super.OnTabCreate(menuRoot, buttonsLayout, index);
		
		if (!m_InterfaceSettings)
			return;
		
		// Find a reliable anchor widget to inject our sub-toggles after.
		// In some multiplayer environments, the master "Notifications" toggle might be missing or hidden,
		// but the "Hint" (HINTS_SPINBOX_WIDGET_NAME) toggle is reliably present in the Interactions category.
		Widget anchorWidget = m_wRoot.FindAnyWidget(HINTS_SPINBOX_WIDGET_NAME);
		if (!anchorWidget)
		{
			// Fallback to Notifications if Hints is somehow missing
			anchorWidget = m_wRoot.FindAnyWidget(NOTIFICATIONS_SPINBOX_WIDGET_NAME);
			if (!anchorWidget)
			{
				Print("SCR_InterfaceSettingsSubMenuMod: Could not find any anchor widget for injection", LogLevel.WARNING);
				return;
			}
		}
		
		// Get the parent container (typically the ScrollLayout or a VerticalLayout child)
		Widget parentContainer = anchorWidget.GetParent();
		if (!parentContainer)
		{
			Print("SCR_InterfaceSettingsSubMenuMod: Anchor widget has no parent", LogLevel.WARNING);
			return;
		}
		
		// Calculate Z-Orders to place our new widgets right after the anchor widget
		Widget child = parentContainer.GetChildren();
		int z = 0;
		int targetZOrder = 0;
		
		while (child)
		{
			child.SetZOrder(z);
			
			if (child == anchorWidget)
			{
				// This is our anchor widget! We want our widgets to come right after.
				// Leave a gap for our injected widgets.
				targetZOrder = z + 1;
				z += 10;
			}
			else
			{
				z += 1;
			}
			
			child = child.GetSibling();
		}
		
		// If we couldn't find it in the loop for some reason, just put them at the end
		if (targetZOrder == 0)
			targetZOrder = z;
		
		// Inject the widgets, incrementing Z-order so they stay in order
		InjectCategorySpinbox(parentContainer, NOTIF_PLAYER_JOIN_LEAVE_WIDGET, "m_bShowNotif_PlayerJoinLeave", "   Player Join/Leave", targetZOrder++);
		InjectCategorySpinbox(parentContainer, NOTIF_KILLFEED_WIDGET, "m_bShowNotif_Killfeed", "   Kill Feed", targetZOrder++);
		InjectCategorySpinbox(parentContainer, NOTIF_OBJECTIVES_WIDGET, "m_bShowNotif_Objectives", "   Objectives", targetZOrder++);
		InjectCategorySpinbox(parentContainer, NOTIF_BASE_CAPTURE_WIDGET, "m_bShowNotif_BaseCapture", "   Base Capture", targetZOrder++);
		InjectCategorySpinbox(parentContainer, NOTIF_FACTION_CHANGES_WIDGET, "m_bShowNotif_FactionChanges", "   Faction Changes", targetZOrder++);
		InjectCategorySpinbox(parentContainer, NOTIF_EDITOR_GM_WIDGET, "m_bShowNotif_EditorGM", "   Editor / GM", targetZOrder++);
		InjectCategorySpinbox(parentContainer, NOTIF_WEATHER_WIDGET, "m_bShowNotif_Weather", "   Weather", targetZOrder++);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Injects a single spinbox layout, configures its name and text, populates items, then runs standard setup.
	//! \param parentContainer The parent widget to inject into
	//! \param widgetName The name to assign to the new widget (for lookup)
	//! \param settingName The setting field name to bind to
	//! \param labelText The display text for the spinbox label
	//! \param zOrder The Z-order to apply to the widget for sorting
	protected void InjectCategorySpinbox(Widget parentContainer, string widgetName, string settingName, string labelText, int zOrder)
	{
		// Create the vanilla spinbox layout instance
		Widget w = GetGame().GetWorkspace().CreateWidgets(SPINBOX_LAYOUT, parentContainer);
		if (!w) return;
		
		// Keep track for cleanup
		m_aInjectedWidgets.Insert(w);
		
		// Apply Z-order to place it correctly in the list
		w.SetZOrder(zOrder);
		
		// The root widget of WLib_SpinBox.layout is named "SpinBox". We must rename it so SetupSpinboxes can find it.
		w.SetName(widgetName);
		
		// Get the spinbox component to add our Hide/Show options and set the label
		SCR_SpinBoxComponent comp = SCR_SpinBoxComponent.Cast(w.FindHandler(SCR_SpinBoxComponent));
		if (comp)
		{
			// Add boolean options. Index 0 = Hide (false), Index 1 = Show (true)
			comp.AddItem("Hide");
			comp.AddItem("Show");
			
			// Enable cycle mode so both arrows remain visible/active (like the vanilla Hint setting)
			comp.SetCycleMode(true);
			
			// Try to find the label widget. WLib_SpinBox uses "Text" or similar for label.
			TextWidget label = TextWidget.Cast(w.FindAnyWidget("Text")); 
			if (!label) label = TextWidget.Cast(w.FindAnyWidget("Name")); 
			if (!label) label = TextWidget.Cast(w.FindAnyWidget("NameText"));
				
			if (label)
				label.SetText(labelText);
		}
		
		// Now call the vanilla SetupSpinboxes which will find this newly named widget in m_wRoot hierarchy
		SetupSpinboxes(widgetName, settingName);
		
		// Add to our tracked array for master switch changes
		if (m_SpinnerBoxComp)
		{
			m_SpinnerBoxComp.m_OnChanged.Insert(OnSpinBoxChange);
			m_aNotifCategorySpinBoxes.Insert(m_SpinnerBoxComp);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnTabHide()
	{
		super.OnTabHide();
		
		// Clean up dynamically injected widgets when tab is hidden/destroyed
		foreach (Widget w : m_aInjectedWidgets)
		{
			if (w) w.RemoveFromHierarchy();
		}
		m_aInjectedWidgets.Clear();
		m_aNotifCategorySpinBoxes.Clear();
	}
}
