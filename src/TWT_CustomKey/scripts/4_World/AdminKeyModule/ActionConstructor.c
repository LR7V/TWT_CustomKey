modded class ActionConstructor
{
    override void RegisterActions(TTypenameArray actions)
    {
        super.RegisterActions(actions);
        actions.Insert(ActionShowDoorIndex); 
        Print("[SHOWIDX][REG] ActionShowDoorIndex registered in ActionConstructor");
    }
}

modded class Edible_Base
{
    override void SetActions()
    {
        super.SetActions();
        AddAction(ActionShowDoorIndex);
        Print("[SHOWIDX][EDIBLE] ActionShowDoorIndex added to " + this.GetType());
    }
}

modded class PlayerBase
{
    override void SetActions()
    {
        super.SetActions();
        AddAction(ActionShowDoorIndex);
        Print("[SHOWIDX][PLY] Added ActionShowDoorIndex to PlayerBase");
    }
}
