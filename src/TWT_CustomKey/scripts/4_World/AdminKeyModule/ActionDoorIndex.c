class ActionShowDoorIndexCB : ActionContinuousBaseCB
{
    override void CreateActionComponent()
    {
        // sichtbarer Progress: z.B. 2 Sekunden
        m_ActionData.m_ActionComponent = new CAContinuousTime(2.0);
    }
};

class ActionShowDoorIndex : ActionContinuousBase
{
    void ActionShowDoorIndex()
    {
        m_CallbackClass     = ActionShowDoorIndexCB;
        m_CommandUID        = DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
        m_FullBody          = true;  
        m_LockTargetOnUse   = true; 
        m_StanceMask        = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
        GetTWT_CustomKeyLogger().LogDebug("[SHOWIDX] ctor");
    }

    override bool HasTarget() { return true; }

    override string GetText() { return "Türstatus anzeigen"; }

    override void CreateConditionComponents()
    {
        m_ConditionItem   = new CCINone;
        m_ConditionTarget = new CCTCursor;
    }

    override typename GetInputType()
    {
        return ContinuousInteractActionInput;  // Hold F
    }

    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        string side = "CLIENT";
        if (GetGame() && GetGame().IsServer()) { side = "SERVER"; }
        GetTWT_CustomKeyLogger().LogDebug("[SHOWIDX][AC][" + side + "] begin");

        if (!player) return false;
        if (!target) return false;

        // IMMER aus der Hand lesen (item kann null sein)
        ItemBase inHands = ItemBase.Cast(player.GetItemInHands());
        if (!inHands) {
            GetTWT_CustomKeyLogger().LogDebug("[SHOWIDX][AC][" + side + "] abort: no item in hands");
            return false;
        }

        string heldType = inHands.GetType();
        GetTWT_CustomKeyLogger().LogDebug("[SHOWIDX][AC][" + side + "] heldType=" + heldType);

        // Client: AdminKey-Filter
        if (GetGame() && !GetGame().IsServer())
        {
            if (!TWT_KeyClientCache.IsAdminKeyClient(heldType)) {
                GetTWT_CustomKeyLogger().LogDebug("[SHOWIDX][AC][CLIENT] hidden: not AdminKey");
                return false;
            }
        }

        Object obj = Object.Cast(target.GetObject());
        if (!obj) return false;

        BuildingBase building = BuildingBase.Cast(obj);
        if (!building)
        {
            Object parent = Object.Cast(obj.GetParent());
            if (!parent) return false;

            building = BuildingBase.Cast(parent);
            if (!building) return false;
        }

        int comp = target.GetComponentIndex();
        int doorIndex = building.GetDoorIndex(comp);
        GetTWT_CustomKeyLogger().LogDebug("[SHOWIDX][AC][" + side + "] comp=" + comp.ToString() + " doorIndex=" + doorIndex.ToString());

        if (doorIndex < 0) return false;
        return true;
    }

    override void OnFinishProgressServer(ActionData action_data)
    {
        GetTWT_CustomKeyLogger().LogDebug("[SHOWIDX][SV] finish start");

        if (!action_data) return;
        PlayerBase player = PlayerBase.Cast(action_data.m_Player);
        if (!player) { GetTWT_CustomKeyLogger().LogDebug("[UNLOCK] abort: no player"); return; }
        
        if (!action_data.m_Target) return;

        Object obj = action_data.m_Target.GetObject(); if (!obj) return;

        string sid = "";
        if (player.GetIdentity()) { sid = player.GetIdentity().GetPlainId(); }
        if (!TWT_KeyConfig.IsAdminSteamId(sid))
        {
            string adminmsg = "Ich glaube der Schlüssel ist nichts für dich!";
            ItemBase held = ItemBase.Cast(player.GetItemInHands());

            if (held)
            {
                string heldType = held.GetType();

                string adminType = TWT_KeyConfig.GetAdminKeyType();
                string heldNorm  = heldType;  heldNorm.Trim();  heldNorm.ToLower();
                string adminNorm = adminType; adminNorm.Trim(); adminNorm.ToLower();

                if (adminNorm != string.Empty && heldNorm == adminNorm)
                {
                    if (GetGame() && GetGame().IsServer())
                    {
                        GetTWT_CustomKeyLogger().LogInfo("[SHOWIDX] Confiscated AdminKey from non-admin " + sid + " item=" + heldType);
                        GetGame().ObjectDelete(held);
                    }
                }
            }

            NotificationSystem.SendNotificationToPlayerIdentityExtended(player.GetIdentity(), 5.0, "Türstatus", adminmsg, "");
            GetTWT_CustomKeyLogger().LogDebug("[SHOWIDX][SV] abort: not admin");
            return;
        }


        BuildingBase building = BuildingBase.Cast(obj);
        if (!building)
        {
            Object parent = Object.Cast(obj.GetParent());
            if (!parent) {
                GetTWT_CustomKeyLogger().LogDebug("[BUILD] abort: no parent");
                return;
            }

            building = BuildingBase.Cast(parent);
            if (!building) {
                GetTWT_CustomKeyLogger().LogDebug("[BUILD] abort: parent not BuildingBase");
                return;
            }
        }

        int comp = action_data.m_Target.GetComponentIndex();
        int doorIndex = building.GetDoorIndex(comp);
        GetTWT_CustomKeyLogger().LogDebug("[SHOWIDX][SV] comp=" + comp.ToString() + " doorIndex=" + doorIndex.ToString());
        if (doorIndex < 0) return;

        bool isLocked = building.IsDoorLocked(doorIndex);

        string status = "";
        if (isLocked) { status = "ABGESCHLOSSEN"; } else { status = "OFFEN"; }

        string keyType = "";
        if (isLocked) {
            keyType = GetTWT_DoorLockDB().GetLastKeyType(building, doorIndex);
            if (!keyType || keyType == string.Empty) { keyType = "unbekannt"; }
        }

        string msg = "[Index " + doorIndex.ToString() + "] Status: " + status;
        if (isLocked) { msg = msg + " | Key: " + keyType; }

        NotificationSystem.SendNotificationToPlayerIdentityExtended(player.GetIdentity(), 6.0, "Türstatus", msg, "");
        GetTWT_CustomKeyLogger().LogDebug("[SHOWIDX][SV] finish done");
    }
}
