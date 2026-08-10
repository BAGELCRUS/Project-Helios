//------------------------------------------------------------------------------------------------
//! ATH_AttributesManagerEditorComponentClass
//! Modded override of SCR_AttributesManagerEditorComponentClass.
//! Injects Athena's custom editor attributes (Invisibility, Invincibility)
//! into the attribute manager at construction time, so they appear alongside
//! vanilla attributes in the Entity Properties panel without needing to
//! modify the vanilla editor mode prefab or its attribute list configs.
//------------------------------------------------------------------------------------------------
modded class SCR_AttributesManagerEditorComponentClass : SCR_BaseEditorComponentClass
{
	//------------------------------------------------------------------------------------------------
	void SCR_AttributesManagerEditorComponentClass(IEntityComponentSource componentSource, IEntitySource parentSource, IEntitySource prefabSource)
	{
		// Inject our custom Athena attributes into the already-built list
		Resource res = BaseContainerTools.LoadContainer("{F3D6C6D25642352C}Configs/Editor/AttributeLists/Edit.conf");
		if (res && res.IsValid())
		{
			BaseContainer container = res.GetResource().ToBaseContainer();
			if (container)
			{
				SCR_EditorAttributeList athenaList = SCR_EditorAttributeList.Cast(BaseContainerTools.CreateInstanceFromContainer(container));
				if (athenaList)
				{
					// Store it strongly so the attributes don't get garbage collected
					if (!m_AttributeLists)
						m_AttributeLists = {};
					m_AttributeLists.Insert(athenaList);
					
					array<SCR_BaseEditorAttribute> temp = {};
					athenaList.InsertAllAttributes(temp);
					
					foreach (SCR_BaseEditorAttribute attr : temp)
					{
						if (attr)
							m_aAttributes.Insert(attr);
						else
							Print("[Athena] Found NULL attribute in Edit.conf!", LogLevel.ERROR);
					}
				}
			}
		}
	}
}
