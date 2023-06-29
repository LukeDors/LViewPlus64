/*
for evade use spellbook activespell and/or missilelist
*/

#include "Offsets.h"

Offsets::Offsets() {};

int Offsets::GameTime = 0x521EE00; //

int Offsets::ObjIndex = 0x10; 
int Offsets::ObjTeam = 0x3C; 
int Offsets::ObjMissileName = 0x54; //
int Offsets::ObjNetworkID = 0xC8; 
int Offsets::ObjPos = 0x220; 
int Offsets::ObjMissileSpellCast = 0x250;
int Offsets::ObjVisibility = 0x310; 
int Offsets::ObjSpawnCount = 0x328; 
int Offsets::ObjSrcIndex = 0x294;
int Offsets::ObjMana = 0x0340; 
int Offsets::ObjMaxMana = 0x0358; 
int Offsets::ObjRecallState = 0xF40;
int Offsets::ObjHealth = 0x1068; 
int Offsets::ObjMaxHealth = 0x1080; 
int Offsets::ObjAbilityHaste = 0x14A8; 
int Offsets::ObjLethality = 0x15A8; 
int Offsets::ObjArmor = 0x16A4; 
int Offsets::ObjBonusArmor = 0x16A8; 
int Offsets::ObjMagicRes = 0x16AC; 
int Offsets::ObjBonusMagicRes = 0x16B0; 
int Offsets::ObjBaseAtk = 0x167C;  //
int Offsets::ObjBonusAtk = 0x1AA8; 
int Offsets::ObjMoveSpeed = 0x16BC; 
int Offsets::ObjSpellBook = 0x30C8; //
int Offsets::ObjTransformation = 0x3040;
int Offsets::ObjName = 0x3848; //
int Offsets::PlayerName = 0x60; 
int Offsets::ObjLvl = 0x3FF0; //
int Offsets::ObjExpiry = 0x298;
int Offsets::ObjCrit = 0x1C88; 
int Offsets::ObjCritMulti = 0x12D4; 
int Offsets::ObjAbilityPower = 0x15F8; 
int Offsets::ObjAtkSpeedMulti = 0x11C8; 
int Offsets::ObjAtkRange = 0x16C4; //
int Offsets::ObjTargetable = 0xEB0; 
int Offsets::ObjInvulnerable = 0x4F0; 
int Offsets::ObjIsMoving = 0x3F00; //
int Offsets::ObjDirection = 0x2198;
int Offsets::ObjItemList = 0x35F8;

int Offsets::ObjExpierience = 0x359C;
int Offsets::ObjMagicPen = 0x158C;
int Offsets::ObjMagicPenMulti = 0x11C8;
int Offsets::ObjAdditionalApMulti = 0x12E8;
int Offsets::ObjManaRegen = 0x14DC;
int Offsets::ObjHealthRegen = 0x16B8; 

int Offsets::Ping = 0x51AD648; 
int Offsets::OffPing = 0x178; 
int Offsets::ShowPing = 0x6D8; 

int Offsets::Chat = 0x518ECB8; 
int Offsets::ChatIsOpen = 0xC90; 

int Offsets::SpellBookActiveSpellCast = 0x2A30; //
int Offsets::SpellBookSpellSlots = 0x4C8;

int Offsets::ObjBuffManager = 0x27C0;
int Offsets::BuffManagerEntriesArray = 0x18;
int Offsets::BuffEntryBuff = 0x10;
int Offsets::BuffType = 0x4;
int Offsets::BuffEntryBuffStartTime = 0x18;
int Offsets::BuffEntryBuffEndTime = 0x1C;
int Offsets::BuffEntryBuffCount = 0x3C;
int Offsets::BuffEntryBuffCountAlt = 0x24;
int Offsets::BuffEntryBuffCountAlt2 = 0x20;
int Offsets::BuffName = 0x8;
int Offsets::BuffEntryBuffNodeStart = 0x20;
int Offsets::BuffEntryBuffNodeCurrent = 0x24;

int Offsets::ItemListItem = 0xC; //
int Offsets::ItemInfo = 0x20; //
int Offsets::ItemInfoId = 0x68; //

int Offsets::CurrentDashSpeed = 0x1D0;
int Offsets::IsDashing = 0x398;
int Offsets::DashPos = 0x1FC;
int Offsets::IsMoving = 0x198;
int Offsets::NavBegin = 0x1BC;
int Offsets::NavEnd = 0x1C0;

int Offsets::ViewProjMatrices = 0x526C1B0; //
int Offsets::Renderer = 0x5274650; //
int Offsets::RendererWidth = 0xC; 
int Offsets::RendererHeight = 0x10;

int Offsets::SpellSlotLevel = 0x28;  
int Offsets::SpellSlotTime = 0x30; 
int Offsets::SpellSlotAmmo = 0x54; 
int Offsets::SpellSlotCharges = 0x5C;
int Offsets::SpellSlotTimeCharge = 0x6C; 
int Offsets::SpellSlotDamage = 0x90;
int Offsets::SpellSlotSpellInfo = 0x130; //
int Offsets::SpellInfoSpellData = 0x60; //
int Offsets::SpellDataSpellName = 0x80; //
int Offsets::SpellDataMissileName = 0xB0; //
int Offsets::SpellSlotSmiteTimer = 0x60;
int Offsets::SpellSlotSmiteCharges = 0x5C;

int Offsets::ObjectManager = 0x2173220; //

int Offsets::HeroList = 0x2173340; //
int Offsets::MinionList = 0x39CDCE0; //
int Offsets::MissileList = 0x522B828; //
int Offsets::TurretList = 0x521A3F0; //

int Offsets::LocalPlayer = 0x522B768; //
int Offsets::UnderMouseObject = 0x39CDED8; //

int Offsets::ObjectMapCount = 0x48; 
int Offsets::ObjectMapRoot = 0x40; 
int Offsets::ObjectMapNodeNetId = 0x20; 
int Offsets::ObjectMapNodeObject = 0x28; 

int Offsets::SpellCastSpellInfo = 0x8;
int Offsets::SpellCastStartTime = 0x544;
int Offsets::SpellCastStartTimeAlt = 0x534;
int Offsets::SpellCastCastTime = 0x4C0;
int Offsets::SpellCastStart = 0x80;
int Offsets::SpellCastEnd = 0x8C;
int Offsets::SpellCastSrcIdx = 0x68;
int Offsets::SpellCastDestIdx = 0xC0;

int Offsets::MissileSpellInfo = 0x2E8; //
int Offsets::MissileSrcIdx = 0x370; //
int Offsets::MissileDestIdx = 0x3C8; //
int Offsets::MissileStartPos = 0x38C; //
int Offsets::MissileEndPos = 0x398; //
int Offsets::MissileMapCount = 0x10;
int Offsets::MissileMapRoot = 0x8;
int Offsets::MissileMapKey = 0x20;
int Offsets::MissileMapVal = 0x28;

int Offsets::MinimapObject = 0x51E8CD0; //
int Offsets::MinimapObjectHud = 0x320; //
int Offsets::MinimapHudPos = 0x60; //
int Offsets::MinimapHudSize = 0x68; 

int Offsets::AiManager = 0x4F8;  // 
int Offsets::AiManagerStartPath = 0x1cc;
int Offsets::AiManagerEndPath = 0x1D8;
int Offsets::AiManagerTargetPosition = 0x10;
int Offsets::AiManagerIsMoving = 0x1C0;
int Offsets::AiManagerIsDashing = 0x214;
int Offsets::AiManagerCurrentSegment = 0x1C4;
int Offsets::AiManagerDashSpeed = 0x1F8;
int Offsets::ServerPos = 0x2EC;
int Offsets::Velocity = 0x2F8;

int Offsets::HudInstance = 0x213DB58; //