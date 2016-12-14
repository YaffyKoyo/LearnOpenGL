local assets=
{
    Asset("ANIM", "anim/lgq.zip"),
    Asset("ANIM", "anim/swap_lgq.zip"),
 
    Asset("ATLAS", "images/inventoryimages/lgq.xml"),
    Asset("IMAGE", "images/inventoryimages/lgq.tex"),

}
local prefabs =
{
    "groundpoundring_fx",
}

local function onattack_lgq(inst, attacker, target)

	target.AnimState:SetMultColour(125/255,125/255,125/255,1)
    SpawnPrefab("groundpoundring_fx").Transform:SetPosition(target.Transform:GetWorldPosition())
	
	 if attacker and attacker.components.health and attacker.components.hunger then
        attacker.components.health:DoDelta(0)
		attacker.components.hunger:DoDelta(0)
    end
end
	
	


local function fn()
	local inst = CreateEntity()
	inst.entity:AddTransform()
	inst.entity:AddAnimState()
	 inst.entity:AddSoundEmitter()
    MakeInventoryPhysics(inst)

	inst.entity:AddTransform()
	inst.entity:AddAnimState()
	inst.entity:AddSoundEmitter()
    inst.entity:AddNetwork()
	
	inst.AnimState:SetBank("rosebow")
    inst.AnimState:SetBuild("lgq")
    inst.AnimState:PlayAnimation("idle")
	inst.entity:AddSoundEmitter()
		inst.foleysound = "dontstarve/zg/lgq"

	
	if not TheWorld.ismastersim then
        return inst
    end
 
    local function OnEquip(inst, owner)
        owner.AnimState:OverrideSymbol("swap_object", "swap_lgq", "swap_rosebow")
        owner.AnimState:Show("ARM_carry")
        owner.AnimState:Hide("ARM_normal")
		owner.components.combat:SetAreaDamage(6, 0.6)
   --inst.hauntsfxtask = inst:DoPeriodicTask(.3, function(inst)
            inst.SoundEmitter:PlaySound(inst.foleysound)
        --end)

    end
 
    local function OnUnequip(inst, owner)
        owner.AnimState:Hide("ARM_carry")
        owner.AnimState:Show("ARM_normal")
		
		  -- inst.hauntsfxtask:Cancel()
       -- inst.hauntsfxtask = nil
		
    end
 
    inst:AddComponent("inventoryitem")
	--inst.components.inventoryitem.keepondeath = true
    --inst.components.inventoryitem.imagename = "rosebow"
    inst.components.inventoryitem.atlasname = "images/inventoryimages/lgq.xml"