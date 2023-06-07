local network;

function vector4str(a)
	return string.format("[%.6f, %.6f, %.6f, %.6f]", a.x, a.y, a.z, a.w)
end

function nullable(a)
	if not a then
		return ""
	end
	
	return a
end

registerForEvent("onInit", function()
	network = NewObject("NetworkController")
	print("DamageSender init")
	
	Observe("DamageSystem", "PostProcess", function(self, hitEvent, cache) 
		if hitEvent.target:IsPlayer() == false then
			return
		end
		
		local fmt = [[{
			"type": "%s",
			"hit_position": %s,
			"hit_direction": %s,
			"attack_position": %s,
			"attack_pentration": %.6f
		}]]
		local data = string.format(
			fmt,
			hitEvent.attackData.attackType.value,
			vector4str(hitEvent.hitPosition),
			vector4str(hitEvent.hitDirection),
			vector4str(hitEvent.attackData.attackPosition),
			hitEvent.attackPentration
		)
		network:SendUDP(data)
		
		-- print(hitEvent.hitRepresentationResult.hitShapes[1].result.hitPositionEnter, ' -> ', hitEvent.hitRepresentationResult.hitShapes[1].result.hitPositionExit)
		
		
		print("Source position: ", hitEvent.attackData.source:GetWorldPosition())
		
		--print("source: ", hitEvent.attackData.source:GetDisplayName())
		--print("weapon: ", hitEvent.attackData.weapon:GetDisplayName())
		print("Hit position: ", hitEvent.hitPosition)
		--print(Dump(hitEvent.attackComputed, false))
		print("Attack values:")
		for i, value in ipairs(hitEvent.attackComputed:GetAttackValues()) do
			print(i, " -> ", value)
		end
		--print(Dump(hitEvent.hitRepresentationResult.hitShapes[1].result, false))
	end)
end)

Observe("PlayerPuppet", "OnHitUI", function(self, hitEvent) 
	print("Hit!")
end)