class ActionShowDoorIndex : ActionInteractBase
{
    void ActionShowDoorIndex()
    {
        m_HUDCursorIcon = CursorIcons.None;
    }

    override string GetText()
    {
        return "Türindex anzeigen";
    }

    override void CreateConditionComponents()
    {
        m_ConditionItem   = new CCINone;
        m_ConditionTarget = new CCTCursor;
    }

    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        if (!player || !target) return false;

        if (!item) return false;
        if (!TWT_KeyConfig.IsAdminKey(item.GetType())) return false;

        BuildingBase building;
        if (!Class.CastTo(building, target.GetObject())) return false;

        int doorIndex = building.GetDoorIndex(target.GetComponentIndex());
        return (doorIndex != -1);
    }

    override void OnStartServer(ActionData action_data)
    {
        PlayerBase player = action_data.m_Player;
        BuildingBase building = BuildingBase.Cast(action_data.m_Target.GetObject());
        if (!player || !building) return;

        int doorIndex = building.GetDoorIndex(action_data.m_Target.GetComponentIndex());
        if (doorIndex == -1) return;

        player.MessageStatus("[AdminKey] Türindex: " + doorIndex.ToString());
    }
};
