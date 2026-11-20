-- 4P wrapper: shrink judgments so they fit inside a quarter-screen field.
local player = Var "Player"
local base = LoadActor("/Themes/Simply Love/Graphics/Player judgment.lua", {Player=player})
if not IsQuadStyle() then return base end
return Def.ActorFrame{ InitCommand=function(self) self:zoom(0.6) end, base }
