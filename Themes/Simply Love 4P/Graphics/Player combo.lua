-- 4P wrapper: smaller combo with a COMBO label stacked under the number.
local player = Var "Player"
local base = LoadActor("/Themes/Simply Love/Graphics/Player combo.lua", {Player=player})
if not IsQuadStyle() then return base end
local ShowComboAt = THEME:GetMetric("Combo", "ShowComboAt")
return Def.ActorFrame{
	InitCommand=function(self) self:zoom(0.6):draworder(101) end,
	Def.ActorFrame{ InitCommand=function(self) self:y(-14) end, base },
	LoadFont("Common Normal")..{
		Text="COMBO",
		Name="ComboLabel",
		InitCommand=function(self) self:y(14):zoom(0.8):visible(false) end,
		ComboCommand=function(self, params)
			local n = params.Combo or params.Misses
			self:settext(params.Combo and 'COMBO' or 'MISSES'):visible(n ~= nil and n >= ShowComboAt)
		end,
	},
}
