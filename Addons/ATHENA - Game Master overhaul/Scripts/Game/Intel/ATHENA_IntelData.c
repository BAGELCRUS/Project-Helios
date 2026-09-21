[BaseContainerProps()]
class ATHENA_IntelData
{
	[Attribute("Confidential Intel", UIWidgets.EditBox)]
	string m_sTitle;
	
	[Attribute("Document", UIWidgets.EditBox)]
	string m_sType;
	
	[Attribute("Top secret description...", UIWidgets.EditBoxMultiline)]
	string m_sDescription;
	
	EAthenaIntelShareType m_ShareType;
	string m_sMethod;
	string m_sTime;
	
	void ATHENA_IntelData(string title = "", string type = "", string description = "", EAthenaIntelShareType shareType = EAthenaIntelShareType.Individual, string method = "Collected", string time = "")
	{
		m_sTitle = title;
		m_sType = type;
		m_sDescription = description;
		m_ShareType = shareType;
		m_sMethod = method;
		m_sTime = time;
	}
}
