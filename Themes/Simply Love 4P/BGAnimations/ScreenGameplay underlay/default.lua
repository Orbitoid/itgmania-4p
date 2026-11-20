-- 4P overlay: for the quad style, draw a compact per-quarter HUD; otherwise defer to Simply Love.
local base = "/Themes/Simply Love/BGAnimations/ScreenGameplay underlay/"
if not IsQuadStyle() then return LoadActor(base .. "default.lua") end

local Players = GAMESTATE:GetHumanPlayers()
local t = Def.ActorFrame{ Name="GameplayUnderlay" }

-- smaller playfields so the four 4-column fields (and their judgments) fit in a quarter each
for player in ivalues(Players) do
	for _, lvl in ipairs{"ModsLevel_Preferred","ModsLevel_Stage","ModsLevel_Song","ModsLevel_Current"} do
		GAMESTATE:GetPlayerState(player):GetPlayerOptions(lvl):Mini(0.25)
	end
end

for player in ivalues(Players) do
	t[#t+1] = LoadActor(base .. "PerPlayer/BackgroundFilter.lua", player)
end
t[#t+1] = LoadActor(base .. "Shared/Header.lua")

for player in ivalues(Players) do
	t[#t+1] = LoadActor(base .. "PerPlayer/NoteField/default.lua", player)
end
return t
