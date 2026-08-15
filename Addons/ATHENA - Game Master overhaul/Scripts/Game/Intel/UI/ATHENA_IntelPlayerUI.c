// ATHENA_IntelPlayerUI
// Manages the Intel Player Dialog UI using the standard SCR_ConfigurableDialogUi system.
class ATHENA_IntelPlayerUI : SCR_ConfigurableDialogUi
{
	protected TextWidget m_wIntelTitle;
	protected RichTextWidget m_wIntelDescription;

	static string s_sNextTitle;
	static string s_sNextDescription;

	//---------------------------------------------------------------------
	// Static entry point: set data and tell the Dialog Manager to spawn it
	//---------------------------------------------------------------------
	static ATHENA_IntelPlayerUI Open(string title, string description)
	{
		s_sNextTitle = title;
		s_sNextDescription = description;
		
		// Create the dialog using the preset from our new config file
		ATHENA_IntelPlayerUI dialog = new ATHENA_IntelPlayerUI();
		SCR_ConfigurableDialogUi.CreateFromPreset("{69FB32E48756F0A4}Configs/Dialogs/IntelDialogs.conf", "intel", dialog);
		return dialog;
	}

	//---------------------------------------------------------------------
	// Dialog hooks
	//---------------------------------------------------------------------
	override void OnMenuOpen(SCR_ConfigurableDialogUiPreset preset)
	{
		// Let the base dialog handle its own init
		super.OnMenuOpen(preset);
		
		Widget root = GetRootWidget();
		if (!root)
			return;

		m_wIntelTitle = TextWidget.Cast(root.FindAnyWidget("BodyTitleText"));
		m_wIntelDescription = RichTextWidget.Cast(root.FindAnyWidget("BodyDescriptionText"));

		if (m_wIntelTitle)
			m_wIntelTitle.SetText(s_sNextTitle);

		if (m_wIntelDescription)
			m_wIntelDescription.SetText(s_sNextDescription);

		// Hook up the close button prefab
		Widget closeBtn = root.FindAnyWidget("CloseButton0");
		if (closeBtn)
		{
			SCR_ButtonBaseComponent btnComp = SCR_ButtonBaseComponent.Cast(closeBtn.FindHandler(SCR_ButtonBaseComponent));
			if (btnComp)
			{
				btnComp.m_OnClicked.Insert(Close);
			}
		}

		// Register Escape key to trigger Close()
		InputManager inputMgr = GetGame().GetInputManager();
		if (inputMgr)
		{
			inputMgr.AddActionListener("MenuBack", EActionTrigger.DOWN, Close);
		}
	}

	override void HandlerDeattached(Widget w)
	{
		super.HandlerDeattached(w);
		
		// Clean up the action listener when the dialog is destroyed
		InputManager inputMgr = GetGame().GetInputManager();
		if (inputMgr)
		{
			inputMgr.RemoveActionListener("MenuBack", EActionTrigger.DOWN, Close);
		}
	}
}
