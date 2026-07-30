//------------------------------------------------------------------------------------------------
//! ATH_AthenaSettingsSubMenu
//! Athena - Game Master Overhaul
//!
//! Script handler for the Athena settings panel inside the ODIN Helios tab.
//! Manages the NV Color SpinBox and exposes the selected color mode to the
//! NightVision toolbar action via a static accessor.
//------------------------------------------------------------------------------------------------

class ATH_AthenaSettingsSubMenu : ScriptedWidgetComponent
{
	// NV color mode indices — must match the SpinBox item order in AthenaSettings.layout
	static const int NV_COLOR_GREEN = 0;
	static const int NV_COLOR_WP    = 1;
	
	// Persisted selection across UI rebuilds
	protected static int s_iSelectedNVColor = NV_COLOR_GREEN;
	
	protected SCR_SpinBoxComponent m_NVColorSpinBox;
	
	//------------------------------------------------------------------------------------------------
	override void HandlerAttached(Widget w)
	{
		super.HandlerAttached(w);
		
		Widget spinWidget = w.FindAnyWidget("NVColorSpinBox");
		if (spinWidget)
			m_NVColorSpinBox = SCR_SpinBoxComponent.Cast(spinWidget.FindHandler(SCR_SpinBoxComponent));
		
		if (m_NVColorSpinBox)
		{
			// Restore previously selected value
			m_NVColorSpinBox.SetCurrentItem(s_iSelectedNVColor);
			m_NVColorSpinBox.m_OnChanged.Insert(OnNVColorChanged);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override void HandlerDeattached(Widget w)
	{
		if (m_NVColorSpinBox)
			m_NVColorSpinBox.m_OnChanged.Remove(OnNVColorChanged);
		
		super.HandlerDeattached(w);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnNVColorChanged(SCR_SpinBoxComponent comp, int itemIndex)
	{
		s_iSelectedNVColor = itemIndex;
		
		// If NV is currently active, hot-swap the post-process effect immediately
		ATH_NightVisionToolbarAction.OnNVColorPreferenceChanged();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Returns the currently selected NV color index.
	static int GetSelectedNVColor()
	{
		return s_iSelectedNVColor;
	}
}
