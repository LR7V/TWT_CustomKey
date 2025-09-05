modded class ActionLockDoorsCB : ActionContinuousBaseCB
{
    override void CreateActionComponent()
    {
        m_ActionData.m_ActionComponent = new CAContinuousTime(2);
    }
};

modded class ActionLockDoors : ActionContinuousBase
{
    void ActionLockDoors()
    {
        m_CallbackClass = ActionLockDoorsCB;
        m_CommandUID    = DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
        m_FullBody      = true;
        m_StanceMask    = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
    }

    override string GetText()
    {
        return "Tür abschließen";
    }

    override void CreateConditionComponents()
    {
        m_ConditionItem   = new CCINonRuined;
        m_ConditionTarget = new CCTCursor;
    }

 
    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {

        BuildingBase building;
        Object obj = target.GetObject();
        if (!Class.CastTo(building, obj))
        {
            Object parent = obj.GetParent();
            if (!parent || !Class.CastTo(building, parent))
                return false;
        }

        int doorIndex = building.GetDoorIndex(target.GetComponentIndex());
        if (doorIndex < 0) return false;


        if (building.IsDoorLocked(doorIndex)) return false;

        if (!item) return false;
        string heldType = item.GetType();


        if (GetGame() && !GetGame().IsServer())
        {
            if (TWT_KeyClientCache.IsAdminKeyClient(heldType)) return true;
            if (TWT_KeyClientCache.IsAllowedTypeClient(heldType)) return true;
            return false;
        }


        return true;
    }

    override void OnFinishProgressServer(ActionData action_data)
    {
        GetTWT_CustomKeyLogger().LogDebug("[LOCK] start");


        if (!action_data) { GetTWT_CustomKeyLogger().LogDebug("[LOCK] abort: no action_data"); return; }

        PlayerBase player = PlayerBase.Cast(action_data.m_Player);
        if (!player) { GetTWT_CustomKeyLogger().LogDebug("[LOCK] abort: no player"); return; }

        string steamID = "";
        string playerName = "Unknown";
        if (player.GetIdentity()) {
            steamID = player.GetIdentity().GetPlainId();
            playerName = player.GetIdentity().GetName();
        }

        if (!action_data.m_Target) { GetTWT_CustomKeyLogger().LogDebug("[LOCK] abort: no target"); return; }
        Object tgt = action_data.m_Target.GetObject();
        if (!tgt) { GetTWT_CustomKeyLogger().LogDebug("[LOCK] abort: target obj null"); return; }


        BuildingBase building;
        if (!Class.CastTo(building, tgt)) {
            Object parent = tgt.GetParent();
            if (!parent || !Class.CastTo(building, parent)) {
                GetTWT_CustomKeyLogger().LogDebug("[LOCK] abort: no building");
                return;
            }
        }


        int comp = action_data.m_Target.GetComponentIndex();
        int doorIndex = building.GetDoorIndex(comp);
        if (doorIndex < 0) { GetTWT_CustomKeyLogger().LogDebug("[LOCK] abort: doorIndex < 0"); return; }

        if (building.IsDoorLocked(doorIndex)) { GetTWT_CustomKeyLogger().LogDebug("[LOCK] abort: already locked"); return; }


        ItemBase held = player.GetItemInHands();
        if (!held) { GetTWT_CustomKeyLogger().LogDebug("[LOCK] abort: no item in hands"); return; }

        string heldType = held.GetType();

        bool isAdminKey = TWT_KeyConfig.IsAdminKey(heldType);
        if (!isAdminKey)
        {
            if (!TWT_KeyConfig.IsAllowedType(heldType)) { GetTWT_CustomKeyLogger().LogDebug("[LOCK] abort: type not allowed"); return; }
            if (!TWT_KeyConfig.CanUseKey(heldType, steamID)) 
            { 
                NotificationSystem.SendNotificationToPlayerIdentityExtended(player.GetIdentity(), 5.0, "Türschloss", "Finger weg , sonst Finger ab!", "");
                GetTWT_CustomKeyLogger().LogDebug("[LOCK] abort: not whitelisted"); 
                return; 
            }
        }


        building.LockDoor(doorIndex);
        bool nowLocked = building.IsDoorLocked(doorIndex);
        if (!nowLocked) { GetTWT_CustomKeyLogger().LogDebug("[LOCK] abort: lock failed"); return; }

        GetTWT_DoorLockDB().SetLocked(building, doorIndex, true);

        vector pos = building.GetPosition();
        string msg = "Abgeschlossen | " + playerName + " (" + steamID + ") | " + building.GetType() + " (" + pos.ToString() + ") | DoorIndex [" + doorIndex.ToString() + "]";
        if (isAdminKey) msg = msg + " | via AdminKey";
        GetTWT_CustomKeyLogger().LogInfo(msg);
    }
};
