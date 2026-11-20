-- 4P overlay: base overlay logic plus the per-quarter HUD (drawn above the arrows).
local base_overlay = LoadActor("/Themes/Simply Love/BGAnimations/ScreenGameplay overlay/default.lua")
if not IsQuadStyle() then return base_overlay end
local base = "/Themes/Simply Love/BGAnimations/ScreenGameplay underlay/"
local Players = GAMESTATE:GetHumanPlayers()
local t = Def.ActorFrame{ Name="QuadHUD", base_overlay }

for player in ivalues(Players) do
	local pn = ToEnumShortString(player)
	local cx = Get4PQuarterX(player)
	local col = PlayerColor(player)
	local pss = STATSMAN:GetCurStageStats():GetPlayerStageStats(player)
	local steps = GAMESTATE:GetCurrentSteps(player)
	local w = 170

	local af = Def.ActorFrame{ Name="Quad HUD "..pn, InitCommand=function(self) self:x(cx) end }

	-- life bar
	af[#af+1] = Def.Quad{ InitCommand=function(self) self:xy(0,14):zoomto(w+4,16) end }
	af[#af+1] = Def.Quad{ InitCommand=function(self) self:xy(0,14):zoomto(w,12):diffuse(0,0,0,1) end }
	af[#af+1] = Def.Quad{
		Name="Fill",
		InitCommand=function(self) self:xy(-w/2,14):zoomto(w*0.5,12):horizalign(left):diffuse(col) end,
		LifeChangedMessageCommand=function(self, params)
			if params.Player == player then
				self:finishtweening():bouncebegin(0.1):zoomx(params.LifeMeter:GetLife() * w)
			end
		end,
		HealthStateChangedMessageCommand=function(self, params)
			if params.PlayerNumber == player then
				self:diffuse(params.HealthState == "HealthState_Hot" and Color.White or col)
			end
		end,
	}

	-- difficulty meter box
	af[#af+1] = Def.Quad{
		InitCommand=function(self)
			self:xy(-w/2+12, 48):zoomto(24,24)
			if steps then self:diffuse(DifficultyColor(steps:GetDifficulty())) end
		end
	}
	af[#af+1] = LoadFont("Common Bold")..{
		InitCommand=function(self)
			self:xy(-w/2+12, 48):diffuse(Color.Black):zoom(0.4)
			if steps then self:settext(steps:GetMeter()) end
		end
	}

	-- score
	af[#af+1] = LoadFont("Wendy/_wendy monospace numbers")..{
		Text="0.00",
		InitCommand=function(self) self:xy(w/2, 58):horizalign(right):valign(1):zoom(0.45):diffuse(col) end,
		JudgmentMessageCommand=function(self) self:queuecommand("Redraw") end,
		RedrawCommand=function(self)
			self:settext(FormatPercentScore(pss:GetPercentDancePoints()):sub(1,-2))
		end,
	}

	-- profile name
	af[#af+1] = LoadFont("Common Normal")..{
		InitCommand=function(self)
			local name = PROFILEMAN:GetPlayerName(player)
			if name == nil or name == "" then name = "PLAYER "..pn:sub(2) end
			self:xy(-w/2, 76):horizalign(left):zoom(0.6):diffuse(col):settext(name)
		end
	}
	t[#t+1] = af
end

return t
