-- Simply Love 4P overlay: loaded after Simply Love's own scripts.

-- Quarter-screen X centre for a player in 4-player mode.
function Get4PQuarterX(pn)
	local idx = tonumber(ToEnumShortString(pn):match("%d")) or 1
	return _screen.w * (0.125 + 0.25 * (idx - 1))
end

local orig_GetNotefieldX = GetNotefieldX
function GetNotefieldX(player)
	local style = GAMESTATE:GetCurrentStyle(player)
	if style and style:GetStyleType() == "StyleType_FourPlayersFourSides" then
		return Get4PQuarterX(player)
	end
	return orig_GetNotefieldX(player)
end

-- Simply Love builds file paths from THEME:GetCurrentThemeDirectory(); in this
-- overlay theme that is the (nearly empty) 4P directory, so point it at the
-- base theme instead. Everything else is forwarded to the real THEME object.
do
	local real = THEME
	local proxy = {}
	function proxy:GetCurrentThemeDirectory() return "Themes/Simply Love/" end
	setmetatable(proxy, {__index = function(_, k)
		local v = real[k]
		if type(v) == "function" then return function(_, ...) return v(real, ...) end end
		return v
	end})
	THEME = proxy
end

-- Per-player state for players 3 and 4 (cloned from P1's metatable).
do
	local mt = getmetatable(SL.P1)
	SL.P3 = setmetatable({}, mt)
	SL.P4 = setmetatable({}, mt)
	local orig_init = InitializeSimplyLove
	function InitializeSimplyLove()
		orig_init()
		SL.P3:initialize()
		SL.P4:initialize()
	end
	InitializeSimplyLove()
end

-- Four clearly distinct, bright player colours (P3/P4 would otherwise be white / near-black).
local quad_colors = {
	["PlayerNumber_P1"] = color("#2EC8FF"), -- cyan
	["PlayerNumber_P2"] = color("#FFB020"), -- amber
	["PlayerNumber_P3"] = color("#4CFF5C"), -- green
	["PlayerNumber_P4"] = color("#FF4CD8"), -- magenta
}
local orig_PlayerColor = PlayerColor
function PlayerColor(pn, decorative)
	if GAMESTATE:GetCurrentStyle() and GAMESTATE:GetCurrentStyle():GetStyleType() == "StyleType_FourPlayersFourSides" and quad_colors[pn] then
		return quad_colors[pn]
	end
	return orig_PlayerColor(pn, decorative)
end

function IsQuadStyle()
	local s = GAMESTATE:GetCurrentStyle()
	return s ~= nil and s:GetStyleType() == "StyleType_FourPlayersFourSides"
end

-- Notefield width for the quad style (the base table has no 'quad' entry).
local orig_GetNotefieldWidth = GetNotefieldWidth
function GetNotefieldWidth()
	if IsQuadStyle() then return 256 end
	return orig_GetNotefieldWidth()
end

-- The stock margin function only knows P1/P2; the quad style positions fields from metrics.
local orig_GameplayMargins = GameplayMargins
function GameplayMargins(enabled_players, styletype)
	if styletype == "StyleType_FourPlayersFourSides" then return 40, 80, 40 end
	return orig_GameplayMargins(enabled_players, styletype)
end
