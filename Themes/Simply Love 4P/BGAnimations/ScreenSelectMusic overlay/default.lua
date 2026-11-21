-- 4P overlay: for the quad style, replace the 2-player pane/difficulty-list layout with four compact
-- per-player panels in a 2x2 grid under the banner. Other styles use the stock overlay.
local base = "/Themes/Simply Love/BGAnimations/ScreenSelectMusic overlay/"
if not IsQuadStyle() then return LoadActor(base .. "default.lua") end

local af = Def.ActorFrame{
	InitCommand=function(self)
		SL.Global.GameplayReloadCheck = false
		generateFavoritesForMusicWheel()
		local songOptions = GAMESTATE:GetSongOptionsObject("ModsLevel_Preferred")
		songOptions:MusicRate(SL.Global.ActiveModifiers.MusicRate)
	end,
	PlayerProfileSetMessageCommand=function(self, params)
		if not PROFILEMAN:IsPersistentProfile(params.Player) then LoadGuest(params.Player) end
		generateFavoritesForMusicWheel()
		ApplyMods(params.Player)
	end,
	PlayerJoinedMessageCommand=function(self, params)
		if not PROFILEMAN:IsPersistentProfile(params.Player) then LoadGuest(params.Player) end
		ApplyMods(params.Player)
	end,
	CodeMessageCommand=function(self, params)
		if params.Name == "EscapeFromEventMode" then SCREENMAN:GetTopScreen():Cancel() end
	end,
	LoadActor(base .. "PreserveMenuTimer.lua"),
	LoadActor(base .. "SongDescription/SongDescription.lua"),
	LoadActor(base .. "banner.lua"),
	LoadActor(base .. "SortMenu/default.lua"),
	LoadActor(base .. "SongSearch/default.lua"),
}

local PW, PH = 204, 112
local function panel(player, idx)
	local pn = ToEnumShortString(player)
	local col = PlayerColor(player)
	local x = 8 + ((idx - 1) % 2) * (PW + 6)
	local y = 222 + math.floor((idx - 1) / 2) * (PH + 6)
	local p = Def.ActorFrame{ Name="QuadPanel"..pn, InitCommand=function(self) self:xy(x, y) end }
	p[#p+1] = Def.Quad{ InitCommand=function(self) self:halign(0):valign(0):zoomto(PW, PH):diffuse(col) end }
	p[#p+1] = Def.Quad{ InitCommand=function(self) self:xy(3,3):halign(0):valign(0):zoomto(PW-6, PH-6):diffuse(0.05,0.05,0.08,0.92) end }
	-- header: player label + profile name
	p[#p+1] = Def.BitmapText{
		Font="Common Normal",
		InitCommand=function(self)
			local name = PROFILEMAN:GetPlayerName(player)
			if not name or name == "" then name = "" end
			self:xy(8, 14):halign(0):zoom(0.9):diffuse(col):settext(pn .. "  " .. name)
		end,
	}
	-- difficulty meter, big
	p[#p+1] = Def.BitmapText{
		Font="Wendy/_wendy white",
		Name="Meter",
		InitCommand=function(self) self:xy(PW - 14, 62):halign(1):zoom(0.9):settext("") end,
		SetCommand=function(self)
			local steps = GAMESTATE:GetCurrentSteps(player)
			if steps then
				self:settext(steps:GetMeter()):diffuse(DifficultyColor(steps:GetDifficulty()))
			else
				self:settext("")
			end
		end,
		OnCommand=function(self) self:queuecommand("Set") end,
		CurrentSongChangedMessageCommand=function(self) self:queuecommand("Set") end,
		["CurrentSteps"..pn.."ChangedMessageCommand"]=function(self) self:queuecommand("Set") end,
	}
	-- difficulty name + step count
	p[#p+1] = Def.BitmapText{
		Font="Common Normal",
		InitCommand=function(self) self:xy(8, 50):halign(0):zoom(0.9) end,
		SetCommand=function(self)
			local steps = GAMESTATE:GetCurrentSteps(player)
			if steps then
				self:settext(THEME:GetString("Difficulty", ToEnumShortString(steps:GetDifficulty())))
					:diffuse(DifficultyColor(steps:GetDifficulty()))
			else self:settext("") end
		end,
		OnCommand=function(self) self:queuecommand("Set") end,
		CurrentSongChangedMessageCommand=function(self) self:queuecommand("Set") end,
		["CurrentSteps"..pn.."ChangedMessageCommand"]=function(self) self:queuecommand("Set") end,
	}
	p[#p+1] = Def.BitmapText{
		Font="Common Normal",
		InitCommand=function(self) self:xy(8, 80):halign(0):zoom(0.8):diffuse(1,1,1,1) end,
		SetCommand=function(self)
			local steps = GAMESTATE:GetCurrentSteps(player)
			if steps and steps:GetRadarValues(player) then
				local rv = steps:GetRadarValues(player)
				self:settext(rv:GetValue("RadarCategory_TapsAndHolds") .. " steps\n" ..
					rv:GetValue("RadarCategory_Jumps") .. " jumps  " .. rv:GetValue("RadarCategory_Holds") .. " holds")
			else self:settext("") end
		end,
		OnCommand=function(self) self:queuecommand("Set") end,
		CurrentSongChangedMessageCommand=function(self) self:queuecommand("Set") end,
		["CurrentSteps"..pn.."ChangedMessageCommand"]=function(self) self:queuecommand("Set") end,
	}
	return p
end

for i, player in ipairs(PlayerNumber) do
	af[#af+1] = panel(player, i)
end
return af
