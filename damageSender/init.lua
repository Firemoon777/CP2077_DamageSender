local network;
-- local blackboardDefs = Game.GetAllBlackboardDefs()
-- local blackboardUI = Game.GetBlackboardSystem():Get(blackboardDefs.UI_Notifications)


registerForEvent("onInit", function()
	network = NewObject("NetworkController")
	print("DamageSender init")
	
	Observe("DamageSystem", "PostProcess", function(self, hitEvent, cache) 
		if hitEvent.target:IsPlayer() == false then
			return
		end
		
		network:Clear()
		-- network:AddString("hit_type", hitEvent.attackData.hitType.value)
		network:AddString("attack_type", hitEvent.attackData.attackType.value)
		network:AddVector4("hit_position", hitEvent.hitPosition)
		network:AddVector4("hit_direction", hitEvent.hitDirection)
		network:AddVector4("attack_position", hitEvent.attackData.attackPosition)
		--network:AddFloat("attack_pentration", hitEvent.attackPentration)
	
		flags = {}
		for i, flag in ipairs(hitEvent.attackData.flags) do
			flags[i] = flag.flag.value
			network:AddStringArrayValue("flags", flag.flag.value)
		end
		
		network:AddFloat("damage_chemical", hitEvent.attackComputed:GetAttackValue(gamedataDamageType.Chemical))
		network:AddFloat("damage_electric", hitEvent.attackComputed:GetAttackValue(gamedataDamageType.Electric))
		network:AddFloat("damage_physical", hitEvent.attackComputed:GetAttackValue(gamedataDamageType.Physical))
		network:AddFloat("damage_thermal", hitEvent.attackComputed:GetAttackValue(gamedataDamageType.Thermal))

		local sps = Game.GetStatPoolsSystem()
		local currHealth = sps:GetStatPoolValue(hitEvent.target:GetEntityID(), gamedataStatPoolType.Health)
		network:AddFloat("health", currHealth)
		
		local direction = Game.GetAttackAngleInInt(hitEvent)
		if direction == 1 then
			network:AddString("direction", "left")
		elseif direction == 2 then
			network:AddString("direction", "back")
		elseif direction == 3 then
			network:AddString("direction", "right")
		else
			network:AddString("direction", "front")
		end

		network:Send()
	end)
end)