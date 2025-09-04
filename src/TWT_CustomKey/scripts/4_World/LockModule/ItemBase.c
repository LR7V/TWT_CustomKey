modded class ItemBase
{
    override void SetActions()
    {
        super.SetActions();
        AddAction(ActionLockDoors);
        AddAction(ActionUnlockDoors);
        AddAction(ActionShowDoorIndex);

    }
}
