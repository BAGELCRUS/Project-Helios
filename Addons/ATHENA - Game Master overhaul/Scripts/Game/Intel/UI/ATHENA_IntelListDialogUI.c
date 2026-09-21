// Attach this to the root of the IntelTabButton.layout
class ATHENA_IntelTabButtonComponent : SCR_ButtonBaseComponent
{
	protected ATHENA_IntelListDialogUI m_ParentDialog;
	protected ATHENA_IntelData m_IntelData;

	void Init(ATHENA_IntelListDialogUI parent, ATHENA_IntelData data)
	{
		m_ParentDialog = parent;
		m_IntelData = data;
		m_OnClicked.Insert(OnTabClicked);
	}

	protected void OnTabClicked()
	{
		if (m_ParentDialog && m_IntelData)
		{
			m_ParentDialog.SelectIntel(m_IntelData);
		}
	}
}

class ATHENA_IntelListDialogUI : SCR_ConfigurableDialogUi
{
	protected Widget m_wTabListArea;
	protected TextWidget m_wTitleText;
	protected TextWidget m_wTypeText;
	protected RichTextWidget m_wDescriptionText;
	protected ATHENA_IntelData m_SelectedIntel;

	[Attribute("{E024DA643662834C}UI/layouts/Intel/IntelTabButton.layout", UIWidgets.ResourcePickerThumbnail, desc: "Prefab used for the tab buttons on the left")]
	protected ResourceName m_sTabButtonPrefab;

	static ATHENA_IntelListDialogUI OpenIntelList()
	{
		ATHENA_IntelListDialogUI dialog = new ATHENA_IntelListDialogUI();
		// The preset 'intelList' needs to be configured in IntelDialogs.conf pointing to the new layout
		SCR_ConfigurableDialogUi.CreateFromPreset("{5938AB21CE701D21}Configs/Dialogs/IntelDialogs.conf", "intelList", dialog);
		return dialog;
	}

	override void OnMenuOpen(SCR_ConfigurableDialogUiPreset preset)
	{
		super.OnMenuOpen(preset);

		Widget root = GetRootWidget();
		if (!root)
			return;

		m_wTabListArea = root.FindAnyWidget("TabListArea"); // A VerticalLayoutWidget inside a ScrollLayoutWidget
		m_wTitleText = TextWidget.Cast(root.FindAnyWidget("Title_Text"));
		m_wTypeText = TextWidget.Cast(root.FindAnyWidget("Type_Text"));
		m_wDescriptionText = RichTextWidget.Cast(root.FindAnyWidget("IntelBodyText"));

		// Clear details initially until an item is selected
		if (m_wTitleText)
			m_wTitleText.SetText("");
		if (m_wTypeText)
			m_wTypeText.SetText("");
		if (m_wDescriptionText)
			m_wDescriptionText.SetText("");

		// Hook up the close button
		Widget closeBtn = root.FindAnyWidget("CloseButton0");
		if (closeBtn)
		{
			SCR_ButtonBaseComponent btnComp = SCR_ButtonBaseComponent.Cast(closeBtn.FindHandler(SCR_ButtonBaseComponent));
			if (btnComp)
			{
				btnComp.m_OnClicked.Insert(Close);
			}
		}

		// Hook up the share button
		Widget shareBtn = root.FindAnyWidget("ShareButton0");
		if (shareBtn)
		{
			SCR_ButtonBaseComponent btnComp = SCR_ButtonBaseComponent.Cast(shareBtn.FindHandler(SCR_ButtonBaseComponent));
			if (btnComp)
			{
				btnComp.m_OnClicked.Insert(OnShareClicked);
			}
		}

		InputManager inputMgr = GetGame().GetInputManager();
		if (inputMgr)
		{
			inputMgr.AddActionListener("MenuBack", EActionTrigger.DOWN, Close);
			inputMgr.AddActionListener("PlayerList", EActionTrigger.DOWN, OnShareClicked);
			inputMgr.AddActionListener("Inventory", EActionTrigger.DOWN, OnShareClicked);
		}

		PopulateTabs();
	}

	protected void PopulateTabs()
	{
		Print("ATHENA_IntelListDialogUI: PopulateTabs called.", LogLevel.NORMAL);
		
		if (!m_wTabListArea)
		{
			Print("ATHENA_IntelListDialogUI: m_wTabListArea is null! Could not find TabListArea widget.", LogLevel.ERROR);
			return;
		}

		ATHENA_PlayerIntelComponent pic = ATHENA_PlayerIntelComponent.GetLocalInstance();
		if (!pic)
		{
			Print("ATHENA_IntelListDialogUI: ATHENA_PlayerIntelComponent.GetLocalInstance() returned null!", LogLevel.ERROR);
			return;
		}

		array<ref ATHENA_IntelData> collectedIntel = pic.GetCollectedIntel();
		Print("ATHENA_IntelListDialogUI: Found " + collectedIntel.Count() + " collected intel items.", LogLevel.NORMAL);
		
		// Fallback for m_sTabButtonPrefab if it wasn't initialized correctly
		if (m_sTabButtonPrefab == string.Empty)
		{
			m_sTabButtonPrefab = "{E024DA643662834C}UI/layouts/Intel/IntelTabButton.layout";
			Print("ATHENA_IntelListDialogUI: m_sTabButtonPrefab was empty, setting to default: " + m_sTabButtonPrefab, LogLevel.WARNING);
		}

		// Spawn a button for each intel
		foreach (ATHENA_IntelData intel : collectedIntel)
		{
			Print("ATHENA_IntelListDialogUI: Spawning tab for intel: " + intel.m_sTitle, LogLevel.NORMAL);
			Widget tabBtnWidget = GetGame().GetWorkspace().CreateWidgets(m_sTabButtonPrefab, m_wTabListArea);
			if (!tabBtnWidget)
			{
				Print("ATHENA_IntelListDialogUI: Failed to create widget from prefab: " + m_sTabButtonPrefab, LogLevel.ERROR);
				continue;
			}
			
			// Change text of button
			TextWidget textW = TextWidget.Cast(tabBtnWidget.FindAnyWidget("IntelTitle")); // Widget is named IntelTitle in layout
			if (textW)
				textW.SetText(intel.m_sTitle);
			else
				Print("ATHENA_IntelListDialogUI: Could not find 'IntelTitle' widget in spawned tab.", LogLevel.WARNING);

			// Handle SHARED text widget visibility
			TextWidget sharedW = TextWidget.Cast(tabBtnWidget.FindAnyWidget("SHARED"));
			if (sharedW)
				sharedW.SetVisible(intel.m_ShareType == EAthenaIntelShareType.Group || intel.m_ShareType == EAthenaIntelShareType.Faction || intel.m_ShareType == EAthenaIntelShareType.Everyone);

			// Handle Method and TIME
			TextWidget methodW = TextWidget.Cast(tabBtnWidget.FindAnyWidget("METHOOD"));
			if (methodW)
				methodW.SetText(intel.m_sMethod);

			TextWidget timeW = TextWidget.Cast(tabBtnWidget.FindAnyWidget("TIME"));
			if (timeW)
				timeW.SetText(intel.m_sTime);

			ATHENA_IntelTabButtonComponent btnComp = ATHENA_IntelTabButtonComponent.Cast(tabBtnWidget.FindHandler(ATHENA_IntelTabButtonComponent));
			if (btnComp)
			{
				btnComp.Init(this, intel);
				Print("ATHENA_IntelListDialogUI: Successfully initialized btnComp for: " + intel.m_sTitle, LogLevel.NORMAL);
			}
			else
			{
				Print("ATHENA_IntelListDialogUI: Could not find ATHENA_IntelTabButtonComponent on spawned tab.", LogLevel.ERROR);
			}
		}
		
		// Removed auto-select so details only populate when an item is explicitly selected
		/*
		if (!collectedIntel.IsEmpty())
		{
			SelectIntel(collectedIntel[0]);
		}
		*/
	}

	void SelectIntel(ATHENA_IntelData intel)
	{
		m_SelectedIntel = intel;
		if (m_wTitleText) m_wTitleText.SetText(intel.m_sTitle);
		if (m_wTypeText) m_wTypeText.SetText(intel.m_sType);
		if (m_wDescriptionText) m_wDescriptionText.SetText(intel.m_sDescription);
	}

	protected void OnShareClicked()
	{
		if (!m_SelectedIntel)
			return;

		ATHENA_PlayerIntelComponent pic = ATHENA_PlayerIntelComponent.GetLocalInstance();
		if (pic)
		{
			pic.ShareIntelNearby(m_SelectedIntel.m_sTitle, m_SelectedIntel.m_sType, m_SelectedIntel.m_sDescription);
			pic.MarkIntelAsSharedLocal(m_SelectedIntel.m_sTitle);
		}
	}

	override void HandlerDeattached(Widget w)
	{
		super.HandlerDeattached(w);
		InputManager inputMgr = GetGame().GetInputManager();
		if (inputMgr)
		{
			inputMgr.RemoveActionListener("MenuBack", EActionTrigger.DOWN, Close);
			inputMgr.RemoveActionListener("PlayerList", EActionTrigger.DOWN, OnShareClicked);
			inputMgr.RemoveActionListener("Inventory", EActionTrigger.DOWN, OnShareClicked);
		}
	}
}
