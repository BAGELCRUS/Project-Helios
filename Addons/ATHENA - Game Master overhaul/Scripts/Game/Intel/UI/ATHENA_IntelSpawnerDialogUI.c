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
class ATHENA_IntelSpawnerDialogUI : SCR_ConfigurableDialogUi
{
	protected static ATHENA_IntelSpawnerComponent s_NextSpawnerComp;

	protected ATHENA_IntelSpawnerComponent m_SpawnerComp;
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

	protected bool m_bSpawned = false;

	//------------------------------------------------------------------------------------------------
	static ATHENA_IntelSpawnerDialogUI Open(ATHENA_IntelSpawnerComponent spawnerComp)
	{
		s_NextSpawnerComp = spawnerComp;

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

		m_IntelConfig = ATHENA_IntelConfig.LoadConfig();

		Widget root = GetRootWidget();
		if (!root)
			return;

		// Strip any SCR_BaseEditorAttributeUIComponent handlers (e.g. if AttributePrefab_Slider is used)
		// so they do not crash with a NULL m_InputManager outside the GM Attributes dialog.
		StripEditorAttributeHandlers(root);

		// Locate widgets in custom layout
		m_wPrefabSelect = root.FindAnyWidget("PrefabSelect");
		if (!m_wPrefabSelect)
		{
			Widget formContainer = root.FindAnyWidget("VerticalLayout0");
			if (formContainer)
			{
				m_wPrefabSelect = GetGame().GetWorkspace().CreateWidgets("{4B5AE6E64037FFB4}UI/layouts/WidgetLibrary/ComboBox/WLib_ComboBox.layout", formContainer);
				if (m_wPrefabSelect)
				{
					m_wPrefabSelect.SetName("PrefabSelect");
					LayoutSlot.SetHorizontalAlign(m_wPrefabSelect, LayoutHorizontalAlign.Stretch);
					LayoutSlot.SetPadding(m_wPrefabSelect, 4, 4, 4, 4);
				}
			}
		}
		m_wTitleInput = root.FindAnyWidget("TitleInput");
		m_wTypeSelect = root.FindAnyWidget("TypeSelect");
		m_wDescriptionInput = root.FindAnyWidget("DescriptionInput");
		m_wShareTypeSelect = root.FindAnyWidget("ShareTypeSelect");
		m_wPingGMInput = root.FindAnyWidget("PingGMInput");
		m_wDeleteOnCompletionInput = root.FindAnyWidget("DeleteOnCompletionInput");
		m_wActionTextInput = root.FindAnyWidget("ActionTextInput");
		m_wActionSoundSelect = root.FindAnyWidget("ActionSoundSelect");
		m_wActionDurationInput = root.FindAnyWidget("ActionDurationInput");

		PopulateDefaults();

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

		if (m_SpawnerComp)
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
		array<string> prefabNames = new array<string>();
		if (m_IntelConfig && m_IntelConfig.m_aAvailablePrefabs)
		{
			foreach (ResourceName prefabRes : m_IntelConfig.m_aAvailablePrefabs)
			{
				prefabNames.Insert(ATHENA_IntelConfig.GetPrefabDisplayName(prefabRes));
			}
		}
		PopulateSelectionWidget(m_wPrefabSelect, "Intel Prefab", prefabNames, 0);

		// 2. Populate Intel Types from ATHENA_IntelConfig
		int typeIdx = 0;
		if (m_IntelConfig && m_IntelConfig.m_aIntelTypes)
		{
			typeIdx = m_IntelConfig.m_aIntelTypes.Find(defaultType);
			if (typeIdx < 0)
				typeIdx = 0;
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
		array<string> soundNames = new array<string>();
		if (m_IntelConfig && m_IntelConfig.m_aActionSounds)
		{
			foreach (ATHENA_IntelSoundSetting soundSetting : m_IntelConfig.m_aActionSounds)
			{
				soundNames.Insert(soundSetting.m_sName);
			}
		}
		PopulateSelectionWidget(m_wActionSoundSelect, "Action Sound", soundNames, 0);

		// 5. Set default texts, booleans, and duration
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
		if (m_bSpawned || !m_SpawnerComp)
			return;

		m_bSpawned = true;

		// 1. Selected Prefab
		ResourceName selectedPrefab = m_SpawnerComp.GetDefaultFallbackPrefab();
		if (m_IntelConfig && m_IntelConfig.m_aAvailablePrefabs && !m_IntelConfig.m_aAvailablePrefabs.IsEmpty())
		{
			int prefabIdx = Math.ClampInt(ReadSelectionIndex(m_wPrefabSelect, 0), 0, m_IntelConfig.m_aAvailablePrefabs.Count() - 1);
			selectedPrefab = m_IntelConfig.m_aAvailablePrefabs[prefabIdx];
		}

		// 2. Title
		string title = ReadWidgetText(m_wTitleInput, m_SpawnerComp.GetDefaultTitle());

		// 3. Intel Type
		string intelType = m_SpawnerComp.GetDefaultType();
		if (m_IntelConfig && m_IntelConfig.m_aIntelTypes && !m_IntelConfig.m_aIntelTypes.IsEmpty())
		{
			int typeIdx = Math.ClampInt(ReadSelectionIndex(m_wTypeSelect, 0), 0, m_IntelConfig.m_aIntelTypes.Count() - 1);
			intelType = m_IntelConfig.m_aIntelTypes[typeIdx];
		}

		// 4. Description
		string description = ReadWidgetText(m_wDescriptionInput, m_SpawnerComp.GetDefaultDescription());

		// 5. Share Type
		EAthenaIntelShareType shareType = Math.ClampInt(ReadSelectionIndex(m_wShareTypeSelect, m_SpawnerComp.GetDefaultShareType()), 0, 4);

		// 6. Ping GM & Delete on Completion
		bool pingGM = ReadWidgetBool(m_wPingGMInput, m_SpawnerComp.GetDefaultPingGM());
		bool deleteOnCompletion = ReadWidgetBool(m_wDeleteOnCompletionInput, m_SpawnerComp.GetDefaultDeleteOnCompletion());

		// 7. Action Text
		string actionText = ReadWidgetText(m_wActionTextInput, m_SpawnerComp.GetDefaultActionText());

		// 8. Action Sound Index
		int soundIdx = 0;
		if (m_IntelConfig && m_IntelConfig.m_aActionSounds && !m_IntelConfig.m_aActionSounds.IsEmpty())
		{
			soundIdx = Math.ClampInt(ReadSelectionIndex(m_wActionSoundSelect, 0), 0, m_IntelConfig.m_aActionSounds.Count() - 1);
		}

		// 9. Action Duration
		float duration = ReadWidgetFloat(m_wActionDurationInput, m_SpawnerComp.GetDefaultActionDuration());

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

