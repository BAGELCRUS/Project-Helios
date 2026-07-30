//------------------------------------------------------------------------------------------------
//! ATH_NightVisionToolbarAction
//! Athena - Game Master Overhaul
//!
//! A toggle toolbar action for the editor to enable Night Vision.
//! Supports Green (G) and White Phosphorus (WP) color modes, selectable
//! via the Athena settings SubMenu in the Helios settings tab.
//! Can be added to SharedToolbar.conf via Workbench.
//------------------------------------------------------------------------------------------------

[BaseContainerProps(), SCR_BaseContainerCustomTitleUIInfo("m_Info")]
class ATH_NightVisionToolbarAction : SCR_BaseToggleToolbarAction
{
	[Attribute("{AACFACA17D0A10CA}Assets/NV/Colors/GM_NV_G.emat", UIWidgets.ResourceNamePicker, "The Green HDR material for NVG.", "emat")]
	protected ResourceName m_sHDRMaterial_Green;

	[Attribute("{08BC012E8984531F}Assets/NV/Colors/GM_NV_WP.emat", UIWidgets.ResourceNamePicker, "The White Phosphorus HDR material for NVG.", "emat")]
	protected ResourceName m_sHDRMaterial_WP;

	protected static const ResourceName NORMAL_HDR = "{9DEECCABE8357209}Common/Postprocess/HDR.emat";
	protected static const int NV_HDR_SLOT = 16;

	protected static bool m_s_bNVActive;
	protected static bool m_s_bEffectApplied;

	// Keep a static reference so the settings handler can trigger a hot-swap
	protected static ATH_NightVisionToolbarAction s_Instance;

	//------------------------------------------------------------------------------------------------
	override void OnInit(SCR_ActionsToolbarEditorUIComponent toolbar)
	{
		super.OnInit(toolbar);
		s_Instance = this;
		
		InputManager inputManager = GetGame().GetInputManager();
		if (inputManager)
		{
			inputManager.AddActionListener("ATH_ToggleNightVision", EActionTrigger.DOWN, OnHotkeyToggled);
		}
	}

	//------------------------------------------------------------------------------------------------
	override void OnExit(SCR_ActionsToolbarEditorUIComponent toolbar)
	{
		InputManager inputManager = GetGame().GetInputManager();
		if (inputManager)
		{
			inputManager.RemoveActionListener("ATH_ToggleNightVision", EActionTrigger.DOWN, OnHotkeyToggled);
		}
		
		if (s_Instance == this)
			s_Instance = null;
		
		super.OnExit(toolbar);
	}

	//------------------------------------------------------------------------------------------------
	protected void OnHotkeyToggled()
	{
		// Manually trigger the toolbar button's Perform action
		Perform(null, new set<SCR_EditableEntityComponent>, vector.Zero, 0);
	}

	//------------------------------------------------------------------------------------------------
	override bool IsServer()
	{
		return false;
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBeShown(SCR_EditableEntityComponent hoveredEntity, notnull set<SCR_EditableEntityComponent> selectedEntities, vector cursorWorldPosition, int flags)
	{
		const SCR_EditorManagerEntity editorManager = SCR_EditorManagerEntity.GetInstance();
		return editorManager && !editorManager.IsLimited();
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBePerformed(SCR_EditableEntityComponent hoveredEntity, notnull set<SCR_EditableEntityComponent> selectedEntities, vector cursorWorldPosition, int flags)
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------
	override void Perform(SCR_EditableEntityComponent hoveredEntity, notnull set<SCR_EditableEntityComponent> selectedEntities, vector cursorWorldPosition, int flags, int param = -1)
	{
		SetNightVision(!m_s_bNVActive);
	}

	//------------------------------------------------------------------------------------------------
	//! Returns the correct HDR material based on the user's color preference.
	protected ResourceName GetSelectedMaterial()
	{
		int color = ATH_AthenaSettingsSubMenu.GetSelectedNVColor();
		if (color == ATH_AthenaSettingsSubMenu.NV_COLOR_WP)
			return m_sHDRMaterial_WP;

		return m_sHDRMaterial_Green;
	}

	//------------------------------------------------------------------------------------------------
	protected void SetNightVision(bool enable)
	{
		const BaseWorld world = GetGame().GetWorld();
		if (!world)
			return;

		const int cameraId = world.GetCurrentCameraId();

		if (enable)
		{
			ResourceName mat = GetSelectedMaterial();
			if (mat.IsEmpty())
			{
				Print("[Athena|NVG] No HDR material assigned for the selected NV color mode - skipped.", LogLevel.WARNING);
				return;
			}

			world.SetCameraPostProcessEffect(cameraId, NV_HDR_SLOT, PostProcessEffectType.HDR, mat);
			m_s_bEffectApplied = true;
			Print("[Athena|NVG] Night Vision ENABLED.", LogLevel.NORMAL);
		}
		else
		{
			if (m_s_bEffectApplied)
			{
				world.SetCameraPostProcessEffect(cameraId, NV_HDR_SLOT, PostProcessEffectType.HDR, NORMAL_HDR);
				m_s_bEffectApplied = false;
				Print("[Athena|NVG] Night Vision DISABLED.", LogLevel.NORMAL);
			}
		}

		m_s_bNVActive = enable;

		Toggle(m_s_bNVActive, m_s_bNVActive);
	}

	//------------------------------------------------------------------------------------------------
	//! Called by ATH_AthenaSettingsSubMenu when the user changes the NV color
	//! while NV is currently active. Hot-swaps the post-process material.
	static void OnNVColorPreferenceChanged()
	{
		if (!m_s_bNVActive || !m_s_bEffectApplied || !s_Instance)
			return;

		const BaseWorld world = GetGame().GetWorld();
		if (!world)
			return;

		ResourceName mat = s_Instance.GetSelectedMaterial();
		if (mat.IsEmpty())
			return;

		world.SetCameraPostProcessEffect(world.GetCurrentCameraId(), NV_HDR_SLOT, PostProcessEffectType.HDR, mat);
		Print("[Athena|NVG] NV color hot-swapped.", LogLevel.NORMAL);
	}

	//------------------------------------------------------------------------------------------------
	override void Track()
	{
		Toggle(m_s_bNVActive, m_s_bNVActive);
		
		// If the toolbar was rebuilt and NV was active, ensure the camera effect is still applied
		// in case the engine reset the post-processing on the active camera.
		if (m_s_bNVActive)
		{
			const BaseWorld world = GetGame().GetWorld();
			if (world)
			{
				ResourceName mat = GetSelectedMaterial();
				if (!mat.IsEmpty())
				{
					world.SetCameraPostProcessEffect(world.GetCurrentCameraId(), NV_HDR_SLOT, PostProcessEffectType.HDR, mat);
				}
			}
		}
	}

	//------------------------------------------------------------------------------------------------
	override void Untrack()
	{
		// Do not disable NV here. Untrack is called when context changes (e.g., entity placement).
		// We want the NV effect to persist while placing holograms.
	}
}
