modded class SCR_BudgetEditorComponent 
{
	override protected bool IsBudgetCapEnabled()
	{
		// E.g., campaign building ignores standard Game Master budget rules
		if (this.ClassName() == "SCR_CampaignBuildingBudgetEditorComponent")
			return super.IsBudgetCapEnabled();
		
		// If our global toggle is turned off (unlimited budgets), bypass budget cap
		if (!SCR_BaseGameMode.s_bBudgetsEnforced)
			return false;

		return super.IsBudgetCapEnabled();
	}
}

modded class SCR_PlacingEditorComponent 
{
	override bool IsThereEnoughBudgetToSpawn(IEntityComponentSource entitySource)
	{
		if (!SCR_BaseGameMode.s_bBudgetsEnforced)
			return true;
		
		return super.IsThereEnoughBudgetToSpawn(entitySource);
	}
	
    override void CheckBudgetOwner()
    {
		if (!SCR_BaseGameMode.s_bBudgetsEnforced)
			return;
		
		super.CheckBudgetOwner();
    }

    override void OnBudgetMaxReached(EEditableEntityBudget entityBudget, bool maxReached)
    {
		if (SCR_BaseGameMode.s_BudgetsToIgnore && SCR_BaseGameMode.s_BudgetsToIgnore.Contains(entityBudget) && !SCR_BaseGameMode.s_bBudgetsEnforced)
			return;
		
		super.OnBudgetMaxReached(entityBudget, maxReached);
    }
}
