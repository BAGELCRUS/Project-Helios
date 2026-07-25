//------------------------------------------------------------------------------------------------
//! ATLAS - Map Improvements
//! Modded SCR_MapMarkerWidgetComponent: keeps marker text visible permanently without hovering.
//! We intercept the layout event handlers and visibility functions, making sure to safely check
//! for null pointers so we don't break markers that lack text widgets.

modded class SCR_MapMarkerWidgetComponent
{
	//------------------------------------------------------------------------------------------------
	override void HandlerAttached(Widget w)
	{
		super.HandlerAttached(w);
		
		// Force text visible upon initialization if this marker has a text widget.
		if (m_bShowText && m_wMarkerText)
			m_wMarkerText.SetVisible(true);
	}

	//------------------------------------------------------------------------------------------------
	// Intercept SetTextVisible (which is called by the game when zooming in/out to hide text).
	// We force the state to true so zooming out never hides it.
	override void SetTextVisible(bool state)
	{
		if (m_bShowText && m_wMarkerText)
			m_wMarkerText.SetVisible(true);
	}

	//------------------------------------------------------------------------------------------------
	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		bool ret = super.OnMouseLeave(w, enterW, x, y);
		
		// vanilla OnMouseLeave directly calls m_wMarkerText.SetVisible(false).
		// We immediately override it back to true.
		if (m_bShowText && m_wMarkerText)
			m_wMarkerText.SetVisible(true);
			
		return ret;
	}
}
