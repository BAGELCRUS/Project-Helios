// ATHENA_IntelSpawnerDialogUI
// Binds to your custom UI layout (UI/layouts/Intel/IntelSpawnerDialog.layout).
// Populates dropdowns/spinboxes from ATHENA_IntelConfig.conf and saves + spawns the
// configured Intel prefab when the Game Master hits Escape (MenuBack) or Close.
//
// Expected Widget Names in IntelSpawnerDialog.layout:
//   - "PrefabSelect"            (SCR_SpinBoxComponent or SCR_ComboBoxComponent)
//   - "TitleInput"              (EditBoxWidget or SCR_EditBoxComponent)
//   - "TypeSelect"              (SCR_SpinBoxComponent or SCR_ComboBoxComponent)
//   - "DescriptionInput"        (MultilineEditBoxWidget, EditBoxWidget, or SCR_EditBoxComponent)
//   - "ShareTypeSelect"         (SCR_SpinBoxComponent or SCR_ComboBoxComponent)
//   - "PingGMInput"             (CheckBoxWidget, SCR_CheckboxComponent, or SCR_SpinBoxComponent)
//   - "DeleteOnCompletionInput" (CheckBoxWidget, SCR_CheckboxComponent, or SCR_SpinBoxComponent)
//   - "ActionTextInput"         (EditBoxWidget or SCR_EditBoxComponent)
//   - "ActionSoundSelect"       (SCR_SpinBoxComponent or SCR_ComboBoxComponent)
//   - "ActionDurationInput"     (SliderWidget, SCR_SliderComponent, EditBoxWidget, or SCR_EditBoxComponent)
//   - "CloseButton0"            (Optional ButtonWidget with SCR_ButtonBaseComponent)
// Handler attached to MultilineEditBoxWidget so pressing Enter or Shift+Enter inserts a newline
// and re-activates write mode so the text box stays open.
class ATHENA_MultilineEnterHandler : ScriptedWidgetEventHandler
{
	override bool OnWriteModeLeave(Widget w)
	{
		MultilineEditBoxWidget multi = MultilineEditBoxWidget.Cast(w);
		if (!multi)
			return false;

		if (Debug.KeyState(KeyCode.KC_RETURN) || Debug.KeyState(KeyCode.KC_NUMPADENTER))
		{
			multi.SetText(multi.GetText() + "\n");
			ScrollParentToBottom(multi);
			GetGame().GetCallqueue().CallLater(ReactivateWriteMode, 1, false, multi);
			return true;
		}

		return false;
	}

	protected void ReactivateWriteMode(MultilineEditBoxWidget multi)
	{
		if (multi)
		{
			multi.ActivateWriteMode();
			ScrollParentToBottom(multi);
		}
	}

	protected void ScrollParentToBottom(Widget w)
	{
		if (!w)
			return;

		ScrollLayoutWidget scroll = ScrollLayoutWidget.Cast(w.GetParent());
		if (scroll)
			scroll.SetSliderPos(0, 1);
	}
}

class ATHENA_IntelSpawnerDialogUI : SCR_ConfigurableDialogUi
{
	protected static ATHENA_IntelSpawnerComponent s_NextSpawnerComp;
	protected static ATHENA_IntelComponent s_NextEditIntelComp;

	protected ATHENA_IntelSpawnerComponent m_SpawnerComp;
	protected ATHENA_IntelComponent m_EditIntelComp;
	protected ref ATHENA_IntelConfig m_IntelConfig;

	protected Widget m_wPrefabSelect;
	protected Widget m_wTitleInput;
	protected Widget m_wTypeSelect;
	protected Widget m_wDescriptionInput;
	protected Widget m_wShareTypeSelect;
	protected Widget m_wPingGMInput;
	protected Widget m_wDeleteOnCompletionInput;
	protected Widget m_wActionTextInput;
	protected Widget m_wActionSoundSelect;
	protected Widget m_wActionDurationInput;

	protected ref ATHENA_MultilineEnterHandler m_DescriptionEnterHandler;
	protected bool m_bSpawned = false;

	//------------------------------------------------------------------------------------------------
	static ATHENA_IntelSpawnerDialogUI Open(ATHENA_IntelSpawnerComponent spawnerComp)
	{
		s_NextSpawnerComp = spawnerComp;
		s_NextEditIntelComp = null;

		ATHENA_IntelSpawnerDialogUI dialog = new ATHENA_IntelSpawnerDialogUI();
		SCR_ConfigurableDialogUi.CreateFromPreset("{5938AB21CE701D21}Configs/Dialogs/IntelDialogs.conf", "intelSpawner", dialog);
		return dialog;
	}

	//------------------------------------------------------------------------------------------------
	static ATHENA_IntelSpawnerDialogUI OpenForEdit(ATHENA_IntelComponent intelComp)
	{
		if (!intelComp)
			return null;

		s_NextSpawnerComp = null;
		s_NextEditIntelComp = intelComp;

		ATHENA_IntelSpawnerDialogUI dialog = new ATHENA_IntelSpawnerDialogUI();
		SCR_ConfigurableDialogUi.CreateFromPreset("{5938AB21CE701D21}Configs/Dialogs/IntelDialogs.conf", "intelSpawner", dialog);
		return dialog;
	}

	//------------------------------------------------------------------------------------------------
	override void OnMenuOpen(SCR_ConfigurableDialogUiPreset preset)
	{
		super.OnMenuOpen(preset);

		m_SpawnerComp = s_NextSpawnerComp;
		s_NextSpawnerComp = null;

		m_EditIntelComp = s_NextEditIntelComp;
		s_NextEditIntelComp = null;

		m_IntelConfig = ATHENA_IntelConfig.LoadConfig();

		Widget root = GetRootWidget();
		if (!root)
			return;

		// Update header title when editing an existing Intel entity
		if (m_EditIntelComp)
		{
			Widget headerFrame = root.FindAnyWidget("HeaderFrame");
			if (headerFrame)
			{
				TextWidget headerTitle = TextWidget.Cast(headerFrame.FindAnyWidget("Text0"));
				if (headerTitle)
					headerTitle.SetText("Edit Intel");
			}
		}

		// Strip any SCR_BaseEditorAttributeUIComponent handlers (e.g. if AttributePrefab_Slider is used)
		// so they do not crash with a NULL m_InputManager outside the GM Attributes dialog.
		StripEditorAttributeHandlers(root);

		// Locate widgets in custom layout
		m_wPrefabSelect = root.FindAnyWidget("PrefabSelect");
		m_wTitleInput = root.FindAnyWidget("TitleInput");
		m_wTypeSelect = root.FindAnyWidget("TypeSelect");
		m_wDescriptionInput = root.FindAnyWidget("DescriptionInput");
		m_wShareTypeSelect = root.FindAnyWidget("ShareTypeSelect");
		m_wPingGMInput = root.FindAnyWidget("PingGMInput");
		m_wDeleteOnCompletionInput = root.FindAnyWidget("DeleteOnCompletionInput");
		m_wActionTextInput = root.FindAnyWidget("ActionTextInput");
		m_wActionSoundSelect = root.FindAnyWidget("ActionSoundSelect");
		m_wActionDurationInput = root.FindAnyWidget("ActionDurationInput");

		if (!m_wPrefabSelect)
			Print("ATHENA_IntelSpawnerDialogUI: Missing widget 'PrefabSelect' in layout!", LogLevel.WARNING);
		if (!m_wDescriptionInput)
			Print("ATHENA_IntelSpawnerDialogUI: Missing widget 'DescriptionInput' in layout!", LogLevel.WARNING);

		PopulateDefaults();

		// Allow pressing Enter inside DescriptionInput to insert a new line instead of closing write mode
		if (MultilineEditBoxWidget.Cast(m_wDescriptionInput))
		{
			m_DescriptionEnterHandler = new ATHENA_MultilineEnterHandler();
			m_wDescriptionInput.AddHandler(m_DescriptionEnterHandler);
		}

		// Optional Close/Save button support if present in layout
		Widget closeBtn = root.FindAnyWidget("CloseButton0");
		if (closeBtn)
		{
			SCR_ButtonBaseComponent btnComp = SCR_ButtonBaseComponent.Cast(closeBtn.FindHandler(SCR_ButtonBaseComponent));
			if (btnComp)
				btnComp.m_OnClicked.Insert(OnSaveAndClose);
		}

		// Escape (MenuBack) saves selections, spawns the prefab, and deletes the module
		InputManager inputMgr = GetGame().GetInputManager();
		if (inputMgr)
		{
			inputMgr.AddActionListener("MenuBack", EActionTrigger.DOWN, OnEscapePressed);
		}
	}

	//------------------------------------------------------------------------------------------------
	protected void StripEditorAttributeHandlers(Widget w)
	{
		if (!w)
			return;

		SCR_BaseEditorAttributeUIComponent attrComp = SCR_BaseEditorAttributeUIComponent.Cast(w.FindHandler(SCR_BaseEditorAttributeUIComponent));
		if (attrComp)
			w.RemoveHandler(attrComp);

		Widget child = w.GetChildren();
		while (child)
		{
			StripEditorAttributeHandlers(child);
			child = child.GetSibling();
		}
	}

	//------------------------------------------------------------------------------------------------
	protected void PopulateDefaults()
	{
		string defaultTitle = "Confidential Intel";
		string defaultType = "Document";
		string defaultDesc = "Top secret description...";
		string defaultActionText = "Read Intel";
		EAthenaIntelShareType defaultShare = EAthenaIntelShareType.Individual;
		bool defaultPingGM = false;
		bool defaultDelete = false;
		float defaultDuration = 0;

		ResourceName savedPrefab = ResourceName.Empty;
		ResourceName savedSound = ResourceName.Empty;
		string savedSoundStart = "";
		string savedSoundEnd = "";

		if (m_EditIntelComp)
		{
			defaultTitle = m_EditIntelComp.GetTitle();
			defaultType = m_EditIntelComp.GetType();
			defaultDesc = m_EditIntelComp.GetDescription();
			defaultActionText = m_EditIntelComp.GetActionText();
			defaultShare = m_EditIntelComp.GetShareType();
			defaultPingGM = m_EditIntelComp.ShouldPingGM();
			defaultDelete = m_EditIntelComp.ShouldDeleteOnCompletion();
			defaultDuration = m_EditIntelComp.GetActionDuration();
			savedSound = m_EditIntelComp.GetActionSound();
			savedSoundStart = m_EditIntelComp.GetActionSoundEventStart();
			savedSoundEnd = m_EditIntelComp.GetActionSoundEventEnd();

			IEntity editOwner = m_EditIntelComp.GetOwner();
			if (editOwner && editOwner.GetPrefabData())
				savedPrefab = editOwner.GetPrefabData().GetPrefabName();
		}
		else if (m_SpawnerComp)
		{
			defaultTitle = m_SpawnerComp.GetDefaultTitle();
			defaultType = m_SpawnerComp.GetDefaultType();
			defaultDesc = m_SpawnerComp.GetDefaultDescription();
			defaultActionText = m_SpawnerComp.GetDefaultActionText();
			defaultShare = m_SpawnerComp.GetDefaultShareType();
			defaultPingGM = m_SpawnerComp.GetDefaultPingGM();
			defaultDelete = m_SpawnerComp.GetDefaultDeleteOnCompletion();
			defaultDuration = m_SpawnerComp.GetDefaultActionDuration();
		}

		// 1. Populate Prefabs from ATHENA_IntelConfig
		int prefabIdx = 0;
		array<string> prefabNames = new array<string>();
		if (m_IntelConfig && m_IntelConfig.m_aAvailablePrefabs)
		{
			if (!savedPrefab.IsEmpty())
			{
				prefabIdx = m_IntelConfig.m_aAvailablePrefabs.Find(savedPrefab);
				if (prefabIdx < 0)
				{
					// Preserve the entity's current prefab even if not listed in ATHENA_IntelConfig.conf
					m_IntelConfig.m_aAvailablePrefabs.Insert(savedPrefab);
					prefabIdx = m_IntelConfig.m_aAvailablePrefabs.Count() - 1;
				}
			}

			foreach (ResourceName prefabRes : m_IntelConfig.m_aAvailablePrefabs)
			{
				prefabNames.Insert(ATHENA_IntelConfig.GetPrefabDisplayName(prefabRes));
			}
		}
		PopulateSelectionWidget(m_wPrefabSelect, "Intel Prefab", prefabNames, prefabIdx);

		// 2. Populate Intel Types from ATHENA_IntelConfig
		int typeIdx = 0;
		if (m_IntelConfig && m_IntelConfig.m_aIntelTypes)
		{
			typeIdx = m_IntelConfig.m_aIntelTypes.Find(defaultType);
			if (typeIdx < 0)
			{
				if (m_EditIntelComp && !defaultType.IsEmpty())
				{
					m_IntelConfig.m_aIntelTypes.Insert(defaultType);
					typeIdx = m_IntelConfig.m_aIntelTypes.Count() - 1;
				}
				else
				{
					typeIdx = 0;
				}
			}
			PopulateSelectionWidget(m_wTypeSelect, "Intel Type", m_IntelConfig.m_aIntelTypes, typeIdx);
		}

		// 3. Populate Share Types from EAthenaIntelShareType enum
		array<string> shareTypeNames = new array<string>();
		for (int i = 0; i < 5; i++)
		{
			shareTypeNames.Insert(typename.EnumToString(EAthenaIntelShareType, i));
		}
		PopulateSelectionWidget(m_wShareTypeSelect, "Share Type", shareTypeNames, defaultShare);

		// 4. Populate Action Sounds from ATHENA_IntelConfig
		int soundIdx = 0;
		array<string> soundNames = new array<string>();
		if (m_IntelConfig && m_IntelConfig.m_aActionSounds)
		{
			int partialMatchIdx = -1;
			for (int i = 0; i < m_IntelConfig.m_aActionSounds.Count(); i++)
			{
				ATHENA_IntelSoundSetting soundSetting = m_IntelConfig.m_aActionSounds[i];
				soundNames.Insert(soundSetting.m_sName);

				if (m_EditIntelComp)
				{
					if (soundSetting.m_ActionSound == savedSound
						&& soundSetting.m_sActionSoundEventStart == savedSoundStart
						&& soundSetting.m_sActionSoundEventEnd == savedSoundEnd)
					{
						soundIdx = i;
					}
					else if (partialMatchIdx < 0 && !savedSound.IsEmpty() && soundSetting.m_ActionSound == savedSound)
					{
						partialMatchIdx = i;
					}
				}
			}

			if (m_EditIntelComp && soundIdx == 0 && partialMatchIdx >= 0 && !savedSound.IsEmpty())
				soundIdx = partialMatchIdx;
		}
		PopulateSelectionWidget(m_wActionSoundSelect, "Action Sound", soundNames, soundIdx);

		// 5. Set texts, booleans, and duration
		SetWidgetText(m_wTitleInput, "Intel Title", defaultTitle);
		SetWidgetText(m_wDescriptionInput, "Intel Description", defaultDesc);
		SetWidgetText(m_wActionTextInput, "Action Text", defaultActionText);
		SetWidgetBool(m_wPingGMInput, "Ping Game Master?", defaultPingGM);
		SetWidgetBool(m_wDeleteOnCompletionInput, "Delete on Completion?", defaultDelete);
		SetWidgetFloat(m_wActionDurationInput, "Action Duration (s)", defaultDuration);
	}

	//------------------------------------------------------------------------------------------------
	protected void PopulateSelectionWidget(Widget w, string label, array<string> items, int selectedIdx)
	{
		if (!w || !items)
			return;

		SCR_SelectionWidgetComponent selComp = SCR_SelectionWidgetComponent.Cast(w.FindHandler(SCR_SelectionWidgetComponent));
		if (!selComp)
			selComp = SCR_SelectionWidgetComponent.GetSelectionComponent("", w, true);

		if (selComp)
		{
			if (!label.IsEmpty())
			{
				selComp.UseLabel(true);
				selComp.SetLabel(label);
			}
			selComp.ClearAll();
			foreach (string item : items)
			{
				selComp.AddItem(item);
			}
			if (!items.IsEmpty())
			{
				int clamped = Math.ClampInt(selectedIdx, 0, items.Count() - 1);
				selComp.SetCurrentItem(clamped);
			}
		}
	}

	//------------------------------------------------------------------------------------------------
	protected int ReadSelectionIndex(Widget w, int fallback = 0)
	{
		if (!w)
			return fallback;

		SCR_SelectionWidgetComponent selComp = SCR_SelectionWidgetComponent.Cast(w.FindHandler(SCR_SelectionWidgetComponent));
		if (!selComp)
			selComp = SCR_SelectionWidgetComponent.GetSelectionComponent("", w, true);

		if (selComp)
			return selComp.GetCurrentIndex();

		return fallback;
	}

	//------------------------------------------------------------------------------------------------
	protected void SetWidgetText(Widget w, string label, string val)
	{
		if (!w)
			return;

		SCR_EditBoxComponent editComp = SCR_EditBoxComponent.Cast(w.FindHandler(SCR_EditBoxComponent));
		if (!editComp)
			editComp = SCR_EditBoxComponent.GetEditBoxComponent("", w, true);
		if (editComp)
		{
			if (!label.IsEmpty())
			{
				editComp.UseLabel(true);
				editComp.SetLabel(label);
			}
			editComp.SetValue(val);
			return;
		}

		EditBoxWidget editW = EditBoxWidget.Cast(w);
		if (editW)
		{
			editW.SetText(val);
			return;
		}

		MultilineEditBoxWidget multiW = MultilineEditBoxWidget.Cast(w);
		if (multiW)
		{
			multiW.SetText(val);
			return;
		}

		TextWidget textW = TextWidget.Cast(w);
		if (textW)
			textW.SetText(val);
	}

	//------------------------------------------------------------------------------------------------
	protected string ReadWidgetText(Widget w, string fallback)
	{
		if (!w)
			return fallback;

		SCR_EditBoxComponent editComp = SCR_EditBoxComponent.Cast(w.FindHandler(SCR_EditBoxComponent));
		if (!editComp)
			editComp = SCR_EditBoxComponent.GetEditBoxComponent("", w, true);
		if (editComp)
			return editComp.GetValue();

		EditBoxWidget editW = EditBoxWidget.Cast(w);
		if (editW)
			return editW.GetText();

		MultilineEditBoxWidget multiW = MultilineEditBoxWidget.Cast(w);
		if (multiW)
			return multiW.GetText();

		TextWidget textW = TextWidget.Cast(w);
		if (textW)
			return textW.GetText();

		return fallback;
	}

	//------------------------------------------------------------------------------------------------
	protected void SetWidgetBool(Widget w, string label, bool val)
	{
		if (!w)
			return;

		SCR_CheckboxComponent checkComp = SCR_CheckboxComponent.Cast(w.FindHandler(SCR_CheckboxComponent));
		if (!checkComp)
			checkComp = SCR_CheckboxComponent.GetCheckboxComponent("", w, true);
		if (checkComp)
		{
			if (!label.IsEmpty())
			{
				checkComp.UseLabel(true);
				checkComp.SetLabel(label);
			}
			checkComp.SetChecked(val);
			return;
		}

		CheckBoxWidget checkW = CheckBoxWidget.Cast(w);
		if (checkW)
		{
			checkW.SetChecked(val);
			return;
		}

		array<string> boolItems = { "No", "Yes" };
		int idx = 0;
		if (val)
			idx = 1;
		PopulateSelectionWidget(w, label, boolItems, idx);
	}

	//------------------------------------------------------------------------------------------------
	protected bool ReadWidgetBool(Widget w, bool fallback)
	{
		if (!w)
			return fallback;

		SCR_CheckboxComponent checkComp = SCR_CheckboxComponent.Cast(w.FindHandler(SCR_CheckboxComponent));
		if (!checkComp)
			checkComp = SCR_CheckboxComponent.GetCheckboxComponent("", w, true);
		if (checkComp)
			return checkComp.IsChecked();

		CheckBoxWidget checkW = CheckBoxWidget.Cast(w);
		if (checkW)
			return checkW.IsChecked();

		SCR_SelectionWidgetComponent selComp = SCR_SelectionWidgetComponent.Cast(w.FindHandler(SCR_SelectionWidgetComponent));
		if (!selComp)
			selComp = SCR_SelectionWidgetComponent.GetSelectionComponent("", w, true);
		if (selComp)
			return (selComp.GetCurrentIndex() == 1);

		return fallback;
	}

	//------------------------------------------------------------------------------------------------
	protected void SetWidgetFloat(Widget w, string label, float val)
	{
		if (!w)
			return;

		SCR_SliderComponent sliderComp = SCR_SliderComponent.Cast(w.FindHandler(SCR_SliderComponent));
		if (!sliderComp)
			sliderComp = SCR_SliderComponent.GetSliderComponent("", w, true);
		if (sliderComp)
		{
			if (!label.IsEmpty())
			{
				sliderComp.UseLabel(true);
				sliderComp.SetLabel(label);
			}
			sliderComp.SetSliderSettings(0, 60, 1, "%1 s");
			sliderComp.SetValue(val);
			return;
		}

		SliderWidget sliderW = SliderWidget.Cast(w);
		if (sliderW)
		{
			sliderW.SetRange(0, 60);
			sliderW.SetStep(1);
			sliderW.SetCurrent(val);
			return;
		}

		SetWidgetText(w, label, val.ToString());
	}

	//------------------------------------------------------------------------------------------------
	protected float ReadWidgetFloat(Widget w, float fallback)
	{
		if (!w)
			return fallback;

		SCR_SliderComponent sliderComp = SCR_SliderComponent.Cast(w.FindHandler(SCR_SliderComponent));
		if (!sliderComp)
			sliderComp = SCR_SliderComponent.GetSliderComponent("", w, true);
		if (sliderComp)
			return sliderComp.GetValue();

		SliderWidget sliderW = SliderWidget.Cast(w);
		if (sliderW)
			return sliderW.GetCurrent();

		string textVal = ReadWidgetText(w, "");
		if (!textVal.IsEmpty())
			return Math.Clamp(textVal.ToFloat(), 0, 60);

		return fallback;
	}

	//------------------------------------------------------------------------------------------------
	protected bool IsWidgetInWriteMode(Widget w)
	{
		if (!w)
			return false;

		SCR_EditBoxComponent editComp = SCR_EditBoxComponent.Cast(w.FindHandler(SCR_EditBoxComponent));
		if (!editComp)
			editComp = SCR_EditBoxComponent.GetEditBoxComponent("", w, true);
		if (editComp && editComp.IsInWriteMode())
			return true;

		EditBoxWidget editW = EditBoxWidget.Cast(w);
		if (editW && editW.IsInWriteMode())
			return true;

		MultilineEditBoxWidget multiW = MultilineEditBoxWidget.Cast(w);
		if (multiW && multiW.IsInWriteMode())
			return true;

		return false;
	}

	//------------------------------------------------------------------------------------------------
	protected void OnEscapePressed()
	{
		// If currently typing inside an edit box, let Escape exit write mode first
		if (IsWidgetInWriteMode(m_wTitleInput) || IsWidgetInWriteMode(m_wDescriptionInput) || IsWidgetInWriteMode(m_wActionTextInput) || IsWidgetInWriteMode(m_wActionDurationInput))
			return;

		OnSaveAndClose();
	}

	//------------------------------------------------------------------------------------------------
	protected void OnSaveAndClose()
	{
		SaveAndSpawn();
		Close();
	}

	//------------------------------------------------------------------------------------------------
	protected void SaveAndSpawn()
	{
		if (m_bSpawned || (!m_SpawnerComp && !m_EditIntelComp))
			return;

		m_bSpawned = true;

		// Fallbacks based on active mode (edit vs spawn)
		ResourceName fallbackPrefab = ResourceName.Empty;
		string fallbackTitle = "Confidential Intel";
		string fallbackType = "Document";
		string fallbackDesc = "Top secret description...";
		string fallbackActionText = "Read Intel";
		EAthenaIntelShareType fallbackShare = EAthenaIntelShareType.Individual;
		bool fallbackPingGM = false;
		bool fallbackDelete = false;
		float fallbackDuration = 0;

		if (m_EditIntelComp)
		{
			IEntity editOwner = m_EditIntelComp.GetOwner();
			if (editOwner && editOwner.GetPrefabData())
				fallbackPrefab = editOwner.GetPrefabData().GetPrefabName();
			fallbackTitle = m_EditIntelComp.GetTitle();
			fallbackType = m_EditIntelComp.GetType();
			fallbackDesc = m_EditIntelComp.GetDescription();
			fallbackActionText = m_EditIntelComp.GetActionText();
			fallbackShare = m_EditIntelComp.GetShareType();
			fallbackPingGM = m_EditIntelComp.ShouldPingGM();
			fallbackDelete = m_EditIntelComp.ShouldDeleteOnCompletion();
			fallbackDuration = m_EditIntelComp.GetActionDuration();
		}
		else if (m_SpawnerComp)
		{
			fallbackPrefab = m_SpawnerComp.GetDefaultFallbackPrefab();
			fallbackTitle = m_SpawnerComp.GetDefaultTitle();
			fallbackType = m_SpawnerComp.GetDefaultType();
			fallbackDesc = m_SpawnerComp.GetDefaultDescription();
			fallbackActionText = m_SpawnerComp.GetDefaultActionText();
			fallbackShare = m_SpawnerComp.GetDefaultShareType();
			fallbackPingGM = m_SpawnerComp.GetDefaultPingGM();
			fallbackDelete = m_SpawnerComp.GetDefaultDeleteOnCompletion();
			fallbackDuration = m_SpawnerComp.GetDefaultActionDuration();
		}

		// 1. Selected Prefab
		ResourceName selectedPrefab = fallbackPrefab;
		if (m_IntelConfig && m_IntelConfig.m_aAvailablePrefabs && !m_IntelConfig.m_aAvailablePrefabs.IsEmpty())
		{
			int prefabIdx = Math.ClampInt(ReadSelectionIndex(m_wPrefabSelect, 0), 0, m_IntelConfig.m_aAvailablePrefabs.Count() - 1);
			selectedPrefab = m_IntelConfig.m_aAvailablePrefabs[prefabIdx];
		}

		// 2. Title
		string title = ReadWidgetText(m_wTitleInput, fallbackTitle);

		// 3. Intel Type
		string intelType = fallbackType;
		if (m_IntelConfig && m_IntelConfig.m_aIntelTypes && !m_IntelConfig.m_aIntelTypes.IsEmpty())
		{
			int typeIdx = Math.ClampInt(ReadSelectionIndex(m_wTypeSelect, 0), 0, m_IntelConfig.m_aIntelTypes.Count() - 1);
			intelType = m_IntelConfig.m_aIntelTypes[typeIdx];
		}

		// 4. Description
		string description = ReadWidgetText(m_wDescriptionInput, fallbackDesc);

		// 5. Share Type
		EAthenaIntelShareType shareType = Math.ClampInt(ReadSelectionIndex(m_wShareTypeSelect, fallbackShare), 0, 4);

		// 6. Ping GM & Delete on Completion
		bool pingGM = ReadWidgetBool(m_wPingGMInput, fallbackPingGM);
		bool deleteOnCompletion = ReadWidgetBool(m_wDeleteOnCompletionInput, fallbackDelete);

		// 7. Action Text
		string actionText = ReadWidgetText(m_wActionTextInput, fallbackActionText);

		// 8. Action Sound Index
		int soundIdx = 0;
		if (m_IntelConfig && m_IntelConfig.m_aActionSounds && !m_IntelConfig.m_aActionSounds.IsEmpty())
		{
			soundIdx = Math.ClampInt(ReadSelectionIndex(m_wActionSoundSelect, 0), 0, m_IntelConfig.m_aActionSounds.Count() - 1);
		}

		// 9. Action Duration
		float duration = ReadWidgetFloat(m_wActionDurationInput, fallbackDuration);

		if (m_EditIntelComp)
		{
			m_EditIntelComp.RequestUpdateIntel(
				selectedPrefab,
				title,
				intelType,
				description,
				actionText,
				shareType,
				pingGM,
				deleteOnCompletion,
				soundIdx,
				duration
			);
		}
		else if (m_SpawnerComp)
		{
			m_SpawnerComp.SpawnConfiguredIntel(
				selectedPrefab,
				title,
				intelType,
				description,
				actionText,
				shareType,
				pingGM,
				deleteOnCompletion,
				soundIdx,
				duration
			);
		}
	}

	//------------------------------------------------------------------------------------------------
	override void HandlerDeattached(Widget w)
	{
		// Ensure selections are saved and the prefab is spawned even if the dialog closes via base MenuBack
		if (!m_bSpawned)
			SaveAndSpawn();

		InputManager inputMgr = GetGame().GetInputManager();
		if (inputMgr)
		{
			inputMgr.RemoveActionListener("MenuBack", EActionTrigger.DOWN, OnEscapePressed);
		}

		super.HandlerDeattached(w);
	}
}

