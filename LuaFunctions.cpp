/*
* Copyright (C) 2010 - 2015 Eluna Lua Engine <http://emudevs.com/>
* This program is free software licensed under GPL version 3
* Please see the included DOCS/LICENSE.md for more information
*/

extern "C"
{
#include "lua.h"
};

// Eluna
#include "LuaEngine.h"
#include "ElunaEventMgr.h"
#include "ElunaIncludes.h"
#include "ElunaTemplate.h"
#include "ElunaUtility.h"
#include "lmarshal.h"

// Method includes
#include "GlobalMethods.h"
#include "ObjectMethods.h"
#include "WorldObjectMethods.h"
#include "UnitMethods.h"
#include "PlayerMethods.h"
#include "CreatureMethods.h"
#include "GroupMethods.h"
#include "GuildMethods.h"
#include "GameObjectMethods.h"
#include "ElunaQueryMethods.h"
#include "AuraMethods.h"
#include "ItemMethods.h"
#include "WorldPacketMethods.h"
#include "SpellMethods.h"
#include "QuestMethods.h"
#include "MapMethods.h"
#include "CorpseMethods.h"
#include "VehicleMethods.h"
#include "BattleGroundMethods.h"

ElunaFunction::ElunaRegister GlobalMethods[] =
{
    // Hooks
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "RegisterServerEvent", &LuaGlobalFunctions::RegisterServerEvent },
    { MTMAP_SAFE,                   "RegisterPlayerEvent", &LuaGlobalFunctions::RegisterPlayerEvent },
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "RegisterGuildEvent", &LuaGlobalFunctions::RegisterGuildEvent },
    { MTMAP_SAFE,                   "RegisterGroupEvent", &LuaGlobalFunctions::RegisterGroupEvent },
    { MTMAP_SAFE,                   "RegisterCreatureEvent", &LuaGlobalFunctions::RegisterCreatureEvent },
    { MTMAP_SAFE,                   "RegisterUniqueCreatureEvent", &LuaGlobalFunctions::RegisterUniqueCreatureEvent },
    { MTMAP_SAFE,                   "RegisterCreatureGossipEvent", &LuaGlobalFunctions::RegisterCreatureGossipEvent },
    { MTMAP_SAFE,                   "RegisterGameObjectEvent", &LuaGlobalFunctions::RegisterGameObjectEvent },
    { MTMAP_SAFE,                   "RegisterGameObjectGossipEvent", &LuaGlobalFunctions::RegisterGameObjectGossipEvent },
    { MTMAP_SAFE,                   "RegisterItemEvent", &LuaGlobalFunctions::RegisterItemEvent },
    { MTMAP_SAFE,                   "RegisterItemGossipEvent", &LuaGlobalFunctions::RegisterItemGossipEvent },
    { MTMAP_SAFE,                   "RegisterPlayerGossipEvent", &LuaGlobalFunctions::RegisterPlayerGossipEvent },
    { MTMAP_SAFE,                   "RegisterBGEvent", &LuaGlobalFunctions::RegisterBGEvent },
    { MTMAP_SAFE,                   "RegisterMapEvent", &LuaGlobalFunctions::RegisterMapEvent },
    { MTMAP_SAFE,                   "RegisterInstanceEvent", &LuaGlobalFunctions::RegisterInstanceEvent },

    { MTMAP_SAFE | NOT_IN_SAFEMODE, "ClearBattleGroundEvents", &LuaGlobalFunctions::ClearBattleGroundEvents },
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "ClearCreatureEvents", &LuaGlobalFunctions::ClearCreatureEvents },
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "ClearUniqueCreatureEvents", &LuaGlobalFunctions::ClearUniqueCreatureEvents },
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "ClearCreatureGossipEvents", &LuaGlobalFunctions::ClearCreatureGossipEvents },
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "ClearGameObjectEvents", &LuaGlobalFunctions::ClearGameObjectEvents },
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "ClearGameObjectGossipEvents", &LuaGlobalFunctions::ClearGameObjectGossipEvents },
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "ClearGroupEvents", &LuaGlobalFunctions::ClearGroupEvents },
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "ClearGuildEvents", &LuaGlobalFunctions::ClearGuildEvents },
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "ClearItemEvents", &LuaGlobalFunctions::ClearItemEvents },
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "ClearItemGossipEvents", &LuaGlobalFunctions::ClearItemGossipEvents },
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "ClearPlayerEvents", &LuaGlobalFunctions::ClearPlayerEvents },
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "ClearPlayerGossipEvents", &LuaGlobalFunctions::ClearPlayerGossipEvents },
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "ClearServerEvents", &LuaGlobalFunctions::ClearServerEvents },
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "ClearMapEvents", &LuaGlobalFunctions::ClearMapEvents },
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "ClearInstanceEvents", &LuaGlobalFunctions::ClearInstanceEvents },

    // Getters
    { MTMAP_SAFE,                   "GetMap", &LuaGlobalFunctions::GetMap },
    { MTMAP_SAFE,                   "GetLuaEngine", &LuaGlobalFunctions::GetLuaEngine },
    { MTMAP_SAFE,                   "GetCoreName", &LuaGlobalFunctions::GetCoreName },
    { MTMAP_SAFE,                   "GetCoreVersion", &LuaGlobalFunctions::GetCoreVersion },
    { MTMAP_SAFE,                   "GetCoreExpansion", &LuaGlobalFunctions::GetCoreExpansion },
    { MTMAP_SAFE,                   "GetQuest", &LuaGlobalFunctions::GetQuest },
    { NOT_IN_SAFEMODE,              "GetPlayerByGUID", &LuaGlobalFunctions::GetPlayerByGUID },
    { NOT_IN_SAFEMODE,              "GetPlayerByName", &LuaGlobalFunctions::GetPlayerByName },
    { MTMAP_SAFE,                   "GetGameTime", &LuaGlobalFunctions::GetGameTime },
    { NOT_IN_SAFEMODE,              "GetPlayersInWorld", &LuaGlobalFunctions::GetPlayersInWorld },
    { NOT_IN_SAFEMODE,              "GetGuildByName", &LuaGlobalFunctions::GetGuildByName },
    { NOT_IN_SAFEMODE,              "GetGuildByLeaderGUID", &LuaGlobalFunctions::GetGuildByLeaderGUID },
    { NOT_IN_SAFEMODE,              "GetPlayerCount", &LuaGlobalFunctions::GetPlayerCount },
    { MTMAP_SAFE,                   "GetPlayerGUID", &LuaGlobalFunctions::GetPlayerGUID },
    { MTMAP_SAFE,                   "GetItemGUID", &LuaGlobalFunctions::GetItemGUID },
    { MTMAP_SAFE,                   "GetObjectGUID", &LuaGlobalFunctions::GetObjectGUID },
    { MTMAP_SAFE,                   "GetUnitGUID", &LuaGlobalFunctions::GetUnitGUID },
    { MTMAP_SAFE,                   "GetGUIDLow", &LuaGlobalFunctions::GetGUIDLow },
    { MTMAP_SAFE,                   "GetGUIDType", &LuaGlobalFunctions::GetGUIDType },
    { MTMAP_SAFE,                   "GetGUIDEntry", &LuaGlobalFunctions::GetGUIDEntry },
    { MTMAP_SAFE,                   "GetAreaName", &LuaGlobalFunctions::GetAreaName },
    { MTMAP_SAFE,                   "bit_not", &LuaGlobalFunctions::bit_not },
    { MTMAP_SAFE,                   "bit_xor", &LuaGlobalFunctions::bit_xor },
    { MTMAP_SAFE,                   "bit_rshift", &LuaGlobalFunctions::bit_rshift },
    { MTMAP_SAFE,                   "bit_lshift", &LuaGlobalFunctions::bit_lshift },
    { MTMAP_SAFE,                   "bit_or", &LuaGlobalFunctions::bit_or },
    { MTMAP_SAFE,                   "bit_and", &LuaGlobalFunctions::bit_and },
    { MTMAP_SAFE,                   "GetItemLink", &LuaGlobalFunctions::GetItemLink },
    { NOT_IN_SAFEMODE,              "GetMapById", &LuaGlobalFunctions::GetMapById },
    { MTMAP_SAFE,                   "GetCurrTime", &LuaGlobalFunctions::GetCurrTime },
    { MTMAP_SAFE,                   "GetTimeDiff", &LuaGlobalFunctions::GetTimeDiff },
    { MTMAP_SAFE,                   "PrintInfo", &LuaGlobalFunctions::PrintInfo },
    { MTMAP_SAFE,                   "PrintError", &LuaGlobalFunctions::PrintError },
    { MTMAP_SAFE,                   "PrintDebug", &LuaGlobalFunctions::PrintDebug },

    // Boolean
    { MTMAP_SAFE,                   "IsInventoryPos", &LuaGlobalFunctions::IsInventoryPos },
    { MTMAP_SAFE,                   "IsEquipmentPos", &LuaGlobalFunctions::IsEquipmentPos },
    { MTMAP_SAFE,                   "IsBankPos", &LuaGlobalFunctions::IsBankPos },
    { MTMAP_SAFE,                   "IsBagPos", &LuaGlobalFunctions::IsBagPos },

    // Other
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "ReloadEluna", &LuaGlobalFunctions::ReloadEluna },
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "SendWorldMessage", &LuaGlobalFunctions::SendWorldMessage },
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "WorldDBQuery", &LuaGlobalFunctions::WorldDBQuery },
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "WorldDBExecute", &LuaGlobalFunctions::WorldDBExecute },
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "CharDBQuery", &LuaGlobalFunctions::CharDBQuery },
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "CharDBExecute", &LuaGlobalFunctions::CharDBExecute },
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "AuthDBQuery", &LuaGlobalFunctions::AuthDBQuery },
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "AuthDBExecute", &LuaGlobalFunctions::AuthDBExecute },
    { MTMAP_SAFE,                   "CreateLuaEvent", &LuaGlobalFunctions::CreateLuaEvent },
    { MTMAP_SAFE,                   "RemoveEventById", &LuaGlobalFunctions::RemoveEventById },
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "RemoveEvents", &LuaGlobalFunctions::RemoveEvents },
    { NOT_IN_SAFEMODE,              "PerformIngameSpawn", &LuaGlobalFunctions::PerformIngameSpawn },
    { MTMAP_SAFE,                   "CreatePacket", &LuaGlobalFunctions::CreatePacket },
    { NOT_IN_SAFEMODE,              "AddVendorItem", &LuaGlobalFunctions::AddVendorItem },
    { NOT_IN_SAFEMODE,              "VendorRemoveItem", &LuaGlobalFunctions::VendorRemoveItem },
    { NOT_IN_SAFEMODE,              "VendorRemoveAllItems", &LuaGlobalFunctions::VendorRemoveAllItems },
    { NOT_IN_SAFEMODE,              "Kick", &LuaGlobalFunctions::Kick },
    { NOT_IN_SAFEMODE,              "Ban", &LuaGlobalFunctions::Ban },
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "SaveAllPlayers", &LuaGlobalFunctions::SaveAllPlayers },
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "SendMail", &LuaGlobalFunctions::SendMail },
    { FLAGS_NONE,                   "AddTaxiPath", &LuaGlobalFunctions::AddTaxiPath },
    { MTMAP_SAFE,                   "UintNew", &LuaGlobalFunctions::UintNew },
    { MTMAP_SAFE,                   "UintToString", &LuaGlobalFunctions::UintToString },
    { MTMAP_SAFE,                   "UintToHex", &LuaGlobalFunctions::UintToHex },
    { MTMAP_SAFE,                   "StateChannelSend", &LuaGlobalFunctions::StateChannelSend },
    { MTMAP_SAFE,                   "StateChannelRegister", &LuaGlobalFunctions::StateChannelRegister },
    { MTMAP_SAFE,                   "StateChannelUnregister", &LuaGlobalFunctions::StateChannelUnregister },
    // PlayerBot system
    { FLAGS_NONE,                   "CreateBotSession", &LuaGlobalFunctions::CreateBotSession },
    { FLAGS_NONE,                   "CreatePlayer", &LuaGlobalFunctions::CreatePlayer },
    { FLAGS_NONE,                   "TeleportOfflinePlayer", &LuaGlobalFunctions::TeleportOfflinePlayer },
    { FLAGS_NONE,                   "LoginBotPlayer", &LuaGlobalFunctions::LoginBotPlayer },

    { FLAGS_NONE, nullptr, nullptr },
};

ElunaRegister<Object> ObjectMethods[] =
{
    // Getters
    { MTMAP_SAFE,                   "GetEntry", &LuaObject::GetEntry },                     // :GetEntry() - Returns the object's entryId
    { MTMAP_SAFE,                   "GetGUID", &LuaObject::GetGUID },                       // :GetGUID() - Returns uint64 guid as hex string
    { MTMAP_SAFE,                   "GetGUIDLow", &LuaObject::GetGUIDLow },                 // :GetGUIDLow() - Returns uint32 guid (low guid) that is used to differentiate objects of same type.
    { MTMAP_SAFE,                   "GetInt32Value", &LuaObject::GetInt32Value },           // :GetInt32Value(index) - returns an int value from object fields
    { MTMAP_SAFE,                   "GetUInt32Value", &LuaObject::GetUInt32Value },         // :GetUInt32Value(index) - returns an uint value from object fields
    { MTMAP_SAFE,                   "GetFloatValue", &LuaObject::GetFloatValue },           // :GetFloatValue(index) - returns a float value from object fields
    { MTMAP_SAFE,                   "GetByteValue", &LuaObject::GetByteValue },             // :GetByteValue(index, offset) - returns a byte value from object fields
    { MTMAP_SAFE,                   "GetUInt16Value", &LuaObject::GetUInt16Value },         // :GetUInt16Value(index, offset) - returns a uint16 value from object fields
    { MTMAP_SAFE,                   "GetUInt64Value", &LuaObject::GetUInt64Value },         // :GetUInt64Value(index) - returns a uint64 value from object fields
    { MTMAP_SAFE,                   "GetScale", &LuaObject::GetScale },                     // :GetScale()
    { MTMAP_SAFE,                   "GetTypeId", &LuaObject::GetTypeId },                   // :GetTypeId() - Returns the object's typeId

    // Setters
    { MTMAP_SAFE,                   "SetInt32Value", &LuaObject::SetInt32Value },           // :SetInt32Value(index, value) - Sets an int value for the object
    { MTMAP_SAFE,                   "SetUInt32Value", &LuaObject::SetUInt32Value },         // :SetUInt32Value(index, value) - Sets an uint value for the object
    { MTMAP_SAFE,                   "UpdateUInt32Value", &LuaObject::UpdateUInt32Value },   // :UpdateUInt32Value(index, value) - Sets an uint value for the object
    { MTMAP_SAFE,                   "SetFloatValue", &LuaObject::SetFloatValue },           // :SetFloatValue(index, value) - Sets a float value for the object
    { MTMAP_SAFE,                   "SetByteValue", &LuaObject::SetByteValue },             // :SetByteValue(index, offset, value) - Sets a byte value for the object
    { MTMAP_SAFE,                   "SetUInt16Value", &LuaObject::SetUInt16Value },         // :SetUInt16Value(index, offset, value) - Sets an uint16 value for the object
    { MTMAP_SAFE,                   "SetInt16Value", &LuaObject::SetInt16Value },           // :SetInt16Value(index, offset, value) - Sets an int16 value for the object
    { MTMAP_SAFE,                   "SetUInt64Value", &LuaObject::SetUInt64Value },         // :SetUInt64Value(index, value) - Sets an uint64 value for the object
    { MTMAP_SAFE,                   "SetScale", &LuaObject::SetScale },                     // :SetScale(scale)
    { MTMAP_SAFE,                   "SetFlag", &LuaObject::SetFlag },                       // :SetFlag(index, flag)

    // Boolean
    { MTMAP_SAFE,                   "IsInWorld", &LuaObject::IsInWorld },                   // :IsInWorld() - Returns if the object is in world
    { MTMAP_SAFE,                   "HasFlag", &LuaObject::HasFlag },                       // :HasFlag(index, flag)

    // Other
    { MTMAP_SAFE,                   "ToGameObject", &LuaObject::ToGameObject },             // :ToGameObject()
    { MTMAP_SAFE,                   "ToUnit", &LuaObject::ToUnit },                         // :ToUnit()
    { MTMAP_SAFE,                   "ToCreature", &LuaObject::ToCreature },                 // :ToCreature()
    { MTMAP_SAFE,                   "ToPlayer", &LuaObject::ToPlayer },                     // :ToPlayer()
    { MTMAP_SAFE,                   "ToCorpse", &LuaObject::ToCorpse },                     // :ToCorpse()
    { MTMAP_SAFE,                   "RemoveFlag", &LuaObject::RemoveFlag },                 // :RemoveFlag(index, flag)

    { FLAGS_NONE, nullptr, nullptr },
};

ElunaRegister<WorldObject> WorldObjectMethods[] =
{
    // Getters
    { MTMAP_SAFE,                   "GetName", &LuaWorldObject::GetName },                              // :GetName()
    { MTMAP_SAFE,                   "GetMap", &LuaWorldObject::GetMap },                                // :GetMap() - Returns the WorldObject's current map object
#if (!defined(TBC) && !defined(CLASSIC))
    { MTMAP_SAFE,                   "GetPhaseMask", &LuaWorldObject::GetPhaseMask },                    // :GetPhaseMask()
#endif
    { MTMAP_SAFE,                   "GetInstanceId", &LuaWorldObject::GetInstanceId },                  // :GetInstanceId()
    { MTMAP_SAFE,                   "GetAreaId", &LuaWorldObject::GetAreaId },                          // :GetAreaId()
    { MTMAP_SAFE,                   "GetZoneId", &LuaWorldObject::GetZoneId },                          // :GetZoneId()
    { MTMAP_SAFE,                   "GetMapId", &LuaWorldObject::GetMapId },                            // :GetMapId() - Returns the WorldObject's current map ID
    { MTMAP_SAFE,                   "GetX", &LuaWorldObject::GetX },                                    // :GetX()
    { MTMAP_SAFE,                   "GetY", &LuaWorldObject::GetY },                                    // :GetY()
    { MTMAP_SAFE,                   "GetZ", &LuaWorldObject::GetZ },                                    // :GetZ()
    { MTMAP_SAFE,                   "GetO", &LuaWorldObject::GetO },                                    // :GetO()
    { MTMAP_SAFE,                   "GetLocation", &LuaWorldObject::GetLocation },                      // :GetLocation() - returns X, Y, Z and O co - ords (in that order)
    { MTMAP_SAFE,                   "GetPlayersInRange", &LuaWorldObject::GetPlayersInRange },          // :GetPlayersInRange([range]) - Returns a table with players in range of the WorldObject.
    { MTMAP_SAFE,                   "GetCreaturesInRange", &LuaWorldObject::GetCreaturesInRange },      // :GetCreaturesInRange([range, entry]) - Returns a table with creatures of given entry in range of the WorldObject.
    { MTMAP_SAFE,                   "GetGameObjectsInRange", &LuaWorldObject::GetGameObjectsInRange },  // :GetGameObjectsInRange([range, entry]) - Returns a table with gameobjects of given entry in range of the WorldObject.
    { MTMAP_SAFE,                   "GetNearestPlayer", &LuaWorldObject::GetNearestPlayer },            // :GetNearestPlayer([range]) - Returns nearest player in sight or given range.
    { MTMAP_SAFE,                   "GetNearestGameObject", &LuaWorldObject::GetNearestGameObject },    // :GetNearestGameObject([range, entry]) - Returns nearest gameobject with given entry in sight or given range entry can be 0 or nil for any.
    { MTMAP_SAFE,                   "GetNearestCreature", &LuaWorldObject::GetNearestCreature },        // :GetNearestCreature([range, entry]) - Returns nearest creature with given entry in sight or given range entry can be 0 or nil for any.
    { MTMAP_SAFE,                   "GetNearObject", &LuaWorldObject::GetNearObject },
    { MTMAP_SAFE,                   "GetNearObjects", &LuaWorldObject::GetNearObjects },
    { MTMAP_SAFE,                   "GetDistance", &LuaWorldObject::GetDistance },
    { MTMAP_SAFE,                   "GetExactDistance", &LuaWorldObject::GetExactDistance },
    { MTMAP_SAFE,                   "GetDistance2d", &LuaWorldObject::GetDistance2d },
    { MTMAP_SAFE,                   "GetExactDistance2d", &LuaWorldObject::GetExactDistance2d },
    { MTMAP_SAFE,                   "GetRelativePoint", &LuaWorldObject::GetRelativePoint },            // :GetRelativePoint(dist, rad) - Returns the x, y and z of a point dist away from worldobject.
    { MTMAP_SAFE,                   "GetAngle", &LuaWorldObject::GetAngle },                            // :GetAngle(WorldObject or x, y) - Returns angle between world object and target or x and y coords.

    // Boolean
    { MTMAP_SAFE,                   "IsWithinLoS", &LuaWorldObject::IsWithinLoS },

    // Other
    { MTMAP_SAFE,                   "SummonGameObject", &LuaWorldObject::SummonGameObject },            // :SummonGameObject(entry, x, y, z, o[, respawnDelay]) - Spawns an object to location. Returns the object or nil
    { MTMAP_SAFE,                   "SpawnCreature", &LuaWorldObject::SpawnCreature },                  // :SpawnCreature(entry, x, y, z, o[, spawnType, despawnDelay]) - Spawns a creature to location that despawns after given time (0 for infinite). Returns the creature or nil
    { MTMAP_SAFE,                   "SendPacket", &LuaWorldObject::SendPacket },                        // :SendPacket(packet) - Sends a specified packet to everyone around
    { MTMAP_SAFE,                   "RegisterEvent", &LuaWorldObject::RegisterEvent },
    { MTMAP_SAFE,                   "RemoveEventById", &LuaWorldObject::RemoveEventById },
    { MTMAP_SAFE,                   "RemoveEvents", &LuaWorldObject::RemoveEvents },

    { FLAGS_NONE, nullptr, nullptr },
};

ElunaRegister<Unit> UnitMethods[] =
{
    // Getters
    { MTMAP_SAFE,                   "GetLevel", &LuaUnit::GetLevel },                                   // :GetLevel()
    { MTMAP_SAFE,                   "GetHealth", &LuaUnit::GetHealth },                                 // :GetHealth()
    { MTMAP_SAFE,                   "GetDisplayId", &LuaUnit::GetDisplayId },                           // :GetDisplayId()
    { MTMAP_SAFE,                   "GetNativeDisplayId", &LuaUnit::GetNativeDisplayId },               // :GetNativeDisplayId()
    { MTMAP_SAFE,                   "GetPower", &LuaUnit::GetPower },                                   // :GetPower([type]) - returns power for power type. type can be omitted
    { MTMAP_SAFE,                   "GetMaxPower", &LuaUnit::GetMaxPower },                             // :GetMaxPower([type]) - returns max power for power type. type can be omitted
    { MTMAP_SAFE,                   "GetPowerType", &LuaUnit::GetPowerType },                           // :GetPowerType() - Returns the power type tye unit uses
    { MTMAP_SAFE,                   "GetMaxHealth", &LuaUnit::GetMaxHealth },                           // :GetMaxHealth()
    { MTMAP_SAFE,                   "GetHealthPct", &LuaUnit::GetHealthPct },                           // :GetHealthPct()
    { MTMAP_SAFE,                   "GetPowerPct", &LuaUnit::GetPowerPct },                             // :GetPowerPct([type]) - returns power percent for power type. type can be omitted
    { MTMAP_SAFE,                   "GetGender", &LuaUnit::GetGender },                                 // :GetGender() - returns the gender where male = 0 female = 1
    { MTMAP_SAFE,                   "GetRace", &LuaUnit::GetRace },                                     // :GetRace()
    { MTMAP_SAFE,                   "GetClass", &LuaUnit::GetClass },                                   // :GetClass()
    { MTMAP_SAFE,                   "GetRaceAsString", &LuaUnit::GetRaceAsString },                     // :GetRaceAsString([locale])
    { MTMAP_SAFE,                   "GetClassAsString", &LuaUnit::GetClassAsString },                   // :GetClassAsString([locale])
    { MTMAP_SAFE,                   "GetAura", &LuaUnit::GetAura },                                     // :GetAura(spellID) - returns aura object
    { MTMAP_SAFE,                   "GetCombatTime", &LuaUnit::GetCombatTime },                         // :GetCombatTime() - Returns how long the unit has been in combat
    { MTMAP_SAFE,                   "GetFaction", &LuaUnit::GetFaction },                               // :GetFaction() - Returns the unit's factionId
    { MTMAP_SAFE,                   "GetCurrentSpell", &LuaUnit::GetCurrentSpell },                     // :GetCurrentSpell(type) - Returns the currently casted spell of given type if any
    { MTMAP_SAFE,                   "GetCreatureType", &LuaUnit::GetCreatureType },                     // :GetCreatureType() - Returns the unit's type
    { MTMAP_SAFE,                   "GetMountId", &LuaUnit::GetMountId },                               // :GetMountId()
    { MTMAP_SAFE,                   "GetOwner", &LuaUnit::GetOwner },                                   // :GetOwner() - Returns the owner
    { MTMAP_SAFE,                   "GetFriendlyUnitsInRange", &LuaUnit::GetFriendlyUnitsInRange },     // :GetFriendlyUnitsInRange([range]) - Returns a list of friendly units in range, can return nil
    { MTMAP_SAFE,                   "GetUnfriendlyUnitsInRange", &LuaUnit::GetUnfriendlyUnitsInRange }, // :GetUnfriendlyUnitsInRange([range]) - Returns a list of unfriendly units in range, can return nil
    { MTMAP_SAFE,                   "GetOwnerGUID", &LuaUnit::GetOwnerGUID },                           // :GetOwnerGUID() - Returns the UNIT_FIELD_SUMMONEDBY owner
    { MTMAP_SAFE,                   "GetCreatorGUID", &LuaUnit::GetCreatorGUID },                       // :GetCreatorGUID() - Returns the UNIT_FIELD_CREATEDBY creator
    { MTMAP_SAFE,                   "GetMinionGUID", &LuaUnit::GetPetGUID },                            // :GetMinionGUID() - Decapreted. GetMinionGUID is same as GetPetGUID
    { MTMAP_SAFE,                   "GetCharmerGUID", &LuaUnit::GetCharmerGUID },                       // :GetCharmerGUID() - Returns the UNIT_FIELD_CHARMEDBY charmer
    { MTMAP_SAFE,                   "GetCharmGUID", &LuaUnit::GetCharmGUID },                           // :GetCharmGUID() - Returns the unit's UNIT_FIELD_CHARM guid
    { MTMAP_SAFE,                   "GetPetGUID", &LuaUnit::GetPetGUID },                               // :GetPetGUID() - Returns the unit's pet GUID
#if (!defined(TBC) && !defined(CLASSIC))
    { MTMAP_SAFE,                   "GetCritterGUID", &LuaUnit::GetCritterGUID },                       // :GetCritterGUID() - Returns the critter's GUID
#endif
    { MTMAP_SAFE,                   "GetControllerGUID", &LuaUnit::GetControllerGUID },                 // :GetControllerGUID() - Returns the Charmer or Owner GUID
    { MTMAP_SAFE,                   "GetControllerGUIDS", &LuaUnit::GetControllerGUIDS },               // :GetControllerGUIDS() - Returns the charmer, owner or unit's own GUID
    { MTMAP_SAFE,                   "GetStandState", &LuaUnit::GetStandState },                         // :GetStandState() - Returns the unit's stand state
    { MTMAP_SAFE,                   "GetVictim", &LuaUnit::GetVictim },                                 // :GetVictim() - Returns creature's current target
    { MTMAP_SAFE,                   "GetSpeed", &LuaUnit::GetSpeed },                                   // :GetSpeed(movementType) - Returns the unit's speed
    { MTMAP_SAFE,                   "GetStat", &LuaUnit::GetStat },                                     // :GetStat(stat)
    { MTMAP_SAFE,                   "GetBaseSpellPower", &LuaUnit::GetBaseSpellPower },                 // :GetBaseSpellPower()
#if (!defined(TBC) && !defined(CLASSIC))
    { MTMAP_SAFE,                   "GetVehicleKit", &LuaUnit::GetVehicleKit },                         // :GetVehicleKit() - Gets unit's Vehicle kit if the unit is a vehicle
    // { "GetVehicle", &LuaUnit::GetVehicle },                            // :GetVehicle() - Gets the Vehicle kit of the vehicle the unit is on
#endif
    { MTMAP_SAFE,                   "GetMovementType", &LuaUnit::GetMovementType },

    // Setters
    { MTMAP_SAFE,                   "SetFaction", &LuaUnit::SetFaction },                   // :SetFaction(factionId) - Sets the unit's faction
    { MTMAP_SAFE,                   "SetLevel", &LuaUnit::SetLevel },                       // :SetLevel(amount)
    { MTMAP_SAFE,                   "SetHealth", &LuaUnit::SetHealth },                     // :SetHealth(amount)
    { MTMAP_SAFE,                   "SetMaxHealth", &LuaUnit::SetMaxHealth },               // :SetMaxHealth(amount)
    { MTMAP_SAFE,                   "SetPower", &LuaUnit::SetPower },                       // :SetPower([type,] amount)
    { MTMAP_SAFE,                   "SetMaxPower", &LuaUnit::SetMaxPower },                 // :SetMaxPower([type,] amount)
    { MTMAP_SAFE,                   "SetPowerType", &LuaUnit::SetPowerType },               // :SetPowerType(type)
    { MTMAP_SAFE,                   "SetDisplayId", &LuaUnit::SetDisplayId },               // :SetDisplayId(id)
    { MTMAP_SAFE,                   "SetNativeDisplayId", &LuaUnit::SetNativeDisplayId },   // :SetNativeDisplayId(id)
    { MTMAP_SAFE,                   "SetFacing", &LuaUnit::SetFacing },                     // :SetFacing(o) - Sets the Unit facing / orientation to arg
    { MTMAP_SAFE,                   "SetFacingToObject", &LuaUnit::SetFacingToObject },     // :SetFacingToObject(worldObject) - Sets the Unit facing / orientation towards the WorldObject
#if (!defined(TBC) && !defined(CLASSIC))
    { MTMAP_SAFE,                   "SetPhaseMask", &LuaUnit::SetPhaseMask },               // :SetPhaseMask(Phase[, update]) - Sets the phase of the unit
#endif
    { MTMAP_SAFE,                   "SetSpeed", &LuaUnit::SetSpeed },                       // :SetSpeed(type, speed[, forced]) - Sets speed for the movement type (0 = walk, 1 = run ..)
    // { "SetStunned", &LuaUnit::SetStunned },                // :SetStunned([enable]) - Stuns or removes stun
    { MTMAP_SAFE,                   "SetRooted", &LuaUnit::SetRooted },                       // :SetRooted([enable]) - Roots or removes root
    { MTMAP_SAFE,                   "SetConfused", &LuaUnit::SetConfused },                   // :SetConfused([enable]) - Sets confused or removes confusion
    { MTMAP_SAFE,                   "SetFeared", &LuaUnit::SetFeared },                       // :SetFeared([enable]) - Fears or removes fear
    { MTMAP_SAFE,                   "SetPvP", &LuaUnit::SetPvP },                           // :SetPvP([apply]) - Sets the units PvP on or off
#if (!defined(TBC) && !defined(CLASSIC))
    { MTMAP_SAFE,                   "SetFFA", &LuaUnit::SetFFA },                           // :SetFFA([apply]) - Sets the units FFA tag on or off
    { MTMAP_SAFE,                   "SetSanctuary", &LuaUnit::SetSanctuary },               // :SetSanctuary([apply]) - Enables or disables units sanctuary flag
#endif
    // { "SetCanFly", &LuaUnit::SetCanFly },                  // :SetCanFly(apply)
    // { "SetVisible", &LuaUnit::SetVisible },                // :SetVisible(x)
    { MTMAP_SAFE,                   "SetOwnerGUID", &LuaUnit::SetOwnerGUID },               // :SetOwnerGUID(guid) - Sets the guid of the owner
    { MTMAP_SAFE,                   "SetName", &LuaUnit::SetName },                         // :SetName(name) - Sets the unit's name
    { MTMAP_SAFE,                   "SetSheath", &LuaUnit::SetSheath },                     // :SetSheath(SheathState) - Sets unit's sheathstate
    { MTMAP_SAFE,                   "SetCreatorGUID", &LuaUnit::SetCreatorGUID },           // :SetCreatorGUID(uint64 ownerGUID) - Sets the owner's guid of a summoned creature, etc
    { MTMAP_SAFE,                   "SetMinionGUID", &LuaUnit::SetPetGUID },                // Decapreted. Same as SetPetGUID
    { MTMAP_SAFE,                   "SetCharmerGUID", &LuaUnit::SetCharmerGUID },           // :SetCharmerGUID(uint64 ownerGUID) - Sets the UNIT_FIELD_CHARMEDBY charmer GUID
    { MTMAP_SAFE,                   "SetPetGUID", &LuaUnit::SetPetGUID },                   // :SetPetGUID(uint64 guid) - Sets the pet's guid
#if (!defined(TBC) && !defined(CLASSIC))
    { MTMAP_SAFE,                   "SetCritterGUID", &LuaUnit::SetCritterGUID },           // :SetCritterGUID(uint64 guid) - Sets the critter's guid
#endif
    { MTMAP_SAFE,                   "SetWaterWalk", &LuaUnit::SetWaterWalk },               // :SetWaterWalk([enable]) - Sets WaterWalk on or off
    { MTMAP_SAFE,                   "SetStandState", &LuaUnit::SetStandState },             // :SetStandState(state) - Sets the stand state (Stand, Kneel, sleep, etc) of the unit

    // Boolean
    { MTMAP_SAFE,                   "IsAlive", &LuaUnit::IsAlive },                                 // :IsAlive()
    { MTMAP_SAFE,                   "IsDead", &LuaUnit::IsDead },                                   // :IsDead() - Returns true if the unit is dead, false if they are alive
    { MTMAP_SAFE,                   "IsDying", &LuaUnit::IsDying },                                 // :IsDying() - Returns true if the unit death state is JUST_DIED.
    { MTMAP_SAFE,                   "IsPvPFlagged", &LuaUnit::IsPvPFlagged },                       // :IsPvPFlagged()
    { MTMAP_SAFE,                   "IsInCombat", &LuaUnit::IsInCombat },                           // :IsInCombat()
    { MTMAP_SAFE,                   "IsBanker", &LuaUnit::IsBanker },                               // :IsBanker() - Returns true if the unit is a banker, false if not
    { MTMAP_SAFE,                   "IsBattleMaster", &LuaUnit::IsBattleMaster },                   // :IsBattleMaster() - Returns true if the unit is a battle master, false if not
    { MTMAP_SAFE,                   "IsCharmed", &LuaUnit::IsCharmed },                             // :IsCharmed() - Returns true if the unit is charmed, false if not
    { MTMAP_SAFE,                   "IsArmorer", &LuaUnit::IsArmorer },                             // :IsArmorer() - Returns true if the unit is an Armorer, false if not
    { MTMAP_SAFE,                   "IsAttackingPlayer", &LuaUnit::IsAttackingPlayer },             // :IsAttackingPlayer() - Returns true if the unit is attacking a player, false if not
    { MTMAP_SAFE,                   "IsInWater", &LuaUnit::IsInWater },                             // :IsInWater() - Returns true if the unit is in water
    { MTMAP_SAFE,                   "IsUnderWater", &LuaUnit::IsUnderWater },                       // :IsUnderWater() - Returns true if the unit is under water
    { MTMAP_SAFE,                   "IsAuctioneer", &LuaUnit::IsAuctioneer },                       // :IsAuctioneer()
    { MTMAP_SAFE,                   "IsGuildMaster", &LuaUnit::IsGuildMaster },                     // :IsGuildMaster()
    { MTMAP_SAFE,                   "IsInnkeeper", &LuaUnit::IsInnkeeper },                         // :IsInnkeeper()
    { MTMAP_SAFE,                   "IsTrainer", &LuaUnit::IsTrainer },                             // :IsTrainer()
    { MTMAP_SAFE,                   "IsGossip", &LuaUnit::IsGossip },                               // :IsGossip()
    { MTMAP_SAFE,                   "IsTaxi", &LuaUnit::IsTaxi },                                   // :IsTaxi()
    { MTMAP_SAFE,                   "IsSpiritHealer", &LuaUnit::IsSpiritHealer },                   // :IsSpiritHealer()
    { MTMAP_SAFE,                   "IsSpiritGuide", &LuaUnit::IsSpiritGuide },                     // :IsSpiritGuide()
    { MTMAP_SAFE,                   "IsTabardDesigner", &LuaUnit::IsTabardDesigner },               // :IsTabardDesigner()
    { MTMAP_SAFE,                   "IsServiceProvider", &LuaUnit::IsServiceProvider },             // :IsServiceProvider()
    { MTMAP_SAFE,                   "IsSpiritService", &LuaUnit::IsSpiritService },                 // :IsSpiritService()
    { MTMAP_SAFE,                   "HealthBelowPct", &LuaUnit::HealthBelowPct },                   // :HealthBelowPct(int32 pct)
    { MTMAP_SAFE,                   "HealthAbovePct", &LuaUnit::HealthAbovePct },                   // :HealthAbovePct(int32 pct)
    { MTMAP_SAFE,                   "IsMounted", &LuaUnit::IsMounted },                             // :IsMounted()
    { MTMAP_SAFE,                   "AttackStop", &LuaUnit::AttackStop },                           // :AttackStop()
    { MTMAP_SAFE,                   "Attack", &LuaUnit::Attack },                                   // :Attack(who[, meleeAttack])
    // { "IsVisible", &LuaUnit::IsVisible },                          // :IsVisible()
    // { "IsMoving", &LuaUnit::IsMoving },                            // :IsMoving()
    // { "IsFlying", &LuaUnit::IsFlying },                            // :IsFlying()
    { MTMAP_SAFE,                   "IsStopped", &LuaUnit::IsStopped },                             // :IsStopped()
    { MTMAP_SAFE,                   "HasUnitState", &LuaUnit::HasUnitState },                       // :HasUnitState(state) - state from UnitState enum
    { MTMAP_SAFE,                   "IsQuestGiver", &LuaUnit::IsQuestGiver },                       // :IsQuestGiver() - Returns true if the unit is a quest giver, false if not
    { MTMAP_SAFE,                   "IsWithinDistInMap", &LuaUnit::IsWithinDistInMap },             // :IsWithinDistInMap(worldObject, radius) - Returns if the unit is within distance in map of the worldObject
    { MTMAP_SAFE,                   "IsInAccessiblePlaceFor", &LuaUnit::IsInAccessiblePlaceFor },   // :IsInAccessiblePlaceFor(creature) - Returns if the unit is in an accessible place for the specified creature
    { MTMAP_SAFE,                   "IsVendor", &LuaUnit::IsVendor },                               // :IsVendor() - Returns if the unit is a vendor or not
    { MTMAP_SAFE,                   "IsRooted", &LuaUnit::IsRooted },                               // :IsRooted()
    { MTMAP_SAFE,                   "IsFullHealth", &LuaUnit::IsFullHealth },                       // :IsFullHealth() - Returns if the unit is full health
    { MTMAP_SAFE,                   "HasAura", &LuaUnit::HasAura },                                 // :HasAura(spellId) - Returns true if the unit has the aura from the spell
    { MTMAP_SAFE,                   "IsCasting", &LuaUnit::IsCasting },
    { MTMAP_SAFE,                   "IsStandState", &LuaUnit::IsStandState },                       // :IsStandState() - Returns true if the unit is standing
#ifndef CLASSIC
    { MTMAP_SAFE,                   "IsOnVehicle", &LuaUnit::IsOnVehicle },                         // :IsOnVehicle() - Checks if the unit is on a vehicle
#endif

    // Other
    { MTMAP_SAFE,                   "AddAura", &LuaUnit::AddAura },                                 // :AddAura(spellId, target) - Adds an aura to the specified target
    { MTMAP_SAFE,                   "RemoveAura", &LuaUnit::RemoveAura },                           // :RemoveAura(spellId[, casterGUID]) - Removes an aura from the unit by the spellId, casterGUID(Original caster) is optional
    { MTMAP_SAFE,                   "RemoveAllAuras", &LuaUnit::RemoveAllAuras },                   // :RemoveAllAuras() - Removes all the unit's auras
    { MTMAP_SAFE,                   "ClearInCombat", &LuaUnit::ClearInCombat },                     // :ClearInCombat() - Clears the unit's combat list (unit will be out of combat), resets the timer to 0, etc
    { MTMAP_SAFE,                   "DeMorph", &LuaUnit::DeMorph },                                 // :DeMorph() - Sets display back to native
    { MTMAP_SAFE,                   "SendUnitWhisper", &LuaUnit::SendUnitWhisper },                 // :SendUnitWhisper(msg, lang, receiver[, bossWhisper]) - Sends a whisper to the receiver
    { MTMAP_SAFE,                   "SendUnitEmote", &LuaUnit::SendUnitEmote },                     // :SendUnitEmote(msg[, receiver, bossEmote]) - Sends a text emote
    { MTMAP_SAFE,                   "SendUnitSay", &LuaUnit::SendUnitSay },                         // :SendUnitSay(msg, language) - Sends a "Say" message with the specified language (all languages: 0)
    { MTMAP_SAFE,                   "SendUnitYell", &LuaUnit::SendUnitYell },                       // :SendUnitYell(msg, language) - Sends a "Yell" message with the specified language (all languages: 0)
    { MTMAP_SAFE,                   "CastSpell", &LuaUnit::CastSpell },                             // :CastSpell(target, spellID[, triggered]) - Casts spell on target (player/npc/creature), if triggered is true then instant cast
    { MTMAP_SAFE,                   "CastCustomSpell", &LuaUnit::CastCustomSpell },                 // :CastCustomSpell([Unit] target, uint32 spell, bool triggered = false, int32 bp0 = nil, int32 bp1 = nil, int32 bp2 = nil, [Item] castItem = nil, uint64 originalCaster = 0) - Casts spell on target (player/npc/creature), if triggered is true then instant cast. pb0, 1 and 2 are modifiers for the base points of the spell.
    { MTMAP_SAFE,                   "CastSpellAoF", &LuaUnit::CastSpellAoF },                       // :CastSpellAoF(x, y, z, spellID[, triggered]) - Casts the spell on coordinates, if triggered is false has mana cost and cast time
    { MTMAP_SAFE,                   "PlayDirectSound", &LuaUnit::PlayDirectSound },                 // :PlayDirectSound(soundId[, player]) - Unit plays soundID to player, or everyone around if no player
    { MTMAP_SAFE,                   "PlayDistanceSound", &LuaUnit::PlayDistanceSound },             // :PlayDistanceSound(soundId[, player]) - Unit plays soundID to player, or everyone around if no player. The sound fades the further you are
    { MTMAP_SAFE,                   "Kill", &LuaUnit::Kill },                                       // :Kill(target, durabilityLoss) - Unit kills the target. Durabilityloss is true by default
    { MTMAP_SAFE,                   "StopSpellCast", &LuaUnit::StopSpellCast },                     // :StopSpellCast([spellId]) - Stops the unit from casting a spell. If a spellId is defined, it will stop that unit from casting that spell
    { MTMAP_SAFE,                   "InterruptSpell", &LuaUnit::InterruptSpell },                   // :InterruptSpell(spellType[, delayed]) - Interrupts the unit's spell by the spellType. If delayed is true it will skip if the spell is delayed.
    { MTMAP_SAFE,                   "SendChatMessageToPlayer", &LuaUnit::SendChatMessageToPlayer }, // :SendChatMessageToPlayer(type, lang, msg, target) - Unit sends a chat message to the given target player
    { MTMAP_SAFE,                   "Emote", &LuaUnit::Emote },                                     // :Emote(emote)
    { MTMAP_SAFE,                   "CountPctFromCurHealth", &LuaUnit::CountPctFromCurHealth },     // :CountPctFromCurHealth(int32 pct)
    { MTMAP_SAFE,                   "CountPctFromMaxHealth", &LuaUnit::CountPctFromMaxHealth },     // :CountPctFromMaxHealth()
    { MTMAP_SAFE,                   "Dismount", &LuaUnit::Dismount },                               // :Dismount() - Dismounts the unit.
    { MTMAP_SAFE,                   "Mount", &LuaUnit::Mount },                                     // :Mount(displayId) - Mounts the unit on the specified displayId.
    // { "RestoreDisplayId", &LuaUnit::RestoreDisplayId },            // :RestoreDisplayId()
    // { "RestoreFaction", &LuaUnit::RestoreFaction },                // :RestoreFaction()
    // { "RemoveBindSightAuras", &LuaUnit::RemoveBindSightAuras },    // :RemoveBindSightAuras()
    // { "RemoveCharmAuras", &LuaUnit::RemoveCharmAuras },            // :RemoveCharmAuras()
    { MTMAP_SAFE,                   "ClearThreatList", &LuaUnit::ClearThreatList },                 // :ClearThreatList()
    { MTMAP_SAFE,                   "ClearUnitState", &LuaUnit::ClearUnitState },                   // :ClearUnitState(state)
    { MTMAP_SAFE,                   "AddUnitState", &LuaUnit::AddUnitState },                       // :AddUnitState(state)
    // { "DisableMelee", &LuaUnit::DisableMelee },                    // :DisableMelee([disable]) - if true, enables
    // { "SummonGuardian", &LuaUnit::SummonGuardian },                // :SummonGuardian(entry, x, y, z, o[, duration]) - summons a guardian to location. Scales with summoner, is friendly to him and guards him.
    { MTMAP_SAFE,                   "NearTeleport", &LuaUnit::NearTeleport },                       // :NearTeleport(x, y, z, o) - Teleports to give coordinates. Does not leave combat or unsummon pet.
    { MTMAP_SAFE,                   "MoveIdle", &LuaUnit::MoveIdle },                               // :MoveIdle()
    { MTMAP_SAFE,                   "MoveRandom", &LuaUnit::MoveRandom },                           // :MoveRandom(radius)
    { MTMAP_SAFE,                   "MoveHome", &LuaUnit::MoveHome },                               // :MoveHome()
    { MTMAP_SAFE,                   "MoveFollow", &LuaUnit::MoveFollow },                           // :MoveFollow(target[, dist, angle])
    { MTMAP_SAFE,                   "MoveChase", &LuaUnit::MoveChase },                             // :MoveChase(target[, dist, angle])
    { MTMAP_SAFE,                   "MoveConfused", &LuaUnit::MoveConfused },                       // :MoveConfused()
    { MTMAP_SAFE,                   "MoveFleeing", &LuaUnit::MoveFleeing },                         // :MoveFleeing(enemy[, time])
    { MTMAP_SAFE,                   "MoveTo", &LuaUnit::MoveTo },                                   // :MoveTo(id, x, y, z[, genPath]) - Moves to point. id is sent to WP reach hook. genPath defaults to true
#if (!defined(TBC) && !defined(CLASSIC))
    { MTMAP_SAFE,                   "MoveJump", &LuaUnit::MoveJump },                               // :MoveJump(x, y, z, zSpeed, maxHeight, id)
#endif
    { MTMAP_SAFE,                   "MoveStop", &LuaUnit::MoveStop },                               // :MoveStop()
    { MTMAP_SAFE,                   "MoveExpire", &LuaUnit::MoveExpire },                           // :MoveExpire([reset])
    { MTMAP_SAFE,                   "MoveClear", &LuaUnit::MoveClear },                             // :MoveClear([reset])
    { MTMAP_SAFE,                   "DealDamage", &LuaUnit::DealDamage },                           // :DealDamage(target, amount[, durabilityloss]) - Deals damage to target, durabilityloss is true by default
    { MTMAP_SAFE,                   "DealHeal", &LuaUnit::DealHeal },                               // :DealDamage(target, amount, spell[, critical]) - Heals target by given amount. This will be logged as being healed by spell as critical if true.
    { MTMAP_SAFE,                   "AddThreat", &LuaUnit::AddThreat },

    { FLAGS_NONE, nullptr, nullptr },
};

ElunaRegister<Player> PlayerMethods[] =
{
    // Getters
    { MTMAP_SAFE,                   "GetSelection", &LuaPlayer::GetSelection },                                 // :GetSelection()
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "GetGMRank", &LuaPlayer::GetGMRank },                                       // :GetSecurity()
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "GetGuildId", &LuaPlayer::GetGuildId },                                     // :GetGuildId() - nil on no guild
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "GetCoinage", &LuaPlayer::GetCoinage },                                     // :GetCoinage()
    { MTMAP_SAFE,                   "GetTeam", &LuaPlayer::GetTeam },                                           // :GetTeam() - returns the player's team. 0 for ally, 1 for horde
    { MTMAP_SAFE,                   "GetItemCount", &LuaPlayer::GetItemCount },                                 // :GetItemCount(item_id[, check_bank])
    { MTMAP_SAFE,                   "GetGroup", &LuaPlayer::GetGroup },                                         // :GetGroup()
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "GetGuild", &LuaPlayer::GetGuild },                                         // :GetGuild()
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "GetAccountId", &LuaPlayer::GetAccountId },                                 // :GetAccountId()
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "GetAccountName", &LuaPlayer::GetAccountName },                             // :GetAccountName()
#ifndef CATA
#ifndef CLASSIC
    { MTMAP_SAFE,                   "GetArenaPoints", &LuaPlayer::GetArenaPoints },                             // :GetArenaPoints()
    { MTMAP_SAFE,                   "GetHonorPoints", &LuaPlayer::GetHonorPoints },                             // :GetHonorPoints()
#endif
#endif
    { MTMAP_SAFE,                   "GetLifetimeKills", &LuaPlayer::GetLifetimeKills },                         // :GetLifetimeKills() - Returns the player's lifetime (honorable) kills
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "GetPlayerIP", &LuaPlayer::GetPlayerIP },                                   // :GetPlayerIP() - Returns the player's IP Address
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "GetLevelPlayedTime", &LuaPlayer::GetLevelPlayedTime },                     // :GetLevelPlayedTime() - Returns the player's played time at that level
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "GetTotalPlayedTime", &LuaPlayer::GetTotalPlayedTime },                     // :GetTotalPlayedTime() - Returns the total played time of that player
    { MTMAP_SAFE,                   "GetItemByPos", &LuaPlayer::GetItemByPos },                                 // :GetItemByPos(bag, slot) - Returns item in given slot in a bag (bag: 19-22 slot: 0-35) or inventory (bag: 255 slot : 0-38)
    { MTMAP_SAFE,                   "GetItemByEntry", &LuaPlayer::GetItemByEntry },                             // :GetItemByEntry(entry) - Gets an item if the player has it
    { MTMAP_SAFE,                   "GetItemByGUID", &LuaPlayer::GetItemByGUID },
    { MTMAP_SAFE,                   "GetReputation", &LuaPlayer::GetReputation },                               // :GetReputation(faction) - Gets player's reputation with given faction
    { MTMAP_SAFE,                   "GetEquippedItemBySlot", &LuaPlayer::GetEquippedItemBySlot },               // :GetEquippedItemBySlot(slotId) - Returns equipped item by slot
    { MTMAP_SAFE,                   "GetQuestLevel", &LuaPlayer::GetQuestLevel },                               // :GetQuestLevel(quest) - Returns quest's level
    { MTMAP_SAFE,                   "GetChatTag", &LuaPlayer::GetChatTag },                                     // :GetChatTag() - Returns player chat tag ID
    { MTMAP_SAFE,                   "GetRestBonus", &LuaPlayer::GetRestBonus },                                 // :GetRestBonus() - Gets player's rest bonus
#ifdef WOTLK
    { MTMAP_SAFE,                   "GetPhaseMaskForSpawn", &LuaPlayer::GetPhaseMaskForSpawn },                 // :GetPhaseMaskForSpawn() - Gets the real phasemask for spawning things. Used if the player is in GM mode
#endif
    { MTMAP_SAFE,                   "GetReqKillOrCastCurrentCount", &LuaPlayer::GetReqKillOrCastCurrentCount }, // :GetReqKillOrCastCurrentCount(questId, entry) - Gets the objective (kill or cast) current count done
    { MTMAP_SAFE,                   "GetQuestStatus", &LuaPlayer::GetQuestStatus },                             // :GetQuestStatus(entry) - Gets the quest's status
    { MTMAP_SAFE,                   "GetInGameTime", &LuaPlayer::GetInGameTime },                               // :GetInGameTime() - Returns player's ingame time
    { MTMAP_SAFE,                   "GetComboPoints", &LuaPlayer::GetComboPoints },                             // :GetComboPoints() - Returns player's combo points
    { MTMAP_SAFE,                   "GetComboTarget", &LuaPlayer::GetComboTarget },                             // :GetComboTarget() - Returns the player's combo target
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "GetGuildName", &LuaPlayer::GetGuildName },                                 // :GetGuildName() - Returns player's guild's name or nil
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "GetFreeTalentPoints", &LuaPlayer::GetFreeTalentPoints },                   // :GetFreeTalentPoints() - Returns the amount of unused talent points
#if (!defined(TBC) && !defined(CLASSIC))
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "GetActiveSpec", &LuaPlayer::GetActiveSpec },                               // :GetActiveSpec() - Returns the active specID
    { MTMAP_SAFE,                   "GetSpecsCount", &LuaPlayer::GetSpecsCount },                               // :GetSpecsCount() - Returns the player's spec count
#endif
    { MTMAP_SAFE,                   "GetSpellCooldownDelay", &LuaPlayer::GetSpellCooldownDelay },               // :GetSpellCooldownDelay(spellId) - Returns the spell's cooldown
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "GetGuildRank", &LuaPlayer::GetGuildRank },                                 // :GetGuildRank() - Gets the player's guild rank
    { MTMAP_SAFE,                   "GetDifficulty", &LuaPlayer::GetDifficulty },                               // :GetDifficulty([isRaid]) - Returns the current difficulty
    { MTMAP_SAFE,                   "GetHealthBonusFromStamina", &LuaPlayer::GetHealthBonusFromStamina },       // :GetHealthBonusFromStamina() - Returns the HP bonus from stamina
    { MTMAP_SAFE,                   "GetManaBonusFromIntellect", &LuaPlayer::GetManaBonusFromIntellect },       // :GetManaBonusFromIntellect() - Returns the mana bonus from intellect
    { MTMAP_SAFE,                   "GetMaxSkillValue", &LuaPlayer::GetMaxSkillValue },                         // :GetMaxSkillValue(skill) - Gets max skill value for the given skill
    { MTMAP_SAFE,                   "GetPureMaxSkillValue", &LuaPlayer::GetPureMaxSkillValue },                 // :GetPureMaxSkillValue(skill) - Gets max base skill value
    { MTMAP_SAFE,                   "GetSkillValue", &LuaPlayer::GetSkillValue },                               // :GetSkillValue(skill) - Gets current skill value
    { MTMAP_SAFE,                   "GetBaseSkillValue", &LuaPlayer::GetBaseSkillValue },                       // :GetBaseSkillValue(skill) - Gets current base skill value (no temp bonus)
    { MTMAP_SAFE,                   "GetPureSkillValue", &LuaPlayer::GetPureSkillValue },                       // :GetPureSkillValue(skill) - Gets current base skill value (no bonuses)
    { MTMAP_SAFE,                   "GetSkillPermBonusValue", &LuaPlayer::GetSkillPermBonusValue },             // :GetSkillPermBonusValue(skill) - Returns current permanent bonus
    { MTMAP_SAFE,                   "GetSkillTempBonusValue", &LuaPlayer::GetSkillTempBonusValue },             // :GetSkillTempBonusValue(skill) - Returns current temp bonus
    { MTMAP_SAFE,                   "GetReputationRank", &LuaPlayer::GetReputationRank },                       // :GetReputationRank(faction) - Returns the reputation rank with given faction
    { MTMAP_SAFE,                   "GetDrunkValue", &LuaPlayer::GetDrunkValue },                               // :GetDrunkValue() - Returns the current drunkness value
    { MTMAP_SAFE,                   "GetBattlegroundId", &LuaPlayer::GetBattlegroundId },                       // :GetBattlegroundId() - Returns the player's current battleground ID
    { MTMAP_SAFE,                   "GetBattlegroundTypeId", &LuaPlayer::GetBattlegroundTypeId },               // :GetBattlegroundTypeId() - Returns the player's current battleground type ID
    { MTMAP_SAFE,                   "GetXPRestBonus", &LuaPlayer::GetXPRestBonus },                             // :GetXPRestBonus(xp) - Returns the rested bonus XP from given XP
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "GetGroupInvite", &LuaPlayer::GetGroupInvite },                             // :GetGroupInvite() - Returns the group invited to
    { MTMAP_SAFE,                   "GetSubGroup", &LuaPlayer::GetSubGroup },                                   // :GetSubGroup() - Gets the player's current subgroup ID
    { MTMAP_SAFE,                   "GetNextRandomRaidMember", &LuaPlayer::GetNextRandomRaidMember },           // :GetNextRandomRaidMember(radius) - Gets a random raid member in given radius
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "GetOriginalGroup", &LuaPlayer::GetOriginalGroup },                         // :GetOriginalGroup() - Gets the original group object
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "GetOriginalSubGroup", &LuaPlayer::GetOriginalSubGroup },                   // :GetOriginalSubGroup() - Returns the original subgroup ID
#ifdef TRINITY
    { MTMAP_SAFE,                   "GetChampioningFaction", &LuaPlayer::GetChampioningFaction },               // :GetChampioningFaction() - Returns the player's championing faction
#endif
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "GetLatency", &LuaPlayer::GetLatency },                                     // :GetLatency() - Returns player's latency
    // { "GetRecruiterId", &LuaPlayer::GetRecruiterId },                          // :GetRecruiterId() - Returns player's recruiter's ID
    { MTMAP_SAFE,                   "GetDbLocaleIndex", &LuaPlayer::GetDbLocaleIndex },                         // :GetDbLocaleIndex() - Returns locale index
    { MTMAP_SAFE,                   "GetDbcLocale", &LuaPlayer::GetDbcLocale },                                 // :GetDbcLocale() - Returns DBC locale
    { MTMAP_SAFE,                   "GetCorpse", &LuaPlayer::GetCorpse },                                       // :GetCorpse() - Returns the player's corpse
    { MTMAP_SAFE,                   "GetGossipTextId", &LuaPlayer::GetGossipTextId },                           // :GetGossipTextId(worldObject) - Returns the WorldObject's gossip textId
    { MTMAP_SAFE,                   "GetQuestRewardStatus", &LuaPlayer::GetQuestRewardStatus },                 // :GetQuestRewardStatus(questId) - Returns the true/false of the quest reward status
#ifndef CATA
    { MTMAP_SAFE,                   "GetShieldBlockValue", &LuaPlayer::GetShieldBlockValue },                   // :GetShieldBlockValue() - Returns block value
#endif
#ifdef CLASSIC
    { MTMAP_SAFE,                   "GetHonorStoredKills", &LuaPlayer::GetHonorStoredKills },                     // :GetHonorStoredKills(on/off)
    { MTMAP_SAFE,                   "GetRankPoints", &LuaPlayer::GetRankPoints },                                 // :GetRankPoints()
    { MTMAP_SAFE,                   "GetHonorLastWeekStandingPos", &LuaPlayer::GetHonorLastWeekStandingPos },     // :GetHonorLastWeekStandingPos()
#endif

    // Setters
    { MTMAP_SAFE,                   "AdvanceSkillsToMax", &LuaPlayer::AdvanceSkillsToMax },     // :AdvanceSkillsToMax() - Advances all currently known skills to the currently known max level
    { MTMAP_SAFE,                   "AdvanceSkill", &LuaPlayer::AdvanceSkill },                 // :AdvanceSkill(skill_id, step) - Advances skill by ID and the amount(step)
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "AdvanceAllSkills", &LuaPlayer::AdvanceAllSkills },         // :AdvanceAllSkills(value) - Advances all current skills to your input(value)
    { MTMAP_SAFE,                   "AddLifetimeKills", &LuaPlayer::AddLifetimeKills },         // :AddLifetimeKills(val) - Adds lifetime (honorable) kills to your current lifetime kills
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "SetCoinage", &LuaPlayer::SetCoinage },                     // :SetCoinage(amount) - sets plr's coinage to this
#ifndef CLASSIC
    { MTMAP_SAFE,                   "SetKnownTitle", &LuaPlayer::SetKnownTitle },               // :SetKnownTitle(id)
    { MTMAP_SAFE,                   "UnsetKnownTitle", &LuaPlayer::UnsetKnownTitle },           // :UnsetKnownTitle(id)
#endif
    { MTMAP_SAFE,                   "SetBindPoint", &LuaPlayer::SetBindPoint },                 // :SetBindPoint(x, y, z, map, areaid) - sets home for hearthstone
#ifndef CATA
#ifndef CLASSIC
    { MTMAP_SAFE,                   "SetArenaPoints", &LuaPlayer::SetArenaPoints },             // :SetArenaPoints(amount)
    { MTMAP_SAFE,                   "SetHonorPoints", &LuaPlayer::SetHonorPoints },             // :SetHonorPoints(amount)
#endif
#endif
#ifdef CLASSIC
    { MTMAP_SAFE,                   "SetHonorStoredKills", &LuaPlayer::SetHonorStoredKills },     // :SetHonorStoredKills(kills, [on/off])
    { MTMAP_SAFE,                   "SetRankPoints", &LuaPlayer::SetRankPoints },                 // :SetRankPoints(rankPoints)
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "SetHonorLastWeekStandingPos", &LuaPlayer::SetHonorLastWeekStandingPos }, // :SetHonorLastWeekStandingPos(standingPos)
#endif
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "SetLifetimeKills", &LuaPlayer::SetLifetimeKills },         // :SetLifetimeKills(val) - Sets the overall lifetime (honorable) kills of the player
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "SetGameMaster", &LuaPlayer::SetGameMaster },               // :SetGameMaster([on]) - Sets GM mode on or off
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "SetGMChat", &LuaPlayer::SetGMChat },                       // :SetGMChat([on]) - Sets GM chat on or off
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "SetTaxiCheat", &LuaPlayer::SetTaxiCheat },                 // :SetTaxiCheat([on]) - Sets taxi cheat on or off
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "SetGMVisible", &LuaPlayer::SetGMVisible },                 // :SetGMVisible([on]) - Sets gm visibility on or off
    { MTMAP_SAFE,                   "SetPvPDeath", &LuaPlayer::SetPvPDeath },                   // :SetPvPDeath([on]) - Sets PvP death on or off
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "SetAcceptWhispers", &LuaPlayer::SetAcceptWhispers },       // :SetAcceptWhispers([on]) - Sets whisper accepting death on or off
    { MTMAP_SAFE,                   "SetRestBonus", &LuaPlayer::SetRestBonus },                 // :SetRestBonus(bonusrate) - Sets new restbonus rate
    { MTMAP_SAFE,                   "SetQuestStatus", &LuaPlayer::SetQuestStatus },             // :SetQuestStatus(entry, status) - Sets the quest's status
    { MTMAP_SAFE,                   "SetReputation", &LuaPlayer::SetReputation },               // :SetReputation(faction, value) - Sets the faction reputation for the player
    { MTMAP_SAFE,                   "SetFreeTalentPoints", &LuaPlayer::SetFreeTalentPoints },   // :SetFreeTalentPoints(points) - Sets the amount of unused talent points
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "SetGuildRank", &LuaPlayer::SetGuildRank },                 // :SetGuildRank(rank) - Sets player's guild rank
    // { "SetMovement", &LuaPlayer::SetMovement },                // :SetMovement(type) - Sets player's movement type
    { MTMAP_SAFE,                   "SetSkill", &LuaPlayer::SetSkill },                         // :SetSkill(skill, step, currVal, maxVal) - Sets the skill's boundaries and value
    { MTMAP_SAFE,                   "SetFactionForRace", &LuaPlayer::SetFactionForRace },       // :SetFactionForRace(race) - Sets the faction by raceID
    { MTMAP_SAFE,                   "SetDrunkValue", &LuaPlayer::SetDrunkValue },               // :SetDrunkValue(newDrunkValue) - Sets drunkness value
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "SetAtLoginFlag", &LuaPlayer::SetAtLoginFlag },             // :SetAtLoginFlag(flag) - Adds an at login flag
    { MTMAP_SAFE,                   "SetPlayerLock", &LuaPlayer::SetPlayerLock },               // :SetPlayerLock(on/off)
    { MTMAP_SAFE,                   "SetGender", &LuaPlayer::SetGender },                       // :SetGender(value) - 0 = male 1 = female
    { MTMAP_SAFE,                   "SetSheath", &LuaPlayer::SetSheath },                       // :SetSheath(SheathState) - Sets player's sheathstate
#ifndef TRINITY
    { MTMAP_SAFE,                   "SetFFA", &LuaPlayer::SetFFA },                             // :SetFFA([apply]) - Sets the units FFA tag on or off
#endif

    // Boolean
    { MTMAP_SAFE,                   "IsInGroup", &LuaPlayer::IsInGroup },                                               // :IsInGroup()
    { MTMAP_SAFE,                   "IsInGuild", &LuaPlayer::IsInGuild },                                               // :IsInGuild()
    { MTMAP_SAFE,                   "IsGM", &LuaPlayer::IsGM },                                                         // :IsGM()
    { MTMAP_SAFE,                   "IsAlliance", &LuaPlayer::IsAlliance },                                             // :IsAlliance()
    { MTMAP_SAFE,                   "IsHorde", &LuaPlayer::IsHorde },                                                   // :IsHorde()
#ifndef CLASSIC
    { MTMAP_SAFE,                   "HasTitle", &LuaPlayer::HasTitle },                                                 // :HasTitle(id)
#endif
    { MTMAP_SAFE,                   "HasItem", &LuaPlayer::HasItem },                                                   // :HasItem(itemId[, count, check_bank]) - Returns true if the player has the item(itemId) and specified count, else it will return false
    { MTMAP_SAFE,                   "Teleport", &LuaPlayer::Teleport },                                                 // :Teleport(Map, X, Y, Z, O) - Teleports player to specified co - ordinates. Returns true if success and false if not
    { MTMAP_SAFE,                   "AddItem", &LuaPlayer::AddItem },                                                   // :AddItem(id, amount) - Adds amount of item to player. Returns true if success and false if not
#ifndef CLASSIC
    { MTMAP_SAFE,                   "IsInArenaTeam", &LuaPlayer::IsInArenaTeam },                                       // :IsInArenaTeam(type) - type : 0 = 2v2, 1 = 3v3, 2 = 5v5
#endif
    { MTMAP_SAFE,                   "CanCompleteQuest", &LuaPlayer::CanCompleteQuest },
    { MTMAP_SAFE,                   "CanEquipItem", &LuaPlayer::CanEquipItem },                                         // :CanEquipItem(entry/item, slot) - Returns true if the player can equip given item/item entry
    { MTMAP_SAFE,                   "IsFalling", &LuaPlayer::IsFalling },                                               // :IsFalling() - Returns true if the unit is falling
    { MTMAP_SAFE,                   "ToggleAFK", &LuaPlayer::ToggleAFK },                                               // :ToggleAFK() - Toggles AFK state for player
    { MTMAP_SAFE,                   "ToggleDND", &LuaPlayer::ToggleDND },                                               // :ToggleDND() - Toggles DND state for player
    { MTMAP_SAFE,                   "IsAFK", &LuaPlayer::IsAFK },                                                       // :IsAFK() - Returns true if the player is afk
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "IsDND", &LuaPlayer::IsDND },                                                       // :IsDND() - Returns true if the player is in dnd mode
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "IsAcceptingWhispers", &LuaPlayer::IsAcceptingWhispers },                           // :IsAcceptWhispers() - Returns true if the player accepts whispers
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "IsGMChat", &LuaPlayer::IsGMChat },                                                 // :IsGMChat() - Returns true if the player has GM chat on
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "IsTaxiCheater", &LuaPlayer::IsTaxiCheater },                                       // :IsTaxiCheater() - Returns true if the player has taxi cheat on
    { MTMAP_SAFE,                   "IsGMVisible", &LuaPlayer::IsGMVisible },                                           // :IsGMVisible() - Returns true if the player is GM visible
    { MTMAP_SAFE,                   "HasQuest", &LuaPlayer::HasQuest },                                                 // :HasQuest(entry) - Returns true if player has the quest
    { MTMAP_SAFE,                   "InBattlegroundQueue", &LuaPlayer::InBattlegroundQueue },                           // :InBattlegroundQueue() - Returns true if the player is in a battleground queue
    // { "IsImmuneToEnvironmentalDamage", &LuaPlayer::IsImmuneToEnvironmentalDamage },    // :IsImmuneToEnvironmentalDamage() - Returns true if the player is immune to enviromental damage
    { MTMAP_SAFE,                   "CanSpeak", &LuaPlayer::CanSpeak },                                                 // :CanSpeak() - Returns true if the player can speak
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "HasAtLoginFlag", &LuaPlayer::HasAtLoginFlag },                                     // :HasAtLoginFlag(flag) - returns true if the player has the login flag
    // { "InRandomLfgDungeon", &LuaPlayer::InRandomLfgDungeon },                          // :InRandomLfgDungeon() - Returns true if the player is in a random LFG dungeon
    // { "HasPendingBind", &LuaPlayer::HasPendingBind },                                  // :HasPendingBind() - Returns true if the player has a pending instance bind
#if (!defined(TBC) && !defined(CLASSIC))
    { MTMAP_SAFE,                   "HasAchieved", &LuaPlayer::HasAchieved },                                           // :HasAchieved(achievementID) - Returns true if the player has achieved the achievement
#endif
    { MTMAP_SAFE,                   "CanUninviteFromGroup", &LuaPlayer::CanUninviteFromGroup },                         // :CanUninviteFromGroup() - Returns true if the player can uninvite from group
    { MTMAP_SAFE,                   "IsRested", &LuaPlayer::IsRested },                                                 // :IsRested() - Returns true if the player is rested
    // { "CanFlyInZone", &LuaPlayer::CanFlyInZone },                                      // :CanFlyInZone(mapid, zone) - Returns true if the player can fly in the area
    // { "IsNeverVisible", &LuaPlayer::IsNeverVisible },                                  // :IsNeverVisible() - Returns true if the player is never visible
    { MTMAP_SAFE,                   "IsVisibleForPlayer", &LuaPlayer::IsVisibleForPlayer },                             // :IsVisibleForPlayer(player) - Returns true if the player is visible for the target player
    // { "IsUsingLfg", &LuaPlayer::IsUsingLfg },                                          // :IsUsingLfg() - Returns true if the player is using LFG
    { MTMAP_SAFE,                   "HasQuestForItem", &LuaPlayer::HasQuestForItem },                                   // :HasQuestForItem(entry) - Returns true if the player has the quest for the item
    { MTMAP_SAFE,                   "HasQuestForGO", &LuaPlayer::HasQuestForGO },                                       // :HasQuestForGO(entry) - Returns true if the player has the quest for the gameobject
    { MTMAP_SAFE,                   "CanShareQuest", &LuaPlayer::CanShareQuest },                                       // :CanShareQuest(entry) - Returns true if the quest entry is shareable by the player
    // { "HasReceivedQuestReward", &LuaPlayer::HasReceivedQuestReward },                  // :HasReceivedQuestReward(entry) - Returns true if the player has recieved the quest's reward
#if (!defined(TBC) && !defined(CLASSIC))
    { MTMAP_SAFE,                   "HasTalent", &LuaPlayer::HasTalent },                                               // :HasTalent(talentId, spec) - Returns true if the player has the talent in given spec
#endif
    { MTMAP_SAFE,                   "IsInSameGroupWith", &LuaPlayer::IsInSameGroupWith },                               // :IsInSameGroupWith(player) - Returns true if the players are in the same group
    { MTMAP_SAFE,                   "IsInSameRaidWith", &LuaPlayer::IsInSameRaidWith },                                 // :IsInSameRaidWith(player) - Returns true if the players are in the same raid
    { MTMAP_SAFE,                   "IsGroupVisibleFor", &LuaPlayer::IsGroupVisibleFor },                               // :IsGroupVisibleFor(player) - Player is group visible for the target
    { MTMAP_SAFE,                   "HasSkill", &LuaPlayer::HasSkill },                                                 // :HasSkill(skill) - Returns true if the player has the skill
    { MTMAP_SAFE,                   "IsHonorOrXPTarget", &LuaPlayer::IsHonorOrXPTarget },                               // :IsHonorOrXPTarget(victim) - Returns true if the victim gives honor or XP
    { MTMAP_SAFE,                   "CanParry", &LuaPlayer::CanParry },                                                 // :CanParry() - Returns true if the player can parry
    { MTMAP_SAFE,                   "CanBlock", &LuaPlayer::CanBlock },                                                 // :CanBlock() - Returns true if the player can block
#if (!defined(TBC) && !defined(CLASSIC))
    { MTMAP_SAFE,                   "CanTitanGrip", &LuaPlayer::CanTitanGrip },                                         // :CanTitanGrip() - Returns true if the player has titan grip
#endif
    { MTMAP_SAFE,                   "InBattleground", &LuaPlayer::InBattleground },                                     // :InBattleground() - Returns true if the player is in a battleground
#ifndef CLASSIC
    { MTMAP_SAFE,                   "InArena", &LuaPlayer::InArena },                                                   // :InArena() - Returns true if the player is in an arena
#endif
    // { "IsOutdoorPvPActive", &LuaPlayer::IsOutdoorPvPActive },                          // :IsOutdoorPvPActive() - Returns true if the player is outdoor pvp active
    // { "IsARecruiter", &LuaPlayer::IsARecruiter },                                      // :IsARecruiter() - Returns true if the player is a recruiter
    { MTMAP_SAFE,                   "CanUseItem", &LuaPlayer::CanUseItem },                                             // :CanUseItem(item/entry) - Returns true if the player can use the item or item entry
    { MTMAP_SAFE,                   "HasSpell", &LuaPlayer::HasSpell },                                                 // :HasSpell(id)
    { MTMAP_SAFE,                   "HasSpellCooldown", &LuaPlayer::HasSpellCooldown },                                 // :HasSpellCooldown(spellId) - Returns true if the spell is on cooldown
    { MTMAP_SAFE,                   "IsInWater", &LuaPlayer::IsInWater },                                               // :IsInWater() - Returns true if the player is in water
#ifndef CLASSIC
    { MTMAP_SAFE,                   "CanFly", &LuaPlayer::CanFly },                                                     // :CanFly() - Returns true if the player can fly
#endif
    { MTMAP_SAFE,                   "IsMoving", &LuaPlayer::IsMoving },                                                 // :IsMoving()
#ifndef CLASSIC
    { MTMAP_SAFE,                   "IsFlying", &LuaPlayer::IsFlying },                                                 // :IsFlying()
#endif

    // Gossip
    { MTMAP_SAFE,                   "GossipMenuAddItem", &LuaPlayer::GossipMenuAddItem },                               // :GossipMenuAddItem(icon, msg, sender, intid[, code, popup, money])
    { MTMAP_SAFE,                   "GossipSendMenu", &LuaPlayer::GossipSendMenu },                                     // :GossipSendMenu(npc_text, unit[, menu_id]) - If unit is a player, you need to use a menu_id. menu_id is used to hook the gossip select function to the menu
    { MTMAP_SAFE,                   "GossipComplete", &LuaPlayer::GossipComplete },                                     // :GossipComplete()
    { MTMAP_SAFE,                   "GossipClearMenu", &LuaPlayer::GossipClearMenu },                                   // :GossipClearMenu() - Clears the gossip menu of options. Pretty much only useful with player gossip. Need to use before creating a new menu for the player

    // Other
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "SendClearCooldowns", &LuaPlayer::SendClearCooldowns },                            // :SendClearCooldowns(spellId, (unit)target) - Clears the cooldown of the target with a specified spellId
    { MTMAP_SAFE,                   "SendBroadcastMessage", &LuaPlayer::SendBroadcastMessage },                         // :SendBroadcastMessage(message)
    { MTMAP_SAFE,                   "SendAreaTriggerMessage", &LuaPlayer::SendAreaTriggerMessage },                     // :SendAreaTriggerMessage(message) - Sends a yellow message in the middle of your screen
    { MTMAP_SAFE,                   "SendNotification", &LuaPlayer::SendNotification },                                 // :SendNotification(message) - Sends a red message in the middle of your screen
    { MTMAP_SAFE,                   "SendPacket", &LuaPlayer::SendPacket },                                             // :SendPacket(packet, selfOnly) - Sends a packet to player or everyone around also if selfOnly is false
    { MTMAP_SAFE,                   "SendAddonMessage", &LuaPlayer::SendAddonMessage },                                 // :SendAddonMessage(prefix, message, channel, receiver) - Sends an addon message to the player. 
    { MTMAP_SAFE,                   "ModifyMoney", &LuaPlayer::ModifyMoney },                                           // :ModifyMoney(amount[, sendError]) - Modifies (does not set) money (copper count) of the player. Amount can be negative to remove copper
    { MTMAP_SAFE,                   "LearnSpell", &LuaPlayer::LearnSpell },                                             // :LearnSpell(id) - learns the given spell
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "LearnTalent", &LuaPlayer::LearnTalent },
    { MTMAP_SAFE,                   "RemoveItem", &LuaPlayer::RemoveItem },                                             // :RemoveItem(item/entry, amount) - Removes amount of item from player
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "RemoveLifetimeKills", &LuaPlayer::RemoveLifetimeKills },                           // :RemoveLifetimeKills(val) - Removes a specified amount(val) of the player's lifetime (honorable) kills
    { MTMAP_SAFE,                   "ResurrectPlayer", &LuaPlayer::ResurrectPlayer },                                   // :ResurrectPlayer([percent[, sickness(bool)]]) - Resurrects the player at percentage, player gets resurrection sickness if sickness set to true
    { MTMAP_SAFE,                   "PlaySoundToPlayer", &LuaPlayer::PlaySoundToPlayer },                               // :PlaySoundToPlayer(soundId) - Plays the specified sound to the player
    { MTMAP_SAFE,                   "EquipItem", &LuaPlayer::EquipItem },                                               // :EquipItem(entry/item, slot) - Equips given item or item entry for player to given slot. Returns the equipped item or nil
    { MTMAP_SAFE,                   "ResetSpellCooldown", &LuaPlayer::ResetSpellCooldown },                             // :ResetSpellCooldown(spellId, update(bool~optional)) - Resets cooldown of the specified spellId. If update is true, it will send WorldPacket SMSG_CLEAR_COOLDOWN to the player, else it will just clear the spellId from m_spellCooldowns. This is true by default
    { MTMAP_SAFE,                   "ResetTypeCooldowns", &LuaPlayer::ResetTypeCooldowns },                             // :ResetTypeCooldowns(category, update(bool~optional)) - Resets all cooldowns for the spell category(type). If update is true, it will send WorldPacket SMSG_CLEAR_COOLDOWN to the player, else it will just clear the spellId from m_spellCooldowns. This is true by default
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "ResetAllCooldowns", &LuaPlayer::ResetAllCooldowns },                               // :ResetAllCooldowns() - Resets all spell cooldowns
    { MTMAP_SAFE,                   "GiveXP", &LuaPlayer::GiveXP },                                                     // :GiveXP(xp[, victim, pureXP, triggerHook]) - Gives XP to the player. If pure is false, bonuses are count in. If triggerHook is false, GiveXp hook is not triggered.
    // { "RemovePet", &LuaPlayer::RemovePet },                                            // :RemovePet([mode, returnreagent]) - Removes the player's pet. Mode determines if the pet is saved and how
    // { "SummonPet", &LuaPlayer::SummonPet },                                            // :SummonPet(entry, x, y, z, o, petType, despwtime) - Summons a pet for the player
    { MTMAP_SAFE,                   "Say", &LuaPlayer::Say },                                                           // :Say(text, lang) - The player says the text
    { MTMAP_SAFE,                   "Yell", &LuaPlayer::Yell },                                                         // :Yell(text, lang) - The player yells the text
    { MTMAP_SAFE,                   "TextEmote", &LuaPlayer::TextEmote },                                               // :TextEmote(text) - The player does a textemote with the text
    { MTMAP_SAFE,                   "Whisper", &LuaPlayer::Whisper },                                                   // :Whisper(text, lang, receiver) - The player whispers the text to the receiver
    { MTMAP_SAFE,                   "CompleteQuest", &LuaPlayer::CompleteQuest },                                       // :CompleteQuest(entry) - Completes a quest by entry
    { MTMAP_SAFE,                   "IncompleteQuest", &LuaPlayer::IncompleteQuest },                                   // :IncompleteQuest(entry) - Uncompletes the quest by entry for the player
    { MTMAP_SAFE,                   "FailQuest", &LuaPlayer::FailQuest },                                               // :FailQuest(entry) - Player fails the quest entry
    { MTMAP_SAFE,                   "AddQuest", &LuaPlayer::AddQuest },
    { MTMAP_SAFE,                   "RemoveQuest", &LuaPlayer::RemoveQuest },
    // { "RemoveActiveQuest", &LuaPlayer::RemoveActiveQuest },                            // :RemoveActiveQuest(entry) - Removes an active quest
    // { "RemoveRewardedQuest", &LuaPlayer::RemoveRewardedQuest },                        // :RemoveRewardedQuest(entry) - Removes a rewarded quest
    { MTMAP_SAFE,                   "AreaExploredOrEventHappens", &LuaPlayer::AreaExploredOrEventHappens },             // :AreaExploredOrEventHappens(questId) - Satisfies an area or event requrement for the questId
    { FLAGS_NONE, "GroupEventHappens", &LuaPlayer::GroupEventHappens },                               // :GroupEventHappens(questId, worldObject) - Satisfies a group event for the questId with the world object
    { MTMAP_SAFE,                   "KilledMonsterCredit", &LuaPlayer::KilledMonsterCredit },                           // :KilledMonsterCredit(entry) - Satisfies a monsterkill for the player
    // { "KilledPlayerCredit", &LuaPlayer::KilledPlayerCredit },                          // :KilledPlayerCredit() - Satisfies a player kill for the player
    // { "KillGOCredit", &LuaPlayer::KillGOCredit },                                      // :KillGOCredit(GOEntry[, GUID]) - Credits the player for destroying a GO, guid is optional
    { MTMAP_SAFE,                   "TalkedToCreature", &LuaPlayer::TalkedToCreature },                                 // :TalkedToCreature(npcEntry, creature) - Satisfies creature talk objective for the player
#if (!defined(TBC) && !defined(CLASSIC))
    { MTMAP_SAFE,                   "ResetPetTalents", &LuaPlayer::ResetPetTalents },                                   // :ResetPetTalents() - Resets player's pet's talents
#endif
    { MTMAP_SAFE,                   "AddComboPoints", &LuaPlayer::AddComboPoints },                                     // :AddComboPoints(target, count[, spell]) - Adds combo points to the target for the player
    // { "GainSpellComboPoints", &LuaPlayer::GainSpellComboPoints },                      // :GainSpellComboPoints(amount) - Player gains spell combo points
    { MTMAP_SAFE,                   "ClearComboPoints", &LuaPlayer::ClearComboPoints },                                 // :ClearComboPoints() - Clears player's combo points
    { MTMAP_SAFE,                   "RemoveSpell", &LuaPlayer::RemoveSpell },                                           // :RemoveSpell(entry[, disabled, learn_low_rank]) - Removes (unlearn) the given spell
    { MTMAP_SAFE,                   "ResetTalents", &LuaPlayer::ResetTalents },                                         // :ResetTalents([no_cost]) - Resets player's talents
    { MTMAP_SAFE,                   "ResetTalentsCost", &LuaPlayer::ResetTalentsCost },                                 // :ResetTalentsCost() - Returns the reset talents cost
    // { "AddTalent", &LuaPlayer::AddTalent },                                            // :AddTalent(spellid, spec, learning) - Adds a talent spell for the player to given spec
    { FLAGS_NONE, "RemoveFromGroup", &LuaPlayer::RemoveFromGroup },                                   // :RemoveFromGroup() - Removes the player from his group
    { MTMAP_SAFE,                   "KillPlayer", &LuaPlayer::KillPlayer },                                             // :KillPlayer() - Kills the player
    { MTMAP_SAFE,                   "DurabilityLossAll", &LuaPlayer::DurabilityLossAll },                               // :DurabilityLossAll(percent[, inventory]) - The player's items lose durability. Inventory true by default
    { MTMAP_SAFE,                   "DurabilityLoss", &LuaPlayer::DurabilityLoss },                                     // :DurabilityLoss(item, percent) - The given item loses durability
    { MTMAP_SAFE,                   "DurabilityPointsLoss", &LuaPlayer::DurabilityPointsLoss },                         // :DurabilityPointsLoss(item, points) - The given item loses durability
    { MTMAP_SAFE,                   "DurabilityPointsLossAll", &LuaPlayer::DurabilityPointsLossAll },                   // :DurabilityPointsLossAll(points, inventory) - Player's items lose durability
    { MTMAP_SAFE,                   "DurabilityPointLossForEquipSlot", &LuaPlayer::DurabilityPointLossForEquipSlot },   // :DurabilityPointLossForEquipSlot(slot) - Causes durability loss for the item in the given slot
    { FLAGS_NONE, "DurabilityRepairAll", &LuaPlayer::DurabilityRepairAll },                           // :DurabilityRepairAll([has_cost, discount, guildBank]) - Repairs all durability
    { FLAGS_NONE, "DurabilityRepair", &LuaPlayer::DurabilityRepair },                                 // :DurabilityRepair(position[, has_cost, discount, guildBank]) - Repairs item durability of item in given position
#ifndef CATA
#ifndef CLASSIC
    { MTMAP_SAFE,                   "ModifyHonorPoints", &LuaPlayer::ModifyHonorPoints },                               // :ModifyHonorPoints(amount) - Modifies the player's honor points
    { MTMAP_SAFE,                   "ModifyArenaPoints", &LuaPlayer::ModifyArenaPoints },                               // :ModifyArenaPoints(amount) - Modifies the player's arena points
#endif
#endif
    { MTMAP_SAFE,                   "LeaveBattleground", &LuaPlayer::LeaveBattleground },                               // :LeaveBattleground([teleToEntryPoint]) - The player leaves the battleground
    // { "BindToInstance", &LuaPlayer::BindToInstance },                                  // :BindToInstance() - Binds the player to the current instance
    { MTMAP_SAFE,                   "UnbindInstance", &LuaPlayer::UnbindInstance },                                     // :UnbindInstance(map, difficulty) - Unbinds the player from an instance
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "UnbindAllInstances", &LuaPlayer::UnbindAllInstances },                             // :UnbindAllInstances() - Unbinds the player from all instances
    { MTMAP_SAFE,                   "RemoveFromBattlegroundRaid", &LuaPlayer::RemoveFromBattlegroundRaid },             // :RemoveFromBattlegroundRaid() - Removes the player from a battleground or battlefield raid
#if (!defined(TBC) && !defined(CLASSIC))
    { MTMAP_SAFE,                   "ResetAchievements", &LuaPlayer::ResetAchievements },                               // :ResetAchievements() - Resets player�s achievements
#endif
    { NOT_IN_SAFEMODE,              "KickPlayer", &LuaPlayer::KickPlayer },                                             // :KickPlayer() - Kicks player from server
    { NOT_IN_SAFEMODE,              "LogoutPlayer", &LuaPlayer::LogoutPlayer },                                         // :LogoutPlayer([save]) - Logs the player out and saves if true
    { MTMAP_SAFE,                   "SendTrainerList", &LuaPlayer::SendTrainerList },                                   // :SendTrainerList(WorldObject) - Sends trainer list from object to player
    { MTMAP_SAFE,                   "SendListInventory", &LuaPlayer::SendListInventory },                               // :SendListInventory(WorldObject) - Sends vendor list from object to player
    { MTMAP_SAFE,                   "SendShowBank", &LuaPlayer::SendShowBank },                                         // :SendShowBank(WorldObject) - Sends bank window from object to player
    { MTMAP_SAFE,                   "SendTabardVendorActivate", &LuaPlayer::SendTabardVendorActivate },                 // :SendTabardVendorActivate(WorldObject) - Sends tabard vendor window from object to player
    { MTMAP_SAFE,                   "SendSpiritResurrect", &LuaPlayer::SendSpiritResurrect },                           // :SendSpiritResurrect() - Sends resurrect window to player
    { MTMAP_SAFE,                   "SendTaxiMenu", &LuaPlayer::SendTaxiMenu },                                         // :SendTaxiMenu(creature) - Sends flight window to player from creature
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "RewardQuest", &LuaPlayer::RewardQuest },
    { MTMAP_SAFE,                   "SendAuctionMenu", &LuaPlayer::SendAuctionMenu },                                   // :SendAuctionMenu(unit) - Sends auction window to player. Auction house is sent by object.
    { MTMAP_SAFE,                   "SendShowMailBox", &LuaPlayer::SendShowMailBox },                                   // :SendShowMailBox([mailboxguid]) - Sends the mail window to player from the mailbox gameobject. The guid is required on patches below wotlk.
    { MTMAP_SAFE,                   "StartTaxi", &LuaPlayer::StartTaxi },                                               // :StartTaxi(pathId) - player starts the given flight path
    { MTMAP_SAFE,                   "GossipSendPOI", &LuaPlayer::GossipSendPOI },                                       // :GossipSendPOI(X, Y, Icon, Flags, Data, Name) - Sends a point of interest to the player
    { MTMAP_SAFE,                   "GossipAddQuests", &LuaPlayer::GossipAddQuests },                                   // :GossipAddQuests(unit) - Adds unit's quests to player's gossip menu
    { MTMAP_SAFE,                   "SendQuestTemplate", &LuaPlayer::SendQuestTemplate },                               // :SendQuestTemplate(questId, activeAccept) -- Sends quest template to player
    { FLAGS_NONE, "SpawnBones", &LuaPlayer::SpawnBones },                                             // :SpawnBones() - Removes the player's corpse and spawns bones
    { FLAGS_NONE, "RemovedInsignia", &LuaPlayer::RemovedInsignia },                                   // :RemovedInsignia(looter) - Looter removes the player's corpse, looting the player and replacing with lootable bones
    { NOT_IN_SAFEMODE,              "SendGuildInvite", &LuaPlayer::SendGuildInvite },                                   // :SendGuildInvite(player) - Sends a guild invite to the specific player
    { FLAGS_NONE, "CreateCorpse", &LuaPlayer::CreateCorpse },                                         // :CreateCorpse() - Creates the player's corpse
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "Mute", &LuaPlayer::Mute },                                                         // :Mute(time[, reason]) - Mutes the player for given time in seconds.
    { MTMAP_SAFE,                   "SummonPlayer", &LuaPlayer::SummonPlayer },                                         // :SummonPlayer(player, map, x, y, z, zoneId[, delay]) - Sends a popup to the player asking if he wants to be summoned if yes, teleported to coords. ZoneID defines the location name shown in the popup Delay is the time until the popup closes automatically.
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "SaveToDB", &LuaPlayer::SaveToDB },                                                 // :SaveToDB() - Saves to database
    { NOT_IN_SAFEMODE,              "GroupInvite", &LuaPlayer::GroupInvite },
    { NOT_IN_SAFEMODE,              "GroupCreate", &LuaPlayer::GroupCreate },
#ifdef CLASSIC
    { MTMAP_SAFE,                   "UpdateHonor", &LuaPlayer::UpdateHonor },                                             // :UpdateHonor() - Updates Player Honor
    { MTMAP_SAFE,                   "ResetHonor", &LuaPlayer::ResetHonor },                                               // :ResetHonor() - Resets Player Honor
    { MTMAP_SAFE,                   "ClearHonorInfo", &LuaPlayer::ClearHonorInfo },                                       // :ClearHonorInfo() - Clear Player Honor Info
#endif

    { FLAGS_NONE, nullptr, nullptr },
};

ElunaRegister<Creature> CreatureMethods[] =
{
    // Getters
    { MTMAP_SAFE,                   "GetAITarget", &LuaCreature::GetAITarget },
    { MTMAP_SAFE,                   "GetAITargets", &LuaCreature::GetAITargets },
    { MTMAP_SAFE,                   "GetAITargetsCount", &LuaCreature::GetAITargetsCount },
    { MTMAP_SAFE,                   "GetHomePosition", &LuaCreature::GetHomePosition },
    { MTMAP_SAFE,                   "GetCorpseDelay", &LuaCreature::GetCorpseDelay },
    { MTMAP_SAFE,                   "GetCreatureSpellCooldownDelay", &LuaCreature::GetCreatureSpellCooldownDelay },
    { MTMAP_SAFE,                   "GetScriptId", &LuaCreature::GetScriptId },
    { MTMAP_SAFE,                   "GetAIName", &LuaCreature::GetAIName },
    { MTMAP_SAFE,                   "GetScriptName", &LuaCreature::GetScriptName },
    { MTMAP_SAFE,                   "GetAttackDistance", &LuaCreature::GetAttackDistance },
    { MTMAP_SAFE,                   "GetAggroRange", &LuaCreature::GetAggroRange },
    { MTMAP_SAFE,                   "GetDefaultMovementType", &LuaCreature::GetDefaultMovementType },
    { MTMAP_SAFE,                   "GetRespawnDelay", &LuaCreature::GetRespawnDelay },
    { MTMAP_SAFE,                   "GetWanderRadius", &LuaCreature::GetWanderRadius },
    { MTMAP_SAFE,                   "GetCurrentWaypointId", &LuaCreature::GetCurrentWaypointId },
#ifdef TRINITY
    { MTMAP_SAFE,                   "GetWaypointPath", &LuaCreature::GetWaypointPath },
    { MTMAP_SAFE,                   "GetLootMode", &LuaCreature::GetLootMode },
#endif
    { MTMAP_SAFE,                   "GetLootRecipient", &LuaCreature::GetLootRecipient },
    { MTMAP_SAFE,                   "GetLootRecipientGroup", &LuaCreature::GetLootRecipientGroup },
    { MTMAP_SAFE,                   "GetNPCFlags", &LuaCreature::GetNPCFlags },
#ifndef CATA
    { MTMAP_SAFE,                   "GetShieldBlockValue", &LuaCreature::GetShieldBlockValue },
#endif
    { MTMAP_SAFE,                   "GetDBTableGUIDLow", &LuaCreature::GetDBTableGUIDLow },

    // Setters
    { MTMAP_SAFE,                   "SetHover", &LuaCreature::SetHover },
    { MTMAP_SAFE,                   "SetDisableGravity", &LuaCreature::SetDisableGravity },
    { MTMAP_SAFE,                   "SetAggroEnabled", &LuaCreature::SetAggroEnabled },
    { MTMAP_SAFE,                   "SetNoCallAssistance", &LuaCreature::SetNoCallAssistance },
    { MTMAP_SAFE,                   "SetNoSearchAssistance", &LuaCreature::SetNoSearchAssistance },
    { MTMAP_SAFE,                   "SetDefaultMovementType", &LuaCreature::SetDefaultMovementType },
    { MTMAP_SAFE,                   "SetRespawnDelay", &LuaCreature::SetRespawnDelay },
    { MTMAP_SAFE,                   "SetWanderRadius", &LuaCreature::SetWanderRadius },
    { MTMAP_SAFE,                   "SetInCombatWithZone", &LuaCreature::SetInCombatWithZone },
    { MTMAP_SAFE,                   "SetDisableReputationGain", &LuaCreature::SetDisableReputationGain },
#ifdef TRINITY
    { MTMAP_SAFE,                   "SetLootMode", &LuaCreature::SetLootMode },
#endif
    { MTMAP_SAFE,                   "SetNPCFlags", &LuaCreature::SetNPCFlags },
    { MTMAP_SAFE,                   "SetDeathState", &LuaCreature::SetDeathState },
    { MTMAP_SAFE,                   "SetWalk", &LuaCreature::SetWalk },
    { MTMAP_SAFE,                   "SetHomePosition", &LuaCreature::SetHomePosition },
    { MTMAP_SAFE,                   "SetEquipmentSlots", &LuaCreature::SetEquipmentSlots },

    // Booleans
    { MTMAP_SAFE,                   "IsWorldBoss", &LuaCreature::IsWorldBoss },
    { MTMAP_SAFE,                   "IsRacialLeader", &LuaCreature::IsRacialLeader },
    { MTMAP_SAFE,                   "IsCivilian", &LuaCreature::IsCivilian },
#ifdef TRINITY
    { MTMAP_SAFE,                   "IsTrigger", &LuaCreature::IsTrigger },
#endif
    { MTMAP_SAFE,                   "IsGuard", &LuaCreature::IsGuard },
    { MTMAP_SAFE,                   "IsElite", &LuaCreature::IsElite },
    { MTMAP_SAFE,                   "IsInEvadeMode", &LuaCreature::IsInEvadeMode },
    { MTMAP_SAFE,                   "HasCategoryCooldown", &LuaCreature::HasCategoryCooldown },
    { MTMAP_SAFE,                   "CanWalk", &LuaCreature::CanWalk },
    { MTMAP_SAFE,                   "CanSwim", &LuaCreature::CanSwim },
    { MTMAP_SAFE,                   "CanAggro", &LuaCreature::CanAggro },
#ifdef TRINITY
    { MTMAP_SAFE,                   "CanStartAttack", &LuaCreature::CanStartAttack },
#endif
    { MTMAP_SAFE,                   "HasSearchedAssistance", &LuaCreature::HasSearchedAssistance },
    { MTMAP_SAFE,                   "IsTappedBy", &LuaCreature::IsTappedBy },
    { MTMAP_SAFE,                   "HasLootRecipient", &LuaCreature::HasLootRecipient },
    { MTMAP_SAFE,                   "CanAssistTo", &LuaCreature::CanAssistTo },
    { MTMAP_SAFE,                   "IsTargetableForAttack", &LuaCreature::IsTargetableForAttack },
    { MTMAP_SAFE,                   "CanCompleteQuest", &LuaCreature::CanCompleteQuest },
    { MTMAP_SAFE,                   "CanRegenerateHealth", &LuaCreature::CanRegenerateHealth },
    { MTMAP_SAFE,                   "IsReputationGainDisabled", &LuaCreature::IsReputationGainDisabled },
#ifdef TRINITY
    { MTMAP_SAFE,                   "IsDamageEnoughForLootingAndReward", &LuaCreature::IsDamageEnoughForLootingAndReward },
    { MTMAP_SAFE,                   "HasLootMode", &LuaCreature::HasLootMode },
#endif
    { MTMAP_SAFE,                   "HasSpell", &LuaCreature::HasSpell },
    { MTMAP_SAFE,                   "HasQuest", &LuaCreature::HasQuest },
    { MTMAP_SAFE,                   "HasSpellCooldown", &LuaCreature::HasSpellCooldown },
    { MTMAP_SAFE,                   "CanFly", &LuaCreature::CanFly },

    // Other
    { MTMAP_SAFE,                   "FleeToGetAssistance", &LuaCreature::FleeToGetAssistance },
    { MTMAP_SAFE,                   "CallForHelp", &LuaCreature::CallForHelp },
    { MTMAP_SAFE,                   "CallAssistance", &LuaCreature::CallAssistance },
    { MTMAP_SAFE,                   "RemoveCorpse", &LuaCreature::RemoveCorpse },
    { MTMAP_SAFE,                   "DespawnOrUnsummon", &LuaCreature::DespawnOrUnsummon },
    { MTMAP_SAFE,                   "Respawn", &LuaCreature::Respawn },
    { MTMAP_SAFE,                   "AttackStart", &LuaCreature::AttackStart },
#ifdef TRINITY
    { MTMAP_SAFE,                   "AddLootMode", &LuaCreature::AddLootMode },
    { MTMAP_SAFE,                   "ResetLootMode", &LuaCreature::ResetLootMode },
    { MTMAP_SAFE,                   "RemoveLootMode", &LuaCreature::RemoveLootMode },
#endif
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "SaveToDB", &LuaCreature::SaveToDB },
    { MTMAP_SAFE,                   "SelectVictim", &LuaCreature::SelectVictim },
    { MTMAP_SAFE,                   "MoveWaypoint", &LuaCreature::MoveWaypoint },
    { MTMAP_SAFE,                   "UpdateEntry", &LuaCreature::UpdateEntry },

    { FLAGS_NONE, nullptr, nullptr },
};

ElunaRegister<GameObject> GameObjectMethods[] =
{
    // Getters
    { MTMAP_SAFE,                   "GetDisplayId", &LuaGameObject::GetDisplayId },
    { MTMAP_SAFE,                   "GetGoState", &LuaGameObject::GetGoState },
    { MTMAP_SAFE,                   "GetLootState", &LuaGameObject::GetLootState },
    { MTMAP_SAFE,                   "GetLootRecipient", &LuaGameObject::GetLootRecipient },
    { MTMAP_SAFE,                   "GetLootRecipientGroup", &LuaGameObject::GetLootRecipientGroup },
    { MTMAP_SAFE,                   "GetDBTableGUIDLow", &LuaGameObject::GetDBTableGUIDLow },

    // Setters
    { MTMAP_SAFE,                   "SetGoState", &LuaGameObject::SetGoState },
    { MTMAP_SAFE,                   "SetLootState", &LuaGameObject::SetLootState },
    { MTMAP_SAFE,                   "SetRespawnTime", &LuaGameObject::SetRespawnTime },

    // Boolean
    { MTMAP_SAFE,                   "IsTransport", &LuaGameObject::IsTransport },
    // { "IsDestructible", &LuaGameObject::IsDestructible },    // :IsDestructible()
    { MTMAP_SAFE,                   "IsActive", &LuaGameObject::IsActive },
    { MTMAP_SAFE,                   "HasQuest", &LuaGameObject::HasQuest },
    { MTMAP_SAFE,                   "IsSpawned", &LuaGameObject::IsSpawned },

    // Other
    { MTMAP_SAFE,                   "RemoveFromWorld", &LuaGameObject::RemoveFromWorld },
    { MTMAP_SAFE,                   "UseDoorOrButton", &LuaGameObject::UseDoorOrButton },
    { MTMAP_SAFE,                   "Despawn", &LuaGameObject::Despawn },
    { MTMAP_SAFE,                   "Respawn", &LuaGameObject::Respawn },
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "SaveToDB", &LuaGameObject::SaveToDB },

    { FLAGS_NONE, nullptr, nullptr },
};

ElunaRegister<Item> ItemMethods[] =
{
    // Getters
    { MTMAP_SAFE,                   "GetOwnerGUID", &LuaItem::GetOwnerGUID },                   // :GetOwnerGUID() - Returns the owner's guid
    { MTMAP_SAFE,                   "GetOwner", &LuaItem::GetOwner },                           // :GetOwner() - Returns the owner object (player)
    { MTMAP_SAFE,                   "GetCount", &LuaItem::GetCount },                           // :GetCount() - Returns item stack count
    { MTMAP_SAFE,                   "GetMaxStackCount", &LuaItem::GetMaxStackCount },           // :GetMaxStackCount() - Returns item max stack count
    { MTMAP_SAFE,                   "GetSlot", &LuaItem::GetSlot },                             // :GetSlot() - returns the slot the item is in
    { MTMAP_SAFE,                   "GetBagSlot", &LuaItem::GetBagSlot },                       // :GetBagSlot() - returns the bagslot of the bag the item is in
    { MTMAP_SAFE,                   "GetEnchantmentId", &LuaItem::GetEnchantmentId },           // :GetEnchantmentId(enchant_slot) - Returns the enchantment in given slot. (permanent = 0)
    { MTMAP_SAFE,                   "GetSpellId", &LuaItem::GetSpellId },                       // :GetSpellId(index) - Returns spellID at given index (0 - 4)
    { MTMAP_SAFE,                   "GetSpellTrigger", &LuaItem::GetSpellTrigger },             // :GetSpellTrigger(index) - Returns spell trigger at given index (0 - 4)
    { MTMAP_SAFE,                   "GetItemLink", &LuaItem::GetItemLink },                     // :GetItemLink([localeID]) - Returns the shift clickable link of the item. Name translated if locale given and exists
    { MTMAP_SAFE,                   "GetClass", &LuaItem::GetClass },                           // :GetClass()
    { MTMAP_SAFE,                   "GetSubClass", &LuaItem::GetSubClass },                     // :GetSubClass()
    { MTMAP_SAFE,                   "GetName", &LuaItem::GetName },                             // :GetName()
    { MTMAP_SAFE,                   "GetDisplayId", &LuaItem::GetDisplayId },                   // :GetDisplayId()
    { MTMAP_SAFE,                   "GetQuality", &LuaItem::GetQuality },                       // :GetQuality()
    { MTMAP_SAFE,                   "GetBuyCount", &LuaItem::GetBuyCount },                     // :GetBuyCount()
    { MTMAP_SAFE,                   "GetBuyPrice", &LuaItem::GetBuyPrice },                     // :GetBuyPrice()
    { MTMAP_SAFE,                   "GetSellPrice", &LuaItem::GetSellPrice },                   // :GetSellPrice()
    { MTMAP_SAFE,                   "GetInventoryType", &LuaItem::GetInventoryType },           // :GetInventoryType()
    { MTMAP_SAFE,                   "GetAllowableClass", &LuaItem::GetAllowableClass },         // :GetAllowableClass()
    { MTMAP_SAFE,                   "GetAllowableRace", &LuaItem::GetAllowableRace },           // :GetAllowableRace()
    { MTMAP_SAFE,                   "GetItemLevel", &LuaItem::GetItemLevel },                   // :GetItemLevel()
    { MTMAP_SAFE,                   "GetRequiredLevel", &LuaItem::GetRequiredLevel },           // :GetRequiredLevel()
#ifdef WOTLK
    { MTMAP_SAFE,                   "GetStatsCount", &LuaItem::GetStatsCount },                 // :GetStatsCount()
#endif
    { MTMAP_SAFE,                   "GetRandomProperty", &LuaItem::GetRandomProperty },         // :GetRandomProperty()
#ifndef CLASSIC
    { MTMAP_SAFE,                   "GetRandomSuffix", &LuaItem::GetRandomSuffix },             // :GetRandomSuffix()
#endif
    { MTMAP_SAFE,                   "GetItemSet", &LuaItem::GetItemSet },                       // :GetItemSet()
    { MTMAP_SAFE,                   "GetBagSize", &LuaItem::GetBagSize },                       // :GetBagSize()

    // Setters
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "SetOwner", &LuaItem::SetOwner },                           // :SetOwner(player) - Sets the owner of the item
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "SetBinding", &LuaItem::SetBinding },                       // :SetBinding(bound) - Sets the item binding to true or false
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "SetCount", &LuaItem::SetCount },                           // :SetCount(count) - Sets the item count

    // Boolean
    { MTMAP_SAFE,                   "IsSoulBound", &LuaItem::IsSoulBound },                     // :IsSoulBound() - Returns true if the item is soulbound
#if (!defined(TBC) && !defined(CLASSIC))
    { MTMAP_SAFE,                   "IsBoundAccountWide", &LuaItem::IsBoundAccountWide },       // :IsBoundAccountWide() - Returns true if the item is account bound
#endif
    { MTMAP_SAFE,                   "IsBoundByEnchant", &LuaItem::IsBoundByEnchant },           // :IsBoundByEnchant() - Returns true if the item is bound with an enchant
    { MTMAP_SAFE,                   "IsNotBoundToPlayer", &LuaItem::IsNotBoundToPlayer },       // :IsNotBoundToPlayer(player) - Returns true if the item is not bound with player
    { MTMAP_SAFE,                   "IsLocked", &LuaItem::IsLocked },                           // :IsLocked() - Returns true if the item is locked
    { MTMAP_SAFE,                   "IsBag", &LuaItem::IsBag },                                 // :IsBag() - Returns true if the item is a bag
#ifndef CLASSIC
    { MTMAP_SAFE,                   "IsCurrencyToken", &LuaItem::IsCurrencyToken },             // :IsCurrencyToken() - Returns true if the item is a currency token
#endif
    { MTMAP_SAFE,                   "IsNotEmptyBag", &LuaItem::IsNotEmptyBag },                 // :IsNotEmptyBag() - Returns true if the item is not an empty bag
    { MTMAP_SAFE,                   "IsBroken", &LuaItem::IsBroken },                           // :IsBroken() - Returns true if the item is broken
    { MTMAP_SAFE,                   "CanBeTraded", &LuaItem::CanBeTraded },                     // :CanBeTraded() - Returns true if the item can be traded
    { MTMAP_SAFE,                   "IsInTrade", &LuaItem::IsInTrade },                         // :IsInTrade() - Returns true if the item is in trade
    { MTMAP_SAFE,                   "IsInBag", &LuaItem::IsInBag },                             // :IsInBag() - Returns true if the item is in a bag
    { MTMAP_SAFE,                   "IsEquipped", &LuaItem::IsEquipped },                       // :IsEquipped() - Returns true if the item is equipped
    { MTMAP_SAFE,                   "HasQuest", &LuaItem::HasQuest },                           // :HasQuest(questId) - Returns true if the item starts the quest
    { MTMAP_SAFE,                   "IsPotion", &LuaItem::IsPotion },                           // :IsPotion() - Returns true if the item is a potion
#ifndef CATA
    { MTMAP_SAFE,                   "IsWeaponVellum", &LuaItem::IsWeaponVellum },               // :IsWeaponVellum() - Returns true if the item is a weapon vellum
    { MTMAP_SAFE,                   "IsArmorVellum", &LuaItem::IsArmorVellum },                 // :IsArmorVellum() - Returns true if the item is an armor vellum
#endif
    { MTMAP_SAFE,                   "IsConjuredConsumable", &LuaItem::IsConjuredConsumable },   // :IsConjuredConsumable() - Returns true if the item is a conjured consumable
    // { "IsRefundExpired", &LuaItem::IsRefundExpired },          // :IsRefundExpired() - Returns true if the item's refund time has expired
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "SetEnchantment", &LuaItem::SetEnchantment },               // :SetEnchantment(enchantid, enchantmentslot) - Sets a new enchantment for the item. Returns true on success
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "ClearEnchantment", &LuaItem::ClearEnchantment },           // :ClearEnchantment(enchantmentslot) - Removes the enchantment from the item if one exists. Returns true on success

    // Other
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "SaveToDB", &LuaItem::SaveToDB },                           // :SaveToDB() - Saves to database

    { FLAGS_NONE, nullptr, nullptr },
};

ElunaRegister<Aura> AuraMethods[] =
{
    // Getters
    { MTMAP_SAFE,                   "GetCaster", &LuaAura::GetCaster },
    { MTMAP_SAFE,                   "GetCasterGUID", &LuaAura::GetCasterGUID },
    { MTMAP_SAFE,                   "GetCasterLevel", &LuaAura::GetCasterLevel },
    { MTMAP_SAFE,                   "GetDuration", &LuaAura::GetDuration },
    { MTMAP_SAFE,                   "GetMaxDuration", &LuaAura::GetMaxDuration },
    { MTMAP_SAFE,                   "GetAuraId", &LuaAura::GetAuraId },
    { MTMAP_SAFE,                   "GetStackAmount", &LuaAura::GetStackAmount },
    { MTMAP_SAFE,                   "GetOwner", &LuaAura::GetOwner },

    // Setters
    { MTMAP_SAFE,                   "SetDuration", &LuaAura::SetDuration },
    { MTMAP_SAFE,                   "SetMaxDuration", &LuaAura::SetMaxDuration },
    { MTMAP_SAFE,                   "SetStackAmount", &LuaAura::SetStackAmount },

    // Other
    { MTMAP_SAFE,                   "Remove", &LuaAura::Remove },

    { FLAGS_NONE, nullptr, nullptr },
};

ElunaRegister<Spell> SpellMethods[] =
{
    // Getters
    { MTMAP_SAFE,                   "GetCaster", &LuaSpell::GetCaster },
    { MTMAP_SAFE,                   "GetCastTime", &LuaSpell::GetCastTime },
    { MTMAP_SAFE,                   "GetEntry", &LuaSpell::GetEntry },
    { MTMAP_SAFE,                   "GetDuration", &LuaSpell::GetDuration },
    { MTMAP_SAFE,                   "GetPowerCost", &LuaSpell::GetPowerCost },
    { MTMAP_SAFE,                   "GetTargetDest", &LuaSpell::GetTargetDest },
    { MTMAP_SAFE,                   "GetTarget", &LuaSpell::GetTarget },

    // Setters
    { MTMAP_SAFE,                   "SetAutoRepeat", &LuaSpell::SetAutoRepeat },

    // Boolean
    { MTMAP_SAFE,                   "IsAutoRepeat", &LuaSpell::IsAutoRepeat },

    // Other
    { MTMAP_SAFE,                   "Cancel", &LuaSpell::Cancel },
    { MTMAP_SAFE,                   "Cast", &LuaSpell::Cast },
    { MTMAP_SAFE,                   "Finish", &LuaSpell::Finish },

    { FLAGS_NONE, nullptr, nullptr },
};

ElunaRegister<Quest> QuestMethods[] =
{
    // Getters
    { MTMAP_SAFE,                   "GetId", &LuaQuest::GetId },
    { MTMAP_SAFE,                   "GetLevel", &LuaQuest::GetLevel },
    // { "GetMaxLevel", &LuaQuest::GetMaxLevel },                   // :GetMaxLevel() - Returns the quest's max level
    { MTMAP_SAFE,                   "GetMinLevel", &LuaQuest::GetMinLevel },
    { MTMAP_SAFE,                   "GetNextQuestId", &LuaQuest::GetNextQuestId },
    { MTMAP_SAFE,                   "GetPrevQuestId", &LuaQuest::GetPrevQuestId },
    { MTMAP_SAFE,                   "GetNextQuestInChain", &LuaQuest::GetNextQuestInChain },
    { MTMAP_SAFE,                   "GetFlags", &LuaQuest::GetFlags },
    { MTMAP_SAFE,                   "GetType", &LuaQuest::GetType },

    // Boolean
    { MTMAP_SAFE,                   "HasFlag", &LuaQuest::HasFlag },
#ifndef CLASSIC
    { MTMAP_SAFE,                   "IsDaily", &LuaQuest::IsDaily },
#endif
    { MTMAP_SAFE,                   "IsRepeatable", &LuaQuest::IsRepeatable },

    { FLAGS_NONE, nullptr, nullptr },
};

ElunaRegister<Group> GroupMethods[] =
{
    // Getters
    { FLAGS_NONE, "GetMembers", &LuaGroup::GetMembers },
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "GetLeaderGUID", &LuaGroup::GetLeaderGUID },
    { MTMAP_SAFE,                   "GetGUID", &LuaGroup::GetGUID },
    { MTMAP_SAFE,                   "GetMemberGroup", &LuaGroup::GetMemberGroup },
    { MTMAP_SAFE,                   "GetMemberGUID", &LuaGroup::GetMemberGUID },
    { MTMAP_SAFE,                   "GetMembersCount", &LuaGroup::GetMembersCount },

    // Setters
    { NOT_IN_SAFEMODE,              "SetLeader", &LuaGroup::SetLeader },
    { NOT_IN_SAFEMODE,              "SetMembersGroup", &LuaGroup::SetMembersGroup },
    { NOT_IN_SAFEMODE,              "SetTargetIcon", &LuaGroup::SetTargetIcon },

    // Boolean
    { MTMAP_SAFE,                   "IsLeader", &LuaGroup::IsLeader },
    { NOT_IN_SAFEMODE,              "AddMember", &LuaGroup::AddMember },
    { NOT_IN_SAFEMODE,              "RemoveMember", &LuaGroup::RemoveMember },
    { NOT_IN_SAFEMODE,              "Disband", &LuaGroup::Disband },
    { MTMAP_SAFE,                   "IsFull", &LuaGroup::IsFull },
    // { "IsLFGGroup", &LuaGroup::IsLFGGroup },                     // :IsLFGGroup() - Returns true if the group is an LFG group
    { MTMAP_SAFE,                   "IsRaidGroup", &LuaGroup::IsRaidGroup },
    { MTMAP_SAFE,                   "IsBGGroup", &LuaGroup::IsBGGroup },
    // { "IsBFGroup", &LuaGroup::IsBFGroup },                       // :IsBFGroup() - Returns true if the group is a battlefield group
    { MTMAP_SAFE,                   "IsMember", &LuaGroup::IsMember },
    { MTMAP_SAFE,                   "IsAssistant", &LuaGroup::IsAssistant },
    { MTMAP_SAFE,                   "SameSubGroup", &LuaGroup::SameSubGroup },
    { MTMAP_SAFE,                   "HasFreeSlotSubGroup", &LuaGroup::HasFreeSlotSubGroup },

    // Other
    { MTMAP_SAFE,                   "SendPacket", &LuaGroup::SendPacket },
    // { "ConvertToLFG", &LuaGroup::ConvertToLFG },                 // :ConvertToLFG() - Converts the group to an LFG group
    { FLAGS_NONE, "ConvertToRaid", &LuaGroup::ConvertToRaid },

    { FLAGS_NONE, nullptr, nullptr },
};

ElunaRegister<Guild> GuildMethods[] =
{
    // Getters
    { NOT_IN_SAFEMODE,              "GetMembers", &LuaGuild::GetMembers },                  // :GetMembers() - returns a table containing the players in this guild. (Online?)
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "GetLeaderGUID", &LuaGuild::GetLeaderGUID },            // :GetLeaderGUID() - Returns the guild learder's guid
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "GetId", &LuaGuild::GetId },                            // :GetId() - Gets the guild's ID
    { MTMAP_SAFE,                   "GetName", &LuaGuild::GetName },                        // :GetName() - Gets the guild name
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "GetMOTD", &LuaGuild::GetMOTD },                        // :GetMOTD() - Gets the guild MOTD string
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "GetInfo", &LuaGuild::GetInfo },                        // :GetInfo() - Gets the guild info string
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "GetMemberCount", &LuaGuild::GetMemberCount },          // :GetMemberCount() - Returns the amount of players in the guild

    // Setters
#ifndef CLASSIC
    { NOT_IN_SAFEMODE,              "SetBankTabText", &LuaGuild::SetBankTabText },          // :SetBankTabText(tabId, text)
#endif
    { NOT_IN_SAFEMODE,              "SetMemberRank", &LuaGuild::SetMemberRank },         // :SetMemberRank(player, newRank) - Sets the player rank in the guild to the new rank
#ifndef CATA
    { NOT_IN_SAFEMODE,              "SetLeader", &LuaGuild::SetLeader },                    // :SetLeader() - Sets the guild's leader
#endif

    // Other
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "SendPacket", &LuaGuild::SendPacket },                  // :SendPacket(packet) - sends packet to guild
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "SendPacketToRanked", &LuaGuild::SendPacketToRanked },  // :SendPacketToRanked(packet, rankId) - sends packet to guild, specifying a rankId will only send the packet to your ranked members
    { NOT_IN_SAFEMODE,              "Disband", &LuaGuild::Disband },                        // :Disband() - Disbands the guild
    { NOT_IN_SAFEMODE,              "AddMember", &LuaGuild::AddMember },                    // :AddMember(player, rank) - adds the player to the guild. Rank is optional
    { NOT_IN_SAFEMODE,              "DeleteMember", &LuaGuild::DeleteMember },              // :DeleteMember(player, disbanding, kicked) - Deletes the player from the guild. Disbanding and kicked are optional bools
#ifndef CLASSIC
    { NOT_IN_SAFEMODE,              "DepositBankMoney", &LuaGuild::DepositBankMoney },      // :DepositBankMoney(money) - Deposits money into the guild bank
    { NOT_IN_SAFEMODE,              "WithdrawBankMoney", &LuaGuild::WithdrawBankMoney },    // :WithdrawBankMoney(money) - Withdraws money from the guild bank
#endif

    { FLAGS_NONE, nullptr, nullptr },
};

#ifndef CLASSIC
#ifndef TBC
ElunaRegister<Vehicle> VehicleMethods[] =
{
    // Getters
    { MTMAP_SAFE,                   "GetOwner", &LuaVehicle::GetOwner },                    // :GetOwner() - Returns the the vehicle unit
    { MTMAP_SAFE,                   "GetEntry", &LuaVehicle::GetEntry },                    // :GetEntry() - Returns vehicle ID
    { MTMAP_SAFE,                   "GetPassenger", &LuaVehicle::GetPassenger },            // :GetPassenger(seatId) - Returns the passenger by seatId

    // Boolean
    { MTMAP_SAFE,                   "IsOnBoard", &LuaVehicle::IsOnBoard },                  // :IsOnBoard(unit) - Returns true if the unit is on board

    // Other
    { MTMAP_SAFE,                   "AddPassenger", &LuaVehicle::AddPassenger },            // :AddPassenger(passenger, seatId) - Adds a vehicle passenger
    { MTMAP_SAFE,                   "RemovePassenger", &LuaVehicle::RemovePassenger },      // :RemovePassenger(passenger) - Removes the passenger from the vehicle

    { FLAGS_NONE, nullptr, nullptr },
};
#endif
#endif

ElunaRegister<ElunaQuery> QueryMethods[] =
{
    { MTMAP_SAFE,                   "NextRow", &LuaQuery::NextRow },                        // :NextRow() - Advances to next rown in the query. Returns true if there is a next row, otherwise false
    { MTMAP_SAFE,                   "GetColumnCount", &LuaQuery::GetColumnCount },          // :GetColumnCount() - Gets the column count of the query
    { MTMAP_SAFE,                   "GetRowCount", &LuaQuery::GetRowCount },                // :GetRowCount() - Gets the row count of the query
    { MTMAP_SAFE,                   "GetRow", &LuaQuery::GetRow },

    { MTMAP_SAFE,                   "GetBool", &LuaQuery::GetBool },                        // :GetBool(column) - returns a bool from a number column (for example tinyint)
    { MTMAP_SAFE,                   "GetUInt8", &LuaQuery::GetUInt8 },                      // :GetUInt8(column) - returns the value of an unsigned tinyint column
    { MTMAP_SAFE,                   "GetUInt16", &LuaQuery::GetUInt16 },                    // :GetUInt16(column) - returns the value of a unsigned smallint column
    { MTMAP_SAFE,                   "GetUInt32", &LuaQuery::GetUInt32 },                    // :GetUInt32(column) - returns the value of an unsigned int or mediumint column
    { MTMAP_SAFE,                   "GetUInt64", &LuaQuery::GetUInt64 },                    // :GetUInt64(column) - returns the value of an unsigned bigint column as string
    { MTMAP_SAFE,                   "GetInt8", &LuaQuery::GetInt8 },                        // :GetInt8(column) - returns the value of an tinyint column
    { MTMAP_SAFE,                   "GetInt16", &LuaQuery::GetInt16 },                      // :GetInt16(column) - returns the value of a smallint column
    { MTMAP_SAFE,                   "GetInt32", &LuaQuery::GetInt32 },                      // :GetInt32(column) - returns the value of an int or mediumint column
    { MTMAP_SAFE,                   "GetInt64", &LuaQuery::GetInt64 },                      // :GetInt64(column) - returns the value of a bigint column as string
    { MTMAP_SAFE,                   "GetFloat", &LuaQuery::GetFloat },                      // :GetFloat(column) - returns the value of a float column
    { MTMAP_SAFE,                   "GetDouble", &LuaQuery::GetDouble },                    // :GetDouble(column) - returns the value of a double column
    { MTMAP_SAFE,                   "GetString", &LuaQuery::GetString },                    // :GetString(column) - returns the value of a string column, always returns a string
    { MTMAP_SAFE,                   "IsNull", &LuaQuery::IsNull },                          // :IsNull(column) - returns true if the column is null

    { FLAGS_NONE, nullptr, nullptr },
};

ElunaRegister<WorldPacket> PacketMethods[] =
{
    // Getters
    { MTMAP_SAFE,                   "GetOpcode", &LuaPacket::GetOpcode },
    { MTMAP_SAFE,                   "GetSize", &LuaPacket::GetSize },

    // Setters
    { MTMAP_SAFE,                   "SetOpcode", &LuaPacket::SetOpcode },

    // Readers
    { MTMAP_SAFE,                   "ReadByte", &LuaPacket::ReadByte },
    { MTMAP_SAFE,                   "ReadUByte", &LuaPacket::ReadUByte },
    { MTMAP_SAFE,                   "ReadShort", &LuaPacket::ReadShort },
    { MTMAP_SAFE,                   "ReadUShort", &LuaPacket::ReadUShort },
    { MTMAP_SAFE,                   "ReadLong", &LuaPacket::ReadLong },
    { MTMAP_SAFE,                   "ReadULong", &LuaPacket::ReadULong },
    { MTMAP_SAFE,                   "ReadGUID", &LuaPacket::ReadGUID },
    { MTMAP_SAFE,                   "ReadString", &LuaPacket::ReadString },
    { MTMAP_SAFE,                   "ReadFloat", &LuaPacket::ReadFloat },
    { MTMAP_SAFE,                   "ReadDouble", &LuaPacket::ReadDouble },

    // Writers
    { MTMAP_SAFE,                   "WriteByte", &LuaPacket::WriteByte },                   // :WriteByte(val) - Writes an int8 value
    { MTMAP_SAFE,                   "WriteUByte", &LuaPacket::WriteUByte },                 // :WriteUByte(val) - Writes a uint8 value
    { MTMAP_SAFE,                   "WriteShort", &LuaPacket::WriteShort },                 // :WriteShort(val) - Writes an int16 value
    { MTMAP_SAFE,                   "WriteUShort", &LuaPacket::WriteUShort },               // :WriteUShort(val) - Writes a uint16 value
    { MTMAP_SAFE,                   "WriteLong", &LuaPacket::WriteLong },                   // :WriteLong(val) - Writes an int32 value
    { MTMAP_SAFE,                   "WriteULong", &LuaPacket::WriteULong },                 // :WriteULong(val) - Writes a uint32 value
    { MTMAP_SAFE,                   "WriteGUID", &LuaPacket::WriteGUID },                   // :WriteGUID(guid) - Writes a uint64 value
    { MTMAP_SAFE,                   "WriteString", &LuaPacket::WriteString },               // :WriteString(val) - Writes a string value
    { MTMAP_SAFE,                   "WriteFloat", &LuaPacket::WriteFloat },                 // :WriteFloat(val) - Writes a float value
    { MTMAP_SAFE,                   "WriteDouble", &LuaPacket::WriteDouble },               // :WriteDouble(val) - Writes a double value

    { FLAGS_NONE, nullptr, nullptr },
};

ElunaRegister<Map> MapMethods[] =
{
    // Getters
    { MTMAP_SAFE,                   "GetName", &LuaMap::GetName },                          // :GetName() - Returns the map's name UNDOCUMENTED
    { MTMAP_SAFE,                   "GetDifficulty", &LuaMap::GetDifficulty },              // :GetDifficulty() - Returns the map's difficulty UNDOCUMENTED
    { MTMAP_SAFE,                   "GetInstanceId", &LuaMap::GetInstanceId },              // :GetInstanceId() - Returns the map's instance ID UNDOCUMENTED
    { MTMAP_SAFE,                    "GetInstanceData", &LuaMap::GetInstanceData },
    { MTMAP_SAFE,                   "GetPlayers", &LuaMap::GetPlayers },
    { MTMAP_SAFE,                   "GetPlayerCount", &LuaMap::GetPlayerCount },            // :GetPlayerCount() - Returns the amount of players on map except GM's UNDOCUMENTED
    { MTMAP_SAFE,                   "GetMapId", &LuaMap::GetMapId },                        // :GetMapId() - Returns the map's ID UNDOCUMENTED
    { MTMAP_SAFE,                   "GetAreaId", &LuaMap::GetAreaId },                      // :GetAreaId(x, y, z) - Returns the map's area ID based on coords UNDOCUMENTED
    { MTMAP_SAFE,                   "GetHeight", &LuaMap::GetHeight },                      // :GetHeight(x, y[, phasemask]) - Returns ground Z coordinate. UNDOCUMENTED
    { MTMAP_SAFE,                   "GetWorldObject", &LuaMap::GetWorldObject },            // :GetWorldObject(guid) - Returns a worldobject (player, creature, gameobject..) from the map by it's guid

    // Setters
    { MTMAP_SAFE,                   "SetWeather", &LuaMap::SetWeather },

    // Boolean
#ifndef CLASSIC
    { MTMAP_SAFE,                   "IsArena", &LuaMap::IsArena },                          // :IsArena() - Returns the true if the map is an arena, else false UNDOCUMENTED
#endif
    { MTMAP_SAFE,                   "IsBattleground", &LuaMap::IsBattleground },            // :IsBattleground() - Returns the true if the map is a battleground, else false UNDOCUMENTED
    { MTMAP_SAFE,                   "IsDungeon", &LuaMap::IsDungeon },                      // :IsDungeon() - Returns the true if the map is a dungeon , else false UNDOCUMENTED
    { MTMAP_SAFE,                   "IsEmpty", &LuaMap::IsEmpty },                          // :IsEmpty() - Returns the true if the map is empty, else false UNDOCUMENTED
#ifndef CLASSIC
    { MTMAP_SAFE,                   "IsHeroic", &LuaMap::IsHeroic },                        // :IsHeroic() - Returns the true if the map is a heroic dungeon, else false UNDOCUMENTED
#endif
    { MTMAP_SAFE,                   "IsRaid", &LuaMap::IsRaid },                            // :IsRaid() - Returns the true if the map is a raid map, else false UNDOCUMENTED

    // Other
    { MTMAP_SAFE,                   "SaveInstanceData", &LuaMap::SaveInstanceData },

    { FLAGS_NONE, nullptr, nullptr },
};

ElunaRegister<Corpse> CorpseMethods[] =
{
    // Getters
    { MTMAP_SAFE,                   "GetOwnerGUID", &LuaCorpse::GetOwnerGUID },
    { MTMAP_SAFE,                   "GetGhostTime", &LuaCorpse::GetGhostTime },
    { MTMAP_SAFE,                   "GetType", &LuaCorpse::GetType },

    // Other
    { MTMAP_SAFE,                   "ResetGhostTime", &LuaCorpse::ResetGhostTime },
    { MTMAP_SAFE | NOT_IN_SAFEMODE, "SaveToDB", &LuaCorpse::SaveToDB },

    { FLAGS_NONE, nullptr, nullptr }
};

ElunaRegister<AuctionHouseEntry> AuctionMethods[] =
{
    { FLAGS_NONE, nullptr, nullptr }
};

ElunaRegister<BattleGround> BattleGroundMethods[] =
{
    // Getters
    { MTMAP_SAFE,                   "GetName", &LuaBattleGround::GetName },
    { MTMAP_SAFE,                   "GetAlivePlayersCountByTeam", &LuaBattleGround::GetAlivePlayersCountByTeam },
    { MTMAP_SAFE,                   "GetMap", &LuaBattleGround::GetMap },
    { MTMAP_SAFE,                   "GetBonusHonorFromKillCount", &LuaBattleGround::GetBonusHonorFromKillCount },
    { MTMAP_SAFE,                   "GetBracketId", &LuaBattleGround::GetBracketId },
    { MTMAP_SAFE,                   "GetEndTime", &LuaBattleGround::GetEndTime },
    { MTMAP_SAFE,                   "GetFreeSlotsForTeam", &LuaBattleGround::GetFreeSlotsForTeam },
    { MTMAP_SAFE,                   "GetInstanceId", &LuaBattleGround::GetInstanceId },
    { MTMAP_SAFE,                   "GetMapId", &LuaBattleGround::GetMapId },
    { MTMAP_SAFE,                   "GetTypeId", &LuaBattleGround::GetTypeId },
    { MTMAP_SAFE,                   "GetMaxLevel", &LuaBattleGround::GetMaxLevel },
    { MTMAP_SAFE,                   "GetMinLevel", &LuaBattleGround::GetMinLevel },
    { MTMAP_SAFE,                   "GetMaxPlayers", &LuaBattleGround::GetMaxPlayers },
    { MTMAP_SAFE,                   "GetMinPlayers", &LuaBattleGround::GetMinPlayers },
    { MTMAP_SAFE,                   "GetMaxPlayersPerTeam", &LuaBattleGround::GetMaxPlayersPerTeam },
    { MTMAP_SAFE,                   "GetMinPlayersPerTeam", &LuaBattleGround::GetMinPlayersPerTeam },
    { MTMAP_SAFE,                   "GetWinner", &LuaBattleGround::GetWinner },
    { MTMAP_SAFE,                   "GetStatus", &LuaBattleGround::GetStatus },

    { FLAGS_NONE, nullptr, nullptr }
};

template<typename T> const char* ElunaTemplate<T>::tname = nullptr;
template<typename T> bool ElunaTemplate<T>::manageMemory = false;

#if (!defined(TBC) && !defined(CLASSIC))
// fix compile error about accessing vehicle destructor
template<> int ElunaTemplate<Vehicle>::CollectGarbage(lua_State* L)
{
    ASSERT(!manageMemory);

    // Get object pointer (and check type, no error)
    ElunaObject* obj = Eluna::CHECKOBJ<ElunaObject>(L, 1, false);
    delete obj;
    return 0;
}
#endif

void RegisterFunctions(Eluna* E)
{
    ElunaFunction::SetMethods(E, GlobalMethods);

    ElunaTemplate<Object>::Register(E, "Object");
    ElunaTemplate<Object>::SetMethods(E, ObjectMethods);

    ElunaTemplate<WorldObject>::Register(E, "WorldObject");
    ElunaTemplate<WorldObject>::SetMethods(E, ObjectMethods);
    ElunaTemplate<WorldObject>::SetMethods(E, WorldObjectMethods);

    ElunaTemplate<Unit>::Register(E, "Unit");
    ElunaTemplate<Unit>::SetMethods(E, ObjectMethods);
    ElunaTemplate<Unit>::SetMethods(E, WorldObjectMethods);
    ElunaTemplate<Unit>::SetMethods(E, UnitMethods);

    ElunaTemplate<Player>::Register(E, "Player");
    ElunaTemplate<Player>::SetMethods(E, ObjectMethods);
    ElunaTemplate<Player>::SetMethods(E, WorldObjectMethods);
    ElunaTemplate<Player>::SetMethods(E, UnitMethods);
    ElunaTemplate<Player>::SetMethods(E, PlayerMethods);

    ElunaTemplate<Creature>::Register(E, "Creature");
    ElunaTemplate<Creature>::SetMethods(E, ObjectMethods);
    ElunaTemplate<Creature>::SetMethods(E, WorldObjectMethods);
    ElunaTemplate<Creature>::SetMethods(E, UnitMethods);
    ElunaTemplate<Creature>::SetMethods(E, CreatureMethods);

    ElunaTemplate<GameObject>::Register(E, "GameObject");
    ElunaTemplate<GameObject>::SetMethods(E, ObjectMethods);
    ElunaTemplate<GameObject>::SetMethods(E, WorldObjectMethods);
    ElunaTemplate<GameObject>::SetMethods(E, GameObjectMethods);

    ElunaTemplate<Corpse>::Register(E, "Corpse");
    ElunaTemplate<Corpse>::SetMethods(E, ObjectMethods);
    ElunaTemplate<Corpse>::SetMethods(E, WorldObjectMethods);
    ElunaTemplate<Corpse>::SetMethods(E, CorpseMethods);

    ElunaTemplate<Item>::Register(E, "Item");
    ElunaTemplate<Item>::SetMethods(E, ObjectMethods);
    ElunaTemplate<Item>::SetMethods(E, ItemMethods);

#ifndef CLASSIC
#ifndef TBC
    ElunaTemplate<Vehicle>::Register(E, "Vehicle");
    ElunaTemplate<Vehicle>::SetMethods(E, VehicleMethods);
#endif
#endif

    ElunaTemplate<Group>::Register(E, "Group");
    ElunaTemplate<Group>::SetMethods(E, GroupMethods);

    ElunaTemplate<Guild>::Register(E, "Guild");
    ElunaTemplate<Guild>::SetMethods(E, GuildMethods);

    ElunaTemplate<Aura>::Register(E, "Aura");
    ElunaTemplate<Aura>::SetMethods(E, AuraMethods);

    ElunaTemplate<Spell>::Register(E, "Spell");
    ElunaTemplate<Spell>::SetMethods(E, SpellMethods);

    ElunaTemplate<Quest>::Register(E, "Quest");
    ElunaTemplate<Quest>::SetMethods(E, QuestMethods);

    ElunaTemplate<Map>::Register(E, "Map");
    ElunaTemplate<Map>::SetMethods(E, MapMethods);

    ElunaTemplate<AuctionHouseEntry>::Register(E, "AuctionHouseEntry");
    ElunaTemplate<AuctionHouseEntry>::SetMethods(E, AuctionMethods);

    ElunaTemplate<BattleGround>::Register(E, "BattleGround");
    ElunaTemplate<BattleGround>::SetMethods(E, BattleGroundMethods);

    ElunaTemplate<WorldPacket>::Register(E, "WorldPacket", true);
    ElunaTemplate<WorldPacket>::SetMethods(E, PacketMethods);

    ElunaTemplate<ElunaQuery>::Register(E, "ElunaQuery", true);
    ElunaTemplate<ElunaQuery>::SetMethods(E, QueryMethods);
}
