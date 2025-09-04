modded class ActionUnlockDoorsCB : ActionContinuousBaseCB
{
    override void CreateActionComponent()
    {
        m_ActionData.m_ActionComponent = new CAContinuousTime(2);
    }
};

modded class ActionUnlockDoors : ActionContinuousBase
{
    void ActionUnlockDoors()
    {
        m_CallbackClass = ActionUnlockDoorsCB;
        m_CommandUID    = DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
        m_FullBody      = true;
        m_StanceMask    = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
    }

    override string GetText()
    {
        return "Tür aufschließen";
    }

    override void CreateConditionComponents()
    {
        m_ConditionItem   = new CCINonRuined;
        m_ConditionTarget = new CCTCursor;
    }

    // Client: nur anzeigen, wenn sinnvoll – keine Whitelist hier
    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        // --- Building + DoorIndex robust ermitteln (Child/Parent) ---
        BuildingBase building;
        Object obj = target.GetObject();
        if (!Class.CastTo(building, obj)) {
            Object parent = obj.GetParent();
            if (!parent || !Class.CastTo(building, parent))
                return false;
        }

        int doorIndex = building.GetDoorIndex(target.GetComponentIndex());
        if (doorIndex < 0) return false;

        // Für UNLOCK: nur wenn aktuell abgeschlossen
        if (!building.IsDoorLocked(doorIndex)) return false;

        if (!item) return false;
        string heldType = item.GetType();

        // --- Client: Anzeige steuern über Cache ---
        if (GetGame() && !GetGame().IsServer()) {
            if (TWT_KeyClientCache.IsAdminKeyClient(heldType)) return true;
            if (TWT_KeyClientCache.IsAllowedTypeClient(heldType)) return true;
            return false;
        }

        // --- Server: gleiche Sichtbarkeits-Logik, aber mit Server-Config ---
        if (TWT_KeyConfig.IsAdminKey(heldType)) return true;
        if (TWT_KeyConfig.IsAllowedType(heldType)) return true;
        return false;
    }

    override void OnFinishProgressServer(ActionData action_data)
    {
        GetTWT_CustomKeyLogger().LogDebug("[UNLOCK] start");

        // --- sanity ---
        if (!action_data) { GetTWT_CustomKeyLogger().LogDebug("[UNLOCK] abort: no action_data"); return; }

        PlayerBase player = PlayerBase.Cast(action_data.m_Player);
        if (!player) { GetTWT_CustomKeyLogger().LogDebug("[UNLOCK] abort: no player"); return; }

        string steamID = "";
        string playerName = "Unknown";
        if (player.GetIdentity()) {
            steamID = player.GetIdentity().GetPlainId();
            playerName = player.GetIdentity().GetName();
        }

        if (!action_data.m_Target) { GetTWT_CustomKeyLogger().LogDebug("[UNLOCK] abort: no target"); return; }
        Object tgt = action_data.m_Target.GetObject();
        if (!tgt) { GetTWT_CustomKeyLogger().LogDebug("[UNLOCK] abort: target obj null"); return; }

        // --- building (mit parent-fallback) ---
        BuildingBase building;
        if (!Class.CastTo(building, tgt)) {
            Object parent = tgt.GetParent();
            if (!parent || !Class.CastTo(building, parent)) {
                GetTWT_CustomKeyLogger().LogDebug("[UNLOCK] abort: no building");
                return;
            }
        }

        // --- door index / zustand ---
        int comp = action_data.m_Target.GetComponentIndex();
        int doorIndex = building.GetDoorIndex(comp);
        if (doorIndex < 0) { GetTWT_CustomKeyLogger().LogDebug("[UNLOCK] abort: doorIndex < 0"); return; }

        if (!building.IsDoorLocked(doorIndex)) { GetTWT_CustomKeyLogger().LogDebug("[UNLOCK] abort: already unlocked"); return; }

        // --- item / key prüfung ---
        ItemBase held = player.GetItemInHands();
        if (!held) { GetTWT_CustomKeyLogger().LogDebug("[UNLOCK] abort: no item in hands"); return; }

        string heldType = held.GetType();

        bool isAdminKey = TWT_KeyConfig.IsAdminKey(heldType);
        if (!isAdminKey) {
            if (!TWT_KeyConfig.IsAllowedType(heldType)) { GetTWT_CustomKeyLogger().LogDebug("[UNLOCK] abort: type not allowed"); return; }
            if (!TWT_KeyConfig.CanUseKey(heldType, steamID)) 
            { 
                NotificationSystem.SendNotificationToPlayerIdentityExtended(player.GetIdentity(), 5.0, "Türschloss", "Finger weg , sonst Finger ab!", "");
                GetTWT_CustomKeyLogger().LogDebug("[UNLOCK] abort: not whitelisted"); 
                return; 
            }
        }

        // --- unlock + persist ---
        building.UnlockDoor(doorIndex);
        bool stillLocked = building.IsDoorLocked(doorIndex);
        if (stillLocked) { GetTWT_CustomKeyLogger().LogDebug("[UNLOCK] abort: unlock failed"); return; }

        GetTWT_DoorLockDB().SetLocked(building, doorIndex, false);

        // --- success log ---
        vector pos = building.GetPosition();
        string msg = "Aufgeschlossen | " + playerName + " (" + steamID + ") | " + building.GetType() + " (" + pos.ToString() + ") | DoorIndex [" + doorIndex.ToString() + "]";
        if (isAdminKey) msg = msg + " | via AdminKey";
        GetTWT_CustomKeyLogger().LogInfo(msg);
    }

};
