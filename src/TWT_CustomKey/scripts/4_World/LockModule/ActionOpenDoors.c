modded class ActionOpenDoors : ActionInteractBase
{
    override string GetText()
    {
        return "Öffnen";
    }

    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        if (!super.ActionCondition(player, target, item))
            return false;

        BuildingBase building;
        if (!Class.CastTo(building, target.GetObject()))
            return false;

        int doorIndex = building.GetDoorIndex(target.GetComponentIndex());
        if (doorIndex == -1)
            return false;


        if (!building.IsDoorLocked(doorIndex))
            return true;


        ItemBase held = ItemBase.Cast(player.GetItemInHands());
        string heldType = "";
        if (held)
            heldType = held.GetType();

        return TWT_KeyConfig.IsAllowedType(heldType);
    }

    override void OnStartServer(ActionData action_data)
    {
        if (!action_data) { super.OnStartServer(action_data); return; }

        PlayerBase player = PlayerBase.Cast(action_data.m_Player);
        if (!player) { GetTWT_CustomKeyLogger().LogDebug("[UNLOCK] abort: no player"); return; }

        BuildingBase building = BuildingBase.Cast(action_data.m_Target.GetObject());
        if (!building) { super.OnStartServer(action_data); return; }

        int doorIndex = building.GetDoorIndex(action_data.m_Target.GetComponentIndex());
        if (doorIndex == -1) { super.OnStartServer(action_data); return; }

        ItemBase held = ItemBase.Cast(player.GetItemInHands());
        string heldType = "";
        if (held)
            heldType = held.GetType();

        bool hasAllowedKeyInHand = TWT_KeyConfig.IsAllowedType(heldType);


        if (building.IsDoorLocked(doorIndex) && !hasAllowedKeyInHand)
        {
            if (GetGame().IsServer())
                player.MessageStatus("Diese Tuer ist verschlossen!");
            return; 
        }


        super.OnStartServer(action_data);
    }
};
