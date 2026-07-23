//------------------------------------------------------------------------------------------------
//! Extends SCR_NotificationsComponent to filter notifications by category.
//! Checks user settings before processing incoming notifications:
//!   1. If master "Notifications" toggle is off → block all
//!   2. If notification has priority flag → always show (bypass category filter)
//!   3. If specific category toggle is off → block that notification
//!   4. Otherwise → show normally
modded class SCR_NotificationsComponent : ScriptComponent
{
	//------------------------------------------------------------------------------------------------
	//! Intercepts incoming notifications and checks category settings before processing.
	//! Priority notifications always pass through. Uncategorized notifications always pass through.
	override protected void ReceiveSCR_NotificationData(ENotification id, SCR_NotificationData data)
	{
		// Check if this notification should be shown based on user settings
		if (!IsNotificationAllowedBySettings(id))
			return;
		
		super.ReceiveSCR_NotificationData(id, data);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Determines if a notification is allowed by the user's interface settings.
	//! \param id The notification enum value
	//! \return true if the notification should be shown
	protected bool IsNotificationAllowedBySettings(ENotification id)
	{
		// Get interface settings
		BaseContainer interfaceSettings = GetGame().GetGameUserSettings().GetModule("SCR_InterfaceSettings");
		if (!interfaceSettings)
			return true;
		
		// Check master notifications toggle
		bool showNotifications;
		interfaceSettings.Get("m_bShowNotifications", showNotifications);
		if (!showNotifications)
			return false;
		
		// Check if this is a priority notification — priority notifications bypass filters
		SCR_NotificationDisplayData displayData = GetNotificationDisplayData(id);
		if (displayData && displayData.GetPriority())
			return true;
		
		// Get the category for this notification
		int category = SCR_NotificationCategoryHelper.GetCategory(id);
		
		// Uncategorized notifications always pass through
		if (category < 0)
			return true;
		
		// Get the setting name for this category
		string settingName = SCR_NotificationCategoryHelper.GetSettingName(category);
		if (settingName.IsEmpty())
			return true;
		
		// Check the category-specific toggle
		bool showCategory;
		interfaceSettings.Get(settingName, showCategory);
		
		return showCategory;
	}
}
