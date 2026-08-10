modded class SCR_BaseGameMode
{
	[RplProp(onRplName: "ATH_OnBudgetsEnforcedChanged")]
	protected bool m_bBudgetsEnforced = false; // Default: Budgets disabled (Unlimited)
	
	// Fast static accessor so we don't have to fetch GameMode constantly in performance-critical placing checks
	static bool s_bBudgetsEnforced = false;
	
	static ref set<EEditableEntityBudget> s_BudgetsToIgnore = new set<EEditableEntityBudget>();

	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		
		if (!s_BudgetsToIgnore)
			s_BudgetsToIgnore = new set<EEditableEntityBudget>();
		
		s_BudgetsToIgnore.Insert(EEditableEntityBudget.PROPS);
		s_BudgetsToIgnore.Insert(EEditableEntityBudget.AI);
		s_BudgetsToIgnore.Insert(EEditableEntityBudget.VEHICLES);
		s_BudgetsToIgnore.Insert(EEditableEntityBudget.WAYPOINTS);
		s_BudgetsToIgnore.Insert(EEditableEntityBudget.SYSTEMS);
		
		s_bBudgetsEnforced = m_bBudgetsEnforced;
	}
	
	// API for Admin menus and Game Master attributes
	void ATH_SetBudgetsEnforced(bool enforce)
	{
		m_bBudgetsEnforced = enforce;
		Replication.BumpMe();
		ATH_OnBudgetsEnforcedChanged();
	}
	
	bool ATH_AreBudgetsEnforced()
	{
		return m_bBudgetsEnforced;
	}
	
	// RPC callback
	protected void ATH_OnBudgetsEnforcedChanged()
	{
		s_bBudgetsEnforced = m_bBudgetsEnforced;
	}
}
